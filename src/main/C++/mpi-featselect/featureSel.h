#ifndef _FEATURESEL_
#define _FEATURESEL_

#include <mpi.h>
#include <deque>
#include <vector>
#include <stdio.h>
#include "OWLQN.h"

typedef std::vector<double> DblVec;
typedef std::vector<double>* DblMat;


class FeatureSelectionProblem{
	size_t numInstance;
	size_t numAdFeature;
	size_t numUserFeature;
	size_t numOtherFeature;
	size_t dimLatent;
	
	std::deque<size_t> features;
	std::deque<size_t> instance_starts;
	std::deque<size_t> nonclk;
	std::deque<size_t> clk;
	
	DblVec W;
	DblVec V;
	DblVec P;
	double epsilon;
public:
	FeatureSelectionProblem(const char* instance_file, const char* feature_file, int K, size_t rankid);
	void AddInstance();

	double ScoreOfForP(size_t i, const std::vector<double>& weights) const;
	double ScoreOfForW(size_t i, const std::vector<double>& weights) const;
	double ScoreOfForV(size_t i, const std::vector<double>& weights) const;
	/*
	double GroupLasso() const;
	
	DblMat getWAsMat() const{
		DblMat WMat = new DblVec[numUserFeature];
		for(size_t i = 0; i < W.size(); i++){
			WMat[i/dimLatent].push_back(W[i]);
		}
		return WMat;
	}
	DblMat getVAsMat() const{
		DblMat VMat = new DblVec[numAdFeature];
		for(size_t i = 0; i < V.size(); i++){
			VMat[i/dimLatent].push_back(V[i]);
		}
		return VMat;
	}
	*/
	void AddMultToP(size_t i, double mult,  std::vector<double>& vec) const {
		for (size_t j = instance_starts[i]; j < instance_starts[i+1]; j++){
			size_t index = features[j] ;
			vec[index] += mult * 1.0;
		}
	}
	
	//mult* u*w_j * T_i
	void AddMultToV(size_t i, double mult, std::vector<double> &vec) const{
		DblVec u, a;
		for (size_t j = instance_starts[i]; j < instance_starts[i+1]; j++){
			size_t index = features[j];
			//Ad Feature
			if(index < numAdFeature){
				a.push_back(index);
			}
			//User Feature
			else if(index < numAdFeature + numUserFeature){
				u.push_back(index-numAdFeature);
			}
		}
		for(size_t a_index = 0; a_index < a.size(); a_index++){
			size_t i_index = a[a_index];
			for(size_t j_index = 0; j_index < dimLatent; j_index++){
				double sum = 0;
				for(size_t u_index = 0; u_index < u.size(); u_index++){
					sum += W[u[u_index]*dimLatent + j_index];
				}
				vec[i_index*dimLatent+j_index] += mult * sum;
			}
		}
	}
	//mult*u_i *T*v_j
	void AddMultToW(size_t i, double mult, std::vector<double> &vec) const{
		DblVec u, a;
		for (size_t j = instance_starts[i]; j < instance_starts[i+1]; j++){
			size_t index = features[j];
			//Ad Feature
			if(index < numAdFeature){
				a.push_back(index);
			}
			//User Feature
			else if(index < numAdFeature + numUserFeature){
				u.push_back(index-numAdFeature);
			}
		}
		for(size_t u_index = 0; u_index < u.size(); u_index++){
			size_t i_index = u[u_index];
			for(size_t j_index = 0; j_index < dimLatent; j_index++){
				double sum = 0;
				for(size_t a_index = 0; a_index < a.size(); a_index++){
					sum += V[a[a_index]*dimLatent+j_index];
				}
				vec[i_index*dimLatent+j_index] += mult * sum;
			}
		}
	}	
	
	void setW(DblVec& w){ W = w; }
	void setV(DblVec& v){ V = v; }
	void setP(DblVec& p){ P = p; }
	DblVec& getW(){ return W; }
	DblVec& getV() { return V;}
	DblVec& getP(){ return P;}
	
	size_t ClkOf(size_t i) const { return clk[i];}
	size_t NonClkOf(size_t i) const { return nonclk[i];}
	size_t NumInstance() const { return numInstance;}
	size_t NumAdFeats() const { return numAdFeature;}
	size_t NumUserFeats() const { return numUserFeature;}
	size_t NumOtherFeats() const { return numOtherFeature;}
	size_t NumAllFeats() const { return numAdFeature+numUserFeature+numOtherFeature;}
	size_t getDimLatent() const {return dimLatent;}
	double getEpsilon() const {return epsilon;}
};

struct FeatureSelectionObjectiveInit : public DifferentiableFunction {
	FeatureSelectionProblem& problem;
	const double l2weight;
	FeatureSelectionObjectiveInit(FeatureSelectionProblem& p, double l2weight = 0) : problem(p), l2weight(l2weight){ }
	~FeatureSelectionObjectiveInit(){}
	double Eval(const DblVec& input, DblVec& gradient);
	double EvalLocal(const DblVec& input, DblVec& gradient);
	int handler(size_t rankid, size_t command); 
	double EvalLocalMultiThread(const DblVec& input, DblVec& gradient);
};


struct FeatureSelectionObjectiveFixUser : public DifferentiableFunction {
	FeatureSelectionProblem& problem;
	const double l2weight;
	const double l21weight;
	FeatureSelectionObjectiveFixUser(FeatureSelectionProblem& p, double l21weight = 0, double l2weight = 0) : problem(p), l21weight(l21weight), l2weight(l2weight){}
	~FeatureSelectionObjectiveFixUser(){}
	double Eval(const DblVec& input, DblVec& gradient);
	double EvalLocal(const DblVec& input, DblVec& gradient);
	int handler(size_t rankid, size_t command); 
	double EvalLocalMultiThread(const DblVec& input, DblVec& gradient);
};


struct FeatureSelectionObjectiveFixAd : public DifferentiableFunction {
	FeatureSelectionProblem& problem;
	const double l2weight;
	const double l21weight;
	FeatureSelectionObjectiveFixAd(FeatureSelectionProblem& p, double l21weight = 0, double l2weight = 0) : problem(p), l21weight(l21weight), l2weight(l2weight){}
	~FeatureSelectionObjectiveFixAd(){}
	double Eval(const DblVec& input, DblVec& gradient);
	double EvalLocal(const DblVec& input, DblVec& gradient);
	int handler(size_t rankid, size_t command); 
	double EvalLocalMultiThread(const DblVec& input, DblVec& gradient);
};

#endif
