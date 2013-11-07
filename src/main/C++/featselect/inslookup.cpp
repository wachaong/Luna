#include <string.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "inslookup.h"
#include <vector>
#include <algorithm>
using namespace std;

map<unsigned int, int> *feasign2id_map;

//feasign2id_map[0]	Ad featuremap 
//feasign2id_map[1] User featuremap
//feasign2id_map[2] Other featuremap

char feamap_path[2048];
char ins_path[2048];

int init()
{
	snprintf(feamap_path, 2048, "%s", "feat");
	snprintf(ins_path, 2048, "%s", "ins");
	feasign2id_map = new map<unsigned int, int>[3];
	return 0;
}

int getAdFeaCount(){
	return feasign2id_map[0].size();
}
int getUserFeaCount(){
	return feasign2id_map[1].size();
}
int getOtherFeaCount(){
	return feasign2id_map[2].size();
}
int getAllFeaCount(){
	return feasign2id_map[0].size() + feasign2id_map[1].size() + feasign2id_map[2].size();
}	
	/*
    * Get feature Type
    * AdFeature: a 		return 0
    * UserFeature: u 	return 1 
    * OtherFeature: o 	return 2
    */
    
int get_feature(const char* fsign, unsigned int& feasign){
	feasign = strtoul(fsign+1, NULL, 16);
	if(fsign[0] == 'a'){
		return 0;
	}
	else if(fsign[0] == 'u'){
		return 1;
	}
	else{
		return 2;
	}
}

// FEATUREIDLIST: 
//	AD FEATURE[0 - feaid[0]-1], 
//	USER FEATURE[feaid[0] - (feaid[1] + feaid[0] - 1)], 
//	OTHER FEATURE[(feaid[1]+feaid[0]) - (feaid[0] + feaid[1] +feaid[2] -1)]
		
int get_featid(const char* fsign){
	unsigned int feasign = 0;
	int type = get_feature(fsign, feasign);
	int featid = 0;
	if(type == 0) {
		featid = feasign2id_map[type][feasign];
	}
	else if(type == 1){
		featid = getAdFeaCount() + feasign2id_map[type][feasign];
	}
	else{
	//	cout << getAdFeaCount()<< endl;
	//	cout << getUserFeaCount() << endl;
	//	cout << feasign2id_map[type][feasign] << endl;
		featid = getAdFeaCount() + getUserFeaCount() + feasign2id_map[type][feasign];
	}
	return featid;
}

int load_feamap(const char* feamap_path){
	unsigned int feasign = 0;
	string line;
	int feaid[3];
	feaid[0] = 0; feaid[1] = 0; feaid[2] = 0;
	ifstream pfeamap(feamap_path);
	
	
	if(!pfeamap.good()){
		cerr << "error feature map file" << endl;
		exit(1);
	}
	
	while (getline(pfeamap, line)){
		int type = get_feature(line.c_str(), feasign);
		feasign2id_map[type][feasign] = feaid[type];
		feaid[type]++;
	}
	cout << "Ad Feature: "<<getAdFeaCount() << endl;
	cout << "User Feature: "<<getUserFeaCount() << endl;
	cout << "Other Feature: "<<getOtherFeaCount() << endl;
	

	cout << "Total Feature: " << getAllFeaCount() << "\n";
	return 0;
}




int trans_ins(const char* ins_path){
	ifstream fins(ins_path);
	
	char line[MAX_BUF_LEN];
	string linestr;
	const char CTRL_A = '';
	const char CTRL_B = '';
    const char END = '\n';
    
	int clk = 0;
	int nonclk = 0;
	char feasign[10];
	int feaid = 0;
	
	if(!fins.good()){
		cerr << "error instance file" << endl;
		exit(1);
	}

	while(getline(fins, linestr)){
		strcpy(line, linestr.c_str());
		line[linestr.size()] = END;
		
		vector<int> instance;
		// Instance parser
		// Get each instance 
		// CLICK_NUM/NONCLICK_NUM/FEATUREIDLIST
		// FEATUREIDLIST: 
		//	AD FEATURE[0 - feaid[0]-1], 
		//	USER FEATURE[feaid[0] - (feaid[1] + feaid[0] - 1)], 
		//	OTHER FEATURE[(feaid[1]+feaid[0]) - (feaid[0] + feaid[1] +feaid[2] -1)]
		
		
		char *p_begin = line;
		char *p_end = p_begin;
		char *p_fea;
		//nonclick
		while(*p_begin != CTRL_A){
			p_begin ++;
		}
		*p_begin=0;
		sscanf(p_end, "%d", &nonclk);
		p_begin++;
		p_end = p_begin;
		
		//clk
		while(*p_begin != CTRL_A){
			p_begin++;
		}
		*p_begin = 0;
		sscanf(p_end, "%d", &clk);
		p_begin++;
		p_end = p_begin;
		
		bool bEnd = false;
		cout << clk << "x" << nonclk << "\t";
		while(!bEnd){
			p_end = p_begin;
			while(*p_end != CTRL_A && *p_end != END){
				p_end++;
			}
			if(*p_end == END){
				bEnd = true;
			}
			
			if(*p_end == CTRL_A || *p_end == END){
				p_fea = p_begin;
				*p_end = 0;
				sscanf(p_fea, "%s", feasign);
				
				//getfeaid
				feaid = get_featid(feasign);
				//cout << feaid << " ";
				//add to instance list
				instance.push_back(feaid);
				
			}
			
			p_begin = p_end+1;
			
		}
		sort(&instance[0], &instance[instance.size()]);
		for(int i = 0; i < instance.size(); i++)
			cout << instance[i] << " ";
		cout << endl;
	//	break;
	}
	return 0;
}
