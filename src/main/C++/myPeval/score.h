#ifndef _INSLOOKUP_
#define _INSLOOKUP_

#include <math.h>
#include <map>
#include <deque>
#include <vector>

using namespace std;
const int MAX_BUF_LEN = 1024*20;
int init(const char* feamap, const char* model, const char* ins);
int load_feamap(const char* feamap_path);
int load_model(const char* model_path);
double cal_score(vector <size_t> instance);
double get_ctr(double score);
int score_ins(const char* score_path);
int getAdFeaCount();
int getUserFeaCount();
int getOtherFeaCount();
int getAllFeaCount();
#endif
