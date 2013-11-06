#ifndef _INSLOOKUP_
#define _INSLOOKUP_

#include <math.h>
#include <map>

using namespace std;

const int MAX_BUF_LEN = 1024*20;

int get_param(int argc, char *argv[]);
int trans_ins(const char* ins_path);
int load_feamap(const char* feamap_path);

#endif
