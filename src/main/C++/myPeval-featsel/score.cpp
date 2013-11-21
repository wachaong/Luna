/*
Load FeatureMap
Load Model
Load instance and eval
*/

#include <string.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "score.h"
#include <vector>
#include <algorithm>
#include <fcntl.h>
using namespace std;


map<unsigned int, int> *feasign2id_map;
typedef std::vector<double> DblVec;

DblVec W;
DblVec V;
DblVec P;

//feasign2id_map[0]	Ad featuremap 
//feasign2id_map[1] User featuremap
//feasign2id_map[2] Other featuremap

char feamap_path[2048];
char ins_path[2048];
char model_path[2048];



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
	pfeamap.close();
	return 0;
}

int load_model(const char* model_path){
	string line;
	char model_file_name[50];
	sprintf(model_file_name, "%sP", model_path);
	double fea_weight;
	
	//load P
	ifstream pmodel(model_file_name);
	if(!pmodel.good()){
		cerr << "error model file " << model_file_name << endl;
		exit(1);
	}
	while (getline(pmodel, line)){
    	fea_weight = atof(line.c_str());
    	P.push_back(fea_weight);
	}
	pmodel.close();
	
	
	//load W
	sprintf(model_file_name, "%sW", model_path);
	ifstream wmodel(model_file_name);
	if(!wmodel.good()){
		cerr << "error model file " << model_file_name  << endl;
		exit(1);
	}
	while (getline(wmodel, line)){
    	fea_weight = atof(line.c_str());
    	W.push_back(fea_weight);
	}
	wmodel.close();
	
	
	//load V
	sprintf(model_file_name, "%sV", model_path);
	ifstream vmodel(model_file_name);
	if(!vmodel.good()){
		cerr << "error model file " << model_file_name << endl;
		exit(1);
	}
	while (getline(vmodel, line)){
    	fea_weight = atof(line.c_str());
    	V.push_back(fea_weight);
	}
	vmodel.close();	
	return 0;
}

double cal_score(vector <size_t> instance){
	//f(x)=(UW)(TV)' + Px
	double score = 0.0;
	DblVec UW;
	DblVec TV;
	int dimLatent = W.size() / getUserFeaCount();
	for (size_t j = 0 ; j < dimLatent; j++){
		UW.push_back(0);
		TV.push_back(0);
	}
	for (size_t j = 0; j < instance.size(); j++){
		score += P[instance[j]] * 1.0;
		if(instance[j] < getAdFeaCount()){
			for(size_t k = 0; k < dimLatent; k++){
				int v_index = instance[j] * dimLatent + k;
				TV[k]+=V[v_index];
			}
		} 
		else if(instance[j] < getAdFeaCount() + getUserFeaCount()){
			for(size_t k = 0; k < dimLatent; k++){
				int w_index = (instance[j] - getAdFeaCount())*dimLatent + k;
				UW[k] += W[w_index];
			}
		}
	}
	
	for (size_t j = 0; j < dimLatent; j++){
		score += UW[j]*TV[j];
	}

	return score;
	
}

double get_ctr(double score) {
    double ctr = 1/(1 + exp(-score));
    return ctr;
}


int score_ins(const char* score_path){
	char insinput[50];
	sprintf(insinput, "%s", ins_path);
	ifstream fins(insinput);
	FILE* p_out = fopen(score_path, "w");
	char line[MAX_BUF_LEN];
	string linestr;
	const char CTRL_A = '';
	const char CTRL_B = '';
    const char END = '';
    
	
	char feasign[10];
	int feaid = 0;
	
	if(!fins.good()){
		cerr << "error instance file" << endl;
		exit(1);
	}

	
	while(getline(fins, linestr)){
		strcpy(line, linestr.c_str());
		line[linestr.size()] = END;
		
		vector <size_t> instance;
		size_t temp_nonclick = 0;
		size_t temp_click = 0;
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
		sscanf(p_end, "%u", &temp_nonclick);
		p_begin++;
		p_end = p_begin;
		
		//clk
		while(*p_begin != CTRL_A){
			p_begin++;
		}
		*p_begin = 0;
		sscanf(p_end, "%u", &temp_click);
		p_begin++;
		p_end = p_begin;
		
		bool bEnd = false;
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
				//add to instance list
				instance.push_back(feaid);
				
			}
			
			p_begin = p_end+1;
			
		}
		sort(&instance[0], &instance[instance.size()]);
		//score and output to file
		double score = cal_score(instance);
		cout << score << endl;
		double ctr = get_ctr(score);
		fprintf(p_out, "%lf%d%d%s\n", 1.0*ctr, temp_nonclick, temp_click, "Q");
	}
	delete []feasign2id_map;
	fins.close();
	fclose(p_out);
	return 0;				
}

int init(const char* feamap, const char* model, const char* ins)
{
	snprintf(feamap_path, 2048, "%s", feamap);
	snprintf(model_path, 2048, "%s", model);
	snprintf(ins_path, 2048, "%s", ins);
	feasign2id_map = new map<unsigned int, int>[3];
	load_feamap(feamap_path);
	load_model(model_path);
	return 0;
}
