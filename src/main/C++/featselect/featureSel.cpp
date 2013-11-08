#include "featureSel.h"
#include "inslookup.h"
#include <string>
#include <iostream>
#include <math.h>
using namespace std;
/*
 User: U:1*m W:m*k
 Ad:   T:1*n V:n*k
 f(x) = (UW)(TV)' + Px
 p(x) = g(f(x)) = 1 / (1+e^-f(x))
 
 L(W,V) = ylog(p(x)) + (1-y)log(1-p(x))
 R(W,V) = sum_i{1,m}(norm1(w_i)) + sum_i{1,n}(norm1(v_i))
 		= sum_i{1,m}(sqrt(sum_j(w_ij^2) + epsilon)) + sum_i{1,n}(sqrt(sum_j(v_ij^2)+epsilon)
*/

void printMatrix(DblMat m, int row){
	if(m == NULL || row == 0){
		cout << "Empty Matrix" << endl;
		return;
	}
	size_t col = m[0].size();
	cout << row << " X " << col << endl;
	for(int i = 0; i < row; i++){
		for(int j = 0; j < col; j++){
			cout << m[i][j] << " ";
		}
		cout << endl;
	}
}

/*
For test
*/
extern char feamap_path[2048];
extern char ins_path[2048];
FeatureSelectionProblem::FeatureSelectionProblem(const char* instance_file, const char* feature_file){
	//initialize
	//read instances and features from files
	//save into four deques
	//features,instance_starts, nonclk, clk
	
	init();
	load_feamap(feamap_path);
	trans_ins(ins_path, features, instance_starts, nonclk, clk, numInstance);
	
	
	numAdFeature = getAdFeaCount();
	numUserFeature = getUserFeaCount();
	numOtherFeature = getOtherFeaCount();
	
	dimLatent = 10;
	epsilon = 1e-4;
	
	
	
	
	
	for(size_t i = 0; i < numUserFeature; i++){
		for(size_t j = 0; j < dimLatent; j++){
			W.push_back(0);
		}
	}
	for(size_t i = 0; i < numAdFeature; i++){
		for(size_t j = 0; j < dimLatent; j++){
			V.push_back(0);
		}
	}
	
	for(size_t i = 0; i<NumAllFeats(); i++){
		P.push_back(0);
	}
	
	cout << numAdFeature << endl;
	cout << numUserFeature << endl;
	cout << numInstance << endl;
	
//	printMatrix(getWAsMat(), numUserFeature);
//	printMatrix(getVAsMat(), numAdFeature);

//	cout << W.size() << endl;
	
	
}


/*
FeatureSelectionProblem::FeatureSelectionProblem(const char* instance_file, const char* feature_file){
	//initialize
	//read instances and features from files
	//save into four deques
	//features,instance_starts, nonclk, clk
	
	numAdFeature = 5;
	numUserFeature = 6;
	numOtherFeature = 5;
	dimLatent = 10;
	numInstance = 0;
	epsilon = 1e-4;
	instance_starts.push_back(0);
	
	
	for(size_t i = 0; i < numUserFeature; i++){
		for(size_t j = 0; j < dimLatent; j++){
			W.push_back(i);
		}
	}
	for(size_t i = 0; i < numAdFeature; i++){
		for(size_t j = 0; j < dimLatent; j++){
			V.push_back(i+1);
		}
	}
	
	for(size_t i = 0; i<NumAllFeats(); i++){
		P.push_back(0);
	}
	
	printMatrix(getWAsMat(), numUserFeature);
	printMatrix(getVAsMat(), numAdFeature);

	cout << W.size() << endl;
}

*/

//calculate f(x)
double FeatureSelectionProblem::ScoreOf(size_t i, const std::vector<double>& weights) const{
	//f(x)=(UW)(TV)' + Px
	double score = 0.0;
/*	DblMat W = getWAsMat();
	DblMat V = getVAsMat();
	DblVec UW;
	DblVec TV;
	DblVec U;
	DblVec T;
	DblVec X;
	
	for (size_t j = 0 ; j < dimLatent; j++){
		UW.push_back(0);
		TV.push_back(0);
	}
	for (size_t j = instance_starts[i]; j < instance_starts[i+1]; j++){
		X.push_back(features[j]);
		score += weights[features[j]];
		if(features[j] < NumAdFeats()){
			T.push_back(features[j]);
			for(size_t k = 0; k < dimLatent; k++){
				TV[k]+=V[features[j]][k];
	//			cout << V[features[j]][k]<< " ";
			}
		} 
		else if(features[j] < NumAdFeats() + NumUserFeats()){
			U.push_back(features[j]);
			for(size_t k = 0; k < dimLatent; k++){
				UW[k] += W[features[j]-NumAdFeats()][k];
			}
		}
		
	}
	
	for (size_t j = 0; j < dimLatent; j++){
		score += UW[j]*TV[j];
	}
	
	delete []W;
	delete []V;
	*/
	
	for (size_t j = instance_starts[i]; j < instance_starts[i+1]; j++){
		score += weights[features[j]];
	}
	return score;
}

double FeatureSelectionProblem::GroupLasso() const{
	//||W||_21 + ||V||_21
	//R(W,V) = sum_i{1,m}(norm1(w_i)) + sum_i{1,n}(norm1(v_i))
 	//	= sum_i{1,m}(sqrt(sum_j(w_ij^2) + epsilon)) + sum_i{1,n}(sqrt(sum_j(v_ij^2)+epsilon)
	double result = 0;
	DblMat W = getWAsMat();
	DblMat V = getVAsMat();
	for(size_t i = 0; i < NumUserFeats(); i++){
		double sum = 0;
		for(size_t j = 0; j < dimLatent; j++){
			sum += W[i][j] * W[i][j];
		}
		sum += epsilon;
		result += sqrt(sum);
	}
	for(size_t i = 0; i < NumAdFeats(); i++){
		double sum = 0;
		for(size_t j = 0; j < dimLatent; j++){
			sum += V[i][j] * V[i][j];
		}
		sum += epsilon;
		result += sqrt(sum);
	}
	return result;
}


void displayGradient(DblVec& gradientP){
	cout << "DEBUG DISPLAY GRADIENT\n";
	for(size_t i = 0; i < gradientP.size(); i++){
		if(gradientP[i] != 0) cout << i << " : " << gradientP[i] << endl;
	}
	cout << "DEBUG DISPLAY GRADIENT OVER\n";
}
double FeatureSelectionObjectiveInit::Eval(const DblVec& input, DblVec& gradientP){
	double loss = 0.0;
//	DblVec P = problem.getP();
	for(size_t i = 0; i < input.size(); i++){
	//	P[i] = input[i];
		loss += 0.5*input[i]*input[i]*l2weight;
		gradientP[i] = l2weight*input[i];
	}
	
	for(size_t i = 0; i < problem.NumInstance(); i++){
		double score = problem.ScoreOf(i, input);
		double insLoss, insProb;
		if(problem.ClkOf(i) > 0){
			if(score < -30){
				insLoss = -score;
				insProb = 0;
			}
			else if(score > 30){
				insLoss = 0;
				insProb = 1;
			}
			else{
				double temp = 1.0 + exp(-score);
				if(temp < 1) cout << temp << endl;
				insLoss = log(temp);
				insProb = 1.0/ temp;
			}
			loss +=  problem.ClkOf(i) * insLoss;
			problem.AddMultToP(i, -1.0*problem.ClkOf(i)*(1.0 - insProb), gradientP);
		}
	//	displayGradient(gradientP);
		if(problem.NonClkOf(i) > 0){
			score = -score;
			if(score < -30){
				insLoss = -score;
				insProb = 0;
			}
			else if(score > 30){
				insLoss = 0;
				insProb = 1;
			}
			else{
				double temp = 1.0 + exp(-score);
				insLoss = log(temp);
				insProb = 1.0/ temp;
			}
			loss +=  problem.NonClkOf(i) * insLoss;
			problem.AddMultToP(i, 1.0*problem.NonClkOf(i)*(1.0 - insProb), gradientP);
		}	
		
//		if(i==2)	break;
		
	}
//	cout << "LOSS:" <<loss << endl;
//	exit(1);
//	displayGradient(gradientP);
//	exit(1);
	return loss;
}


