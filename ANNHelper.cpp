#include "ANNHelper.hpp"

cv::Point2d ANNHelper::findClosestPt(cv::Point2d pt){
	query_pt[0] = pt.x;
	query_pt[1] = pt.y;
	// find closest point to compute the error distance
	kd_tree->annkSearch(				// search
		query_pt,						// query point
		k,								// number of near neighbors
		nn_idx,							// nearest neighbors (returned)
		dists,							// distance (returned)
		eps);							// error bound
	return cv::Point2d(data_pts[nn_idx[0]][0], data_pts[nn_idx[0]][1]);
}

void ANNHelper::initializeKdTree () {
	kd_tree = new ANNkd_tree(			// build search structure
				data_pts,				// the data points
				n_pts,					// number of points
				dim);					// dimension of space
}