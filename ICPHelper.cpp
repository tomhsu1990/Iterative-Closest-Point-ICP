#include "ICPHelper.hpp"

// can be improved by ANN or other nearest neighbor library
cv::Point2d ICPHelper::findClosestPt (cv::Point2d pt, std::vector<cv::Point2d> ref) {
	cv::Point2d ret(ref[0]);
	double min_distance(distance(pt, ref[0]));
	for (unsigned i=1;i<ref.size();++i) {
		double dist(distance(pt, ref[i]));
		if (dist < min_distance) {
			ret = ref[i];
			min_distance = dist;
		}
	}
	return ret;
}

void ICPHelper::icpIter (std::vector<cv::Point2d> &src, std::vector<cv::Point2d> &tgt) {
	// find closest point
	std::vector<cv::Point2d> closestPt;
	for (unsigned i=0;i<src.size();++i)
		closestPt.push_back(findClosestPt(src[i], tgt));
	
	// initialization
	initialization(closestPt, src);
	cv::Point2d src2closest(0,0), src2closest_inv(0,0);
	for (unsigned i=0;i<src.size();++i) {
		src2closest.x     += (src[i].x - mean_src.x)*(closestPt[i].x - mean_closest.x);
		src2closest.y     += (src[i].y - mean_src.y)*(closestPt[i].y - mean_closest.y);
		src2closest_inv.x += (src[i].y - mean_src.y)*(closestPt[i].x - mean_closest.x);
		src2closest_inv.y += (src[i].x - mean_src.x)*(closestPt[i].y - mean_closest.y);
	}

	// key:
	// After centering at each mean, every point lives in its own coordinate.
	// Suppose the rotation we want to compute is r.
	// r = r_closest - r_src.
	// Here, r_closest is the shifted closest points' coordinate relative to the global coordinate
	// Here, r_src is the shifted source points' coordinate relative to the global coordinate
	// the formula below is an approximation to r.
	// r = atan2(y'/x') - atan2(y/x), (x, y) is the source coordinate and (x', y') is the closest points' coordinate.
	// app(r) = (atan2(y'/x') - atan2(y/x))/(1+yy'/xx')
	// app(r) is proportional to r.
	r = atan2(src2closest_inv.y, src2closest.x+src2closest.y)-
		atan2(src2closest_inv.x, src2closest.x+src2closest.y);
	t.x = mean_closest.x - ((mean_src.x * cos(r)) - (mean_src.y * sin(r)));
	t.y = mean_closest.y - ((mean_src.x * sin(r)) + (mean_src.y * cos(r)));
}

void ICPHelper::initialization (std::vector<cv::Point2d> &closestPt, std::vector<cv::Point2d> &src) {
	t = cv::Point2d(0,0);
	r = 0;
	mean_closest = getMean(closestPt);
	mean_src = getMean(src);
}

cv::Point2d ICPHelper::getMean (std::vector<cv::Point2d> &pt) {
	cv::Point2d mean(0,0);
	for (unsigned i=0;i<pt.size();++i)
		mean += pt[i];
	mean /= (double)pt.size();
	return mean;
}

double ICPHelper::distance (cv::Point2d a, cv::Point2d b) {
	return sqrt(pow(a.x-b.x, 2)+pow(a.y-b.y, 2));
}
