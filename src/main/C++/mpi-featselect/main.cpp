#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>

#include "OWLQN.h"
#include "featureSel.h"
using namespace std;

void printUsageAndExit() {
	cout << "options:" << endl;
	cout << "  -tol <value>   sets convergence tolerance (default is 1e-4)" << endl;
 	cout << "  -l2weight <value>" << endl;
 	cout << "  -l12weight <value>" << endl;
 	cout << "  -K <value>" << endl;
	cout << endl;
	exit(0);
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
		else if (!strcmp(argv[i], "-l12weight")) {
			++i;
			if (i >= argc || (l12reg = atof(argv[i])) < 0) {
				cout << "-l12weight flag requires 1 non-negative real argument." << endl;
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
			cout << ">>Iter" <<iter << " OPT1 END" << endl;
		
		MPI_Bcast(&((fsp->getP())[0]), fsp->getP().size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Bcast(&((fsp->getW())[0]), fsp->getW().size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
		
		
		for(int i = 0; i < fsp->getV().size(); i++) input2[i] = fsp->getV()[i];                                                                          
		for(int i = 0; i < fsp->getP().size(); i++) input2[i+fsp->getV().size()] = fsp->getP()[i];  
		
		if(my_rankid == 0){
			OWLQN opt2;
			double newloss = opt2.Minimize(*o2, fsp->getV(), fsp->getV(), l1regweight, tol, m, iter);
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
		
		if(my_rankid == 0){
			cout << ">>Iter" <<iter << " OPT2 END" << endl;
		
		MPI_Bcast(&((fsp->getP())[0]), fsp->getP().size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Bcast(&((fsp->getW())[0]), fsp->getV().size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
		
		
		if(my_rankid == 0){
//				if ((loss - newloss) / loss > 1e-8){
//					loss = newloss;
//				}
//				else{
//					cout << "LARGE ITERATION: " << iter << " END" << endl;
//					break;
//				}
		}
	
	}

	printVector(fsp->getP(), "./rank-00000/modelP");
	printVector(fsp->getW(), "./rank-00000/modelW");
	printVector(fsp->getV(), "./rank-00000/modelV");
	
	if(my_rankid == 0)
		cout <<"HAHAHHAHAHA GAME OVER\n";
	MPI_Finalize();
	return 0;
	
}
