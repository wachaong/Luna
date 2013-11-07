#include <iostream>
#include "inslookup.h"
#include <stdlib.h>
#include <string>

extern char feamap_path[2048];
extern char ins_path[2048];
int main(int argc, char** argv) {
	get_param(argc, argv);
	load_feamap(feamap_path);
	trans_ins(ins_path);
	return 0;
}
