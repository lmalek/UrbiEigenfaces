/*
 * Copyright (c) 2011. Philipp Wagner <bytefish[at]gmx[dot]de>.
 * Released to public domain under terms of the BSD Simplified license.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the organization nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *   See <http://www.opensource.org/licenses/bsd-license>
 */

#ifndef HELPER_CPP_
#define HELPER_CPP_

#include "helper.hpp"

using namespace cv;

//! sort order for shuffle
template<typename _Tp>
class cv::SortByFirstAscending_ {
public:
	bool operator()(const std::pair<_Tp,int>& left, const std::pair<_Tp,int>& right) {
		return left.first < right.first;
	}
};

//! descending sort operator
template<typename _Tp>
class cv::SortByFirstDescending_ {
public:
	bool operator()(const std::pair<_Tp,int>& left, const std::pair<_Tp,int>& right) {
		return left.first > right.first;
	}
};


void cv::sortMatrixByColumn(const Mat& src, Mat& dst, vector<int> sorted_indices) {
	dst.create(src.rows, src.cols, src.type());
	for(int idx = 0; idx < sorted_indices.size(); idx++) {
		Mat originalCol = src.col(sorted_indices[idx]);
		Mat sortedCol = dst.col(idx);
		originalCol.copyTo(sortedCol);
	}
}

Mat cv::sortMatrixByColumn(const Mat& src, vector<int> sorted_indices) {
	Mat dst;
	sortMatrixByColumn(src, dst, sorted_indices);
	return dst;
}

void cv::sortMatrixByRow(const Mat& src, Mat& dst, vector<int> sorted_indices) {
	dst.create(src.rows, src.cols, src.type());
	for(int idx = 0; idx < sorted_indices.size(); idx++) {
		Mat originalRow = src.row(sorted_indices[idx]);
		Mat sortedRow = dst.row(idx);
		originalRow.copyTo(sortedRow);
	}
}

Mat cv::sortMatrixByRow(const Mat& src, vector<int> sorted_indices) {
	Mat dst;
	sortMatrixByRow(src, dst, sorted_indices);
	return dst;
}

Mat cv::asColumnMatrix(const vector<Mat>& src, int type) {
	int n = src.size();
	// no data? so here's an empty matrix
	if(n == 0)
		return Mat();
	int d = src[0].total();
	Mat data(d, n, type);
	for(int i = 0; i < src.size(); i++) {
		Mat tmp,
			xi = data.col(i);
		src[i].convertTo(tmp, type);
		tmp.reshape(1, d).copyTo(xi);
	}
	return data;
}

Mat cv::asRowMatrix(const vector<Mat>& src, int type) {
	int n = src.size();
	// no data? so here's an empty matrix
	if(n == 0)
		return Mat();
	int d = src[0].total();
	Mat data(n, d, type);
	for(int i = 0; i < src.size(); i++) {
		Mat tmp,
			xi = data.row(i);
		src[i].convertTo(tmp, type);
		tmp.reshape(1, 1).copyTo(xi);
	}
	return data;
}

Mat cv::transpose(const Mat& src) {
		Mat dst;
		transpose(src, dst);
		return dst;
}

Mat cv::toGrayscale(const Mat& src) {
	Mat dst;
	cv::normalize(src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
	return dst;
}

string cv::num2str(int i) {
	stringstream ss;
	ss << i;
	return ss.str();
}

#endif /* HELPER_CPP_ */
