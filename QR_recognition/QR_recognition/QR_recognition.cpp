// QR_recognition.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <time.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "imgProcessing.h"
using namespace std;
using namespace cv;

int main()
{
	clock_t start, end;
	Mat src_img = imread("../../src/er.jpg");
	resize(src_img, src_img, Size(400, 300));
	Mat dest, drawing = src_img.clone();
	Mat warpimg(src_img.size(), src_img.type());

	if (src_img.empty())
	{
		cout << "Can't open the image!" << endl;
		return 1;
	}

	start = time(NULL);
	if (qr::preProcess(src_img, dest))
	{
		vector<vector<Point>> posRectContours;
		qr::findPosRect(dest, posRectContours);                // 找到定位矩形的轮廓
		drawContours(drawing, posRectContours, -1, Scalar(0, 255, 0), 1, 8);

		vector<Point> center(3);
		for (int i = 0; i < posRectContours.size(); i++)
		{
			center[i] = qr::centerCal(posRectContours, i);
			circle(drawing, center[i], 1, Scalar(0, 255, 0), 1);
		}
		qr::imgAffine(src_img, warpimg, center);
		/*
		Point2f srcTri[3], dstTri[3];
		srcTri[0] = Point2f(center[0].x, center[0].y);
		srcTri[1] = Point2f(center[1].x, center[1].y);
		srcTri[2] = Point2f(center[2].x, center[2].y);
		dstTri[0] = Point2f(center[0].x, center[0].y);
		dstTri[1] = Point2f(center[1].x, center[1].y);
		dstTri[2] = Point2f(center[2].x, center[2].y);
		Mat warpMat = getAffineTransform(srcTri, dstTri);
		warpAffine(src_img, warpimg, warpMat, warpimg.size());

		imshow("warp", warpimg);
		*/
		imshow("warp", warpimg);
		imshow("drawing", drawing);
	}
	end = time(NULL);
	cout << "Time Used: " << end - start << "ms" << endl;
	//imshow("src_img", src_img);
	//threshold(src_img, dest, 103, 255, 0);
	//imshow("dest", dest);

	waitKey(0);
    return 0;
}

