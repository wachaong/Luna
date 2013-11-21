#include <iostream>
#include "score.h""

int main(int argc, char** argv) {
	init("feat", "model", "ins");
	score_ins("score");
	return 0;
}
