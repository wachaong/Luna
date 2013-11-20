#include <mpi.h>
#include <iostream>
#include <deque>
#include <fstream>
#include <stdio.h>
#include "OWLQN.h"
#include "logreg.h"

using namespace std;


void printVector(const DblVec &vec, const char* filename) {
	ofstream outfile(filename);
	if (!outfile.good()) {
		cerr << "error opening matrix file " << filename << endl;
		exit(1);
	}
	for (size_t i=0; i<vec.size(); i++) {
		outfile << i << "\t" << vec[i] << endl;
	}
	outfile.close();
}

int main(int argc, char** argv) {


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
	double tol = 1e-6, l2weight = 0;
	obj = new LogisticRegressionObjective(*prob, l2weight);
	size_t size = prob->NumFeats();
	DblVec init(size), ans(size);
	if(my_rankid == 0){
		int regweight = 0;
	//	char output_file[100] = "model";
		int m = 10;
		OWLQN opt;
		opt.Minimize(*obj, init, ans, regweight, tol, m);
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
	cout <<"HAHAHHAHAHA GAME OVER\n";
	MPI_Finalize();
//	delete obj;
//	delete prob;
	return 0;
}
