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
	int n = contours[i].size();       // n ��Ϊ�������ܳ�
	
	for (int j = 0; j < 4; j++)
	{
		centerX += contours[i][int((float)j * n / 4)].x;
		centerY += contours[i][int((float)j * n / 4)].y;
	}

	return Point(centerX / 4, centerY / 4);
}

float qr::getDistance(const Point &p1, const Point &p2)
{
	return sqrt(pow((float)p1.x - p2.x, 2) + pow((float)p1.y - p2.y, 2));
}

bool qr::preProcess(const Mat &src, Mat &dest)
{
	Mat tmpimg;
	
	cvtColor(src, tmpimg, CV_BGR2GRAY);
	blur(tmpimg, tmpimg, Size(3, 3));                  // �� 3*3 �� kernal ��˹ģ��
	qr::threshold(tmpimg, dest, OTSUTHRE);             // dest Ϊ���ɵĶ�ֵͼ��

	return true;
}

bool qr::isPosRect(const vector<vector<Point>> &contours, const vector<Vec4i> &hierarchy, int parentIdx)
{
	float thresholdx = 5;
	int sonIdx = hierarchy[parentIdx][2];
	int grandsonIdx = hierarchy[sonIdx][2];
	Point parentCenter, sonCenter, grandsonCenter;

	parentCenter = qr::centerCal(contours, parentIdx);
	sonCenter = qr::centerCal(contours, sonIdx);
	grandsonCenter = qr::centerCal(contours, grandsonIdx);

	if (getDistance(parentCenter, sonCenter) >= thresholdx || getDistance(sonCenter, grandsonCenter) >= thresholdx \
		|| getDistance(parentCenter, grandsonCenter) >= thresholdx)
	{
		return false;
	}

	return true;
}

void qr::findPosRect(const Mat &src, vector<vector<Point>> &contours2)
{
	//drawing = Mat::zeros(src.size(), CV_8UC3);
	// ��ȡͼ��ı�Ե������ contours ��
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;                        // hierarchy [next, previous, child, parent]
	findContours(src, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE, Point());

	int c = 0, ic = 0, area = 0;                    // ic ���ڼ�¼�����ڲ��������ĸ���
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
		else if (-1 == hierarchy[i][2])             // ��û��������ʱ������
		{
			parentIdx = -1;
			ic = 0;
		}

		// �жϵ���������>=2 ʱΪ��λ�������򣬴��� contours2 ��
		if (ic >= 2 && isPosRect(contours, hierarchy, parentIdx))
		{
			contours2.push_back(contours[parentIdx]);
			ic = 0;
			parentIdx = -1;
		}
	}

	if (0 == contours2.size())
	{
		cout << "Can't find the QR code!" << endl;
		return;
	}
}

void qr::imgAffine(const Mat &src, Mat &dest, const vector<Point> &centers)
{
	Point2f srcPoint[3], dstPoint[3];
	vector<Point> newCenters(3);
	float dis01, dis02, dis12;
	dis01 = getDistance(centers[0], centers[1]);
	dis02 = getDistance(centers[0], centers[2]);
	dis12 = getDistance(centers[1], centers[2]);

	// ֻ������˳ʱ��ת��90�����ڵ����
	if (dis01 > dis02 && dis01 > dis12)
	{
		newCenters[0] = centers[2];
		if (centers[0].x > newCenters[0].x)
		{
			newCenters[1] = centers[0];
			newCenters[2] = centers[1];
		}
		else
		{
			newCenters[1] = centers[1];
			newCenters[2] = centers[0];
		}
	}

	else if (dis02 > dis01 && dis02 > dis12)
	{
		newCenters[0] = centers[1];
		if (centers[0].x > newCenters[0].x)
		{
			newCenters[1] = centers[0];
			newCenters[2] = centers[2];
		}
		else
		{
			newCenters[1] = centers[2];
			newCenters[2] = centers[0];
		}
	}
	else
	{
		newCenters[0] = centers[0];
		if (centers[1].x > newCenters[0].x)
		{
			newCenters[1] = centers[1];
			newCenters[2] = centers[2];
		}
		else
		{
			newCenters[1] = centers[2];
			newCenters[2] = centers[1];
		}
	}

	cout << newCenters[0] << endl;
	cout << newCenters[1] << endl;
	cout << newCenters[2] << endl;

	srcPoint[0] = Point2f(newCenters[0].x, newCenters[0].y);
	srcPoint[1] = Point2f(newCenters[1].x, newCenters[1].y);
	srcPoint[2] = Point2f(newCenters[2].x, newCenters[2].y);
	dstPoint[0] = Point2f(newCenters[0].x, newCenters[0].y);
	dstPoint[1] = Point2f(newCenters[1].x, newCenters[0].y);
	dstPoint[2] = Point2f(newCenters[0].x, newCenters[2].y);

	Mat warpMat = getAffineTransform(srcPoint, dstPoint);
	warpAffine(src, dest, warpMat, dest.size());
}