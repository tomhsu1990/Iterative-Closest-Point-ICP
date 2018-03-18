#include "RandomGenerator.hpp"

cv::Point2d RandomGenerator::getRandomPoint(){
    std::uniform_int_distribution<int> c(0, col-1);
    std::uniform_int_distribution<int> r(0, row-1);
    return cv::Point2d(c(gen), r(gen));
}

int RandomGenerator::getRandomPoint(int size){
    std::uniform_int_distribution<int> deg(0, size-1);
    return deg(gen);
}