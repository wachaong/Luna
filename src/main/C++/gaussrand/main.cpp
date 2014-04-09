#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <string.h>

using namespace std;

vector<double>* randMatrix;
double gaussrand(){
	static double V1, V2, S;
	static int phase = 0;
	double X;
	if(phase == 0){
		do {
			double U1 = (double)rand() / RAND_MAX;
			double U2 = (double)rand() / RAND_MAX;
			
			V1 = 2 * U1 - 1;
			V2 = 2 * U2 - 1;
			S = V1 * V1 + V2 * V2;
		} while (S >= 1 || S == 0);
		
		X = V1*sqrt(-2*log(S) / S);
		
	} else {
		X = V2*sqrt(-2*log(S) / S);
	}
	return X;
}

void testsave(){
	ofstream outfile("test.txt");
	if(!outfile.good()){
		cerr << "error opening file test.txt" << endl;
		exit(1);
	}
	int rownum = 5;
	int colnum = 4;
	outfile << rownum << "X" << colnum << endl;
	for(int i = 0; i < rownum; i++){
		outfile << gaussrand();
		for(int j = 1; j < colnum; j++){
			outfile << "\t" << gaussrand();
		}
		outfile << endl;
	}
	outfile.close();
}

vector<double> split(string str, string pattern){
	int pos;
	vector<double> result;
	str += pattern;
	int size = str.size();
	for(int i = 0; i < size; i++){
		pos = str.find(pattern, i);
		if(pos < size){
			string s = str.substr(i, pos-i);
			result.push_back(atof(s.c_str()));
			i = pos+pattern.size() -1;
		}
	}
	return result;
}

void load(){
	string line;
	ifstream pfeamap("test.txt");
	if(!pfeamap.good()){
		cerr << "error rand mat file" << endl;
		exit(1);
	}
	getline(pfeamap, line);
	int pos = line.find("X");
	int rownum = atoi(line.substr(0, pos).c_str());
	int colnum = atoi(line.substr(pos+1, line.length()-pos-1).c_str());
	randMatrix = new vector<double>[rownum];
	int i = 0;
	while (getline(pfeamap, line)){
		randMatrix[i++] = split(line, "\t");
	}
	cout << i << endl;
	for(i = 0; i < rownum; i++){
		for(int j = 0; j < colnum; j++){
			cout << randMatrix[i][j] << "\t";
		}
		cout << endl;
	}
	pfeamap.close();
}

void save(const char* filename, int rownum, int colnum){
	ofstream outfile(filename);
	if(!outfile.good()){
		cerr << "error opening file " << filename << endl;
		exit(1);
	}
	outfile << rownum << "X" << colnum << endl;
	for(int i = 0; i < rownum; i++){
		outfile << gaussrand();
		for(int j = 0; j < colnum; j++){
			outfile << "\t" << gaussrand();
		}
		outfile << endl;
	}
	outfile.close();
}

//usage GaussRand FILENAME ROWNUM COLNUM
int main(int argc, char** argv) {
	
	if(argc <= 3){
		cout << "Usage: GaussRand FILENAME ROWNUM COLNUM" << endl;
		exit(0);
	}
	int rownum = atoi(argv[2]);
	int colnum = atoi(argv[3]);
	save(argv[1], rownum, colnum);
	
	return 0;
}


