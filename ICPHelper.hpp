#ifndef ICPHELPER_H
#define ICPHELPER_H

#include <vector>
#include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "ANNHelper.hpp"

class ICPHelper {
public:
	ICPHelper (){}
	~ICPHelper (){}

	cv::Point2d findClosestPt (cv::Point2d pt, std::vector<cv::Point2d> ref);
	void icpIter (std::vector<cv::Point2d> &src, std::vector<cv::Point2d> &tgt, ANNHelper &ann);

	cv::Point2d t, mean_closest, mean_src;
	double r;

private:
	void initialization (std::vector<cv::Point2d> &closestPt, std::vector<cv::Point2d> &src);
	cv::Point2d getMean (std::vector<cv::Point2d> &pt);
	double distance (cv::Point2d a, cv::Point2d b);
};

#endif