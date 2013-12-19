

#ifndef LR4CTR_PREDICTION_H
#define LR4CTR_PREDICTION_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>
#include <dirent.h>
#include <sys/types.h>
#include <regex.h>
#include <iconv.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <map>
#include <set>

#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>

#include "Log_r.h"
#include "AlgoConfig.h"

using namespace std;

#define CTRL_A ""
#define CTRL_B ""
#define COMMA ","
#define AUC_VERSION "1.0.0"
#define BUILD_DATE "(2012-04-13)"

#define MASTER_ID (0)
#define MPI_NON_CLK_TAG (92)
#define MPI_CLK_TAG (93)

const int MAX_PATH_SIZE = 1024;
const int MAX_BUF_SIZE = 2048;
const int MAX_ARRAY_SIZE = 1024*1024;

extern double g_total_clk;
extern double g_total_nonclk;
extern double g_mean_ctr1;
extern double g_mean_ctr2;
extern double g_mean_pctr2;
extern int64_t g_ins;
extern double g_mse;
extern double g_mae;
extern double g_nll;


typedef struct {
    float clk;
    float non_clk;
    int idx;
    string qid;
} clkinfo;


extern vector<clkinfo> clkinfo_list;
extern set<string> all_qid_list;

/**
 * @brief load predict file
 *
 * @param p_str_ins_path
 * @param p_non_clk
 * @param p_clk
 * @param rank
 *
 * @return 
 */
int load_predict(const char * p_str_ins_path, int rank);


int load_qid(const char * p_str_qid_path);


/**
 * @brief calculate auc
 *
 * @param p_all_nclick all no click array
 * @param p_all_click all click array
 * @param auc output auc res
 *
 * @return
 */
int auc_cal(float * p_all_nclick, float * p_all_click, double& auc, double& weight);

#endif
