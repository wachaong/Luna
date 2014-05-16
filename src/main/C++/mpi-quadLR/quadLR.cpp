#include "quadLR.h"
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
 f(x) = (UXT)' + Px
 p(x) = g(f(x)) = 1 / (1+e^-f(x))
 
 L(X,P) = ylog(p(x)) + (1-y)log(1-p(x))
 R(X) = ||X||_F + ||P||_2
 	
*/
pthread_t* threadList;

extern char feamap_path[2048];
extern char ins_path[2048];
extern int init_inslookup(const char*instance_file, const char* feature_file, const char* ad, const char* user);
QuadLRProblem::QuadLRProblem(const char* instance_file, const char* feature_file, const char* randmat_path_ad, const char* randmat_path_user, size_t rankid){
	//initialize
	//read instances and features from files
	//save into four deques
	//features,instance_starts, nonclk, clk
	init_inslookup(instance_file, feature_file, randmat_path_ad, randmat_path_user);
	load_feamap(feamap_path);
	load_randmat_for_user(randmat_path_user);
	load_randmat_for_ad(randmat_path_ad);
	trans_ins(ins_path, rankid, features, instance_starts, nonclk, clk, numInstance, rpFeatures, rpInstance_starts);
	
	numAdFeature = getAdFeaCount();
	numRpAdFeature = getRpAdFeaCount();
	numUserFeature = getUserFeaCount();
	numRpUserFeature = getRpUserFeaCount();
	numOtherFeature = getOtherFeaCount();
}

//calculate f(x) for P
double QuadLRProblem::ScoreOf(size_t ins, const std::vector<double>& weights){
	//f(x)=Px+tXu
	double score = 0.0;	
	int a_size = 0;
	int u_size = 0;
	for (size_t j = rpInstance_starts[ins]; j < rpInstance_starts[ins+1]; j++){
		size_t index = rpFeatures[j];
		//Ad Feature
		if(index < numRpAdFeature){
			a[thread_id][a_size++] = index;
		}
		//User Feature
		else if(index < numRpAdFeature + numRpUserFeature){
			u[thread_id][u_size++] = index-numRpAdFeature;
		}
	}
	for(size_t i = 0; i < a_size; i++){
		for(size_t j = 0; j < u_size; j++){
			score += weights[a[thread_id][i]*numRpUserFeature + u[thread_id][j]];
		}
	}
		
	for (size_t j = instance_starts[ins]; j < instance_starts[ins+1]; j++){
		if(features[j] >= NumAdFeats()+NumUserFeats())
			score += weights[numRpAdFeature * numRpUserFeature + features[j]]*1.0;
	}
	return score;
}



void displayGradient(DblVec& gradientP){
	cout << "DEBUG DISPLAY GRADIENT\n";
	for(size_t i = 0; i < gradientP.size(); i++){
		if(gradientP[i] != 0) cout << i << " : " << gradientP[i] << endl;
	}
	cout << "DEBUG DISPLAY GRADIENT OVER\n";
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

void* ThreadEvalLocal(void * arg){
	
	Parameter* p  = ( Parameter*) arg;
	p->loss = 0.0;
	QuadLRObjective& o = (QuadLRObjective&)(p->obj);
	for (size_t i = 0; i < p->input.size(); i++){
		p->loss += 0.5 * p->input[i] * p->input[i] * o.l2weight / p->threadNum;
		p->gradient[i] = o.l2weight * p->input[i] / p->threadNum;
	}

	for(size_t i = 0; i < o.problem.NumInstance(); i++){
		if(i % p->threadNum != p->threadId) continue;
		double score = o.problem.ScoreOf(i, p->input, p->threadId);
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
			o.problem.AddMultTo(i, -1.0*o.problem.ClkOf(i)*(1.0 - insProb), p->gradient, p->threadId);
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
			p->loss +=  10.0*o.problem.NonClkOf(i) * insLoss;
			o.problem.AddMultTo(i, 10.0*o.problem.NonClkOf(i)*(1.0 - insProb), p->gradient, p->threadId);
		}		
	}
}




double QuadLRObjective::EvalLocalMultiThread(const DblVec& input, DblVec& gradient){
	
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
		pthread_create(&threadList[i], NULL, ThreadEvalLocal, p);
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

int QuadLRObjective::handler(size_t rankid, size_t command){
	MPI_Bcast(&command, 1, MPI_INT, 0, MPI_COMM_WORLD);
	if(command == 0) return 0;
	return 1;
}



double QuadLRObjective::Eval(const DblVec& input, DblVec& gradient){
	DblVec localInput = input;
	DblVec localGradient = gradient;
	MPI_Bcast(&localInput[0], localInput.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);	
	MPI_Bcast(&localGradient[0], localGradient.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
	double loss = EvalLocalMultiThread(localInput, localGradient);	
	double gloss = 0.0;
	MPI_Reduce(&localGradient[0], &gradient[0], input.size(), MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&loss, &gloss, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	return gloss;
}





