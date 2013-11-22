#include <math.h>

#include "prediction.h"

using namespace std;

set<string> all_qid_list;

vector<clkinfo> clkinfo_list;

double g_mse = 0.0;
double g_mae = 0.0;
double g_total_clk = 0.0;
double g_total_nonclk = 0.0;
double g_mean_ctr1 = 0.0;   // total_clk / (total_clk + total_nonclk)
double g_mean_ctr2 = 0.0;   // 1/n * sum (clk_i/(clk_i + nonclk_i))
double g_mean_pctr2 = 0.0;  // 1/n * sum (pctr_i)
int64_t g_ins = 0;

int load_qid(const char * p_str_qid_path)
{
    int ret = 0;
    ifstream ifs;
    string qid = "";

    if (NULL == p_str_qid_path) {
        Log_r_Error("p_str_qid_path[%p]", p_str_qid_path);
        ret = -1;
        goto Err;
    }

    ifs.open(p_str_qid_path);
    if (!ifs.is_open()) {
        Log_r_Error("Unable to open file[%s]", p_str_qid_path);
        ret = -1;
        goto Err;
    }

    all_qid_list.clear();

    while (getline(ifs, qid)) {
        all_qid_list.insert(qid);
        Log_r_Info("qid:[%s]", qid.c_str());
    }



Err:
    if (ifs.is_open()) {
        ifs.close();
    }
    return ret;
}
    
int load_predict(const char * p_str_ins_path, int rank)
{
    int ret = 0;
    ifstream ifs;

    string line = "";
    string tempstr = "";
    string qid = "";
    char ins_path[2048];

    float pctr = 0.0;
    float non_clk = 0;
    float clk = 0;
    
    int id = 0;
    double diff = 0.0;

    set<string>::iterator it ;


    if (NULL == p_str_ins_path) {
        Log_r_Error("p_str_ins_path[%p]", p_str_ins_path);
        ret = -1;
        goto Err;
    }

    snprintf(ins_path, 2048, "%s-%05d", p_str_ins_path, rank);

    ifs.open(ins_path);
    if (!ifs.is_open()) {
        Log_r_Error("Unable to open file[%s]", ins_path);
        ifs.open(p_str_ins_path);
        if (!ifs.is_open()) {
            Log_r_Error("Unable to open file[%s]", p_str_ins_path);
            ret = -1;
            goto Err;
        }
    }

    clkinfo_list.clear();

    while (getline(ifs,line)) {

      //Log_r_Info("%s", line.c_str());
      // "%lf^A%f^A%f^Aqid^B******\n", 1.0*score, nonclk, clk, qid);
      int pos = line.find(CTRL_B);
      if (0 >= pos) {
        tempstr = line;
      } else {
        tempstr = line.substr(0, pos);
      }

      pos = tempstr.find(CTRL_A);
      if (0 >= pos) {
        Log_r_Warn("instance[%s]at pos[%d]is irrcorrect", 
            tempstr.c_str(), pos);
        continue;
      }
      pctr = atof(tempstr.substr(0, pos).c_str());
      tempstr = tempstr.substr(pos+1, tempstr.size()-pos-1);

      pos = tempstr.find(CTRL_A);
      if (0 >= pos) {
        Log_r_Warn("instance[%s]at pos[%d]is irrcorrect", 
            tempstr.c_str(), pos);
        continue;
      }
      non_clk = atof(tempstr.substr(0, pos).c_str());
      tempstr = tempstr.substr(pos+1, tempstr.size()-pos-1);

      pos = tempstr.find(CTRL_A);
      if (0 >= pos) {
        Log_r_Warn("instance[%s]at pos[%d]is irrcorrect", 
            tempstr.c_str(), pos);
        continue;
      }
      clk = atof(tempstr.substr(0, pos).c_str());
      tempstr = tempstr.substr(pos+1, tempstr.size()-pos-1);

      pos = tempstr.find(CTRL_A);
      if (0 >= pos) {
        Log_r_Warn("instance[%s]at pos[%d]is irrcorrect", 
            tempstr.c_str(), pos);
        continue;
      }
      qid = tempstr.substr(0, pos);

      if ((non_clk + clk) <=0 || non_clk < 0 || clk < 0 || pctr < 0 || pctr>1) {
        Log_r_Warn("[%s][%f][%f][%lf]", line.c_str(), clk, non_clk, pctr);
        continue;
      }

      id = int(pctr*MAX_ARRAY_SIZE);
      if (0 > id || MAX_ARRAY_SIZE < id) {
        Log_r_Warn("predict nline[%s]  of file[%s]fail",
            line.c_str(), p_str_ins_path);
        continue;
      }

      //防止溢出
      if (MAX_ARRAY_SIZE==id) {
        id--;
      }

      clkinfo _clkinfo;
      _clkinfo.non_clk = non_clk;
      _clkinfo.clk = clk;
      _clkinfo.idx = id;
      _clkinfo.qid = qid;

      //Log_r_Info("nonclk:%f clk:%f id:%d qid:%s", non_clk, clk, id, qid.c_str());

      clkinfo_list.push_back(_clkinfo);

      g_ins++;
      g_total_nonclk += non_clk;
      g_total_clk += clk;
      g_mean_ctr2 += clk/(non_clk+clk);
      g_mean_pctr2 += pctr; 
      diff = clk/(non_clk+clk) - pctr;
      g_mae += fabs(diff);
      g_mse += diff*diff;
    }
    Log_r_Info("rank:%d end process %s", rank, p_str_ins_path);

Err:
    if (ifs.is_open()) {
      ifs.close();
    }
    return ret;
}


int auc_cal(float * p_all_nclk, float * p_all_clk, double &auc_final, double& weight)
{ 
    int ret = 0;
    double clk_sum = 0;
    double old_clk_sum = 0;
    double no_clk_sum = 0;
    double clksum_multi_nclksum = 0;
    double auc = 0.0;
    auc_final = 0.0;
    weight = 0.0;

    if (NULL == p_all_nclk || NULL == p_all_clk) {
        ret = -1;
        Log_r_Error("p_all_nclk[%p]p_all_clk[%p]", 
                p_all_nclk, p_all_clk);
        goto Err;
    }

    for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
        old_clk_sum = clk_sum;
        clk_sum += p_all_clk[i];
        no_clk_sum += p_all_nclk[i];
        auc += (old_clk_sum + clk_sum)*p_all_nclk[i]/2;
    }

    clksum_multi_nclksum = clk_sum * no_clk_sum;
    weight =  clk_sum + no_clk_sum;
    if (0 > clksum_multi_nclksum) {
        ret = -1;
        Log_r_Error("clk_sum:%lf no_clk_sum:%lf", clk_sum, no_clk_sum);
        goto Err;
    } else if (0 == clksum_multi_nclksum) {
        auc_final = 1;
    } else {
        auc_final = 1 - auc/(clksum_multi_nclksum);
    }
    
    Log_r_Info("clk_sum[%lf] no_clk_sum[%lf] auc[%lf] weight[%lf]",
            clk_sum, no_clk_sum, auc_final, weight);

Err:
    return ret;
}


