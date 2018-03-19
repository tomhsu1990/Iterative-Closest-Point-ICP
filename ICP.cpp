
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
double icp (cv::Point2d &trans, double &rotates, 
	std::vector<cv::Point2d> &tmp_tgt, std::vector<cv::Point2d> &tgt,
	cv::Mat &img, cv::Mat ori_img,
	ICPHelper helper, ANNHelper *ann, cv::Vec3d clr);

int main (int argc, char *argv[]) {

	std::string filename("../img/curve.png");
	int noise_add(0), noise_remove(0);
	cv::Vec3d red(0,0,255), green(0,255,0), blue(255,0,0), black(0,0,0), white(255, 255, 255);
	cv::Vec3d magenta(255,0,255), yellow(0,255,255), cyan(255,255,0);

	if (argc >= 2) filename = argv[1];
	if (argc >= 3) noise_add = atoi(argv[2]);
	if (argc >= 4) noise_remove = atoi(argv[3]);

	cv::Mat img = cv::imread(filename.c_str(), CV_LOAD_IMAGE_COLOR);
	cv::imshow("ICP demo", img);
	cv::waitKey(0);
	
	ICPHelper helper;
	std::vector<cv::Point2d> src, tgt;
	for (unsigned r=0;r<img.rows;++r)
		for (unsigned c=0;c<img.cols;++c) {
			cv::Vec3b clr(img.at<cv::Vec3b>(cv::Point(c, r)));
			if (clr[0] < white[0] && clr[1] < white[1] && clr[2] < white[2]) {
				src.push_back(cv::Point2d(c, r));
				tgt.push_back(cv::Point2d(c, r));
			}
			img.at<cv::Vec3b>(cv::Point(c, r)) = white;
		}
	fprintf(stderr, "#pt %ld\n", src.size());
	cv::Point2d center = helper.getMean(tgt);
	RandomGenerator random_gen(img.cols, img.rows);
	// move points to (trans_ref, rotates_ref)
	cv::Point2d trans_ref(random_gen.getRandomPoint());
	double rotates_ref(random_gen.getRandomPoint(360));
	movePts(tgt, -center);
	rotatePts(tgt, rotates_ref);
	movePts(tgt, trans_ref+center);

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

	ANNHelper *ann = new ANNHelper(2, 1, 0, tgt.size());
	for (unsigned i=0;i<tgt.size();++i){
		ann->data_pts[i][0] = tgt[i].x;
		ann->data_pts[i][1] = tgt[i].y;
	}
	ann->initializeKdTree();

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
	std::vector<cv::Point2d> tmp_tgt;
	for (unsigned i=0;i<src.size();++i)
		tmp_tgt.push_back(src[i]);
	double error = icp(trans, rotates, tmp_tgt, tgt, img, ori_img, helper, ann, magenta);
	trans = helper.getMean(tmp_tgt)-center;
	fprintf(stderr, "translation(%.2lf, %.2lf), rotate(%.2lf deg)\n", trans.x, trans.y, rotates*360.0f/M_PI);
	fprintf(stderr, "referrence translation(%.2lf, %.2lf), rotate(%.2lf deg)\n", trans_ref.x, trans_ref.y, rotates_ref);

	cv::imshow("ICP demo", img);
	cv::waitKey(0);
	img.copyTo(ori_img);

	// move to the coordinate's center, rotate (360 deg or 360/n), and move back
	// do icp again to compare with the current error
	center += trans;
	std::vector<cv::Point2d> final_tmp_tgt;
	cv::Point2d final_trans(0,0);
	double final_rotates(0);
	for (int i=36;i<360;i+=36) { // 36 degrees as a unit
		std::vector<cv::Point2d> cp_tmp_tgt(tmp_tgt);
		movePts(cp_tmp_tgt, -center);
		rotatePts(cp_tmp_tgt, i);
		movePts(cp_tmp_tgt, center);

		cv::Point2d tmp_trans(0,0);
		double tmp_rotates(0);
		ori_img.copyTo(img);
		double tmp_error = icp(tmp_trans, tmp_rotates, cp_tmp_tgt, tgt, img, ori_img, helper, ann, yellow);
		if (error > tmp_error) {
			error = tmp_error;
			final_trans = trans+tmp_trans;
			final_rotates = rotates+tmp_rotates+(double)i*M_PI/360.0f;
			final_tmp_tgt = cp_tmp_tgt;
		}
	}
	ori_img.copyTo(img);
	for (unsigned i=0;i<final_tmp_tgt.size();++i){
		if (final_tmp_tgt[i].x >= 0 && final_tmp_tgt[i].x < img.cols && 
			final_tmp_tgt[i].y >= 0 && final_tmp_tgt[i].y < img.rows)
			img.at<cv::Vec3b>(final_tmp_tgt[i]) = blue;
	}
	final_trans = helper.getMean(final_tmp_tgt)-helper.getMean(src);
	fprintf(stderr, "final translation(%.2lf, %.2lf), rotate(%.2lf deg)\n", final_trans.x, final_trans.y, final_rotates*360.0f/M_PI);
	fprintf(stderr, "referrence translation(%.2lf, %.2lf), rotate(%.2lf deg)\n", trans_ref.x, trans_ref.y, rotates_ref);
	cv::imshow("ICP demo", img);
	cv::waitKey(0);

	delete ann;

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

double icp (cv::Point2d &trans, double &rotates, 
	std::vector<cv::Point2d> &tmp_tgt, std::vector<cv::Point2d> &tgt,
	cv::Mat &img, cv::Mat ori_img,
	ICPHelper helper, ANNHelper *ann, cv::Vec3d clr) {

	int iter_count(0);
	double error(0), pre_err(0);
	cv::Point2d tmp(0,0);
	do {
		iter_count++;
		helper.icpIter(tmp_tgt, tgt, ann);
		
		pre_err = error;
		error = 0;
		for (unsigned i=0;i<tmp_tgt.size();++i) {
			tmp.x = (tmp_tgt[i].x*cos(helper.r)) - (tmp_tgt[i].y*sin(helper.r)) + helper.t.x;
			tmp.y = (tmp_tgt[i].x*sin(helper.r)) + (tmp_tgt[i].y*cos(helper.r)) + helper.t.y;
			tmp_tgt[i] = tmp;

			// find closest point to compute the error distance
			cv::Point2d closestPt = ann->findClosestPt(tmp);

			// we don't need one-to-one mapping!
			error += pow(tmp_tgt[i].x - closestPt.x, 2) + 
					 pow(tmp_tgt[i].y - closestPt.y, 2);
		}
		error = error/(double)tmp_tgt.size();

		trans += helper.t;
		rotates += helper.r;

		ori_img.copyTo(img);
		for (unsigned i=0;i<tmp_tgt.size();++i)
			if (tmp_tgt[i].x >= 0 && tmp_tgt[i].x < img.cols && 
				tmp_tgt[i].y >= 0 && tmp_tgt[i].y < img.rows)
				img.at<cv::Vec3b>(tmp_tgt[i]) = clr;

		cv::imshow("ICP demo", img);
		cv::waitKey(5);
	} while (fabs(error - pre_err) > 1e-6 && iter_count < 1000);

	return error;
}
