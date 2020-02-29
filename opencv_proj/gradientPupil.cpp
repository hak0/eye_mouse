#include "stdafx.h"
#include "gradientPupil.h"

cv::Point2i findPupilCenterGradient(cv::Mat image) {
	// referenced: https://github.com/trishume/eyeLike
	// consts:
	const double kGradientThreshold = 50.0;
	const int kWeightBlurSize = 5;
	cv::Mat imageGray;
	cvtColor(image, imageGray, cv::COLOR_BGR2GRAY);
	cv::equalizeHist(imageGray, imageGray);
	// Calculate the gradient
	cv::Mat gradientX = computeMatXGradient(imageGray), gradientY = computeMatXGradient(imageGray.t()).t();
	// Normalize and threshold the gradient
	// compute gradients' magnitude
	cv::Mat mags = matrixMagnitude(gradientX, gradientY);
	// compute the threshold
	double gradientThresh = computeDynamicThreshold(mags, kGradientThreshold);
	// normalize and threshold
	for (int y = 0; y < imageGray.rows; ++y) {
		double *Xr = gradientX.ptr<double>(y), *Yr = gradientY.ptr<double>(y);
		const double *Mr = mags.ptr<double>(y);
		for (int x = 0; x < imageGray.cols; ++x) {
			double gX = Xr[x], gY = Yr[x];
			double magnitude = Mr[x];
			if (magnitude > gradientThresh) {
				Xr[x] = gX / magnitude;
				Yr[x] = gY / magnitude;
			}
			else {
				Xr[x] = 0.0;
				Yr[x] = 0.0;
			}
		}
	}
	// create a blurred and inverted image for weighting
	// why blurring? Aimed at denoising.
	cv::Mat weight;
	GaussianBlur(imageGray, weight, cv::Size(kWeightBlurSize, kWeightBlurSize), 0, 0);
	for (int y = 0; y < weight.rows; ++y) {
		unsigned char *row = weight.ptr<unsigned char>(y);
		for (int x = 0; x < weight.cols; ++x) {
			row[x] = (255 - row[x]);
		}
	}
	cv::Mat outSum = cv::Mat::zeros(image.rows, image.cols, CV_64F);
	// run the algorithm for each possible gradient location
	// Note: these loops are reversed from the way the paper does them
	// it evaluates every possible center for each gradient location instead of
	// every possible gradient location for every center.
	// 我根本不会英文上面都是我编的现在我编不下去了。
	//     这个算法的本质就是暴力穷举图像上的任意两点，看看它们之间的方向(即两点连成的矢量除
	// 以该矢量的模长）是否和出发点的梯度方向相同。两单位方向向量作点乘，乘积越小，说明方向
	// 差距越大，终点就越不可能是瞳孔中心。因为瞳孔中心是各梯度方向的会聚点，所以任意一点到
	// 它的方向一定和该点的梯度方向相同。因此中心点表现在这个函数里就是值总和最大。
	// printf("Eye Size: %ix%i\n", outSum.cols, outSum.rows);
	for (int y = 0; y < weight.rows; ++y) {
		const double *Xr = gradientX.ptr<double>(y), *Yr = gradientY.ptr<double>(y);
		for (int x = 0; x < weight.cols; ++x) {
			double gX = Xr[x], gY = Yr[x];
			if (gX == 0.0 && gY == 0.0) {
				continue;
			}
			testPossibleCentersFormula(x, y, weight, gX, gY, outSum);
		}
	}
	// the result is a very very big number, so we need to scale it down
	// to load it into a 32-bit float datatype.
	double numGradients = (weight.rows * weight.cols);
	cv::Mat out;
	//outSum.convertTo(out, CV_32F, 1.0 / numGradients);
	// find the maxmium point
	cv::Point2i maxP;
	double maxVal;
	//cv::minMaxLoc(out, NULL, &maxVal, &maxP);
	cv::minMaxLoc(outSum, NULL, &maxVal, NULL, &maxP);
	return maxP;
}

cv::Mat matrixMagnitude(const cv::Mat &matX, const cv::Mat &matY) {
	cv::Mat mags(matX.rows, matX.cols, CV_64F);
	for (int y = 0; y < matX.rows; ++y) {
		const double *Xr = matX.ptr<double>(y), *Yr = matY.ptr<double>(y);
		double *Mr = mags.ptr<double>(y);
		for (int x = 0; x < matX.cols; ++x) {
			double gX = Xr[x], gY = Yr[x];
			double magnitude = sqrt((gX * gX) + (gY * gY));
			Mr[x] = magnitude;
		}
	}
	return mags;
}

cv::Mat computeMatXGradient(const cv::Mat &mat) {
	//calculate gradient in horizontal direction for a mat
	cv::Mat out(mat.rows, mat.cols, CV_64F);
	for (int y = 0; y < mat.rows; y++) {
		// initialize two pointers for input and output mat
		const uchar *InputPointer = mat.ptr<uchar>(y);
		double *OutputPointer = out.ptr<double>(y);

		//calculate the difference(差分) in x direction
		OutputPointer[0] = InputPointer[1] - InputPointer[0];
		for (int x = 1; x < mat.cols - 1; x++) {
			OutputPointer[x] = (InputPointer[x + 1] - InputPointer[x - 1]) / 2;
		}
		OutputPointer[mat.cols - 1] = InputPointer[mat.cols - 1] - InputPointer[mat.cols - 2];
	}
	return out;
}

double computeDynamicThreshold(const cv::Mat &mat, double stdDevFactor) {
	cv::Scalar stdMagnGrad, meanMagnGrad;
	cv::meanStdDev(mat, meanMagnGrad, stdMagnGrad);
	double stdDev = stdMagnGrad[0] / sqrt(mat.rows*mat.cols);
	return stdDevFactor * stdDev + meanMagnGrad[0];
}

void testPossibleCentersFormula(int x, int y, const cv::Mat &weight, double gx, double gy, cv::Mat &out) {
	// for all possible centers
	for (int cy = 0; cy < out.rows; ++cy) {
		double *Or = out.ptr<double>(cy);
		const unsigned char *Wr = weight.ptr<unsigned char>(cy);
		for (int cx = 0; cx < out.cols; ++cx) {
			if (x == cx && y == cy) {
				continue;
			}
			// create a vector from the possible center to the gradient origin
			double dx = x - cx;
			double dy = y - cy;
			// normalize d
			double magnitude = sqrt((dx * dx) + (dy * dy));
			dx = dx / magnitude;
			dy = dy / magnitude;
			double dotProduct = dx * gx + dy * gy;
			dotProduct = std::max(0.0, dotProduct);
			// square and multiply by the weight
			Or[cx] += dotProduct * dotProduct * Wr[cx];
		}
	}
}