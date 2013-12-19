#ifndef _LOGREG_
#define _LOGREG_

#include <deque>
#include <vector>
#include <cmath>
#include <iostream>
#include "inslookup.h"
#include "OWLQN.h"


class LogisticRegressionProblem {
	std::deque<size_t> indices;
	std::deque<float> values;
	std::deque<size_t> instance_starts;
	std::deque<bool> labels;
	size_t numFeats;
	size_t rankid;
	
public:	
	LogisticRegressionProblem(const char*train, size_t rankid);
	void AddInstance(const std::deque<size_t>& inds, const std::deque<float>& vals, bool label);
	void AddInstance(const std::vector<float>& vals, bool label);
	double ScoreOf(size_t i, const std::vector<double>& weights) const;
	
	bool LabelOf(size_t i) const {
		return labels[i];
	}
	
	void AddMultTo(size_t i, double mult, std::vector<double>& vec) const {
		if (labels[i]) mult *= -1;
		for (size_t j = instance_starts[i]; j < instance_starts[i+1]; j++){
			vec[indices[j]] += mult;
		}
	}
	
	size_t NumInstances() const {
		return labels.size();
	}
	
	size_t NumFeats() const {
		return numFeats;
	}
	
	size_t getRankId() const{
		return rankid;
	}
};


struct LogisticRegressionObjective : public DifferentiableFunction {
	const LogisticRegressionProblem& problem;
	const double l2weight;
	LogisticRegressionObjective(const LogisticRegressionProblem& p, double l2weight = 0) : problem(p), l2weight(l2weight){	}
	~LogisticRegressionObjective(){}
	double Eval(const DblVec& input, DblVec& gradient);
	double EvalLocal(const DblVec& input, DblVec& gradient);
	int handler(size_t rankid, size_t command); 
	double EvalLocalMultiThread(const DblVec& input, DblVec& gradient);
};

#endif
