#include "inslookup.h"
#include "logreg.h"
#include <fstream>
#include <sstream>
#include <string>
#include <mpi.h>

LogisticRegressionProblem::LogisticRegressionProblem(const char* ins_path, size_t rankid):rankid(rankid){
	init();
	load_feamap("./FeaDict.dat");
	trans_ins(ins_path, rankid, indices, values, instance_starts, labels, numFeats);
}
	
double LogisticRegressionProblem::ScoreOf(size_t i, const vector<double>& weights) const{
	double score = 0;
	for (size_t j = instance_starts[i]; j < instance_starts[i+1]; j++){
		double value = values[j];
		size_t index = (indices.size() > 0) ? indices[j] : j - instance_starts[i];
		score += weights[index] * value;
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
	DblVec localInput = input;
	DblVec localGradient = gradient;
	MPI_Bcast(&localInput[0], localInput.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);	
	MPI_Bcast(&localGradient[0], localGradient.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
	double loss = EvalLocal(localInput, localGradient);	
	double gloss = 0.0;
	MPI_Reduce(&localGradient[0], &gradient[0], input.size(), MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&loss, &gloss, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
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


int LogisticRegressionObjective::handler(size_t rankid, size_t command){
	MPI_Bcast(&command, 1, MPI_INT, 0, MPI_COMM_WORLD);
	if(command == 0) return 0;
	return 1;
}


