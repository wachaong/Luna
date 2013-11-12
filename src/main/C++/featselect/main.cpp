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
	outfile << "%%MatrixMarket matrix array real general" << endl;
	outfile << "1 " << vec.size() << endl;
	for (size_t i=0; i<vec.size(); i++) {
		outfile << vec[i] << endl;
	}
	outfile.close();
}

int main(int argc, char** argv) {	
	int K = 10; //latent factor dimension
	FeatureSelectionProblem *fsp = new FeatureSelectionProblem("ins", "fea", K);
	DifferentiableFunction* o0  = new FeatureSelectionObjectiveInit(*fsp);
	DifferentiableFunction* o1  = new FeatureSelectionObjectiveFixAd(*fsp);
	DifferentiableFunction* o2  = new FeatureSelectionObjectiveFixUser(*fsp);
	
	
	
	int l1regweight = 0;
	double tol = 1e-6, l2weight = 0;
	int m = 10;
	int size = fsp->NumAllFeats();
	OWLQN opt;
	opt.Minimize(*o0, fsp->getP(), fsp->getP(), l1regweight, tol, m);
	
	
		OWLQN opt1;
		opt1.Minimize(*o1, fsp->getW(), fsp->getW(), l1regweight, tol, m);
			
	/*
	for(int iter = 0; iter < 10; iter++){
		
		OWLQN opt1;
		opt1.Minimize(*o1, fsp->getW(), fsp->getW(), l1regweight, tol, m);
		OWLQN opt2;
		opt2.Minimize(*o2, fsp->getV(), fsp->getV(), l1regweight, tol, m);
	}

*/
	printVector(fsp->getP(), "OUT_P");
	printVector(fsp->getW(), "OUT_W");
	printVector(fsp->getV(), "OUT_V");
	return 0;
	
}
