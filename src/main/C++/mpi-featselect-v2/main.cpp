#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>

#include "OWLQN.h"
#include "featureSel.h"
using namespace std;

void printVector(const DblVec &vec, const char* filename) {
	ofstream outfile(filename);
	if (!outfile.good()) {
		cerr << "error opening matrix file " << filename << endl;
		exit(1);
	}
//	outfile << "%%MatrixMarket matrix array real general" << endl;
//	outfile << "1 " << vec.size() << endl;
	for (size_t i=0; i<vec.size(); i++) {
//		if(vec[i] < 1e-6){
//			outfile << 0 << endl;
//		}
//		else
			outfile << vec[i] << endl;
	}
	outfile.close();
}

int main(int argc, char** argv) {

	int my_rankid;
	int cnt_processors;
	char train_file[100] = "./data/train/ins";
//	char train_file[100] = "D:\\workspace\\Luna\\src\\main\\C++\\mpi-featselect\\ins";
//	char fea_file[100] = "D:\\workspace\\Luna\\src\\main\\C++\\mpi-featselect\\feat";
	char fea_file[100] = "./FeaDict.dat";
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rankid);
	MPI_Comm_size(MPI_COMM_WORLD, &cnt_processors);
	
	
	int K = 10; //latent factor dimension
	double l21reg = 0;
	FeatureSelectionProblem *fsp = new FeatureSelectionProblem(train_file, fea_file, K, my_rankid);
	DifferentiableFunction* o0  = new FeatureSelectionObjectiveInit(*fsp);
	DifferentiableFunction* o1  = new FeatureSelectionObjectiveFixAd(*fsp, l21reg);
	DifferentiableFunction* o2  = new FeatureSelectionObjectiveFixUser(*fsp, l21reg);
	int size = fsp->getP().size() + fsp->getP1().size() + fsp->getP2().size();

	int l1regweight = 0;
	double tol = 1e-6, l2weight = 0;
	int m = 5;
	DblVec input(size), gradient(size);
	for(int i = 0; i < fsp->getP().size(); i++) input[i] = fsp->getP()[i];
	for(int i = 0; i < fsp->getP1().size(); i++) input[i+fsp->getP().size()] = fsp->getP1()[i];
	for(int i = 0; i < fsp->getP2().size(); i++) input[i+fsp->getP().size()+fsp->getP1().size()] = fsp->getP2()[i];
	
	if(my_rankid == 0){
		OWLQN opt;
		opt.Minimize(*o0, input, input, l1regweight, tol, m);
		o0->handler(0, 0); // inform all non-root worker finish
		for(int i = 0; i < fsp->getP().size(); i++) fsp->getP()[i] = input[i];
		for(int i = 0; i < fsp->getP1().size(); i++) fsp->getP1()[i] = input[i+fsp->getP().size()];
		for(int i = 0; i < fsp->getP2().size(); i++) fsp->getP2()[i] = input[i+fsp->getP().size()+fsp->getP1().size()];
	}
	else{
		int ret;
		int command = 0;
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
	MPI_Bcast(&((fsp->getP1())[0]), fsp->getP1().size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&((fsp->getP2())[0]), fsp->getP2().size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	if(my_rankid == 0)
		cout << "HAHAHAHAHAHA INIT finished" << endl;
		
	
	//INIT finish
	
	
	//ALternate optimization for user and ad parts
	MPI_Bcast(&((fsp->getW())[0]), fsp->getW().size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&((fsp->getV())[0]), fsp->getV().size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	double loss = 1e8;
	int size1 = fsp->getW().size();
	int size2 = fsp->getV().size();
	DblVec input1(size1), gradient1(size1);
	DblVec input2(size2), gradient2(size2);
	
	for(int iter = 0; iter < 5; iter++){
		if(my_rankid == 0){
			OWLQN opt1;	
			opt1.Minimize(*o1, fsp->getW(), fsp->getW(), l1regweight, tol, m, iter);
		//	opt1.Minimize(*o1, input1, input1, l1regweight, tol, m, iter);
			o1->handler(0, 0); // inform all non-root worker finish
				
		}
		else{
			int ret;
			int command = 0;
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
		
		if(my_rankid == 0) cout << ">>Iter" <<iter << " OPT1 END" << endl;
		MPI_Bcast(&((fsp->getW())[0]), fsp->getW().size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
		
		if(my_rankid == 0){
			OWLQN opt2;
			double newloss = opt2.Minimize(*o2, fsp->getV(), fsp->getV(), l1regweight, tol, m, iter);
			o2->handler(0, 0); // inform all non-root worker finish
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
		
		if(my_rankid == 0)  cout << ">>Iter" <<iter << " OPT2 END" << endl;
		MPI_Bcast(&((fsp->getV())[0]), fsp->getV().size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
		
		
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
	printVector(fsp->getP1(), "./rank-00000/modelP1");
	printVector(fsp->getP2(), "./rank-00000/modelP2");
	printVector(fsp->getW(), "./rank-00000/modelW");
	printVector(fsp->getV(), "./rank-00000/modelV");
	if(my_rankid == 0)
		cout <<"HAHAHHAHAHA GAME OVER\n";
	MPI_Finalize();
	return 0;
	
}
