#ifndef _QUADLOGREG_
#define _QUADLOGREG_

#include <mpi.h>
#include <deque>
#include <vector>
#include <stdio.h>
#include "OWLQN.h"

typedef std::vector<double> DblVec;
typedef std::vector<int> IntVec;
typedef std::vector<double>* DblMat;


class QuadLRProblem{
	size_t numInstance;
	size_t numAdFeature;
	size_t numRpAdFeature;
	size_t numUserFeature;
	size_t numRpUserFeature;
	size_t numOtherFeature;
	
	std::deque<size_t> features;
	std::deque<size_t> instance_starts;
	std::deque<size_t> rpFeatures;
	std::deque<size_t> rpInstance_starts;
	std::deque<size_t> nonclk;
	std::deque<size_t> clk;
	
	IntVec a[24];
	IntVec u[24];
public:
	QuadLRProblem(const char* instance_file, const char* feature_file, const char* randmat_path_ad, const char* randmat_path_user, size_t rankid);
	double ScoreOf(size_t i, const std::vector<double>& weights, int thread_id) const;
	
	void AddMultTo(size_t i, double mult,  std::vector<double>& vec, int thread_id) const {
		int a_size = 0;
		int u_size = 0;
		for (size_t j = rpInstance_starts[i]; j < rpInstance_starts[i+1]; j++){
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
				vec[i*numRpUserFeature+j] += mult * 1.0;
			}
		}
		for (size_t j = instance_starts[i]; j < instance_starts[i+1]; j++){
			size_t index = features[j];
			if(index >= NumAdFeats()+NumUserFeats())
				vec[index+numRpUserFeature*numRpAdFeature] += mult*1.0;
		}
	}
	
	size_t ClkOf(size_t i) const { return clk[i];}
	size_t NonClkOf(size_t i) const { return nonclk[i];}
	size_t NumInstance() const { return numInstance;}
	size_t NumAdFeats() const { return numAdFeature;}
	size_t NumRpAdFeats() const { return numRpAdFeature;}
	size_t NumUserFeats() const { return numUserFeature;}
	size_t NumRpUserFeats() const { return numRpUserFeature;}
	size_t NumOtherFeats() const { return numOtherFeature;}
	size_t NumAllFeats() const { return numAdFeature+numUserFeature+numOtherFeature;}
};

struct QuadLRObjective : public DifferentiableFunction {
	QuadLRProblem& problem;
	const double l2weight;
	QuadLRObjective(QuadLRProblem& p, double l2weight = 0.0) : problem(p), l2weight(l2weight){ }
	~QuadLRObjective(){}
	double Eval(const DblVec& input, DblVec& gradient);
	double EvalLocal(const DblVec& input, DblVec& gradient);
	int handler(size_t rankid, size_t command); 
	double EvalLocalMultiThread(const DblVec& input, DblVec& gradient);
};


#endif
