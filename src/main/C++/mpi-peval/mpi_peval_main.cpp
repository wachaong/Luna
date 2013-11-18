/** 
 * @copyright (c) 2012 Taobao.com, Inc. All Rights Reserved
 * @author : huayong.ljx
 * @fax : +86.10.815.72625
 * @e-mail : huayong.ljx@taobao.com
 * @date : 2012-04-18 - 18:40
 * @version: 1.0.0.1
 * 
 * @file : mpi_pauc.cpp
 * @brief :
 */

#include "mpi_peval.h"
#define MAX_FILENAME_LEN 4096
using namespace std;

char g_str_logconf[1024] ="/home/a/share/phoenix/mpi-algo-platform/conf/log4cpp.conf";

int main (int argc, char * argv[])
{
    int ret = 0;
    int rank_id = 0;
    int num_procs = 0;
    
    double start_time = 0.0;
    double end_time = 0.0;
    char score_file[MAX_FILENAME_LEN];
    char qid_file[MAX_FILENAME_LEN];
	  char *auc_file = NULL;
    
    signal(SIGPIPE, SIG_IGN);
    MPI_Init(&argc,&argv);
    start_time = MPI_Wtime();
    MPI_Comm_size(MPI_COMM_WORLD,&num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank_id);
    if (argc < 3) {
        fprintf(stderr, "Usage: %s score_file qid_file [auc_file]\n", argv[0]);
        goto Err;
    }
    snprintf(score_file, sizeof(score_file), "%s", argv[1]);
    snprintf(qid_file, sizeof(qid_file), "%s", argv[2]);
    
    //是否把auc结果写到文件中
    if (argc == 4 && rank_id == MASTER_ID) {
        auc_file = (char *)malloc(MAX_FILENAME_LEN*sizeof(char));
        snprintf(auc_file, MAX_FILENAME_LEN, "%s", argv[3]);
    } else {
        auc_file = NULL;
    }
    fprintf(stderr, "score_file is [%s].\n", score_file);
    fprintf(stderr, "qid_file is [%s].\n", qid_file);
    if (auc_file != NULL) {
        fprintf(stderr, "auc_file is [%s].\n", auc_file);
    } else {
        fprintf(stderr, "auc_file is [stderr].\n");
    }
    
    if (Log_r::Init(g_str_logconf)) {
        ret = -1;
        fprintf(stderr, "log init fail str_logconf[%s]\n", g_str_logconf);
        goto Err;
    }
    Log_r_Info("Log_r::Init ok");

    if (load_predict(score_file, rank_id)) {
        ret = -1;
        Log_r_Fatal("load [%s] fail : [rank=%d]", argv[1], rank_id);
        goto Err;
    }
    Log_r_Info("load predict ok");

    if (load_qid(qid_file)) {
        ret = -1;
        Log_r_Fatal("load .data/score/qid fail : [rank=%d]", rank_id);
        goto Err;
    }

    if (mpi_peval(num_procs, rank_id, auc_file)) {
        ret = -1;
        Log_r_Fatal("auc_merge fail num_procs[%d]rank_id[%d]", 
                num_procs, rank_id);
        goto Err;
    }
    Log_r_Info("eval merge ok");

    end_time = MPI_Wtime();
    Log_r_Info("Process %d of %d that tooks %lf seconds", 
            rank_id, num_procs, end_time - start_time);

Err:
    if (ret) {
        Log_r_Fatal("mpi-peval fail");
    }
    MPI_Finalize();
    return ret;
}

