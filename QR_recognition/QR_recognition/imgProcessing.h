#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>

#define COUNT_SIZE 256
#define OTSUTHRE 0
typedef int THRESHOLD;

using namespace std;
using namespace cv;

namespace qr {
	bool threshold(const Mat &src, Mat &dest, THRESHOLD thre = OTSUTHRE);
	bool preProcess(const Mat &src, Mat &dest);
	Point centerCal(vector<vector<Point>> contours, int i);
	void findPosRect(const Mat &src, vector<vector<Point>> &contours2);
	float getDistance(const Point &p1, const Point &p2);
	bool isPosRect(const vector<vector<Point>> &contours, const vector<Vec4i> &hierarchy, int parentIdx);
	void imgAffine(const Mat &src, Mat &dest,const vector<Point> &centers);
};