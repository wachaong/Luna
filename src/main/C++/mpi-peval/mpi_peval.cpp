#include "mpi_peval.h"
#include "prediction.h"

using namespace std;

float g_all_non_clk[MAX_ARRAY_SIZE];
float g_all_clk[MAX_ARRAY_SIZE];

float g_non_clk[MAX_ARRAY_SIZE];
float g_clk[MAX_ARRAY_SIZE];

int merge_clk_info(string qid)
{
    bzero(g_non_clk, MAX_ARRAY_SIZE*sizeof(float));
    bzero(g_clk, MAX_ARRAY_SIZE*sizeof(float));

    int cnt = clkinfo_list.size();
    for (int i = 0; i < cnt ; i++) {
        //Log_r_Info("i:%d [%s][%ld]:[%s][%ld]", i,
                //clkinfo_list[i].qid.c_str(),  clkinfo_list[i].qid.length(),
                //qid.c_str(), qid.length());

        if (clkinfo_list[i].qid==qid || qid =="") {
            int idx = clkinfo_list[i].idx;
            g_non_clk[idx] += clkinfo_list[i].non_clk;
            g_clk[idx] += clkinfo_list[i].clk;
            //Log_r_Info("%s:%f:%f", clkinfo_list[i].qid.c_str(), clkinfo_list[i].non_clk, clkinfo_list[i].clk);
        }
    }

    return 0;
}

int mpi_auc (int num_procs, int rank_id, double& auc, double& weight)
{
    MPI_Status status;
    int ret = 0;

    if (MASTER_ID != rank_id) {
        if (MPI_Send(g_non_clk, MAX_ARRAY_SIZE, MPI_FLOAT, MASTER_ID, MPI_NON_CLK_TAG, MPI_COMM_WORLD)) {
            ret = -1;
            Log_r_Error("send p_non_clk fail num_procs[%d] rank_id[%d]", 
                    num_procs, rank_id);
            goto Err;
        }

        if (MPI_Send(g_clk, MAX_ARRAY_SIZE, MPI_FLOAT, MASTER_ID, MPI_CLK_TAG, MPI_COMM_WORLD)) {
            ret = -1;
            Log_r_Error("send p_clk fail num_procs[%d] rank_id[%d]", 
                    num_procs, rank_id);
            goto Err;
        }

    } else {
        for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
            g_all_non_clk[i] = g_non_clk[i];
            g_all_clk[i] = g_clk[i];
        }

        for (int i = 1; i < num_procs; i++) {
            if (MPI_Recv(g_non_clk, MAX_ARRAY_SIZE, MPI_FLOAT, i, MPI_NON_CLK_TAG, 
                        MPI_COMM_WORLD, &status) ) {
                ret = -1;
                Log_r_Error("source[%d] with tag[%d] error[%d]\n", 
                        status.MPI_SOURCE, status.MPI_TAG, status.MPI_ERROR);
                goto Err;
            }
           
            if (MPI_Recv(g_clk, MAX_ARRAY_SIZE, MPI_FLOAT, i, MPI_CLK_TAG,
                        MPI_COMM_WORLD, &status)) {
                ret = -1;
                Log_r_Error("source[%d] with tag[%d] error[%d]\n", 
                        status.MPI_SOURCE, status.MPI_TAG, status.MPI_ERROR);
                goto Err;
            }
            
            for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
                g_all_non_clk[i] += g_non_clk[i];
                g_all_clk[i] += g_clk[i];
            }
        }

        if (auc_cal(g_all_non_clk, g_all_clk, auc, weight)) {
            ret = -1;
            Log_r_Error("auc_cal fail");
            goto Err;
        }
    }

Err:
    return ret;
}


int mpi_peval (int num_procs, int rank_id, char *auc_file)
{
    int ret = 0;
    int64_t total_ins = 0;
    double total_clk = 0.0;
    double total_nonclk = 0.0;
    double mean_ctr1 = 0.0;
    double mean_ctr2 = 0.0;
    double mean_pctr2 = 0.0;
    double mse = 0.0;
    double mae = 0.0;
    double auc = 0.0;
    double weight = 0.0;
    double total_weight = 0.0;
    double total_auc = 0.0;
    FILE *fp = NULL;
    set<string>::iterator it = all_qid_list.begin();

    MPI_Allreduce(&g_total_clk, &total_clk,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);
    MPI_Allreduce(&g_total_nonclk, &total_nonclk,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);
    MPI_Allreduce(&g_mean_ctr2, &mean_ctr2, 1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);
    MPI_Allreduce(&g_mean_pctr2, &mean_pctr2, 1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);
    
    
    MPI_Allreduce(&g_mse, &mse,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);
    MPI_Allreduce(&g_mae, &mae,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);
    MPI_Allreduce(&g_ins, &total_ins,1,MPI_LONG,MPI_SUM,MPI_COMM_WORLD);

    if (mse<0 || mae<0 || total_ins<0) {
        ret = -1;
        Log_r_Error("mse:%lf mae:%lf total_ins:%ld", mse, mae, total_ins);
        goto Err;
    }
    
    // cal mse mae info
    if (MASTER_ID == rank_id) {
        mse = mse/total_ins;
        mae = mae/total_ins;
        mean_ctr1 = total_clk/(total_clk + total_nonclk);
        mean_ctr2 = mean_ctr2/total_ins;
        mean_pctr2 = mean_pctr2/total_ins;

        printf("total_Clk=%lf\n", total_clk);
        printf("total_nonClk=%lf\n", total_nonclk);
        printf("total_ins=%ld\n", total_ins);
        
        printf("MEAN_CTR=%lf(total_clk/total_pv)\n", mean_ctr1);
        printf("MEAN_CTR=%lf(1/n sum(ctr))\n", mean_ctr2);
        printf("MEAN_PCTR=%lf(1/n sum(pctr))\n", mean_pctr2);
        
        printf("MSE=%lf\n", mse);
        printf("MAE=%lf\n", mae);

        Log_r_Info("total_Clk=%lf", total_clk);
        Log_r_Info("total_nonClk=%lf", total_nonclk);
        Log_r_Info("total_ins=%ld", total_ins);
        
        Log_r_Info("MEAN_CTR=%lf(total_clk/total_pv)", mean_ctr1);
        Log_r_Info("MEAN_CTR=%lf(1/n sum(ctr))", mean_ctr2);
        Log_r_Info("MEAN_PCTR=%lf(1/n sum(pctr))", mean_pctr2);

        Log_r_Info("MSE=%lf", mse);
        Log_r_Info("MAE=%lf", mae);
        if (auc_file != NULL) {
            fp = fopen(auc_file, "a+");
            if (fp != NULL) {
                fprintf(fp, "total_Clk=%lf\n", total_clk); 
                fprintf(fp, "total_nonClk=%lf\n", total_nonclk);
                fprintf(fp, "total_ins=%ld\n", total_ins);
                
                fprintf(fp, "MEAN_CTR=%lf(total_clk/total_pv)\n", mean_ctr1);
                fprintf(fp, "MEAN_CTR=%lf(1/n sum(ctr))\n", mean_ctr2);
                fprintf(fp, "MEAN_PCTR=%lf(1/n sum(pctr))\n", mean_pctr2);
                
                fprintf(fp, "MSE=%lf\n", mse);
                fprintf(fp, "MAE=%lf\n", mae);
            }
        }
    }

    if (merge_clk_info("")) {
        ret = -1;
        Log_r_Error("merge clk info all fail");
        goto Err;
    }

    // cal auc info
    if (mpi_auc(num_procs, rank_id, auc, weight)) {
        ret = -1;
        Log_r_Error("cal auc fail");
        goto Err;
    }

    if (MASTER_ID == rank_id) {
        printf("AUC=%lf\n", auc);
        if (fp != NULL) {
            fprintf(fp, "AUC=%lf\n", auc);
        }
        Log_r_Info("auc=%lf weight:%lf", auc, weight);
    }


    total_auc = 0.0;
    total_weight = 0.0;
    for (; it != all_qid_list.end(); ++it) {

        if (merge_clk_info(*it)) {
            ret = -1;
            Log_r_Error("%s merge clk info all fail", (*it).c_str());
            goto Err;
        }

        // cal auc info
        auc = 0.0;
        weight = 0.0;
        if (mpi_auc(num_procs, rank_id, auc, weight)) {
            ret = -1;
            Log_r_Error("%s cal auc fail", (*it).c_str());
            goto Err;
        }
        Log_r_Info("%s:%lf:%lf", (*it).c_str(), auc, weight);

        total_auc += auc*weight;
        total_weight += weight;
    }

    if (MASTER_ID == rank_id) {
        if(total_weight<=0) {
            ret = -1;
            Log_r_Error("total_weight : %lf", total_weight);
            goto Err;
        }
        printf("WAUC=%lf\n", total_auc/total_weight);
        if (fp != NULL) {
            fprintf(fp, "WAUC=%lf\n", total_auc/total_weight);
        }
        Log_r_Info("total_auc=%lf total_weight=%lf wauc=%lf", 
                total_auc, total_weight, total_auc/total_weight);
    }

Err:
    if (fp != NULL) {
      fclose(fp);
    }
    return ret;
}

