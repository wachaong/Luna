#ifndef _INSLOOKUP_
#define _INSLOOKUP_

#include <math.h>
#include <map>
#include <deque>

using namespace std;
const int MAX_BUF_LEN = 1024*20;
int init();
int	trans_ins(const char* ins_path, size_t rankid, std::deque<size_t>& indices, std::deque<float>& values, std::deque<size_t>& instance_starts, std::deque<bool>& labels, size_t& numFeats);
int load_feamap(const char* feamap_path);
int getAdFeaCount();
int getUserFeaCount();
int getOtherFeaCount();
int getAllFeaCount();
#endif
