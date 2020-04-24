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
	cartToPolar(gx, gy, mag, angle, false);

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
	Mat refMat, img1, img2;
	refMat = imread("hogTemplate.jpg", 0);
	img1 = imread("img1.jpg", 0);
	img2 = imread("img2.jpg", 0);
		 
	int cellSize = 16;
	int nX = refMat.cols / cellSize;
	int nY = refMat.rows / cellSize;
	int num = nX * nY * 8;
	float * refHist = new float[num];
	memset(refHist, 0, sizeof(float) * num);
	float * img1Hist = new float[num];
	memset(img1Hist, 0, sizeof(float) * num);
	float * img2Hist = new float[num];
	memset(img2Hist, 0, sizeof(float) * num);

	//refMat的直方图数组
	int cnt = 0;
	for (int j = 0; j < nY; j++)
	{
		for (int i = 0; i < nX; i++)
		{
			Rect rectL(i*16, j*16, 16, 16);  //分割图像
			Mat nowMat = refMat(rectL);
			for (int k = 0; k < 8; k++)  //合并直方图数组
			{
				refHist[k + cnt * 8] = cellHist(nowMat)[k];
			}
			cnt++;
		}
	}

	//img1的直方图数组
	int cnt1 = 0;
	for (int j = 0; j < nY; j++)
	{
		for (int i = 0; i < nX; i++)
		{
			Rect rectL(i * 16, j * 16, 16, 16);  //分割图像
			Mat nowMat = img1(rectL);
			for (int k = 0; k < 8; k++)  //合并直方图数组
			{
				img1Hist[k + cnt1 * 8] = cellHist(nowMat)[k];
			}
			cnt1++;
		}
	}

	//img2的直方图数组
	int cnt2 = 0;
	for (int j = 0; j < nY; j++)
	{
		for (int i = 0; i < nX; i++)
		{
			Rect rectL(i * 16, j * 16, 16, 16);  //分割图像
			Mat nowMat = img2(rectL);
			for (int k = 0; k < 8; k++)  //合并直方图数组
			{
				img2Hist[k + cnt2 * 8] = cellHist(nowMat)[k];
			}
			cnt2++;
		}
	}

	//计算欧几里得距离
	float distance1, distance2;
	float sum1 = 0, sum2 = 0;
	for (int i = 0; i < num; i++)
	{
		sum1 += (refHist[i] - img1Hist[i]) * (refHist[i] - img1Hist[i]);
		sum2 += (refHist[i] - img2Hist[i]) * (refHist[i] - img2Hist[i]);
	}
	distance1 = sqrt(sum1);
	distance2 = sqrt(sum2);
	cout << "img1与参考图的欧几里得距离：" << distance1 << endl;
	cout << "img2与参考图的欧几里得距离：" << distance2 << endl;
	if (distance1 < distance2)
	{
		cout << "img1与参考图更为接近" << endl;
	}
	else
	{
		cout << "img2与参考图更为接近" << endl;
	}
	delete[] refHist;
	delete[] img1Hist;
	delete[] img2Hist;

	return 0;
}

