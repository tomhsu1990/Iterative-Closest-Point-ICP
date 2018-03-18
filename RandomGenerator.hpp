#ifndef RANDOMGENERATOR_H
#define RANDOMGENERATOR_H

#include <random>

#include <opencv2/core/core.hpp>

class RandomGenerator {
public:
	RandomGenerator ():col(640), row(480){}
	RandomGenerator (int c, int r):col(c), row(r){}
	~RandomGenerator (){}
	
	cv::Point2d getRandomPoint();
	int getRandomPoint(int size);

	int col, row;
	std::default_random_engine gen;
};

#endif