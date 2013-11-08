#include <iostream>
#include <deque>
#include <fstream>

#include "OWLQN.h"
#include "logreg.h"

using namespace std;

/* run this program using the console pauser or add your own getch, system("pause") or input loop */


void printUsageAndExit() {
	cout << "Qrthant-Wise Limited-memory Quasi-Newton trainer" << endl;
	cout << "trains L1-regularized logistic regression models" << endl;
	cout << "usage: feature_file label_file regWeight output_file [options]" << endl;
	cout << " feature_file input feature matrix in Matrix Market format (mXn real coordinate or array)" << endl;
	cout << "			rows represent features for each instance" << endl;
	cout << " label_file  input instance labels in Matrix Market format (mX1 real array)" << endl;
	cout << "			rows contain single real value" << endl;
	cout << " regWeight cofficient of L1 regularizer" << endl;
	cout << " output_file output weight vector in Matrix Market format (1Xm real array)" << endl << endl;
	cout << " -tol <value> sets convergence tolerance (default is 1e-4)" << endl;
	cout << " -m <value> sets L-BFGS memory parameter (default is 10)" << endl;
	cout << " -l2weight <value> sets L2 regularization weight (default is 0)" << endl;
	cout << endl;
	exit(0);
}


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

	//char* feature_file = "feature_file.mat";
	//char* label_file = "label_file.mat";
	char train_file[20] = "ins";
	int regweight = 0;
	char output_file[20] = "output.mat";
	
	double tol = 1e-6, l2weight = 0;
	int m = 10;
	
	
	
	DifferentiableFunction *obj;
	size_t size;
	
	//LogisticRegressionProblem *prob = new LogisticRegressionProblem(feature_file, label_file);
	LogisticRegressionProblem *prob = new LogisticRegressionProblem(train_file);
	obj = new LogisticRegressionObjective(*prob, l2weight);
	size = prob->NumFeats();
	
	DblVec init(size), ans(size);
	
	OWLQN opt;
	opt.Minimize(*obj, init, ans, regweight, tol, m);
	
	int nonZero = 0;
	for(size_t i = 0; i < ans.size(); i++){
		if (ans[i] != 0) nonZero++;	
	}
	cout << "Finished with optimization. " << nonZero << "/" << size << " non-zero weights." << endl;
	
	printVector(ans, output_file);

	return 0;
}
