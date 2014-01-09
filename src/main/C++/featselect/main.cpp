#include <iostream>
#include <vector>
#include <fstream>

#include "OWLQN.h"
#include "featureSel.h"
using namespace std;

void printVector(const DblVec &vec, const char* filename) {
	ofstream outfile(filename);
	if (!outfile.good()) {
		cerr << "error opening vector file " << filename << endl;
		exit(1);
	}
	for (size_t i=0; i<vec.size(); i++) {
		outfile << vec[i] << endl;
	}
	outfile.close();
}

int main(int argc, char** argv) {	
	int K = 10; //latent factor dimension
	double l21reg = 10;
	FeatureSelectionProblem *fsp = new FeatureSelectionProblem("ins", "fea", K);
	DifferentiableFunction* o0  = new FeatureSelectionObjectiveInit(*fsp);
	DifferentiableFunction* o1  = new FeatureSelectionObjectiveFixAd(*fsp, l21reg);
	DifferentiableFunction* o2  = new FeatureSelectionObjectiveFixUser(*fsp, l21reg);
	
	int l1regweight = 0;
	double tol = 1e-6, l2weight = 0;
	int m = 10;
	int size = fsp->NumAllFeats();
	OWLQN opt;
	opt.Minimize(*o0, fsp->getP(), fsp->getP(), l1regweight, tol, m);

	double loss = 1e8;

	for(int iter = 0; iter < 8; iter++){	
		OWLQN opt1;
		opt1.Minimize(*o1, fsp->getW(), fsp->getW(), l1regweight, tol, m);
		OWLQN opt2;
		double newloss = opt2.Minimize(*o2, fsp->getV(), fsp->getV(), l1regweight, tol, m);
		if ((loss - newloss) / loss > 1e-8){
			loss = newloss;
		}
		else{
			cout << "LARGE ITERATION: " << iter << " END" << endl;
			break;
		}
	}


	printVector(fsp->getP(), "modelP");
	printVector(fsp->getW(), "modelW");
	printVector(fsp->getV(), "modelV");
	return 0;	
}
