#ifndef _TERMCRIT_
#define _TERMCRIT_

#include <deque>
#include <iostream>
#include <string>

class OptimizerState;
struct TerminationCriterion {
	virtual double GetValue(const OptimizerState& state, std::ostream& message) = 0;
	virtual ~TerminationCriterion() {}
};

class RelativeMeanImprovementCriterion : public TerminationCriterion {
	const int numItersToAvg;
	std::deque<double> prevVals;
	
public:
	RelativeMeanImprovementCriterion(int numItersToAvg = 5): numItersToAvg(numItersToAvg) {}
	
	double GetValue(const OptimizerState& state, std::ostream& message);
};

#endif 
