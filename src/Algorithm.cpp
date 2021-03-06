//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: Algorithm.cpp
// @brief: ISP algorithm source file.
//////////////////////////////////////////////////////////////////////////////////////

#include "Algorithm.h"
#include "Param.h"

using namespace std;

ISPParameter gParam;

//EdgePreservedNR not used, it should be redeveloped
ISPResult EdgePreservedNR(Mat YUV, Mat NRYUV, float arph, bool enable) {
	ISPResult result = ISPSuccess;

	if (enable == true) {
		int32_t WIDTH;
		int32_t HEIGHT;
		result = gParam.GetIMGDimension(&WIDTH, &HEIGHT);

		int i, j;
		Mat Edge, YUVEdge, NoEdgeNR, AYUV, BYUV;
		Edge = YUV.clone();
		YUVEdge = YUV.clone();
		NoEdgeNR = YUV.clone();
		AYUV = YUV.clone();
		BYUV = YUV.clone();
		Canny(YUV, Edge, 40, 90, 3);

		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				YUVEdge.data[i * 3 * WIDTH + 3 * j] = YUVEdge.data[i * 3 * WIDTH + 3 * j] & Edge.data[i * WIDTH + j];
				YUVEdge.data[i * 3 * WIDTH + 3 * j + 1] = YUVEdge.data[i * 3 * WIDTH + 3 * j + 1] & Edge.data[i * WIDTH + j];
				YUVEdge.data[i * 3 * WIDTH + 3 * j + 2] = YUVEdge.data[i * 3 * WIDTH + 3 * j + 2] & Edge.data[i * WIDTH + j];
			}
		}
		bilateralFilter(YUV, NoEdgeNR, 5, 5, 9, BORDER_DEFAULT);
		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				NoEdgeNR.data[i * 3 * WIDTH + 3 * j] = NoEdgeNR.data[i * 3 * WIDTH + 3 * j] & (~Edge.data[i * WIDTH + j]);
				NoEdgeNR.data[i * 3 * WIDTH + 3 * j + 1] = NoEdgeNR.data[i * 3 * WIDTH + 3 * j + 1] & (~Edge.data[i * WIDTH + j]);
				NoEdgeNR.data[i * 3 * WIDTH + 3 * j + 2] = NoEdgeNR.data[i * 3 * WIDTH + 3 * j + 2] & (~Edge.data[i * WIDTH + j]);
			}
		}
		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				AYUV.data[i * 3 * WIDTH + 3 * j] = NoEdgeNR.data[i * 3 * WIDTH + 3 * j] + YUVEdge.data[i * 3 * WIDTH + 3 * j];
				AYUV.data[i * 3 * WIDTH + 3 * j + 1] = NoEdgeNR.data[i * 3 * WIDTH + 3 * j + 1] + YUVEdge.data[i * 3 * WIDTH + 3 * j + 1];
				AYUV.data[i * 3 * WIDTH + 3 * j + 2] = NoEdgeNR.data[i * 3 * WIDTH + 3 * j + 2] + YUVEdge.data[i * 3 * WIDTH + 3 * j + 2];
			}
		}
		blur(YUV, BYUV, Size(3, 3));
		//namedWindow(TEMP, 0);
		//resizeWindow(TEMP, Imgsizex, Imgsizey);
		//imshow(TEMP, AYUV);

		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				NRYUV.data[i * 3 * WIDTH + 3 * j] = (unsigned int)AYUV.data[i * 3 * WIDTH + 3 * j] * arph + (unsigned int)BYUV.data[i * 3 * WIDTH + 3 * j] * (1 - arph);
				NRYUV.data[i * 3 * WIDTH + 3 * j + 1] = (unsigned int)AYUV.data[i * 3 * WIDTH + 3 * j + 1] * arph + (unsigned int)BYUV.data[i * 3 * WIDTH + 3 * j + 1] * (1 - arph);
				NRYUV.data[i * 3 * WIDTH + 3 * j + 2] = (unsigned int)AYUV.data[i * 3 * WIDTH + 3 * j + 2] * arph + (unsigned int)BYUV.data[i * 3 * WIDTH + 3 * j + 2] * (1 - arph);
				//NRYUV.data[i * 3 * WIDTH + 3 * j + 1] =127;
				//NRYUV.data[i * 3 * WIDTH + 3 * j + 2] =127;
			}
		}
	}

	return result;
}

//BF not used, it use opencvlib. it should be develop with self alg
void BF(unsigned char* b, unsigned char* g, unsigned char* r, int dec, int Colorsigma, int Spacesigma, bool enable)
{
	if (enable == true) {
		int32_t WIDTH;
		int32_t HEIGHT;
		gParam.GetIMGDimension(&WIDTH, &HEIGHT);
		Mat ABFsrc(HEIGHT, WIDTH, CV_8UC1);
		Mat ABFb(HEIGHT, WIDTH, CV_8UC1);
		Mat ABFg(HEIGHT, WIDTH, CV_8UC1);
		Mat ABFr(HEIGHT, WIDTH, CV_8UC1);
		ABFsrc.data = b;
		bilateralFilter(ABFsrc, ABFb, dec, Colorsigma, Spacesigma, BORDER_DEFAULT);
		ABFsrc.data = g;
		bilateralFilter(ABFsrc, ABFg, dec, Colorsigma, Spacesigma, BORDER_DEFAULT);
		ABFsrc.data = r;
		bilateralFilter(ABFsrc, ABFr, dec, Colorsigma, Spacesigma, BORDER_DEFAULT);
		for (int i = 0; i < HEIGHT * WIDTH; i++) {
			b[i] = ABFb.data[i];
			g[i] = ABFg.data[i];
			r[i] = ABFr.data[i];
		}
		cout << __FUNCTION__ << " finished" << endl;
	}
}

ISPResult BlackLevelCorrection(void* data, int32_t argNum, ...) {
	ISPResult result = ISPSuccess;

	uint16_t offset;
	int32_t WIDTH, HEIGHT;
	result = gParam.GetIMGDimension(&WIDTH, &HEIGHT);
	if (result == ISPSuccess) {
		result = gParam.GetBLCParam(&offset);
		if (result != ISPSuccess) {
			cout << __FUNCTION__ << " get BLC offset failed. result:" << result << endl;
		}
	}
	else {
		cout << __FUNCTION__ << " get IMG Dimension failed. result:" << result << endl;
	}

	if (result == ISPSuccess) {
		for (int32_t i = 0; i < WIDTH * HEIGHT; i++) {
			static_cast<uint16_t*>(data)[i] -= offset;
		}
		cout << __FUNCTION__ << " finished" << endl;
	}

	return result;
}

float LSCinterpolation(int32_t WIDTH, int32_t HEIGHT, 
	float LT, float RT, float LB, float RB, 
	int32_t row, int32_t col) 
{
	float TempT, TempB, result;
	TempT = LT - (LT - RT) * (col % (WIDTH / 16)) * 16 / WIDTH;
	TempB = LB - (LB - RB) * (col % (WIDTH / 16)) * 16 / WIDTH;
	result = TempT - (TempT - TempB) * (row % (HEIGHT / 12)) * 12 / HEIGHT;
	return result;
}

ISPResult LensShadingCorrection(void* data, int32_t argNum, ...)
{
	ISPResult result = ISPSuccess;

	int32_t WIDTH, HEIGHT;
	int32_t i, j;
	float R_lsc[13][17], Gr_lsc[13][17], Gb_lsc[13][17], B_lsc[13][17];
	float** ppR = NULL;
	float** ppGr = NULL;
	float** ppGb = NULL;
	float** ppB = NULL;

	ppR = (float**)malloc(sizeof(float*) * 13);
	ppGr = (float**)malloc(sizeof(float*) * 13);
	ppGb = (float**)malloc(sizeof(float*) * 13);
	ppB = (float**)malloc(sizeof(float*) * 13);

	for (i = 0; i < 13; i++) {
		ppR[i] = R_lsc[i];
		ppGr[i] = Gr_lsc[i];
		ppGb[i] = Gb_lsc[i];
		ppB[i] = B_lsc[i];
	}

	result = gParam.GetIMGDimension(&WIDTH, &HEIGHT);
	if (result == ISPSuccess) {
		result = gParam.GetLSCParam(ppR, ppGr, ppGb, ppB);
		if (result != ISPSuccess) {
			cout << __FUNCTION__ << " get LSC lut failed. result:" << result << endl;
		}
	}
	else {
		cout << __FUNCTION__ << " get IMG Dimension failed. result:" << result << endl;
	}
	
	if (result == ISPSuccess) {
		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				if (i % 2 == 0 && j % 2 == 0) {
					static_cast<uint16_t*>(data)[i * WIDTH + j] = static_cast<uint16_t*>(data)[i * WIDTH + j] *
						LSCinterpolation(WIDTH, HEIGHT,
							B_lsc[i * 12 / HEIGHT][j * 16 / WIDTH],
							B_lsc[i * 12 / HEIGHT][j * 16 / WIDTH + 1],
							B_lsc[i * 12 / HEIGHT + 1][j * 16 / WIDTH],
							B_lsc[i * 12 / HEIGHT + 1][j * 16 / WIDTH + 1], i, j);
				}
				if (i % 2 == 0 && j % 2 == 1) {
					static_cast<uint16_t*>(data)[i * WIDTH + j] = static_cast<uint16_t*>(data)[i * WIDTH + j] *
						LSCinterpolation(WIDTH, HEIGHT,
							Gb_lsc[i * 12 / HEIGHT][j * 16 / WIDTH],
							Gb_lsc[i * 12 / HEIGHT][j * 16 / WIDTH + 1],
							Gb_lsc[i * 12 / HEIGHT + 1][j * 16 / WIDTH],
							Gb_lsc[i * 12 / HEIGHT + 1][j * 16 / WIDTH + 1], i, j);
				}
				if (i % 2 == 1 && j % 2 == 0) {
					static_cast<uint16_t*>(data)[i * WIDTH + j] = static_cast<uint16_t*>(data)[i * WIDTH + j] *
						LSCinterpolation(WIDTH, HEIGHT,
							Gr_lsc[i * 12 / HEIGHT][j * 16 / WIDTH],
							Gr_lsc[i * 12 / HEIGHT][j * 16 / WIDTH + 1],
							Gr_lsc[i * 12 / HEIGHT + 1][j * 16 / WIDTH],
							Gr_lsc[i * 12 / HEIGHT + 1][j * 16 / WIDTH + 1], i, j);
				}
				if (i % 2 == 1 && j % 2 == 1) {
					static_cast<uint16_t*>(data)[i * WIDTH + j] = static_cast<uint16_t*>(data)[i * WIDTH + j] *
						LSCinterpolation(WIDTH, HEIGHT,
							R_lsc[i * 12 / HEIGHT][j * 16 / WIDTH],
							R_lsc[i * 12 / HEIGHT][j * 16 / WIDTH + 1],
							R_lsc[i * 12 / HEIGHT + 1][j * 16 / WIDTH],
							R_lsc[i * 12 / HEIGHT + 1][j * 16 / WIDTH + 1], i, j);
				}
			}
		}
		cout << __FUNCTION__ << " finished" << endl;
	}
	free(ppR);
	free(ppGr);
	free(ppGb);
	free(ppB);

	return result;
}

//GCC now is too simple, it should be considered more
ISPResult GreenChannelsCorrection(void* gdata, int32_t argNum, ...)
{
	ISPResult result = ISPSuccess;

	int32_t WIDTH, HEIGHT;
	double GCCWeight;

	result = gParam.GetIMGDimension(&WIDTH, &HEIGHT);
	if (result == ISPSuccess) {
		result = gParam.GetGCCParam(&GCCWeight);
		if(result != ISPSuccess) {
			cout << __FUNCTION__ << " get GCCWeight failed. result:" << result << endl;
		}
	}
	else {
		cout << __FUNCTION__ << " get IMG Dimension failed. result:" << result << endl;
	}

	if (result == ISPSuccess) {
		float temp = 1.0;
		for (int32_t i = 0; i < HEIGHT; i++) {
			for (int32_t j = 1; j < WIDTH; j++) {
				if (i % 2 == 0 && j % 2 == 1 && i > 0 &&
					i < HEIGHT - 1 && j > 0 && j < WIDTH - 1) {
					temp = (static_cast<uint16_t*>(gdata)[(i - 1) * WIDTH + j - 1] +
						static_cast<uint16_t*>(gdata)[(i - 1) * WIDTH + j + 1] +
						static_cast<uint16_t*>(gdata)[(i + 1) * WIDTH + j - 1] +
						static_cast<uint16_t*>(gdata)[(i + 1) * WIDTH + j + 1]) << 2;
					temp = (static_cast<uint16_t*>(gdata)[i * WIDTH + j] << 2) - temp / 4.0;
					if (temp >= 0) {
						static_cast<uint16_t*>(gdata)[i * WIDTH + j] =
							((static_cast<uint16_t*>(gdata)[i * WIDTH + j] << 2) - (uint16_t)(temp * GCCWeight)) >> 2 & 0xffff;
					}
					else {
						temp = -temp;
						static_cast<uint16_t*>(gdata)[i * WIDTH + j] =
							((static_cast<uint16_t*>(gdata)[i * WIDTH + j] << 2) + (uint16_t)(temp * GCCWeight)) >> 2 & 0xffff;
					}
				}
			}
		}
		/*for (int32_t i = 0; i < HEIGHT; i++) {
			for (int32_t j = 1; j < WIDTH; j++) {
				if (i % 2 == 1 && j % 2 == 0 && i > 0 &&
					i < HEIGHT - 1 && j > 0 && j < WIDTH - 1) {
					temp = (static_cast<uint16_t*>(gdata)[(i - 1) * WIDTH + j - 1] +
						static_cast<uint16_t*>(gdata)[(i - 1) * WIDTH + j + 1] +
						static_cast<uint16_t*>(gdata)[(i + 1) * WIDTH + j - 1] +
						static_cast<uint16_t*>(gdata)[(i + 1) * WIDTH + j + 1]) << 2;
					temp = (static_cast<uint16_t*>(gdata)[i * WIDTH + j] << 2) - temp / 4.0;
					if (temp >= 0) {
						static_cast<uint16_t*>(gdata)[i * WIDTH + j] =
							((static_cast<uint16_t*>(gdata)[i * WIDTH + j] << 2) - (uint16_t)(temp * GCCWeight)) >> 2 & 0xffff;
					}
					else {
						temp = -temp;
						static_cast<uint16_t*>(gdata)[i * WIDTH + j] =
							((static_cast<uint16_t*>(gdata)[i * WIDTH + j] << 2) + (uint16_t)(temp * GCCWeight)) >> 2 & 0xffff;
					}
				}
			}
		}*/
		cout << __FUNCTION__ << " finished" << endl;
	}

	return result;
}

ISPResult WhiteBalance(void* data, int32_t argNum, ...)
{
	ISPResult result = ISPSuccess;

	int32_t WIDTH, HEIGHT;
	double bGain, gGain, rGain;

	result = gParam.GetIMGDimension(&WIDTH, &HEIGHT);
	if (result == ISPSuccess) {
		result = gParam.GetWBParam(&rGain, &gGain, &bGain);
		if (result != ISPSuccess) {
			cout << __FUNCTION__ << " get WB gain failed. result:" << result << endl;
		}
	}
	else {
		cout << __FUNCTION__ << " get IMG Dimension failed. result:" << result << endl;
	}

	uint16_t* B = static_cast<uint16_t*>(data);
	uint16_t* G = B + WIDTH * HEIGHT;
	uint16_t* R = G + WIDTH * HEIGHT;

	if (result == ISPSuccess) {
		for (int i = 0; i < WIDTH * HEIGHT; i++)
		{
			B[i] = B[i] * bGain;
			G[i] = G[i] * gGain;
			R[i] = R[i] * rGain;
		}
		cout << __FUNCTION__ << " finished" << endl;
	}

	return result;
}

ISPResult ColorCorrection(void* data, int32_t argNum, ...) {

	ISPResult result = ISPSuccess;
	int32_t WIDTH, HEIGHT;

	Mat A_cc = Mat::zeros(3, 3, CV_32FC1);
	int32_t i, j;
	float temp;
	result = gParam.GetIMGDimension(&WIDTH, &HEIGHT);
	if (result == ISPSuccess) {
		for (i = 0; i < 3; i++) {
			for (j = 0; j < 3; j++) {
				result = gParam.GetCCParam(&temp, i, j);
				A_cc.at<float>(i, j) = temp;
			}
			if (result != ISPSuccess) {
				cout << __FUNCTION__ << " get Color Correction Matrix failed. result:" << result << endl;
				break;
			}
		}
	}
	else {
		cout << __FUNCTION__ << " get IMG Dimension failed. result:" << result << endl;
	}

	if (result == ISPSuccess) {
		uint16_t* B = static_cast<uint16_t*>(data);
		uint16_t* G = B + WIDTH * HEIGHT;
		uint16_t* R = G + WIDTH * HEIGHT;

		Mat Pmatric;
		Mat Oritransform(WIDTH * HEIGHT, 3, CV_32FC1);

		for (i = 0; i < HEIGHT * WIDTH; i++) {
			Oritransform.at<float>(i, 0) = (float)B[i];
			Oritransform.at<float>(i, 1) = (float)G[i];
			Oritransform.at<float>(i, 2) = (float)R[i];
		}

		//ofstream OutFile2("C:\\Users\\penghao6\\Desktop\\变形.txt");
		//OutFile2 << Oritransform;
		//OutFile2.close();

		Pmatric = Oritransform * A_cc;

		//ofstream OutFile3("C:\\Users\\penghao6\\Desktop\\计算结果.txt");
		//OutFile3 << Pmatric;
		//OutFile3.close();

		for (i = 0; i < HEIGHT * WIDTH; i++) {
			if (Pmatric.at<float>(i, 0) > 1023)
				Pmatric.at<float>(i, 0) = 1023;
			if (Pmatric.at<float>(i, 0) < 0)
				Pmatric.at<float>(i, 0) = 0;
			if (Pmatric.at<float>(i, 1) > 1023)
				Pmatric.at<float>(i, 1) = 1023;
			if (Pmatric.at<float>(i, 1) < 0)
				Pmatric.at<float>(i, 1) = 0;
			if (Pmatric.at<float>(i, 2) > 1023)
				Pmatric.at<float>(i, 2) = 1023;
			if (Pmatric.at<float>(i, 2) < 0)
				Pmatric.at<float>(i, 2) = 0;
			B[i] = (uint16_t)Pmatric.at<float>(i, 0);
			G[i] = (uint16_t)Pmatric.at<float>(i, 1);
			R[i] = (uint16_t)Pmatric.at<float>(i, 2);
		}

		//ofstream OutFile4("C:\\Users\\penghao6\\Desktop\\反变形.txt");
		//OutFile4 << Ori;
		//OutFile4.close();

		//ofstream OutFile5("C:\\Users\\penghao6\\Desktop\\反归一化.txt");
		//OutFile5 <<income;
		//OutFile5.close();
		//CharDataSaveAsText(dst.data, "C:\\Users\\penghao6\\Desktop\\output2.txt");
		cout << __FUNCTION__ << " finished" << endl;
	}
	return result;
}

ISPResult GammaCorrection(void* data, int32_t argNum, ...)
{
	ISPResult result = ISPSuccess;

	int32_t WIDTH, HEIGHT;
	uint16_t lut[1024];
	uint16_t* plut = nullptr;
	plut = lut;
	result = gParam.GetIMGDimension(&WIDTH, &HEIGHT);
	if (result == ISPSuccess) {
		result = gParam.GetGAMMAPARAM(plut);
		if (result != ISPSuccess) {
			cout << __FUNCTION__ << " get Gamma lut failed. result:" << result << endl;
		}
	}
	else {
		cout << __FUNCTION__ << " get IMG Dimension failed. result:" << result << endl;
	}

	if (result == ISPSuccess) {
		uint16_t* B = static_cast<uint16_t*>(data);
		uint16_t* G = B + WIDTH * HEIGHT;
		uint16_t* R = G + WIDTH * HEIGHT;

		for (int32_t i = 0; i < WIDTH * HEIGHT; i++)
		{
			B[i] = plut[B[i]];
			G[i] = plut[G[i]];
			R[i] = plut[R[i]];
		}
		cout << __FUNCTION__ << " finished" << endl;
	}
	return result;
}



//生成不同类型的小波
void wavelet(const string _wname, Mat& _lowFilter, Mat& _highFilter)
{

	if (_wname == "haar" || _wname == "db1")
	{
		int N = 2;
		_lowFilter = Mat::zeros(1, N, CV_32F);
		_highFilter = Mat::zeros(1, N, CV_32F);

		_lowFilter.at<float>(0, 0) = 1 / sqrtf(N);
		_lowFilter.at<float>(0, 1) = 1 / sqrtf(N);

		_highFilter.at<float>(0, 0) = -1 / sqrtf(N);
		_highFilter.at<float>(0, 1) = 1 / sqrtf(N);
	}
	if (_wname == "sym2")
	{
		int N = 4;
		float h[] = { -0.483, 0.836, -0.224, -0.129 };
		float l[] = { -0.129, 0.224,    0.837, 0.483 };

		_lowFilter = Mat::zeros(1, N, CV_32F);
		_highFilter = Mat::zeros(1, N, CV_32F);

		for (int i = 0; i < N; i++)
		{
			_lowFilter.at<float>(0, i) = l[i];
			_highFilter.at<float>(0, i) = h[i];
		}
	}
}

//小波分解
Mat waveletDecompose(Mat _src, Mat _lowFilter, Mat _highFilter)
{
	//assert(_src.rows == 1 && _lowFilter.rows == 1 && _highFilter.rows == 1);
	//assert(_src.cols >= _lowFilter.cols && _src.cols >= _highFilter.cols);
	Mat& src = _src;

	int D = src.cols;

	Mat& lowFilter = _lowFilter;
	Mat& highFilter = _highFilter;

	//频域滤波或时域卷积；ifft( fft(x) * fft(filter)) = cov(x,filter) 
	Mat dst1 = Mat::zeros(1, D, src.type());
	Mat dst2 = Mat::zeros(1, D, src.type());

	filter2D(src, dst1, -1, lowFilter);
	filter2D(src, dst2, -1, highFilter);

	//下采样
	Mat downDst1 = Mat::zeros(1, D / 2, src.type());
	Mat downDst2 = Mat::zeros(1, D / 2, src.type());

	resize(dst1, downDst1, downDst1.size());
	resize(dst2, downDst2, downDst2.size());

	//数据拼接
	for (int i = 0; i < D / 2; i++)
	{
		src.at<float>(0, i) = downDst1.at<float>(0, i);
		src.at<float>(0, i + D / 2) = downDst2.at<float>(0, i);

	}
	return src;
}

Mat WDT(const Mat& _src, const string _wname, const int _level)
{
	Mat src = Mat_<float>(_src);
	Mat dst = Mat::zeros(src.rows, src.cols, src.type());
	int N = src.rows;
	int D = src.cols;
	//高通低通滤波器
	Mat lowFilter;
	Mat highFilter;
	wavelet(_wname, lowFilter, highFilter);
	//小波变换
	int t = 1;
	int row = N;
	int col = D;
	while (t <= _level)
	{
		//先进行 行小波变换
		for (int i = 0; i < row; i++)
		{
			//取出src中要处理的数据的一行
			Mat oneRow = Mat::zeros(1, col, src.type());
			for (int j = 0; j < col; j++)
			{
				oneRow.at<float>(0, j) = src.at<float>(i, j);
			}
			oneRow = waveletDecompose(oneRow, lowFilter, highFilter);
			for (int j = 0; j < col; j++)
			{
				dst.at<float>(i, j) = oneRow.at<float>(0, j);
			}
		}
		char s[10];
		_itoa_s(t, s, 10);
		//imshow(s, dst);
		//waitKey();
#if 0
		//    normalize(dst,dst,0,255,NORM_MINMAX);
		IplImage dstImg1 = IplImage(dst);
		cvSaveImage("dst1.jpg", &dstImg1);
#endif

		//小波列变换
		for (int j = 0; j < col; j++)
		{
			Mat oneCol = Mat::zeros(row, 1, src.type());
			for (int i = 0; i < row; i++)
			{
				oneCol.at<float>(i, 0) = dst.at<float>(i, j);//dst,not src
			}
			oneCol = (waveletDecompose(oneCol.t(), lowFilter, highFilter)).t();
			for (int i = 0; i < row; i++)
			{
				dst.at<float>(i, j) = oneCol.at<float>(i, 0);
			}
		}
#if 0
		//    normalize(dst,dst,0,255,NORM_MINMAX);
		IplImage dstImg2 = IplImage(dst);
		cvSaveImage("dst2.jpg", &dstImg2);
#endif
		//更新 
		row /= 2;
		col /= 2;
		t++;
		src = dst;

	}
	return dst;
}



Mat getim(Mat src, int32_t WIDTH, int32_t HEIGHT,
	int depth, int Imgsizey, int Imgsizex, int channel, 
	int strength1, int strength2, int strength3)
{
	int i, j;
	Mat im, im1, im2, im3, im4, im5, im6, temp, im11, im12, im13, im14, imi, imd, imr, tempHL;
	imr = Mat::zeros(src.rows, src.cols, CV_32F);
	imd = Mat::zeros(src.rows, src.cols, CV_32F);
	Mat imL11, imL12, imL13, imL21, imL22, imL23;

	float a, b, c, d;

	im = src.clone(); //Load image in Gray Scale
	imi = Mat::zeros(im.rows, im.cols, CV_8U);
	im.copyTo(imi);

	im.convertTo(im, CV_32F, 1.0, 0.0);

	int redepth = 1;

	//--------------Decomposition-------------------
	while (redepth - 1 < depth) {


		im1 = Mat::zeros(im.rows / 2, im.cols, CV_32F);
		im2 = Mat::zeros(im.rows / 2, im.cols, CV_32F);
		im3 = Mat::zeros(im.rows / 2, im.cols / 2, CV_32F);
		im4 = Mat::zeros(im.rows / 2, im.cols / 2, CV_32F);
		im5 = Mat::zeros(im.rows / 2, im.cols / 2, CV_32F);
		im6 = Mat::zeros(im.rows / 2, im.cols / 2, CV_32F);


		//L1 H and L
		for (int rcnt = 0; rcnt < im.rows; rcnt += 2)
		{
			for (int ccnt = 0; ccnt < im.cols; ccnt++)
			{

				a = im.at<float>(rcnt, ccnt);
				b = im.at<float>(rcnt + 1, ccnt);
				c = (a + b) * 0.707;
				d = (a - b) * 0.707;
				int _rcnt = rcnt / 2;
				im1.at<float>(_rcnt, ccnt) = c;
				im2.at<float>(_rcnt, ccnt) = d;
			}
		}


		//L1 HH and HL
		for (int rcnt = 0; rcnt < im.rows / 2; rcnt++)
		{
			for (int ccnt = 0; ccnt < im.cols; ccnt += 2)
			{

				a = im1.at<float>(rcnt, ccnt);
				b = im1.at<float>(rcnt, ccnt + 1);
				c = (a + b) * 0.707;
				d = (a - b) * 0.707;
				int _ccnt = ccnt / 2;
				im3.at<float>(rcnt, _ccnt) = c;
				im4.at<float>(rcnt, _ccnt) = d;
			}
		}

		//L1 LH and LL
		for (int rcnt = 0; rcnt < im.rows / 2; rcnt++)
		{
			for (int ccnt = 0; ccnt < im.cols; ccnt += 2)
			{

				a = im2.at<float>(rcnt, ccnt);
				b = im2.at<float>(rcnt, ccnt + 1);
				c = (a + b) * 0.707;
				d = (a - b) * 0.707;
				int _ccnt = ccnt / 2;
				im5.at<float>(rcnt, _ccnt) = c;
				im6.at<float>(rcnt, _ccnt) = d;
			}
		}

		for (i = 0; i < src.rows / (pow(2, redepth)); i++) {
			for (j = 0; j < src.cols / (pow(2, redepth)); j++) {
				imd.at<float>(i, j) = im3.at<float>(i, j);
				imd.at<float>(i, j + src.cols / (pow(2, redepth))) = im4.at<float>(i, j);
				imd.at<float>(i + src.rows / (pow(2, redepth)), j) = im5.at<float>(i, j);
				imd.at<float>(i + src.rows / (pow(2, redepth)), j + src.cols / (pow(2, redepth))) = im6.at<float>(i, j);
			}
		}



		im = im3.clone();
		if (redepth == 1) {
			imL11 = im4.clone();
			imL12 = im5.clone();
			imL13 = im6.clone();
		}
		if (redepth == 2) {
			imL21 = im4.clone();
			imL22 = im5.clone();
			imL23 = im6.clone();
		}
		redepth += 1;
	}

	//---------------------------------NR-------------------------------------
	// 高频降噪
	// 1/4 

	for (i = 0; i < imL11.rows; i++) {
		for (j = 0; j < imL11.cols; j++) {
			if (channel == 0) {//Lunma noise
				if (imL11.at<float>(i, j) < strength1)
					imL11.at<float>(i, j) = imL11.at<float>(i, j) / 2;
			}
			else {//Color noise
				if (imL11.at<float>(i, j) < strength1)
					imL11.at<float>(i, j) = imL11.at<float>(i, j) / 2;
			}
		}
	}

	for (i = 0; i < imL12.rows; i++) {
		for (j = 0; j < imL12.cols; j++) {
			if (channel == 0) {//Lunma noise
				if (imL11.at<float>(i, j) < strength2)
					imL11.at<float>(i, j) = imL11.at<float>(i, j) / 2;
			}
			else {//Color noise
				if (imL12.at<float>(i, j) < strength2)
					imL12.at<float>(i, j) = imL12.at<float>(i, j) / 2;
			}
		}
	}

	for (i = 0; i < imL13.rows; i++) {
		for (j = 0; j < imL13.cols; j++) {
			if (channel == 0) {//Lunma noise
				if (imL11.at<float>(i, j) < strength3)
					imL11.at<float>(i, j) = imL11.at<float>(i, j) / 2;
			}
			else {//Color noise
				if (imL13.at<float>(i, j) < strength3)
					imL13.at<float>(i, j) = imL13.at<float>(i, j) / 5;
			}
		}
	}
	// 1/16

	for (i = 0; i < imL21.rows; i++) {
		for (j = 0; j < imL21.cols; j++) {
			if (channel == 0) {//Lunma noise
				if (imL11.at<float>(i, j) < strength1)
					imL11.at<float>(i, j) = imL11.at<float>(i, j) / 2;
			}
			else {//Color noise
				if (imL21.at<float>(i, j) < strength1)
					imL21.at<float>(i, j) = imL21.at<float>(i, j) / 2;
			}
		}
	}

	for (i = 0; i < imL22.rows; i++) {
		for (j = 0; j < imL22.cols; j++) {
			if (channel == 0) {//Lunma noise
				if (imL11.at<float>(i, j) < strength2)
					imL11.at<float>(i, j) = imL11.at<float>(i, j) / 2;
			}
			else {//Color noise
				if (imL21.at<float>(i, j) < strength2)
					imL22.at<float>(i, j) = imL22.at<float>(i, j) / 2;
			}
		}
	}

	for (i = 0; i < imL23.rows; i++) {
		for (j = 0; j < imL23.cols; j++) {
			if (channel == 0) {//Lunma noise
				if (imL11.at<float>(i, j) < strength3)
					imL11.at<float>(i, j) = imL11.at<float>(i, j) / 2;
			}
			else {//Color noise
				if (imL21.at<float>(i, j) < strength3)
					imL23.at<float>(i, j) = imL23.at<float>(i, j) / 5;
			}
		}
	}

	// 低频降噪
	// 1/64
	for (i = 0; i < im.rows; i++) {
		for (j = 0; j < im.cols; j++) {
			if (im6.at<float>(i, j) < strength3)
				im6.at<float>(i, j) = im6.at<float>(i, j) / 5;
		}
	}
	/*
	for (i = 0; i < im.rows; i++) {
		for (j = 0; j < im.cols; j++) {
			if (im5.at<float>(i, j) < 2)
				im5.at<float>(i, j) = im6.at<float>(i, j) / 2;
		}
	}
	for (i = 0; i < im.rows; i++) {
		for (j = 0; j < im.cols; j++) {
			if (im4.at<float>(i, j) < 2)
				im4.at<float>(i, j) = im6.at<float>(i, j) / 2;
		}
	}*/

	//---------------------------------Reconstruction-------------------------------------
	redepth = 1;
	while (redepth - 1 < depth) {

		im11 = Mat::zeros(im.rows, im.cols * 2, CV_32F);
		im12 = Mat::zeros(im.rows, im.cols * 2, CV_32F);
		im13 = Mat::zeros(im.rows, im.cols * 2, CV_32F);
		im14 = Mat::zeros(im.rows, im.cols * 2, CV_32F);

		for (int rcnt = 0; rcnt < im.rows; rcnt++)
		{
			for (int ccnt = 0; ccnt < im.cols; ccnt++)
			{
				int _ccnt = ccnt * 2;
				if (redepth == 1) {
					im11.at<float>(rcnt, _ccnt) = im.at<float>(rcnt, ccnt);     //Upsampling of stage I
					im12.at<float>(rcnt, _ccnt) = im4.at<float>(rcnt, ccnt);
					im13.at<float>(rcnt, _ccnt) = im5.at<float>(rcnt, ccnt);
					im14.at<float>(rcnt, _ccnt) = im6.at<float>(rcnt, ccnt);
				}
				if (redepth == 2) {
					im11.at<float>(rcnt, _ccnt) = im.at<float>(rcnt, ccnt);     //Upsampling of stage I
					im12.at<float>(rcnt, _ccnt) = imL21.at<float>(rcnt, ccnt);
					im13.at<float>(rcnt, _ccnt) = imL22.at<float>(rcnt, ccnt);
					im14.at<float>(rcnt, _ccnt) = imL23.at<float>(rcnt, ccnt);
				}
				if (redepth == 3) {
					im11.at<float>(rcnt, _ccnt) = im.at<float>(rcnt, ccnt);     //Upsampling of stage I
					im12.at<float>(rcnt, _ccnt) = imL11.at<float>(rcnt, ccnt);
					im13.at<float>(rcnt, _ccnt) = imL12.at<float>(rcnt, ccnt);
					im14.at<float>(rcnt, _ccnt) = imL13.at<float>(rcnt, ccnt);
				}
			}
		}

		for (int rcnt = 0; rcnt < im.rows; rcnt++)
		{
			for (int ccnt = 0; ccnt < im.cols * 2; ccnt += 2)
			{
				a = im11.at<float>(rcnt, ccnt);
				b = im12.at<float>(rcnt, ccnt);
				c = (a + b) * 0.707;
				im11.at<float>(rcnt, ccnt) = c;
				d = (a - b) * 0.707;                           //Filtering at Stage I
				im11.at<float>(rcnt, ccnt + 1) = d;
				a = im13.at<float>(rcnt, ccnt);
				b = im14.at<float>(rcnt, ccnt);
				c = (a + b) * 0.707;
				im13.at<float>(rcnt, ccnt) = c;
				d = (a - b) * 0.707;
				im13.at<float>(rcnt, ccnt + 1) = d;
			}
		}

		temp = Mat::zeros(im.rows * 2, im.cols * 2, CV_32F);
		tempHL = Mat::zeros(im.rows * 2, im.cols * 2, CV_32F);

		for (int rcnt = 0; rcnt < im.rows; rcnt++)
		{
			for (int ccnt = 0; ccnt < im.cols * 2; ccnt++)
			{

				int _rcnt = rcnt * 2;
				tempHL.at<float>(_rcnt, ccnt) = im11.at<float>(rcnt, ccnt);     //Upsampling at stage II
				temp.at<float>(_rcnt, ccnt) = im13.at<float>(rcnt, ccnt);
			}
		}

		for (int rcnt = 0; rcnt < im.rows * 2; rcnt += 2)
		{
			for (int ccnt = 0; ccnt < im.cols * 2; ccnt++)
			{

				a = tempHL.at<float>(rcnt, ccnt);
				b = temp.at<float>(rcnt, ccnt);
				c = (a + b) * 0.707;
				tempHL.at<float>(rcnt, ccnt) = c;                                      //Filtering at Stage II
				d = (a - b) * 0.707;
				tempHL.at<float>(rcnt + 1, ccnt) = d;
			}
		}

		im = tempHL.clone();
		redepth += 1;

	}

	imr = im.clone();
	imr.convertTo(imr, CV_8U);
	imd.convertTo(imd, CV_8U);
	//namedWindow("Wavelet Decomposition",0);
	//resizeWindow("Wavelet Decomposition", Imgsizex, Imgsizey);
	//imshow("Wavelet Decomposition", imd);

	Mat dst(HEIGHT, WIDTH, CV_8UC3, Scalar(0, 0, 0));
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			dst.data[i * 3 * WIDTH + 3 * j] = 0;
			dst.data[i * 3 * WIDTH + 3 * j + 1] = imd.data[i * WIDTH + j];
			dst.data[i * 3 * WIDTH + 3 * j + 2] = 0;
		}
	}
	//BYTE *BMPdata = new BYTE[WIDTH * HEIGHT * dst.channels()];
	//setBMP(BMPdata, dst);
	//setBMP(BMPdata, WIDTH, HEIGHT, Rdata, Gdata, Bdata);

	//saveBMP(BMPdata, "C:\\Users\\penghao6\\Desktop\\小波分解.BMP");
	//delete BMPdata;
	return imr;
}

ISPResult SmallWaveNR(void* data, int32_t argNum, ...)
{
	ISPResult result = ISPSuccess;
	int32_t WIDTH, HEIGHT;

	int32_t strength1;
	int32_t strength2;
	int32_t strength3;

	result = gParam.GetIMGDimension(&WIDTH, &HEIGHT);
	if (result == ISPSuccess) {
		result = gParam.GetWNRPARAM(&strength1, &strength2, &strength3);
		if (result != ISPSuccess) {
			cout << __FUNCTION__ << " get SWNR param failed. result:" << result << endl;
		}
	}
	else {
		cout << __FUNCTION__ << " get IMG Dimension failed. result:" << result << endl;
	}

	va_list(va);
	__crt_va_start(va, argNum);
	int32_t Imgsizey = static_cast<int32_t>(__crt_va_arg(va, int32_t));
	int32_t Imgsizex = static_cast<int32_t>(__crt_va_arg(va, int32_t));
	__crt_va_end(va);

	int32_t i, j;
	Mat onechannel(HEIGHT, WIDTH, CV_8U);
	Mat onechannel2(HEIGHT, WIDTH, CV_8U);

	if (result == ISPSuccess) {
		//会产生严重格子现象
		//Y通道小波
		/*for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				onechannel.data[i*WIDTH + j] = static_cast<uchar*>(data)[i * 3 * WIDTH + 3 * j];
			}
		}
		onechannel2 = getim(onechannel, WIDTH, HEIGHT, 3, Imgsizey, Imgsizex, 1, 50, 30, 20);
		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				static_cast<uchar*>(data)[i * 3 * WIDTH + 3 * j ]= onechannel2.data[i*WIDTH + j];
				//YUV.data[i * 3 * WIDTH + 3 * j + 1] = onechannel2.data[i*WIDTH + j];
				//YUV.data[i * 3 * WIDTH + 3 * j + 2] = onechannel2.data[i*WIDTH + j];
			}
		}*/

		//U通道小波
		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				onechannel.data[i * WIDTH + j] = static_cast<uchar*>(data)[i * 3 * WIDTH + 3 * j + 1];
			}
		}
		onechannel2 = getim(onechannel, WIDTH, HEIGHT, 3, Imgsizey, Imgsizex, 1, strength1, strength2, strength3);
		//onechannel2 = onechannel.clone();
		onechannel = onechannel2.clone();
		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				//YUV.data[i * 3 * WIDTH + 3 * j ]= onechannel2.data[i*WIDTH + j];
				static_cast<uchar*>(data)[i * 3 * WIDTH + 3 * j + 1] = onechannel2.data[i * WIDTH + j];
				//YUV.data[i * 3 * WIDTH + 3 * j + 2] = onechannel2.data[i*WIDTH + j];
			}
		}

		//V通道小波
		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				onechannel.data[i * WIDTH + j] = static_cast<uchar*>(data)[i * 3 * WIDTH + 3 * j + 2];
			}
		}
		onechannel2 = getim(onechannel, WIDTH, HEIGHT, 3, Imgsizey, Imgsizex, 2, strength1, strength2, strength3);
		//onechannel2 = onechannel.clone();
		onechannel = onechannel2.clone();
		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				//YUV.data[i * 3 * WIDTH + 3 * j ]= onechannel2.data[i*WIDTH + j];
				//YUV.data[i * 3 * WIDTH + 3 * j + 1] = onechannel2.data[i*WIDTH + j];
				static_cast<uchar*>(data)[i * 3 * WIDTH + 3 * j + 2] = onechannel2.data[i * WIDTH + j];
			}
		}
		cout << __FUNCTION__ << " finished" << endl;
	}
	return result;
}


ISPResult Sharpness(void* data, int32_t argNum, ...)
{
	ISPResult result = ISPSuccess;
	int32_t WIDTH, HEIGHT;

	double alpha;
	int32_t coreSzie;
	int32_t delta;
	result = gParam.GetIMGDimension(&WIDTH, &HEIGHT);
	if (result == ISPSuccess) {
		result = gParam.GetEERPARAM(&alpha, &coreSzie, &delta);
		if (result != ISPSuccess) {
			cout << __FUNCTION__ << " get SWNR param failed. result:" << result << endl;
		}
	}
	else {
		cout << __FUNCTION__ << " get IMG Dimension failed. result:" << result << endl;
	}

	if (result == ISPSuccess) {
		Mat blurred;
		Mat Y(HEIGHT, WIDTH, CV_8UC1, Scalar(0));
		int32_t i, j;
		int32_t mask;
		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				Y.data[i * WIDTH + j] = static_cast<uchar*>(data)[i * 3 * WIDTH + 3 * j];
			}
		}

		GaussianBlur(Y, blurred, Size(coreSzie, coreSzie), delta, delta);

		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				mask = Y.data[i * WIDTH + j] - blurred.data[i * WIDTH + j];
				mask = (int)(static_cast<uchar*>(data)[i * 3 * WIDTH + 3 * j] + (int)(alpha * mask));
				if (mask > 255) {
					mask = 255;
				}
				else if (mask < 0) {
					mask = 0;
				}
				static_cast<uchar*>(data)[i * 3 * WIDTH + 3 * j] = mask & 255;
			}
		}
		cout << __FUNCTION__ << " finished" << endl;
	}

	/*if (result == ISPSuccess) {
		int32_t i, j;
		Mat onechannel(HEIGHT, WIDTH, CV_8U);

		//NRYUV = YUV.clone();
		//EdgePreservedNR(YUV, NRYUV, 0.7, true);
		//YUV = NRYUV.clone();

		Mat kernel(3, 3, CV_32F, cv::Scalar(0));
		kernel.at<float>(1, 1) = 5.0;
		kernel.at<float>(0, 1) = -1.0;
		kernel.at<float>(1, 0) = -1.0;
		kernel.at<float>(1, 2) = -1.0;
		kernel.at<float>(2, 1) = -1.0;

		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				onechannel.data[i * WIDTH + j] = static_cast<uchar*>(data)[i * 3 * WIDTH + 3 * j];
			}
		}

		Mat blurried, sharped;
		blurried = onechannel.clone();
		sharped = onechannel.clone();
		bilateralFilter(onechannel, blurried, 5, 11, 15, BORDER_DEFAULT);
		filter2D(blurried, sharped, blurried.depth(), kernel);

		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				static_cast<uchar*>(data)[i * 3 * WIDTH + 3 * j] = sharped.data[i * WIDTH + j];
				//YUV.data[i * 3 * WIDTH + 3 * j + 1] = onechannel2.data[i*WIDTH + j];
				//YUV.data[i * 3 * WIDTH + 3 * j + 2] = onechannel2.data[i*WIDTH + j];
			}
		}

		cout << __FUNCTION__ << " finished" << endl;
	}*/

	return result;
}