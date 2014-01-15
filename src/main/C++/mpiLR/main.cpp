#include <mpi.h>
#include <iostream>
#include <deque>
#include <fstream>
#include <stdio.h>
#include "OWLQN.h"
#include "logreg.h"

using namespace std;


void printUsageAndExit() {
	cout << "options:" << endl;
	cout << "  -tol <value>   sets convergence tolerance (default is 1e-4)" << endl;
 	cout << "  -l2weight <value>" << endl;
 	cout << "  -l1weight <value>" << endl;
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

	if (argc < 7 ) {
		printUsageAndExit();
	}
	double tol = 1e-6, l2weight = 0.0;
	double l1weight = 0.0;
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
		else if (!strcmp(argv[i], "-l1weight")) {
			++i;
			if (i >= argc || (l1weight = atof(argv[i])) < 0) {
				cout << "-l1weight flag requires 1 non-negative real argument." << endl;
				exit(1);
			}
		}
	}
	int my_rankid;
	int cnt_processors;
	char train_file[100] = "./data/train/ins";
//	char train_file[100] = "ins";
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rankid);
	MPI_Comm_size(MPI_COMM_WORLD, &cnt_processors);
	
	//Define a LR problem in each machine
	//In the LR problem <load local dataset to memory>
	LogisticRegressionProblem *prob = new LogisticRegressionProblem(train_file, my_rankid);
	DifferentiableFunction *obj;
	obj = new LogisticRegressionObjective(*prob, l2weight);
	size_t size = prob->NumFeats();
	DblVec init(size), ans(size);
	if(my_rankid == 0){
	//	char output_file[100] = "model";
		int m = 10;
		OWLQN opt;
		opt.Minimize(*obj, init, ans, l1weight, tol, m);
		obj->handler(0, 0);  //inform all non-root work finish
		
		int nonZero = 0;
		for(size_t i = 0; i < ans.size(); i++){
			if (ans[i] != 0) nonZero++;	
		}
		
	}
	else{
		int ret;
		int command = 0;
		DblVec input(size), gradient(size);
		while(1){
			ret = obj->handler(my_rankid, command);
			if(ret == 0){
				break;
			}
			else{
				obj->Eval(input, gradient);
			}
		}
	}
	MPI_Bcast(&ans[0], ans.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
	char output_file[100] = "./rank-00000/model";
	printVector(ans, output_file);
	if(my_rankid == 0){
		cout <<"HAHAHHAHAHA GAME OVER\n";
		cout << "Tol: " << tol <<" l2weight: " << l2weight << " l1weight: " << l1weight << endl;
	}
	MPI_Finalize();
	delete obj;
	delete prob;
	return 0;
}
