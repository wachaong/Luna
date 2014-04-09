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

vector<string> featureNameUser;
vector<string> featureNameAd;
vector<double>* randMatrixForUser;
vector<double>* randMatrixForAd;

//feasign2id_map[0]	Ad featuremap
//feasign2id_map[1] User featuremap
//feasign2id_map[2] Other featuremap

char feamap_path[2048];
char ins_path[2048];
char randmat_path_user[2048];
char randmat_path_ad[2048];
int rpAdFeaCount = 0;
int rpUserFeaCount = 0;
int init_inslookup(const char*instance_file, const char* feature_file, const char* randmat_file_user, const char* randmat_file_ad)
{
	snprintf(feamap_path, 2048, "%s", feature_file);
	snprintf(ins_path, 2048, "%s", instance_file);
	snprintf(randmat_path_user, 2048, "%s", randmat_file_user);
	snprintf(randmat_path_ad, 2048, "%s", randmat_file_ad);
	feasign2id_map = new map<unsigned int, int>[3];
	return 0;
}

int getAdFeaCount(){
	return feasign2id_map[0].size();
}
int getRpAdFeaCount(){
	return rpAdFeaCount;
}
int getUserFeaCount(){
	return feasign2id_map[1].size();
}
int getRpUserFeaCount(){
	return rpUserFeaCount;
}
int getOtherFeaCount(){
	return feasign2id_map[2].size();
}
int getAllFeaCount(){
	return feasign2id_map[0].size() + feasign2id_map[1].size() + feasign2id_map[2].size();
}
int getRpAllFeaCount(){
	return rpAdFeaCount+rpUserFeaCount;
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
		featid = getAdFeaCount() + getUserFeaCount() + feasign2id_map[type][feasign];
	}
	return featid;
}

int load_feamap(const char* feamap_path){
	unsigned int feasign = 0;
	string line;
	int feaid[3];
	feaid[0] = 1; feaid[1] = 1; feaid[2] = 0;
	ifstream pfeamap(feamap_path);
	if(!pfeamap.good()){
		cerr << "error feature map file" << endl;
		exit(1);
	}

	while (getline(pfeamap, line)){
		int type = get_feature(line.c_str(), feasign);
		//ad
		if(type == 0){
			featureNameAd.push_back(line.substr(line.find('\t')+1));
		}
		//user
		else if(type == 1){
			featureNameUser.push_back(line.substr(line.find('\t')+1));
		}
		feasign2id_map[type][feasign] = feaid[type];
		feaid[type]++;
	}
	return 0;
}

vector<double> split(string str, string pattern){
	int pos;
	vector<double> result;
	str+=pattern;
	int size = str.size();
	for(int i = 0; i < size; i++){
		pos = str.find(pattern, i);
		if(pos< size){
			string s = str.substr(i, pos-i);
			result.push_back(atof(s));
			i = pos+pattern.size() -1;
		}
	}
	return result;
}

void load_randmat_for_user(const char* randmat_path_user){
	string line;
	ifstream pfeamap(randmat_path_user);
	if(!pfeamap.good()){
		cerr << "error rand mat file for user" << endl;
		exit(1);
	}
	getline(pfeamap, line);
	int pos = line.find("X");
	int rownum = atoi(line.substr(0, pos).c_str());
	int colnum = atoi(line.substr(pos+1, line.length()-pos-1).c_str());
	randMatrixForUser = new vector<double>[rownum];
	int i = 0;
	while (getline(pfeamap, line)){
		randMatrixForUser[i++] = split(line, "\t");
	}
	pfeamap.close();
	rpUserFeaCount = rownum;
}

void load_randmat_for_ad(const char* randmat_path_ad){
	string line;
	ifstream pfeamap(randmat_path_ad);
	if(!pfeamap.good()){
		cerr << "error rand mat file for ad" << endl;
		exit(1);
	}
	getline(pfeamap, line);
	int pos = line.find("X");
	int rownum = atoi(line.substr(0, pos).c_str());
	int colnum = atoi(line.substr(pos+1, line.length()-pos-1).c_str());
	randMatrixForAd = new vector<double>[rownum];
	int i = 0;
	while (getline(pfeamap, line)){
		randMatrixForAd[i++] = split(line, "\t");
	}
	pfeamap.close();
	rpAdFeaCount = rownum;
}


vector<int> get_rp_instance(const vector<int>& instance){
	vector<int> rpInstance;
	for(int i = 0; i < rpAdFeaCount; i++){
		double sum = 0;
		for(int k = 0; k < instance.size(); k++){
			if(instance[k] < getAdFeaCount()){
				sum += randMatrixForAd[i][instance[k]];
			}
		}
		if(sum > 0) rpInstance.push_back(i);
	}
	for(int i = 0; i < rpUserFeaCount; i++){
		double sum = 0;
		for(int k = 0; k < instance.size(); k++){
			if(instance[k] >= getAdFeaCount() && instance[k] < getAdFeaCount() + getUserFeaCount()){
				sum += randMatrixForUser[i][instance[k]-getAdFeaCount()];
			}
		}
		if(sum > 0) rpInstance.push_back(i+rpAdFeaCount);
	}
	return rpInstance;
}


/*
		// Instance parser
		// Get each instance
		// CLICK_NUM/NONCLICK_NUM/FEATUREIDLIST
		// FEATUREIDLIST:
		//	AD FEATURE[0 - feaid[0]-1],
		//	USER FEATURE[feaid[0] - (feaid[1] + feaid[0] - 1)],
		//	OTHER FEATURE[(feaid[1]+feaid[0]) - (feaid[0] + feaid[1] +feaid[2] -1)]

*/
int trans_ins(const char* ins_path, size_t rankid,
			std::deque<size_t>& features,
			std::deque<size_t>& instance_starts,
			std::deque<size_t>& nonClkQ,
			std::deque<size_t>& ClkQ,
			size_t& numInstance,
			std::deque<size_t>& rpFeatures,
			std::deque<size_t>& rpInstance_starts){
	char insinput[50];
	char insoutput[50];
	sprintf(insinput, "%s-%05d", ins_path, rankid);
	ifstream fins(insinput);
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
	instance_starts.push_back(0);
	rpInstance_starts.push_back(0);
	numInstance = 0;
	while(getline(fins, linestr)){
		strcpy(line, linestr.c_str());
		line[linestr.size()] = 0;
		vector<int> instance;
		size_t temp_nonclick = 0;
		size_t temp_click = 0;
		char *p_begin = line;
		char *p_end = p_begin;
		char *p_fea = p_begin;
		while(*p_begin != CTRL_A){
			p_begin ++;
		}
		*p_begin=0;
		sscanf(p_end, "%u", &temp_nonclick);
		p_begin++;
		p_end = p_begin;

		while(*p_begin != CTRL_A){
			p_begin++;
		}
		*p_begin = 0;
		sscanf(p_end, "%u", &temp_click);
		p_begin++;
		p_end = p_begin;
		bool bEnd = false;
    	instance.push_back(0);
    	instance.push_back(getAdFeaCount());
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
				feaid = get_featid(feasign);
				instance.push_back(feaid);
			}
			p_begin = p_end+1;
		}

		sort(&instance[0], &instance[instance.size()]);
		for(size_t i = 0; i < instance.size(); i++){
			features.push_back(instance[i]);
		}
		instance_starts.push_back(features.size());
		vector<int> rpInstance = get_rp_instance(instance);
		for(size_t i = 0; i < rpInstance.size(); i++){
			rpFeatures.push_back(rpInstance[i]);
		}
		rpInstance_starts.push_back(rpFeatures.size());
		nonClkQ.push_back(temp_nonclick);
		ClkQ.push_back(temp_click);
		
		numInstance++;
	}
	return 0;
}
