#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <math.h>


#include <map>
#include <mpi.h>

#include "Log_r.h"

using namespace std;

const uint32_t END = -1;
char g_str_logconf[1024] ="/home/a/share/phoenix/mpi-algo-platform/conf/log4cpp.conf";

int open_inst(const char * p_filename, uint32_t ** p_buf, size_t * p_inst_len)
{
    if(NULL == p_filename || NULL == p_buf || NULL == p_inst_len){
        Log_r_Error("NULL params\n");
        return -1;
    }

    int fd = open(p_filename, O_RDONLY);
    if(0 > fd){
        return -1;
    }
    struct stat statbuf;
    if(0 > fstat(fd, &statbuf)){
        while((0 > close(fd)) && (EINTR == errno));
        return -1;
    }

    if((0 >= statbuf.st_size) 
            || (0 != (statbuf.st_size % sizeof(uint32_t)))){
        while((0 > close(fd)) && (EINTR == errno));
        return -1;
    }

    *p_buf = (uint32_t *)mmap(0, 
            statbuf.st_size, 
            PROT_READ, 
            MAP_FILE | MAP_SHARED | MAP_NORESERVE, 
            fd, 0);
    if(MAP_FAILED == (void *)(*p_buf)){
        while((0 > close(fd)) && (EINTR == errno));
        return -1;
    }   
    *p_inst_len = statbuf.st_size;

    while((0 > close(fd)) && (EINTR == errno));    

    return 0;
}

int load_model(FILE * p_model, map<int, float> * p_map) {
    if(NULL == p_model || NULL == p_map) {
        Log_r_Error("NULL params.[model:%p][map:%p]\n",
                p_model, p_map);
        return -1;
    }
    char buf[1024];
    buf[0] = '\0';
    while(NULL != fgets(buf, sizeof(buf), p_model)) {
        char * p_idx = strchr(buf, '\t');
        if(NULL == p_idx) {
            Log_r_Error("Invalid model line : %s\n", buf);
            return -1;
        }
        *p_idx = '\0';
        int fea_idx = atoi(buf);
        float fea_weight = atof(p_idx + 1);
        p_map->insert(pair<int, float>(fea_idx, fea_weight));
    }
    return 0;
}

float get_ctr(double score) {
    float ctr = 1/(1 + exp(-score));
    return ctr;
}

int score(uint32_t * p_ins, size_t inst_len, map<int,float>* p_model, FILE * p_out) {
    if(NULL == p_ins || NULL == p_model || NULL == p_out) {
        Log_r_Error("NULL params.[ins:%p][model:%p][out:%p]\n", 
                p_ins, p_model, p_out);
        return -1;
    }
    uint32_t * p_end = p_ins + inst_len;
    uint32_t * p_share_beg = p_ins;
    uint32_t * p_share_end = NULL;
    uint32_t * p_exc_beg = NULL;
    uint32_t * p_exc_end = NULL;
    map<int,float>::iterator iter; 
    while(p_share_beg < p_end) {
        uint32_t share_dis_num = *p_share_beg;
        uint32_t share_con_num = *(p_share_beg + share_dis_num + 2);
        p_share_end = p_share_beg + share_dis_num + 2 * share_con_num + 3;
        if(p_share_end >= p_end) {
            Log_r_Error("[%d] out of range.\n", __LINE__);
            return -1;
        }
        uint32_t * p_exc_iter = p_share_end;
        do{
            p_exc_beg = p_exc_iter + 1;
            uint32_t non_clk = *p_exc_beg; 
            uint32_t clk = *(p_exc_beg + 1);
            uint32_t exc_dis_num = *(p_exc_beg + 2);
            uint32_t exc_con_num = *(p_exc_beg + exc_dis_num + 4);
            p_exc_end = p_exc_beg + exc_dis_num + 2 * exc_con_num + 5;
            if(p_exc_end >= p_end) {
                Log_r_Error("[%d] out of range.\n", __LINE__);
                return -1;
            }
            double score = 0.0;
            // share part
            uint32_t * p_idx = p_share_beg + 1;
            for(uint32_t offset = 0; offset < share_dis_num ; ++offset) {
                uint32_t fea_idx = *(p_idx + offset);
                iter = p_model->find(fea_idx);
                if(p_model->end() != iter) {
                    score += iter->second;
                }
            } // share discrete 
            p_idx = p_share_beg + share_dis_num + 3;
            for(uint32_t offset = 0; offset < share_con_num ; ++offset) {
                uint32_t fea_idx = *(p_idx + 2 * offset + 1);
                float fea_val = *((float*)(p_idx + 2 * offset));
                iter = p_model->find(fea_idx);
                if(p_model->end() != iter) {
                    score += (iter->second) * fea_val;
                }
            } // share continous
            p_idx = p_exc_beg + 3;
            for(uint32_t offset = 0; offset < exc_dis_num ; ++offset) {
                uint32_t fea_idx = *(p_idx + offset);
                iter = p_model->find(fea_idx);
                if(p_model->end() != iter) {
                    score += iter->second;
                }
            } // exclusive discrete
            p_idx = p_exc_beg + exc_dis_num + 5;
            for(uint32_t offset = 0; offset < exc_con_num ; ++offset) {
                uint32_t fea_idx = *(p_idx + 2 * offset + 1);
                float fea_val = *((float*)(p_idx + 2 * offset));
                iter = p_model->find(fea_idx);
                if(p_model->end() != iter) {
                    score += (iter->second) * fea_val;
                }
            } // exclusive continous
            float ctr = get_ctr(score);
            fprintf(p_out, "%lf%lf%lf%s\n", 1.0*ctr, 1.0*non_clk, 1.0*clk, "Q");
            p_exc_iter = p_exc_end;
        }while(END != *(p_exc_end + 1));
        p_share_beg = p_exc_end + 2;
    }

    return 0;
}

int main(int argc, char *argv[]) {

    int rank_id = 0;
    int num_procs = 0;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank_id);
    if(3 >= argc) {
        fprintf(stderr, "Args : model instance output.\n");
		MPI_Finalize();
        return -1;
    }
    if (Log_r::Init(g_str_logconf)) {
        fprintf(stderr, "log init fail str_logconf[%s]\n", g_str_logconf);
		MPI_Finalize();
        return -1;
    }
    Log_r_Info("Log_r::Init ok");


    Log_r_Info("argv[1]:%s\n", argv[1]);
    Log_r_Info("argv[2]:%s\n", argv[2]);
    Log_r_Info("argv[3]:%s\n", argv[3]);


    FILE * p_model_file = fopen(argv[1], "r");
    if(NULL == p_model_file) {
        Log_r_Error("open model filename failed.\n");
		MPI_Finalize();
        return -1;
    }
    map<int,float> * p_model_map = new(std::nothrow) map<int, float>();
    if(NULL == p_model_map) {
        Log_r_Error("new map failed.\n");
		MPI_Finalize();
        return -1;
    }

    Log_r_Info("[Rank:%d] load model started.\n", rank_id);
    if(load_model(p_model_file, p_model_map)) {
        Log_r_Error("load model failed.\n");
		MPI_Finalize();
        return -1;
    }
    Log_r_Info("[Rank:%d] load model finished.\n", rank_id);

    uint32_t * p_inst = NULL;
    size_t inst_len = 0;
    char inst_file[1024];
    inst_file[0] = '\0';
    snprintf(inst_file, sizeof(inst_file), "%s-%05d", argv[2], rank_id);

    Log_r_Info("Rank[%d][Eval Ins:%s]\n", rank_id, inst_file);
    if(open_inst(inst_file, &p_inst, &inst_len)) {
        Log_r_Error("open instance file failed.\n");
		MPI_Finalize();
        return -1;
    }
    FILE * p_score_file = fopen(argv[3], "w");
    if(NULL == p_score_file) {
        Log_r_Error("open score file [%s] failed.\n", argv[3]);
		MPI_Finalize();
        return -1;
    }

    Log_r_Info("[Rank:%d] start to score.\n", rank_id);
    if(score(p_inst, inst_len/sizeof(uint32_t), p_model_map, p_score_file)) {
        Log_r_Error("score failed.\n");
		MPI_Finalize();
        return -1;
    }
    Log_r_Info("[Rank:%d] finish score.\n", rank_id);

    delete p_model_map;
    p_model_map = NULL;

    fclose(p_model_file);
    fclose(p_score_file);

    if(NULL != p_inst) {
        munmap((void*)p_inst, inst_len);
        p_inst = NULL;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
