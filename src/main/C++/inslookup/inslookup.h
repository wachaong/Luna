#ifndef _INSLOOKUP_
#define _INSLOOKUP_

#include <math.h>
#include <map>

using namespace std;

const int MAX_BUF_LEN = 1024*20;

int get_param(int argc, char *argv[]);
int trans_ins(const char* ins_path);
int load_feamap(const char* feamap_path);
/*
inline int32_t ltob(int32_t LittleEndian)
{
	unsigned char ch1 = (LittleEndian >> 24) & 0xff;
	unsigned char ch2 = (LittleEndian >> 16) & 0xff;
	unsigned char ch3 = (LittleEndian >> 8) & 0xff;
	unsigned char ch4 = LittleEndian & 0xff;
	
	int32_t BigEndian = ch4;
	BigEndian = (BigEndian<<8) + ch3;
	BigEndian = (BigEndian<<8) + ch2;
	BigEndian = (BigEndian<<8) + ch1;
	return BigEndian;
}
*/
#endif
