#include <errno.h>
#include <string>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "inslookup.h"
using namespace std;

map<unsigned int, int> feasign2id_map;
char feamap_path[2048];
char ins_path[2048];

int get_param(int argc, char *argv[])
{
	int ret = 0;
//	feasign2id_map = new map<uint64_t, int32_t>();
	snprintf(feamap_path, 2048, "%s", "feat");
	snprintf(ins_path, 2048, "%s", "ins");
	return ret;
}

int load_feamap(const char* feamap_path){
	unsigned int feasign = 0;
	size_t len = 0;
	ssize_t read;
	string line;
	int feaid = 0;
	ifstream pfeamap(feamap_path);
	
	if(!pfeamap.good()){
		cerr << "error feature map file" << endl;
		exit(1);
	}
	
	while (getline(pfeamap, line)){
		feasign = strtoul(line.c_str(), NULL, 16);
		feasign2id_map[feasign] = feaid;
		//cout << line << " " << feasign <<  endl;
		//cout << line.c_str();
		feaid++;
	}
	cout << feasign2id_map.size() << endl;
	printf("totla_feature:%u\n", feaid);
	return -1;
}

int32_t trans_ins(const char* ins_path){
	return -1;
}
