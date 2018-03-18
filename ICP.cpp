
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "ANNHelper.hpp"

#include "RandomGenerator.hpp"
#include "ICPHelper.hpp"

void movePts (std::vector<cv::Point2d> &pt, cv::Point2d dd);
void rotatePts (std::vector<cv::Point2d> &pt, double dth);
double icp (cv::Point2d trans, double &rotates, 
	std::vector<cv::Point2d> &src, std::vector<cv::Point2d> &tgt,
	cv::Mat &img, cv::Mat ori_img,
	ANNHelper &ann);

int main (int argc, char *argv[]) {

	std::string filename("../img/curve.png");
	int noise_add(0), noise_remove(0);
	cv::Vec3d red(0,0,255), green(0,255,0), black(0,0,0), white(255, 255, 255);

	if (argc >= 2) filename = argv[1];
	if (argc >= 3) noise_add = atoi(argv[2]);
	if (argc >= 4) noise_remove = atoi(argv[3]);

	cv::Mat img = cv::imread(filename.c_str(), CV_LOAD_IMAGE_COLOR);
	cv::imshow("ICP demo", img);
	cv::waitKey(0);

	cv::Point2d center(0, 0);
	std::vector<cv::Point2d> src, tgt;
	for (unsigned r=0;r<img.rows;++r)
		for (unsigned c=0;c<img.cols;++c) {
			cv::Vec3b clr(img.at<cv::Vec3b>(cv::Point(c, r)));
			if (clr[0] < white[0] && clr[1] < white[1] && clr[2] < white[2]) {
				src.push_back(cv::Point2d(c, r));
				tgt.push_back(cv::Point2d(c, r));
				center.x += c;
				center.y += r;
			}
			img.at<cv::Vec3b>(cv::Point(c, r)) = white;
		}
	fprintf(stderr, "#pt %ld\n", src.size());
	center = center / (double)src.size();
	RandomGenerator random_gen(img.cols, img.rows);
	// move points to (dx, dy, dth)
	movePts(tgt, -center);
	rotatePts(tgt, random_gen.getRandomPoint(360));
	movePts(tgt, random_gen.getRandomPoint()+center);

	// add/remove some random points to/from tgt
	if (noise_remove > 0) {
		for (int i=0;i<noise_remove;++i) {
			int idx = random_gen.getRandomPoint(tgt.size());
			tgt[idx] = tgt.back();
			tgt.pop_back();
		}
	}
	if (noise_add > 0) {
		for (int i=0;i<noise_add;++i)
			tgt.push_back(random_gen.getRandomPoint());
	}

	ANNHelper ann(2, 1, 0, tgt.size());
	for (unsigned i=0;i<tgt.size();++i){
		ann.data_pts[i][0] = tgt[i].x;
		ann.data_pts[i][1] = tgt[i].y;
	}
	ann.kd_tree = new ANNkd_tree(					// build search structure
					  ann.data_pts,					// the data points
					  tgt.size(),					// number of points
					  ann.dim);						// dimension of space

	for (unsigned i=0;i<src.size();++i)
		img.at<cv::Vec3b>(src[i]) = red;
	for (unsigned i=0;i<tgt.size();++i) {
		if (tgt[i].x >= 0 && tgt[i].x < img.cols && 
			tgt[i].y >= 0 && tgt[i].y < img.rows)
			img.at<cv::Vec3b>(tgt[i]) = green;
	}

	// show the src in red, tgt in green
	cv::imshow("ICP demo", img);
	cv::waitKey(0);

	cv::Point2d trans(0,0);
	double rotates(0);
	cv::Mat ori_img;
	img.copyTo(ori_img);
	// do ICP
	icp(trans, rotates, src, tgt, img, ori_img, ann);
	cv::imshow("ICP demo", img);
	cv::waitKey(0);

	return 0;
}

void movePts (std::vector<cv::Point2d> &pt, cv::Point2d dd) {
	for (unsigned i=0;i<pt.size();++i)
		pt[i] += dd;
}

void rotatePts (std::vector<cv::Point2d> &pt, double dth) {
	cv::Point2d tmp;
	dth = M_PI*dth/360.0f;
	for (unsigned i=0;i<pt.size();++i) {
		tmp.x = pt[i].x * cos(dth) - pt[i].y * sin(dth);
		tmp.y = pt[i].x * sin(dth) + pt[i].y * cos(dth);
		pt[i] = tmp;
	}
}

double icp (cv::Point2d trans, double &rotates, 
	std::vector<cv::Point2d> &src, std::vector<cv::Point2d> &tgt,
	cv::Mat &img, cv::Mat ori_img,
	ANNHelper &ann) {
	cv::Vec3d magenta(255,0,255);
	// how to deal with different size of src and tgt
	std::vector<cv::Point2d> tmp_tgt;
	for (unsigned i=0;i<src.size();++i)
		tmp_tgt.push_back(src[i]);

	ICPHelper helper;
	int iter_count(0);
	double error(0), pre_err(0), r(0);
	cv::Point2d tmp(0,0), t(0,0);
	do {
		iter_count++;
		helper.icpIter(tmp_tgt, tgt, ann);
		
		pre_err = error;
		error = 0;
		for (unsigned i=0;i<tmp_tgt.size();++i) {
			tmp.x = (tmp_tgt[i].x*cos(helper.r)) - (tmp_tgt[i].y*sin(helper.r)) + helper.t.x;
			tmp.y = (tmp_tgt[i].x*sin(helper.r)) + (tmp_tgt[i].y*cos(helper.r)) + helper.t.y;
			tmp_tgt[i] = tmp;

			// query_pt[0] = tmp.x;
			// query_pt[1] = tmp.y;
			// // find closest point to compute the error distance
			// kd_tree->annkSearch(				// search
			// 	query_pt,						// query point
			// 	k,								// number of near neighbors
			// 	nn_idx,							// nearest neighbors (returned)
			// 	dists,							// distance (returned)
			// 	eps);							// error bound

			error += sqrt(pow(tmp_tgt[i].x - tgt[i].x, 2) + 
						  pow(tmp_tgt[i].y - tgt[i].y, 2));
		}
		error = error/(double)tgt.size();

		trans += helper.t;
		rotates += helper.r;

		fprintf(stderr, "iter[%d]: err = %lf, translation(%.2lf, %.2lf), rotate(%.2lf deg)\n", 
			iter_count, error, trans.x, trans.y, rotates*360.0f/M_PI);

		img = ori_img;
		for (unsigned i=0;i<tmp_tgt.size();++i)
			if (tmp_tgt[i].x >= 0 && tmp_tgt[i].x < img.cols && 
				tmp_tgt[i].y >= 0 && tmp_tgt[i].y < img.rows)
				img.at<cv::Vec3b>(tmp_tgt[i]) = magenta;
		cv::imshow("ICP demo", img);
		cv::waitKey(30);
	} while (fabs(error - pre_err) > 1e-6 && iter_count < 1000);

	return error;
}
