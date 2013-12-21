#include "featureSel.h"
#include "inslookup.h"
#include <string>
#include <iostream>
#include <math.h>
#include <time.h>  
#include <pthread.h>
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
pthread_t* threadList;
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
extern int init_inslookup(const char*instance_file, const char* feature_file);
FeatureSelectionProblem::FeatureSelectionProblem(const char* instance_file, const char* feature_file, int K, size_t rankid){
	//initialize
	//read instances and features from files
	//save into four deques
	//features,instance_starts, nonclk, clk
	init_inslookup(instance_file, feature_file);
	load_feamap(feamap_path);
	trans_ins(ins_path, rankid, features, instance_starts, nonclk, clk, numInstance);
	
	numAdFeature = getAdFeaCount();
	numUserFeature = getUserFeaCount();
	numOtherFeature = getOtherFeaCount();
	dimLatent = K;
	epsilon = 1e-8;
	
	//initialize W, V, P
	srand((unsigned)time(NULL)); 
	for(size_t i = 0; i < numUserFeature; i++){
		for(size_t j = 0; j < dimLatent; j++){
			W.push_back((rand()  / double(RAND_MAX) * 2 - 1) * 0.01);
		//W.push_back(0);
		}
		for(size_t j = 0; j < 24; j++) u[j].push_back(0);
	}
	for(size_t i = 0; i < numAdFeature; i++){
		for(size_t j = 0; j < dimLatent; j++){
			V.push_back((rand()  / double(RAND_MAX) * 2 - 1) * 0.01);
		}
		for(size_t j = 0; j < 24; j++) a[j].push_back(0);
	}
	
	for(size_t i = 0; i<NumAllFeats(); i++){
		P.push_back(0);
	}
	
	//cout << "NumAdFeature:" << numAdFeature << endl;
	//cout << "NumUserFeature:" << numUserFeature << endl;
	//cout << "NumAllFeature:" << NumAllFeats() << endl;
	//cout << "NumInstance" << numInstance << endl;
}



//calculate f(x) for P
double FeatureSelectionProblem::ScoreOfForP(size_t i, const std::vector<double>& weights) const{
	//f(x)=Px
	double score = 0.0;	
	for (size_t j = instance_starts[i]; j < instance_starts[i+1]; j++){
		score += weights[features[j]]*1.0;
	}
	return score;
}


//calculate f(x) for W when fix V and P
double FeatureSelectionProblem::ScoreOfForW(size_t i, const std::vector<double>& weights) const{
	//f(x)=(UW)(TV)' + Px
	double score = 0.0;
	DblVec UW;
	DblVec TV;
	
	for (size_t j = 0 ; j < dimLatent; j++){
		UW.push_back(0);
		TV.push_back(0);
	}
	for (size_t j = instance_starts[i]; j < instance_starts[i+1]; j++){
		score += weights[W.size()+features[j]];
		if(features[j] < NumAdFeats()){
			for(size_t k = 0; k < dimLatent; k++){
				int v_index = features[j] * dimLatent + k;
				TV[k]+=V[v_index];
			}
		} 
		else if(features[j] < NumAdFeats() + NumUserFeats()){
			for(size_t k = 0; k < dimLatent; k++){
				int w_index = (features[j] - NumAdFeats()) * dimLatent + k;
				UW[k] += weights[w_index];
			}
		}
	}
	
	for (size_t j = 0; j < dimLatent; j++){
		score += UW[j]*TV[j];
	}
	return score;
}


//calculate f(x) for V when fix W and P
double FeatureSelectionProblem::ScoreOfForV(size_t i, const std::vector<double>& weights) const{
	//f(x)=(UW)(TV)' + Px
	double score = 0.0;
	DblVec UW;
	DblVec TV;
	
	for (size_t j = 0 ; j < dimLatent; j++){
		UW.push_back(0);
		TV.push_back(0);
	}
	for (size_t j = instance_starts[i]; j < instance_starts[i+1]; j++){
		score += weights[V.size()+features[j]];
		if(features[j] < NumAdFeats()){
			for(size_t k = 0; k < dimLatent; k++){
				int v_index = features[j] * dimLatent + k;
				TV[k]+=weights[v_index];
			}
		} 
		else if(features[j] < NumAdFeats() + NumUserFeats()){
			for(size_t k = 0; k < dimLatent; k++){
				int w_index = (features[j] - NumAdFeats())*dimLatent + k;
				UW[k] += W[w_index];
			}
		}
	}
	
	for (size_t j = 0; j < dimLatent; j++){
		score += UW[j]*TV[j];
	}

	return score;
}


/*
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
*/
void displayGradient(DblVec& gradientP){
	cout << "DEBUG DISPLAY GRADIENT\n";
	for(size_t i = 0; i < gradientP.size(); i++){
		if(gradientP[i] != 0) cout << i << " : " << gradientP[i] << endl;
	}
	cout << "DEBUG DISPLAY GRADIENT OVER\n";
}

double FeatureSelectionObjectiveInit::EvalLocal(const DblVec& input, DblVec& gradientP){
	double loss = 0.0;
	for(size_t i = 0; i < input.size(); i++){
		loss += 0.5*input[i]*input[i]*l2weight;
		gradientP[i] = l2weight*input[i];
	}
	
	for(size_t i = 0; i < problem.NumInstance(); i++){
		double score = problem.ScoreOfForP(i, input);
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
				insLoss = log(temp);
				insProb = 1.0/ temp;
			}
			loss +=  problem.ClkOf(i) * insLoss;
			problem.AddMultToP(i, -1.0*problem.ClkOf(i)*(1.0 - insProb), gradientP);
		}

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
	}
	return loss;
}

//Fix User part, fix UW,  calculate the gradient of V
double FeatureSelectionObjectiveFixUser::EvalLocal(const DblVec& V, DblVec& gradientV){
	double loss = 0.0;
	double epsilon = problem.getEpsilon();
	size_t dimLatent = problem.getDimLatent();
	DblVec& W = problem.getW();
	DblVec& P = problem.getP();
	for(size_t i = 0; i < P.size(); i++){
		loss += 0.5*P[i]*P[i]*l2weight;
	}
	for(size_t i = 0; i < problem.NumUserFeats(); i++){
		double sum = 0;
		for (size_t j = 0; j < dimLatent; j++){
			sum += W[i*dimLatent + j] * W[i*dimLatent + j];
		}
		sum += epsilon;
		loss += sqrt(sum) * l21weight;
	}
	for(size_t i = 0; i < problem.NumAdFeats(); i++){
		double sum = 0;
		for (size_t j = 0; j < dimLatent; j++){
			sum += V[i*dimLatent + j] * V[i*dimLatent + j];
		}
		sum += epsilon;
		double sqrt_sum = sqrt(sum);
		
		for (size_t j = 0; j < dimLatent; j++){
			gradientV[i*dimLatent + j] = l21weight * V[i*dimLatent + j] / sqrt_sum;
		}
		loss += sqrt_sum * l21weight;
	}
	
	for(size_t i = 0; i < problem.NumInstance(); i++){
		double score = problem.ScoreOfForV(i, V);
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
				insLoss = log(temp);
				insProb = 1.0/ temp;
			}
			loss +=  problem.ClkOf(i) * insLoss;
			problem.AddMultToV(i, -1.0*problem.ClkOf(i)*(1.0 - insProb), gradientV);
		}

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
			problem.AddMultToV(i, 1.0*problem.NonClkOf(i)*(1.0 - insProb), gradientV);
		}		
	}
	return loss;
}


//Fix Ad part, fix TV,  calculate the gradient of W
double FeatureSelectionObjectiveFixAd::EvalLocal(const DblVec& W, DblVec& gradientW){
	double loss = 0.0;
	double epsilon = problem.getEpsilon();
	size_t dimLatent = problem.getDimLatent();
	DblVec& V = problem.getV();
	DblVec& P = problem.getP();
	for(size_t i = 0; i < P.size(); i++){
		loss += 0.5*P[i]*P[i]*l2weight;
	}
	for(size_t i = 0; i < problem.NumAdFeats(); i++){
		double sum = 0;
		for (size_t j = 0; j < dimLatent; j++){
			sum += V[i*dimLatent + j] * V[i*dimLatent + j];
		}
		sum += epsilon;
		loss += sqrt(sum) * l21weight;
	}
	for(size_t i = 0; i < problem.NumUserFeats(); i++){
		double sum = 0;
		for (size_t j = 0; j < dimLatent; j++){
			sum += W[i*dimLatent + j] * W[i*dimLatent + j];
		}
		sum += epsilon;
		double sqrt_sum = sqrt(sum);
		
		for (size_t j = 0; j < dimLatent; j++){
			gradientW[i*dimLatent + j] = l21weight * W[i*dimLatent + j] / sqrt_sum;
		}
		loss += sqrt_sum * l21weight;
	}
	for(size_t i = 0; i < problem.NumInstance(); i++){
		double score = problem.ScoreOfForW(i, W);
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
				insLoss = log(temp);
				insProb = 1.0/ temp;
			}
			loss +=  problem.ClkOf(i) * insLoss;
			problem.AddMultToW(i, -1.0*problem.ClkOf(i)*(1.0 - insProb), gradientW);
		}

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
			problem.AddMultToW(i, 1.0*problem.NonClkOf(i)*(1.0 - insProb), gradientW);
		}		
	}
	return loss;
}

/*
MultiThread
*/

struct Parameter{
	DifferentiableFunction &obj;
	const DblVec&input;
	DblVec&gradient;
	double& loss;
	int threadId;
	int threadNum;
	Parameter(DifferentiableFunction &obj, const DblVec&input, DblVec&gradient, double& loss, int threadId, int threadNum):obj(obj),input(input),gradient(gradient),loss(loss), threadId(threadId), threadNum(threadNum){}
};

void* ThreadEvalLocalForP(void * arg){
	
	Parameter* p  = ( Parameter*) arg;
	p->loss = 0.0;
	FeatureSelectionObjectiveInit& o = (FeatureSelectionObjectiveInit&)(p->obj);
	for (size_t i = 0; i < p->input.size(); i++){
		p->loss += 0.5 * p->input[i] * p->input[i] * o.l2weight / p->threadNum;
		p->gradient[i] = o.l2weight * p->input[i] / p->threadNum;
	}

	for(size_t i = 0; i < o.problem.NumInstance(); i++){
		if(i % p->threadNum != p->threadId) continue;
		double score = o.problem.ScoreOfForP(i, p->input);
		double insLoss, insProb;
		if(o.problem.ClkOf(i) > 0){
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
			p->loss +=  o.problem.ClkOf(i) * insLoss;
			o.problem.AddMultToP(i, -1.0*o.problem.ClkOf(i)*(1.0 - insProb), p->gradient);
		}

		if(o.problem.NonClkOf(i) > 0){
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
			p->loss +=  o.problem.NonClkOf(i) * insLoss;
			o.problem.AddMultToP(i, 1.0*o.problem.NonClkOf(i)*(1.0 - insProb), p->gradient);
		}		
	}
}

void* ThreadEvalLocalForV(void * arg){
	Parameter* p  = ( Parameter*) arg;
	p->loss = 0.0;
	FeatureSelectionObjectiveFixUser& o = (FeatureSelectionObjectiveFixUser&)(p->obj);
	
	double epsilon = o.problem.getEpsilon();
	size_t dimLatent = o.problem.getDimLatent();
	DblVec& W = o.problem.getW();
	
	int Vsize = o.problem.NumAdFeats()*dimLatent;
	int Psize = o.problem.getP().size();
	for(size_t i = 0; i < Psize; i++){
		p->loss += 0.5*p->input[Vsize+i]*p->input[Vsize+i]*o.l2weight / p->threadNum;
		p->gradient[i+Vsize] = o.l2weight * p->input[i+Vsize] / p->threadNum;
	}
	int num_user_feats = o.problem.NumUserFeats();
	for(size_t i = 0; i < num_user_feats; i++){
		double sum = 0.0;
		for (size_t j = 0; j < dimLatent; j++){
			sum += W[i*dimLatent + j] * W[i*dimLatent + j] ;
		}
		sum += epsilon;
		p->loss += sqrt(sum) * o.l21weight / p->threadNum;
	}
	int num_ad_feats = o.problem.NumAdFeats();
	for(size_t i = 0; i < num_ad_feats; i++){
		double sum = 0;
		for (size_t j = 0; j < dimLatent; j++){
			sum += p->input[i*dimLatent + j] * p->input[i*dimLatent + j];
		}
		sum += epsilon;
		double sqrt_sum = sqrt(sum);
		
		for (size_t j = 0; j < dimLatent; j++){
			p->gradient[i*dimLatent + j] = o.l21weight * p->input[i*dimLatent + j] / sqrt_sum / p->threadNum;
		}
		p->loss += sqrt_sum * o.l21weight / p->threadNum;
	}
	
	int num_ins = o.problem.NumInstance();
	for(size_t i = 0; i < num_ins; i++){
		if(i%p->threadNum != p->threadId) continue;
		double score = o.problem.ScoreOfForV(i, p->input);
		double insLoss, insProb;
		if(o.problem.ClkOf(i) > 0){
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
			p->loss +=  o.problem.ClkOf(i) * insLoss;
			o.problem.AddMultToV(i, -1.0*o.problem.ClkOf(i)*(1.0 - insProb), p->gradient, p->threadId);
		}

		if(o.problem.NonClkOf(i) > 0){
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
			p->loss +=  o.problem.NonClkOf(i) * insLoss;
			o.problem.AddMultToV(i, 1.0*o.problem.NonClkOf(i)*(1.0 - insProb), p->gradient, p->threadId);
		}		
	}
}


void* ThreadEvalLocalForW(void * arg){
	Parameter* p  = ( Parameter*) arg;
	p->loss = 0.0;
	FeatureSelectionObjectiveFixAd& o = (FeatureSelectionObjectiveFixAd&)(p->obj);
	
	double epsilon = o.problem.getEpsilon();
	size_t dimLatent = o.problem.getDimLatent();
	DblVec& V = o.problem.getV();
	int Wsize = o.problem.NumUserFeats()*dimLatent;
	int Psize = o.problem.getP().size();
	
	for(size_t i = 0; i < PSize; i++){
		p->loss += 0.5*p->input[Wsize+i]*p->input[Wsize+i]*o.l2weight / p->threadNum;
		p->gradient[i+Wsize] = o.l2weight * p->input[i+Wsize] / p->threadNum;
	}
	int num_ad_feats = o.problem.NumAdFeats();
	for(size_t i = 0; i < num_ad_feats; i++){
		double sum = 0;
		for (size_t j = 0; j < dimLatent; j++){
			sum += V[i*dimLatent + j] * V[i*dimLatent + j];
		}
		sum += epsilon;
		p->loss += sqrt(sum) * o.l21weight / p->threadNum;
	}
	int num_user_feats = o.problem.NumUserFeats();
	for(size_t i = 0; i <num_user_feats; i++){
		double sum = 0;
		for (size_t j = 0; j < dimLatent; j++){
			sum += p->input[i*dimLatent + j] * p->input[i*dimLatent + j];
		}
		sum += epsilon;
		double sqrt_sum = sqrt(sum);
		
		for (size_t j = 0; j < dimLatent; j++){
			p->gradient[i*dimLatent + j] = o.l21weight * p->input[i*dimLatent + j] / sqrt_sum / p->threadNum;
		}
		p->loss += sqrt_sum * o.l21weight / p->threadNum;
	}
	int num_ins = o.problem.NumInstance();
	for(size_t i = 0; i < num_ins; i++){
		if(i % p->threadNum != p->threadId) continue;
		double score = o.problem.ScoreOfForW(i, p->input);
		double insLoss, insProb;
		if(o.problem.ClkOf(i) > 0){
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
			p->loss +=  o.problem.ClkOf(i) * insLoss;
			o.problem.AddMultToW(i, -1.0*o.problem.ClkOf(i)*(1.0 - insProb), p->gradient, p->threadId);
		}

		if(o.problem.NonClkOf(i) > 0){
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
			p->loss +=  o.problem.NonClkOf(i) * insLoss;
			o.problem.AddMultToW(i, 1.0*o.problem.NonClkOf(i)*(1.0 - insProb), p->gradient, p->threadId);
		}		
	}
}


double FeatureSelectionObjectiveInit::EvalLocalMultiThread(const DblVec& input, DblVec& gradient){
	
	/*
	create 24 thread;
	each thread calculate a loss and gradient
	*/
	int threadNum = 24;
	double lossList[threadNum];
	DblVec *gradList = new DblVec[threadNum];
	for(int i = 0; i < threadNum; i++){
		gradList[i] = DblVec(gradient.size());
	}
	threadList = new pthread_t[threadNum];
	
	for(int i = 0; i < threadNum; i++){
		Parameter*p = new Parameter(*this, input, gradList[i], lossList[i], i, threadNum);
		pthread_create(&threadList[i], NULL, ThreadEvalLocalForP, p);
	}
	
	for(int i = 0; i < threadNum; i++){
		pthread_join(threadList[i], NULL);
	}
	
	double loss = 0.0;
	for(int j = 0; j < gradient.size(); j++){
		gradient[j] = 0.0;
	}
	for(int i = 0; i < threadNum; i++){
		loss += lossList[i];
		for(int j = 0; j < gradient.size(); j++){
			gradient[j] += gradList[i][j];
		}
	}
	delete []gradList;
	delete []threadList;
	return loss;
	
}



double FeatureSelectionObjectiveFixAd::EvalLocalMultiThread(const DblVec& input, DblVec& gradient){
	
	/*
	create 24 thread;
	each thread calculate a loss and gradient
	*/
	int threadNum = 24;
	double lossList[threadNum];
	DblVec *gradList = new DblVec[threadNum];
	for(int i = 0; i < threadNum; i++){
		gradList[i] = DblVec(gradient.size());
	}
	threadList = new pthread_t[threadNum];
	
	for(int i = 0; i < threadNum; i++){
		Parameter*p = new Parameter(*this, input, gradList[i], lossList[i], i, threadNum);
		pthread_create(&threadList[i], NULL, ThreadEvalLocalForW, p);
	}
	
	for(int i = 0; i < threadNum; i++){
		pthread_join(threadList[i], NULL);
	}
	
	double loss = 0.0;
	for(int j = 0; j < gradient.size(); j++){
		gradient[j] = 0.0;
	}
	for(int i = 0; i < threadNum; i++){
		loss += lossList[i];
		for(int j = 0; j < gradient.size(); j++){
			gradient[j] += gradList[i][j];
		}
	}
	delete []gradList;
	delete []threadList;
	return loss;
	
}

double FeatureSelectionObjectiveFixUser::EvalLocalMultiThread(const DblVec& input, DblVec& gradient){
	
	/*
	create 24 thread;
	each thread calculate a loss and gradient
	*/
	int threadNum = 24;
	double lossList[threadNum];
	DblVec *gradList = new DblVec[threadNum];
	for(int i = 0; i < threadNum; i++){
		gradList[i] = DblVec(gradient.size());
	}
	threadList = new pthread_t[threadNum];
	
	for(int i = 0; i < threadNum; i++){
		Parameter*p = new Parameter(*this, input, gradList[i], lossList[i], i, threadNum);
		pthread_create(&threadList[i], NULL, ThreadEvalLocalForV, p);
	}
	
	for(int i = 0; i < threadNum; i++){
		pthread_join(threadList[i], NULL);
	}
	
	double loss = 0.0;
	for(int j = 0; j < gradient.size(); j++){
		gradient[j] = 0.0;
	}
	for(int i = 0; i < threadNum; i++){
		loss += lossList[i];
		for(int j = 0; j < gradient.size(); j++){
			gradient[j] += gradList[i][j];
		}
	}
	delete []gradList;
	delete []threadList;
	return loss;
	
}


int FeatureSelectionObjectiveInit::handler(size_t rankid, size_t command){
	MPI_Bcast(&command, 1, MPI_INT, 0, MPI_COMM_WORLD);
	if(command == 0) return 0;
	return 1;
}

int FeatureSelectionObjectiveFixUser::handler(size_t rankid, size_t command){
	MPI_Bcast(&command, 1, MPI_INT, 0, MPI_COMM_WORLD);
	if(command == 0) return 0;
	return 1;
}

int FeatureSelectionObjectiveFixAd::handler(size_t rankid, size_t command){
	MPI_Bcast(&command, 1, MPI_INT, 0, MPI_COMM_WORLD);
	if(command == 0) return 0;
	return 1;
}



double FeatureSelectionObjectiveInit::Eval(const DblVec& input, DblVec& gradient){
	DblVec localInput = input;
	DblVec localGradient = gradient;
	MPI_Bcast(&localInput[0], localInput.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);	
	MPI_Bcast(&localGradient[0], localGradient.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
	double loss = EvalLocalMultiThread(localInput, localGradient);	
//	double loss = EvalLocal(localInput, localGradient);	
	double gloss = 0.0;
	MPI_Reduce(&localGradient[0], &gradient[0], input.size(), MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&loss, &gloss, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	return gloss;
}


double FeatureSelectionObjectiveFixUser::Eval(const DblVec& input, DblVec& gradient){
	DblVec localInput = input;
	DblVec localGradient = gradient;
	MPI_Bcast(&localInput[0], localInput.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);	
	MPI_Bcast(&localGradient[0], localGradient.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
	double loss = EvalLocalMultiThread(localInput, localGradient);	
//	double loss = EvalLocal(localInput, localGradient);	
	double gloss = 0.0;
	MPI_Reduce(&localGradient[0], &gradient[0], input.size(), MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&loss, &gloss, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	return gloss;
}

double FeatureSelectionObjectiveFixAd::Eval(const DblVec& input, DblVec& gradient){
	DblVec localInput = input;
	DblVec localGradient = gradient;
	MPI_Bcast(&localInput[0], localInput.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);	
	MPI_Bcast(&localGradient[0], localGradient.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
	double loss = EvalLocalMultiThread(localInput, localGradient);	
//	double loss = EvalLocal(localInput, localGradient);	
	double gloss = 0.0;
	MPI_Reduce(&localGradient[0], &gradient[0], input.size(), MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&loss, &gloss, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	return gloss;
}



