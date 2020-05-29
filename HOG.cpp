#include <iostream>
#include <opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

float *cellHist(Mat src)
{
	//计算角度及梯度
	Mat gx, gy;
	Mat mag, angle;
	Sobel(src, gx, CV_32F, 1, 0, 1);
	Sobel(src, gy, CV_32F, 0, 1, 1);
	cartToPolar(gx, gy, mag, angle, true);

	//cell直方图数组
	float hist[8] = { 0 };  
	for (int j = 0; j < 16; j++)
	{
		float* angledata = angle.ptr<float>(j);
		float* magdata = mag.ptr<float>(j);
		for (int i = 0; i < 16; i++)
		{
			if (angledata[i] >= 0 && angledata[i] < 45)
			{
				hist[0] += magdata[i];
			}
			else if (angledata[i] >= 45 && angledata[i] < 90)
			{
				hist[1] += magdata[i];
			}
			else if (angledata[i] >= 90 && angledata[i] < 135)
			{
				hist[2] += magdata[i];
			}
			else if (angledata[i] >= 135 && angledata[i] < 180)
			{
				hist[3] += magdata[i];
			}
			else if (angledata[i] >= 180 && angledata[i] < 225)
			{
				hist[4] += magdata[i];
			}
			else if (angledata[i] >= 225 && angledata[i] < 270)
			{
				hist[5] += magdata[i];
			}
			else if (angledata[i] >= 270 && angledata[i] < 315)
			{
				hist[6] += magdata[i];
			}
			else if (angledata[i] >= 315 && angledata[i] < 360)
			{
				hist[7] += magdata[i];
			}
		}
	}
	return hist;
}


int main()
{
	Mat refMat = imread("template.png", 0);
	Mat srcMat = imread("img.png", 0);
	int cellSize = 16;
	int nX = refMat.cols / cellSize;
	int nY = refMat.rows / cellSize;
	Mat img = Mat::zeros(refMat.rows, refMat.cols, CV_8UC1);
	int num = nX * nY * 8;

	float* srcHist = new float[num];
	memset(srcHist, 0, sizeof(float) * num);

	float* refMatHist = new float[num];
	memset(refMatHist, 0, sizeof(float) * num);

	float* imgHist = new float[num];
	memset(imgHist, 0, sizeof(float) * num);

	int cnt1 = 0;
	int cnt2 = 0;
	float distance = 0;
	float distance_min = 10e100;
	int x = 0;
	int y = 0;
	for (int q = 80; q < /*srcMat.rows- refMat.rows*/110; q++)
	{
		for (int p = 80; p < /*srcMat.cols - refMat.cols*/110; p++)
		{
			for (int m = 0; m < refMat.rows; m++) 
			{
				for (int n = 0; n < refMat.cols; n++)
				{
					img.at<uchar>(m, n) = srcMat.at<uchar>(q + m, p + n);
				}
			}
			//refMat
			cnt1 = 0;
			for (int j = 0; j < nY; j++)
			{
				for (int i = 0; i < nX; i++)
				{
					Rect rectL(i * cellSize, j * cellSize, cellSize, cellSize);
					Mat nowMat = refMat(rectL);
					for (int k = 0; k < 8; k++)
					{
						refMatHist[k + cnt1 * 8] = cellHist(nowMat)[k];
					}
					cnt1++;
				}
			}
			//img
			cnt2 = 0;
			for (int j = 0; j < nY; j++)
			{
				for (int i = 0; i < nX; i++)
				{
					Rect rectL(i * cellSize, j * cellSize, cellSize, cellSize);
					Mat nowMat = img(rectL);
					for (int k = 0; k < 8; k++)
					{
						imgHist[k + cnt2 * 8] = cellHist(nowMat)[k];
					}
					cnt2++;
				}
			}
			distance = 0;
			for (int i = 0; i < num; i++)
			{
				distance += (refMatHist[i] - imgHist[i]) * (refMatHist[i] - imgHist[i]);
				if (distance < distance_min) {
					distance_min = distance;
					x = q;
					y = p;
				}
			}
		}
	}

	//框出目标物体
	Rect rect(y, x, refMat.cols, refMat.rows);
	cv::rectangle(srcMat, rect, Scalar(255, 0, 0), 1, LINE_8, 0);
	imshow("srcMat", srcMat);
	waitKey(0);
	delete[] refMatHist;
	delete[] imgHist;
	return 0;

}