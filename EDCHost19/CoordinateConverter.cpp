#include "stdafx.h"
#include "CoordinateConverter.h"

#include <opencv2\imgproc.hpp>
#include <Eigen>

using namespace cv;
using namespace Eigen;

inline Vector3d ToEigenVector3(Point2f p)
{
	return Vector3d(p.x, p.y, 1.0);
}

inline Point2f ToCvPoint(Vector3d v)
{
	return Point2f(v[0] / v[2], v[1] / v[2]);
}


static void getPerspectiveTransform(Point2f(&src)[4], Point2f(&dst)[4], Matrix3d& res)
{
	// Copy from OpenCv Source Code

	Matrix<double, 8, 8> A;
	Matrix<double, 8, 1> B;
	for (int i = 0; i < 4; ++i)
	{
		A(i, 0) = A(i + 4, 3) = src[i].x;
		A(i, 1) = A(i + 4, 4) = src[i].y;
		A(i, 2) = A(i + 4, 5) = 1;

		A(i, 3) = A(i, 4) = A(i, 5) =
			A(i + 4, 0) = A(i + 4, 1) = A(i + 4, 2) = 0.0;

		A(i, 6) = -src[i].x*dst[i].x;
		A(i, 7) = -src[i].y*dst[i].x;
		A(i + 4, 6) = -src[i].x*dst[i].y;
		A(i + 4, 7) = -src[i].y*dst[i].y;

		B[i] = dst[i].x;
		B[i + 4] = dst[i].y;
	}

	Matrix<double, 8, 1> X = A.jacobiSvd(ComputeThinU | ComputeThinV).solve(B);

	res(0, 0) = X[0];
	res(0, 1) = X[1];
	res(0, 2) = X[2];
	res(1, 0) = X[3];
	res(1, 1) = X[4];
	res(1, 2) = X[5];
	res(2, 0) = X[6];
	res(2, 1) = X[7];
	res(2, 2) = 1.0;
}

void CoordinateConverter::updateParam()
{
	Point2f displayCorners[4] = {
		{0,0},
		{(float)_param.DisplaySize.width, 0},
		{0, (float)_param.LogicSize.height},
		{ (float)_param.DisplaySize.width, (float)_param.DisplaySize.height},
	};

	transformMatrix = getPerspectiveTransform(_param.CornersInCamera, displayCorners);

	Point2f logicCorners[4] = {
		{0, 0},
		{ (float)_param.LogicSize.width, 0},
		{0, (float)_param.LogicSize.height},
		{ (float)_param.LogicSize.width, (float)_param.LogicSize.height},
	};

	getPerspectiveTransform(_param.CornersInCamera, logicCorners, _cam2logic);
	_logic2cam = _cam2logic.inverse();
}

void CoordinateConverter::TransformImage(const cv::Mat &src, cv::Mat& dst)
{
	dst.create(_param.DisplaySize, src.type());
	cv::warpPerspective(src, dst, transformMatrix, _param.DisplaySize);
}

Point2f CoordinateConverter::cam2logic(const Point2f &p)
{
	auto p1 = ToCvPoint(_cam2logic*ToEigenVector3(p));
	return Point2f(p1.y, p1.x);
}

Point2f CoordinateConverter::logic2cam(const Point2f &p)
{
	return ToCvPoint(_logic2cam*ToEigenVector3(p));
}

