#include <string>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "inslookup.h"
using namespace std;

map<unsigned int, int> userfeasign2id_map;
map<unsigned int, int> adfeasign2id_map;
map<unsigned int, int> otherfeasign2id_map;


char feamap_path[2048];
char ins_path[2048];

int get_param(int argc, char *argv[])
{
	snprintf(feamap_path, 2048, "%s", "feat");
	snprintf(ins_path, 2048, "%s", "ins");
	return 0;
}

	
	/*
    * Get feature Type
    * AdFeature: a 		return 0
    * UserFeature: u 	return 1 
    * OtherFeature: o 	return 2
    */
    
int get_feature(char* fsign){
	//User feature
	if()
}

int load_feamap(const char* feamap_path){
	unsigned int feasign = 0;
	string line;
	int feaid = 0;
	ifstream pfeamap(feamap_path);
	
	
	if(!pfeamap.good()){
		cerr << "error feature map file" << endl;
		exit(1);
	}
	
	while (getline(pfeamap, line)){
		get_feature(line.c_str());
		feasign = strtoul(line.c_str(), NULL, 16);
		feasign2id_map[feasign] = feaid;
		feaid++;
	}
	cout << feasign2id_map.size() << endl;
	cout << "total_feature:" << feaid << "\n";
	return 0;
}

int trans_ins(const char* ins_path){
	ifstream fins(ins_path);
	
	if(!fins.good()){
		cerr << "error instance file" << endl;
		exit(1);
	}
	
	while(getline(fins, line)){
		//instance parser
		//Get each instance 
		
	}
	
	return 0;
}
