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
	Mat gray_img;
	Mat dest;

	if (src_img.empty())
	{
		cout << "Can't open the image!" << endl;
		return 1;
	}

	cout << src_img.size() << endl;

	start = time(NULL);
	if (qr::preProcess(src_img, dest))
	{
		qr::findPosRect(dest, src_img.clone());
	}
	//qr::threshold(dest, dest);
	end = time(NULL);
	cout << "Time Used: " << end - start << "ms" << endl;
	cout << gray_img.size << endl;

	
	imshow("src_img", src_img);

	//threshold(src_img, dest, 103, 255, 0);
	imshow("dest", dest);

	waitKey(0);
    return 0;
}

