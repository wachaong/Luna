#include "inslookup.h"
#include "logreg.h"
#include <fstream>
#include <sstream>
#include <string>
#include <mpi.h>
#include <pthread.h>
#include <time.h>

typedef long clock_t;
pthread_t* threadList;

LogisticRegressionProblem::LogisticRegressionProblem(const char* ins_path, size_t rankid):rankid(rankid){
	init();
	load_feamap("./FeaDict.dat");
//	load_feamap("feat");
	trans_ins(ins_path, rankid, indices, values, instance_starts, labels, numFeats);
}
	
double LogisticRegressionProblem::ScoreOf(size_t i, const vector<double>& weights) const{
	double score = 0;
	for (size_t j = instance_starts[i]; j < instance_starts[i+1]; j++){
	//	double value = values[j];
	//	size_t index = (indices.size() > 0) ? indices[j] : j - instance_starts[i];
	//	score += weights[index] * value;
		score += weights[indices[j]];
	}
		
	if(!labels[i]) score *= -1;
	return score;
}

void displayGradient(DblVec& gradientP){
	cout << "DEBUG DISPLAY GRADIENT\n";
	for(size_t i = 0; i < gradientP.size(); i++){
		if(gradientP[i] != 0) cout << i << " : " << gradientP[i] << endl;
	}
	cout << "DEBUG DISPLAY GRADIENT OVER\n";
}

double LogisticRegressionObjective::Eval(const DblVec& input, DblVec& gradient){
//	clock_t start,finish;
//	double totaltime;

	DblVec localInput = input;
	DblVec localGradient = gradient;
	MPI_Bcast(&localInput[0], localInput.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);	
	MPI_Bcast(&localGradient[0], localGradient.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);

	

	double loss = EvalLocalMultiThread(localInput, localGradient);
	
	
//	start=clock();
//	double loss = EvalLocal(localInput, localGradient);	
	double gloss = 0.0;
	MPI_Reduce(&localGradient[0], &gradient[0], input.size(), MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&loss, &gloss, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
//	finish=clock();
//	totaltime=(double)(finish-start)/CLOCKS_PER_SEC;
//	cout<<"\nReduce Time: "<<totaltime<<"seconds"<<endl;
	return gloss;
}

double LogisticRegressionObjective::EvalLocal(const DblVec& input, DblVec& gradient){
	double loss = 0.0;
	for (size_t i = 0; i < input.size(); i++){
		loss += 0.5 * input[i] * input[i] * l2weight;
		gradient[i] = l2weight * input[i];
	}
	

	for (size_t i = 0; i < problem.NumInstances(); i++){
		double score = problem.ScoreOf(i, input);
		double insLoss, insProb;
		if (score < -30){
			insLoss = -score;
			insProb = 0;
		}else if (score > 30){
			insLoss = 0;
			insProb = 1;
		}else {
			double temp = 1.0 + exp(-score);
			insLoss = log(temp);
			insProb = 1.0/ temp;
		}
		loss += insLoss;
		problem.AddMultTo(i, 1.0 - insProb, gradient);	
	}
	return loss ;
}

/*
MultiThread
*/

struct Parameter{
	LogisticRegressionObjective &obj;
	const DblVec&input;
	DblVec& gradient;
	double& loss;
	int threadId;
	int threadNum;
	Parameter(LogisticRegressionObjective &obj, const DblVec&input, DblVec&gradient, double& loss, int threadId, int threadNum):obj(obj),input(input),gradient(gradient),loss(loss), threadId(threadId), threadNum(threadNum){}
};

void* ThreadEvalLocal(void * arg){
	clock_t start,finish;
	double totaltime;
	
	Parameter* p  = ( Parameter*) arg;

	p->loss = 0.0;
	int NumIns = p->obj.problem.NumInstances();
//	start=clock();
	for (size_t i = 0; i < NumIns; i++){
		if(i%p->threadNum != p->threadId) continue;
		double score = p->obj.problem.ScoreOf(i, p->input);
		double insLoss, insProb;
		if (score < -30){
			insLoss = -score;
			insProb = 0;
		}else if (score > 30){
			insLoss = 0;
			insProb = 1;
		}else {
			double temp = 1.0 + exp(-score);
			insLoss = log(temp);
			insProb = 1.0/ temp;
		}
		p->loss += insLoss;
		p->obj.problem.AddMultTo(i, 1.0 - insProb, p->gradient);	
	}
	
//	finish=clock();
//	if(p->threadId == 0 && p->obj.problem.getRankId() == 0){
//		totaltime=(double)(finish-start)/CLOCKS_PER_SEC;
//		cout<<"All Time: "<<totaltime<<"seconds"<<endl;
//	}
	
}

double LogisticRegressionObjective::EvalLocalMultiThread(const DblVec& input, DblVec& gradient){
	
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


int LogisticRegressionObjective::handler(size_t rankid, size_t command){
	MPI_Bcast(&command, 1, MPI_INT, 0, MPI_COMM_WORLD);
	if(command == 0) return 0;
	return 1;
}


