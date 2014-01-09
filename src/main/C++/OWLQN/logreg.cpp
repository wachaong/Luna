#include "logreg.h"
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

void skipEmptyAndComment(ifstream& file, string& s) {
	do {
		getline(file, s);
	} while (s.size() == 0 || s[0] == '%');
}
LogisticRegressionProblem::LogisticRegressionProblem(const char* trainfile){
	ifstream train(trainfile);
	if(!train.good()) {
		cerr << "error opening training file " << trainfile << endl;
		exit(1);
	}
	string s;
	size_t numIns = 0;
	numFeats = 3;
	double a, b;
	int label;
	vector<vector<float> > rowVals;
	instance_starts.push_back(0);
	while(getline(train, s)){
		stringstream st(s);
		st >> a >> b >> label;
		vector<float> rowVal;
		rowVal.push_back(1.0);
		rowVal.push_back(a);
		rowVal.push_back(b);
		numIns++;
		rowVals.push_back(rowVal);
		bool bLabel;
		switch(label){
			case 1:
				bLabel = true;
				break;
			case 0:
				bLabel = false;
				break;
			default:
				cout << "error label" << endl;
				exit(1);
		}
		AddInstance(rowVal, bLabel);
	}
	
	train.close();
	
}
LogisticRegressionProblem::LogisticRegressionProblem(const char* matFilename, const char* labelFilename) {
	ifstream matfile(matFilename);
	if (!matfile.good()) {
		cerr << "error opening matrix file " << matFilename << endl;
		exit(1);
	}
	string s;
	getline(matfile, s);
	if (!s.compare("%%MatrixMarket matrix coordinate real general")) {
		skipEmptyAndComment(matfile, s);

		stringstream st(s);
		size_t numIns, numNonZero;
		st >> numIns >> numFeats >> numNonZero;

		vector<deque<size_t> > rowInds(numIns);
		vector<deque<float> > rowVals(numIns);
		for (size_t i = 0; i < numNonZero; i++) {
			size_t row, col;
			float val;
			matfile >> row >> col >> val;
			row--;
			col--;
			rowInds[row].push_back(col);
			rowVals[row].push_back(val);
		}

		matfile.close();

		ifstream labfile(labelFilename);
		getline(labfile, s);
		if (s.compare("%%MatrixMarket matrix array real general")) {
			cerr << "unsupported label file format in " << labelFilename << endl;
			exit(1);
		}

		skipEmptyAndComment(labfile, s);
		stringstream labst(s);
		size_t labNum, labCol;
		labst >> labNum >> labCol;
		if (labNum != numIns) {
			cerr << "number of labels doesn't match number of instances in " << labelFilename << endl;
			exit(1);
		} else if (labCol != 1) {
			cerr << "label matrix may not have more than one column" << endl;
			exit(1);
		}

		instance_starts.push_back(0);

		for (size_t i=0; i<numIns; i++) {
			int label;
			labfile >> label;
			bool bLabel;
			switch (label) {
					case 1:
						bLabel = true;
						break;

					case -1:
						bLabel = false;
						break;

					default:
						cerr << "illegal label: must be 1 or -1" << endl;
						exit(1);
			}

			AddInstance(rowInds[i], rowVals[i], bLabel);
		}

		labfile.close();
	} else if (!s.compare("%%MatrixMarket matrix array real general")) {
		skipEmptyAndComment(matfile, s);
		stringstream st(s);
		size_t numIns;
		st >> numIns >> numFeats;

		vector<vector<float> > rowVals(numIns);

		for (size_t j=0; j<numFeats; j++) {
			for (size_t i=0; i<numIns; i++) {
				float val;
				matfile >> val;
				rowVals[i].push_back(val);
			}
		}

		matfile.close();

		ifstream labfile(labelFilename);
		getline(labfile, s);
		if (s.compare("%%MatrixMarket matrix array real general")) {
			cerr << "unsupported label file format in " << labelFilename << endl;
			exit(1);
		}

		skipEmptyAndComment(labfile, s);
		stringstream labst(s);
		size_t labNum, labCol;
		labst >> labNum >> labCol;
		if (labNum != numIns) {
			cerr << "number of labels doesn't match number of instances in " << labelFilename << endl;
			exit(1);
		} else if (labCol != 1) {
			cerr << "label matrix may not have more than one column" << endl;
			exit(1);
		}

		instance_starts.push_back(0);
		for (size_t i=0; i<numIns; i++) {
			int label;
			labfile >> label;
			bool bLabel;
			switch (label) {
					case 1:
						bLabel = true;
						break;

					case -1:
						bLabel = false;
						break;

					default:
						cerr << "illegal label: must be 1 or -1" << endl;
						exit(1);
			}

			AddInstance(rowVals[i], bLabel);
		}

		labfile.close();
	} else {
		cerr << "unsupported matrix file format in " << matFilename << endl;
		exit(1);
	}
}

void LogisticRegressionProblem::AddInstance(const deque<size_t>& inds, const deque<float>& vals, bool label) {
	for (size_t i=0; i<inds.size(); i++) {
		indices.push_back(inds[i]);
		values.push_back(vals[i]);
	}
	instance_starts.push_back(indices.size());
	labels.push_back(label);
}

void LogisticRegressionProblem::AddInstance(const vector<float>& vals, bool label) {
	for (size_t i=0; i<vals.size(); i++) {
		values.push_back(vals[i]);
	}
	instance_starts.push_back(values.size());
	labels.push_back(label);
}

double LogisticRegressionProblem::ScoreOf(size_t i, const vector<double>& weights) const {
	double score = 0;
	for (size_t j=instance_starts[i]; j < instance_starts[i+1]; j++) {
		double value = values[j];
		size_t index = (indices.size() > 0) ? indices[j] : j - instance_starts[i];
		score += weights[index] * value;
	}
	if (!labels[i]) score *= -1;
	return score;
}


double LogisticRegressionObjective::Eval(const DblVec& input, DblVec& gradient) {
	double loss = 1.0;

	for (size_t i=0; i<input.size(); i++) {
		loss += 0.5 * input[i] * input[i] * l2weight;
		gradient[i] = l2weight * input[i];
	}

	for (size_t i =0 ; i<problem.NumInstances(); i++) {
		double score = problem.ScoreOf(i, input);

		double insLoss, insProb;
		if (score < -30) {
			insLoss = -score;
			insProb = 0;
		} else if (score > 30) {
			insLoss = 0;
			insProb = 1;
		} else {
			double temp = 1.0 + exp(-score);
			insLoss = log(temp);
			insProb = 1.0/temp;
		}
		loss += insLoss;
		problem.AddMultTo(i, 1.0 - insProb, gradient);
	}

	return loss;
}
