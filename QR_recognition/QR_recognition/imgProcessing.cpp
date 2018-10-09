#include "stdafx.h"
#include "imgProcessing.h"

bool qr::threshold(const Mat &src, Mat &dest, THRESHOLD thre)
{
	if (OTSUTHRE == thre)
	{
		int count[COUNT_SIZE];
		double u0, u1, u;
		double pixelSum0, pixelSum1;
		int n0, n1;
		int bestThresold = 0, tmpThresold = 0;
		double w0, w1;
		int height = src.rows;
		int width = src.cols;
		double variable = 0, maxVariable = 0;
		double sumofpixel = height * width;

		for (int i = 0; i < COUNT_SIZE; i++)
		{
			count[i] = 0;
		}

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				count[int(src.at<uchar>(i, j))]++;
			}
		}

		for (tmpThresold = 0; tmpThresold < COUNT_SIZE; tmpThresold++)
		{
			n0 = 0; n1 = 0;
			w0 = 0; w1 = 0;
			pixelSum0 = 0;
			pixelSum1 = 0;

			for (int i = 0; i < tmpThresold; i++)
			{
				n0 += count[i];
				pixelSum0 += i * count[i];
			}

			for (int i = tmpThresold; i < COUNT_SIZE; i++)
			{
				n1 += count[i];
				pixelSum1 += i * count[i];
			}

			w0 = (double)n0 / sumofpixel;
			w1 = (double)n1 / sumofpixel;
			u0 = pixelSum0 / n0;
			u1 = pixelSum1 / n1;
			u = u0 * w0 + u1 * w1;
			variable = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);

			if (variable > maxVariable)
			{
				maxVariable = variable;
				bestThresold = tmpThresold;
			}
		}

		thre = bestThresold;
	}
	
	else if (thre >= 256 || thre < 0)
	{
		cout << "Out of range!" << endl;
		return false;
	}

	cv::threshold(src, dest, thre, 255, 1);
	return true;
}

Point qr::centerCal(vector<vector<Point>> contours, int i)
{
	int centerX = 0, centerY = 0;
	int n = contours[i].size();       // n 即为轮廓的周长
	
	for (int j = 1; j <= 4; j++)
	{
		centerX += contours[i][int((float)j * n / 4)].x;
		centerY += contours[i][int((float)j * n / 4)].y;
	}

	return Point(centerX / 4, centerY / 4);
}

bool qr::preProcess(const Mat &src, Mat &dest)
{
	Mat tmpimg;
	
	cvtColor(src, tmpimg, CV_BGR2GRAY);
	blur(tmpimg, tmpimg, Size(3, 3));                  // 用 3*3 的 kernal 高斯模糊
	qr::threshold(tmpimg, dest, OTSUTHRE);          // dest 为生成的二值图像

	return true;
}

void qr::findPosRect(const Mat &src, Mat &drawing)
{
	//drawing = Mat::zeros(src.size(), CV_8UC3);
	// 提取图像的边缘保存于 contours 中
	vector<vector<Point>> contours, contours2;
	vector<Vec4i> hierarchy;                        // hierarchy [next, previous, child, parent]
	findContours(src, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE, Point());

	int c = 0, ic = 0, area = 0;                    // ic 用于记录轮廓内部子轮廓的个数
	int parentIdx = -1;
	for (int i = 0; i < contours.size(); i++)
	{
		if (hierarchy[i][2] != -1 && ic == 0)
		{
			parentIdx = i;
			ic++;
		}
		else if (hierarchy[i][2] != -1)
			ic++;
		else if (-1 == hierarchy[i][2])             // 当没有子轮廓时，清零
		{
			parentIdx = -1;
			ic = 0;
		}

		// 判断当子轮廓数>=2 时为定位矩形区域，存入 contours2 中
		if (ic >= 2)
		{
			contours2.push_back(contours[parentIdx]);
			ic = 0;
			parentIdx = -1;
		}
	}
	drawContours(drawing, contours2, -1, Scalar(0, 255, 0), 1, 8);         // contourIdx 为负数，表示绘制所有的轮廓
	imshow("drawing", drawing);
	waitKey(0);
}
