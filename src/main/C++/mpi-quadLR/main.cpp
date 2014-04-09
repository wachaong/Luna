#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>

#include "OWLQN.h"
#include "quadLR.h"
using namespace std;


void printUsageAndExit() {
	cout << "options:" << endl;
	cout << "  -tol <value>   sets convergence tolerance (default is 1e-4)" << endl;
 	cout << "  -l2weight <value>" << endl;
 	cout << "  -l21weight <value>" << endl;
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
	char rand_ad[100] = "./rand_ad";
	char rand_user[100] = "./rand_user";
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rankid);
	MPI_Comm_size(MPI_COMM_WORLD, &cnt_processors);
	
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
	}
	
	double begin = MPI_Wtime();
	QuadLRProblem *qlrp = new QuadLRProblem(train_file, fea_file, rand_ad, rand_user, my_rankid);
	DifferentiableFunction* obj  = new QuadLRObjective(*qlrp, l2weight);
	int size = qlrp->NumRpUserFeats()*qlrp->NumRpAdFeats()+qlrp->NumAllFeats();
	double l1regweight = 0.0;
	int m = 10;
	
	DblVec input(size), gradient(size);
	
	if(my_rankid == 0){
		OWLQN opt;
		opt.Minimize(*obj, input, input, l1regweight, tol, m);
		obj->handler(0, 0); // inform all non-root worker finish
	}
	else{
		int ret;
		int command = 0;
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
	//broadcast Parameters to all slaver node
	MPI_Bcast(&(input[0]), size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	if(my_rankid == 0)
		cout << "HAHAHAHAHAHA finished" << endl;
	

	double end = MPI_Wtime();
	int xsize = qlrp->NumRpUserFeats()*qlrp->NumRpAdFeats();
	int psize = qlrp->NumAllFeats()
	DblVec X(xsize);
	DblVec P(psize);
	for(int i = 0; i < xsize; i++){
		X[i] = input[i];
	}
	for(int i = xsize; i < input.size(); i++){
		P[i-xsize] = input[i];
	}
	printVector(X, "./rank-00000/modelX");
	printVector(P, "./rank-00000/modelP");
	MPI_Finalize();
	return 0;
	
}
