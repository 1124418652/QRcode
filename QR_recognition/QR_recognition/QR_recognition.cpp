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
	Mat src_img = imread("../../../../prectice_in_book/src/er.jpg");
	Mat dest, drawing = src_img.clone();

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

		Point *center = new Point[posRectContours.size()];     // 找到定位轮廓的中心坐标
		for (int i = 0; i < posRectContours.size(); i++)
		{
			center[i] = qr::centerCal(posRectContours, i);
			circle(drawing, center[i], 2, Scalar(0, 255, 0), 2);
		}
		imshow("drawing", drawing);

		delete[] center;
	}
	end = time(NULL);
	cout << "Time Used: " << end - start << "ms" << endl;

	imshow("src_img", src_img);
	//threshold(src_img, dest, 103, 255, 0);
	imshow("dest", dest);

	waitKey(0);
    return 0;
}

