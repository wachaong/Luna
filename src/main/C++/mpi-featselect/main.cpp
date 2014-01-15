#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>

#include "OWLQN.h"
#include "featureSel.h"
using namespace std;

extern vector<string> featureNameUser;
extern vector<string> featureNameAd;

struct paramStruct{
	string name;
	double value;
	paramStruct(string name, double value){
		this->name = name;
		this->value = value;
	}
};

void printUsageAndExit() {
	cout << "options:" << endl;
	cout << "  -tol <value>   sets convergence tolerance (default is 1e-4)" << endl;
 	cout << "  -l2weight <value>" << endl;
 	cout << "  -l21weight <value>" << endl;
 	cout << "  -K <value>" << endl;
	cout << endl;
	exit(0);
}

bool sortBy(const paramStruct& p1, const paramStruct& p2){
	return p1.value > p2.value;
}

void printVector(const DblVec &vec, const char* filename) {
	ofstream outfile(filename);
	if (!outfile.good()) {
		cerr << "error opening matrix file " << filename << endl;
		exit(1);
	}

	for (size_t i=0; i<vec.size(); i++) {
		outfile << vec[i] << endl;
	}
	outfile.close();
}

double calSparsity(const DblVec& vec, size_t dimLatent){
	int num = vec.size() / dimLatent;
	int sparseNum = 0;
	double sum = 0;
	for(int i = 0; i < num; i++){ 
		sum = 0;
		for(int j = 0; j < dimLatent; j++)
		sum += vec[i*dimLatent+j] * vec[i*dimLatent+j];
		if(sum < 1e-10) sparseNum++;
	}
	return (double)sparseNum / num;
}  

void saveParameters(const DblVec &vec, const vector<string>& featureName, const char* filename, size_t dimLatent){
	ofstream outfile(filename);
	if(!outfile.good()){
		cerr << "error opening matrix file " << filename << endl;
		exit(1);
	}
	vector<paramStruct> paramVec;
	int num = vec.size() / dimLatent;
	double sum = 0;
	for(int i = 0; i < num; i++){
		sum = 0;
		for(int j = 0; j < dimLatent; j++)
			sum += vec[i*dimLatent+j]*vec[i*dimLatent+j];
		paramVec.push_back(paramStruct(featureName[i], sum));
	}
	std::sort(paramVec.begin(), paramVec.end(), sortBy);
	
	for(size_t i = 0; i < paramVec.size(); i++){
		outfile << paramVec[i].name << "\t" << paramVec[i].value << endl;
	}
	
	for(size_t i = 0; i < 20; i++){
		cout << paramVec[i].name << "\t" << paramVec[i].value << endl;
	}
	
	cout << "=====================\n";
	for(size_t i = paramVec.size()-20; i < paramVec.size(); i++){
		cout << paramVec[i].name << "\t" << paramVec[i].value << endl;
	}
	outfile.close();
}

int main(int argc, char** argv) {

	int my_rankid;
	int cnt_processors;
	char train_file[100] = "./data/train/ins";
	char fea_file[100] = "./FeaDict.dat";
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rankid);
	MPI_Comm_size(MPI_COMM_WORLD, &cnt_processors);
	
	
	int K = 10; //latent factor dimension
	double l21reg = 0.0;
	double l2weight = 0.0;
	double tol = 1e-6;
	
	for (int i=1; i<argc; i++) {
		 if (!strcmp(argv[i], "-tol")) {
			++i;
			if (i >= argc || (tol = atof(argv[i])) <= 0) {
				cout << "-tol (convergence tolerance) flag requires 1 positive real argument." << endl;
				exit(1);
			}
		} else if (!strcmp(argv[i], "-l2weight")) {
			++i;
			if (i >= argc || (l2weight = atof(argv[i])) < 0) {
				cout << "-l2weight flag requires 1 non-negative real argument." << endl;
				exit(1);
			}
		}
		else if (!strcmp(argv[i], "-l21weight")) {
			++i;
			if (i >= argc || (l21reg = atof(argv[i])) < 0) {
				cout << "-l21weight flag requires 1 non-negative real argument." << endl;
				exit(1);
			}
		}
		else if (!strcmp(argv[i], "-K")) {
			++i;
			if (i >= argc || (K = atoi(argv[i])) < 0) {
				cout << "-K flag requires 1 non-negative integer argument." << endl;
				exit(1);
			}
		}
	}
	
	double begin = MPI_Wtime();
	FeatureSelectionProblem *fsp = new FeatureSelectionProblem(train_file, fea_file, K, my_rankid);
	DifferentiableFunction* o0  = new FeatureSelectionObjectiveInit(*fsp, l2weight);
	DifferentiableFunction* o1  = new FeatureSelectionObjectiveFixAd(*fsp, l21reg, l2weight);
	DifferentiableFunction* o2  = new FeatureSelectionObjectiveFixUser(*fsp, l21reg, l2weight);
	int size = fsp->NumAllFeats();
	double l1regweight = 0.0;
	int m = 10;
	
	if(my_rankid == 0){
		OWLQN opt;
		opt.Minimize(*o0, fsp->getP(), fsp->getP(), l1regweight, tol, m);
		o0->handler(0, 0); // inform all non-root worker finish
	}
	else{
		int ret;
		int command = 0;
		DblVec input(size), gradient(size);
		while(1){
			ret = o0->handler(my_rankid, command);
			if(ret == 0){
				break;
			}
			else{
				o0->Eval(input, gradient);
			}
		}
		
	}
	//broadcast P to all slaver node
	MPI_Bcast(&((fsp->getP())[0]), size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	if(my_rankid == 0)
		cout << "HAHAHAHAHAHA INIT finished" << endl;
	
	//INIT finish
	
	
	//ALternate optimization for user and ad parts
	MPI_Bcast(&((fsp->getW())[0]), fsp->getW().size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&((fsp->getV())[0]), fsp->getV().size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	double loss = 1e8;
	int size1 = fsp->getW().size() + fsp->getP().size();
	int size2 = fsp->getV().size() + fsp->getP().size();
	DblVec input1(size1), gradient1(size1);
	DblVec input2(size2), gradient2(size2);
	
	for(int iter = 0; iter < 5; iter++){
		for(int i = 0; i < fsp->getW().size(); i++) input1[i] = fsp->getW()[i];
		for(int i = 0; i < fsp->getP().size(); i++) input1[i+fsp->getW().size()] = fsp->getP()[i];
		if(my_rankid == 0){
			OWLQN opt1;
			opt1.Minimize(*o1, input1, input1, l1regweight, tol, m, iter);
			o1->handler(0, 0); // inform all non-root worker finish
			for(int i = 0; i < fsp->getW().size(); i++) fsp->getW()[i] = input1[i];                                                                      
			for(int i = 0; i < fsp->getP().size(); i++) fsp->getP()[i] = input1[i+fsp->getW().size()]; 
			
		}
		else{
			int ret;
			int command = 0;
			DblVec input(size), gradient(size);
			while(1){
				ret = o1->handler(my_rankid, command);
				if(ret == 0){
					break;
				}
				else{
					o1->Eval(input1, gradient1);
				}
			}
		}
		if(my_rankid == 0)
			cout << ">>Iter" <<iter << " OPT1 END" << "\t";
		
		MPI_Bcast(&((fsp->getP())[0]), fsp->getP().size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Bcast(&((fsp->getW())[0]), fsp->getW().size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
		
		
		for(int i = 0; i < fsp->getV().size(); i++) input2[i] = fsp->getV()[i];                                                                          
		for(int i = 0; i < fsp->getP().size(); i++) input2[i+fsp->getV().size()] = fsp->getP()[i];  
		
		if(my_rankid == 0){
			OWLQN opt2;
			double newloss = opt2.Minimize(*o2, input2, input2, l1regweight, tol, m, iter);
			o2->handler(0, 0); // inform all non-root worker finish
			for(int i = 0; i < fsp->getV().size(); i++) fsp->getV()[i] = input2[i];                                                                      
			for(int i = 0; i < fsp->getP().size(); i++) fsp->getP()[i] = input2[i+fsp->getV().size()]; 
		}
		
		else{
			int ret;
			int command = 0;
			while(1){
				ret = o2->handler(my_rankid, command);
				if(ret == 0){
					break;
				}
				else{
					o2->Eval(input2, gradient2);
				}
			}
		}
		
		if(my_rankid == 0)
			cout << ">>Iter" <<iter << " OPT2 END" << "\t";
		
		MPI_Bcast(&((fsp->getP())[0]), fsp->getP().size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Bcast(&((fsp->getV())[0]), fsp->getV().size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
		
		
//		if(my_rankid == 0){
//				if ((loss - newloss) / loss > 1e-8){
//					loss = newloss;
//				}
//				else{
//					cout << "LARGE ITERATION: " << iter << " END" << endl;
//					break;
//				}
//		}
	
	}

	double end = MPI_Wtime();
	printVector(fsp->getP(), "./rank-00000/modelP");
	printVector(fsp->getW(), "./rank-00000/modelW");
	printVector(fsp->getV(), "./rank-00000/modelV");
	
	if(my_rankid == 0){
		cout <<"\t HAHAHHAHAHA GAME OVER\n";
		cout <<"L2weight:" << l2weight << " L21reg:" << l21reg << " K:" << K << " Tol:"<< tol << endl; 
		double sparsityW = calSparsity(fsp->getW(),K);
		double sparsityV = calSparsity(fsp->getV(),K);
		cout << "Sparsity of W is : " << sparsityW;
		cout << "; Sparsity of V is : " << sparsityV;
		double sparsity = (sparsityW*(fsp->getW().size()/K) + sparsityV*(fsp->getV().size()/K)) / (fsp->getW().size()/K + fsp->getV().size()/K);
		double timecost = end -begin;
		cout <<"; The whole sparsity is " << calSparsity(fsp->getV(), K);
		cout <<"; The time cost is " << timecost  << endl;
	//	saveParameters(fsp->getW(), featureNameUser, "./rank-00000/Windex", K);
	//	cout << "=====================\n";
	//	cout << "=====================\n";
	//	saveParameters(fsp->getV(), featureNameAd, "./rank-00000/Vindex", K);
	}
	
	MPI_Finalize();
	return 0;
	
}
