#ifndef _FEATURESEL_
#define _FEATURESEL_

#include <deque>
#include <vector>
#include <stdio.h>

typedef std::vector<double> DblVec;
typedef std::vector<double>* DblMat;

struct DifferentiableFunction {
	virtual double Eval(const DblVec& input, DblVec& gradient) = 0;
	virtual ~DifferentiableFunction() { }
};


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
	FeatureSelectionProblem(size_t numAdFeature, size_t numUserFeature, size_t numOtherFeature) : numAdFeature(numAdFeature), numUserFeature(numUserFeature), numOtherFeature(numOtherFeature){
		instance_starts.push_back(0);
	}
	
	FeatureSelectionProblem(const char* instance_file, const char* feature_file);
	void AddInstance();

	double ScoreOf(size_t i) const;
	double GroupLasso() const;
	
	void setW(const std::vector<double>& weights){
		for(size_t i = 0; i < weights.size(); i++){
		}
	}
	void setV(){
	}
	void setP(){
	}
	
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
	
	AddMultToP(size_t i, double mult,  std::vector<double>& vec) const {
		for (size_t j = instance_starts[i]; j < instance_starts[i+1]; j++){
			size_t index = features[j] ;
			vec[index] += mult * 1;
		}
	}
	
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
	
	
	
};

struct FeatureSelectionObjectiveInit : public DifferentiableFunction {
	const FeatureSelectionProblem& problem;
	const double l2weight;
	FeatureSelectionObjectiveInit(const FeatureSelectionProblem& p, double l2weight = 0) : problem(p), l2weight(l2weight){ }
	double Eval(const DblVec& input, DblVec& gradient);
};


struct FeatureSelectionObjectiveFixUser : public DifferentiableFunction {
	
};


struct FeatureSelectionObjectiveFixAd : public DifferentiableFunction {
	
};

#endif
