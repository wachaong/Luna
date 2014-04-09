#ifndef _INSLOOKUP_
#define _INSLOOKUP_

#include <math.h>
#include <map>
#include <deque>

using namespace std;

const int MAX_BUF_LEN = 1024*20;

int init_inslookup(const char*instance_file, const char* feature_file);
int trans_ins(const char* ins_path, size_t rankid, std::deque<size_t>& features, 
			std::deque<size_t>& instance_starts, 
			std::deque<size_t>& nonClk, std::deque<size_t>& Clk, size_t& numInstance,
			std::deque<size_t>& rp_features, std::deque<size_t>& rp_instance_starts);	
int load_feamap(const char* feamap_path);
void load_randmat_for_user(const char* randmat_path_user);
void load_randmat_for_ad(const char* randmat_path_ad);
int getAdFeaCount();
int getRpAdFeaCount();
int getUserFeaCount();
int getRpUserFeaCount();
int getOtherFeaCount();
int getAllFeaCount();
int getRpAllFeaCount();
#endif
