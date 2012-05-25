/*
 * Kinect module based on OpenNI for URBI
 * Copyright (C) 2012  Lukasz Malek
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * File:   UEigenfaces.cpp
 * Author: lmalek
 * 
 * Created on 21 marzec 2012, 09:55
 */

#include "UEigenfaces.h"
#include <iostream>
#include <boost/foreach.hpp>
#include <fstream>

using namespace std;
using namespace urbi;

UEigenfaces::UEigenfaces(const std::string& name) : UObject(name) {
    cerr << "[UEigenfaces]::UEigenfaces()" << endl;
    UBindFunction(UEigenfaces, init);
    faceWidth = 92;
    faceHeight = 112;
}

UEigenfaces::~UEigenfaces() {
    cerr << "[UEigenfaces]::~UEigenfaces()" << endl;
}

void UEigenfaces::init(bool imageFlag, bool depthFlag, bool userFlag) {
    cerr << "[UEigenfaces]::init()" << endl;

    // Bind all variables
    UBindVar(UEigenfaces, image);
    UBindVar(UEigenfaces, imageWidth);
    UBindVar(UEigenfaces, imageHeight);

    // Bind all functions
    UBindThreadedFunction(UEigenfaces, loadData, LOCK_INSTANCE);
    UBindThreadedFunction(UEigenfaces, saveData, LOCK_INSTANCE);
    UBindThreadedFunction(UEigenfaces, train, LOCK_INSTANCE);
    UBindThreadedFunction(UEigenfaces, updateDatabase, LOCK_INSTANCE);
    UBindFunction(UEigenfaces, find);
    UBindFunction(UEigenfaces, getFacesCount);
    UBindFunction(UEigenfaces, getFacesNames);
    UBindFunction(UEigenfaces, getFaceImagesCount);
    UBindFunction(UEigenfaces, getFaceImage);
}

bool UEigenfaces::loadData(const std::string& fileName) {
    ifstream ifs(fileName.c_str());

    boost::archive::xml_iarchive ia(ifs);

    // Uzupełnienie facesWidth, facesHeight, faces z pliku fileName
    ia >> boost::serialization::make_nvp("UEigenfaces", *this);

    std::vector<std::string> labels;
    std::vector<cv::Mat> images;

    BOOST_FOREACH(FacePair p, faces) {
        images.push_back(p.first);
        labels.push_back(p.second);
    }

    eigenfaces = new Eigenfaces(images, labels, numComponents);
    std::string predicted = eigenfaces->predict(eigenfaces->mean(),distMean);
    cout << "distMean = " << distMean << endl;

    return true;
}

bool UEigenfaces::saveData(const std::string& fileName) const {
    ofstream ofs(fileName.c_str());

    boost::archive::xml_oarchive oa(ofs);
    oa << boost::serialization::make_nvp("UEigenfaces", *this);

    return true;
}

bool UEigenfaces::train(urbi::UImage src, const std::string& name) {
    uchar channel_type;
    if (src.imageFormat == IMAGE_GREY8) {
        channel_type = CV_8UC1;
    }
    if (src.imageFormat == IMAGE_RGB) {
        channel_type = CV_8UC3;
    } else {
        throw std::runtime_error("[UEigenfaces]::find() : Unsupported image format: ");
    }
    cv::Mat testSample(cv::Size(src.width, src.height), channel_type);
    if (channel_type == CV_8UC3)
        cvtColor(testSample, testSample, CV_RGB2GRAY);
    cv::resize(testSample, testSample, cv::Size(faceWidth, faceHeight));
    FacePair newFace = make_pair(testSample, name);
    faces.push_back(newFace);
}

bool UEigenfaces::updateDatabase() {
    std::vector<std::string> labels;
    std::vector<cv::Mat> images;

    BOOST_FOREACH(FacePair p, faces) {
        images.push_back(p.first);
        labels.push_back(p.second);
    }
    eigenfaces = new Eigenfaces(images, labels, numComponents);
    eigenfaces->predict(eigenfaces->mean(), distMean);
    cout << "distMean = " << distMean << endl;
}

std::string UEigenfaces::find(urbi::UImage src) const {
    uchar channel_type;
    double dist;
    std::string predicted;
    if (src.imageFormat == IMAGE_GREY8) {
        channel_type = CV_8UC1;
    }
    if (src.imageFormat == IMAGE_RGB) {
        channel_type = CV_8UC3;
    } else {
        throw std::runtime_error("[UEigenfaces]::find() : Unsupported image format: ");
    }
    cv::Mat testSample(cv::Size(src.width, src.height), channel_type);
    if (channel_type == CV_8UC3)
        cvtColor(testSample, testSample, CV_RGB2GRAY);
    cv::resize(testSample, testSample, cv::Size(faceWidth, faceHeight));
    predicted = eigenfaces->predict(testSample, dist);
    if (dist > distMean)
        predicted = "";
    return predicted;
}

int UEigenfaces::getFacesCount() const {
    std::vector<std::string> labels;

    BOOST_FOREACH(FacePair p, faces) {
        labels.push_back(p.second);
    }
    std::sort(labels.begin(), labels.end());
    labels.erase(std::unique(labels.begin(), labels.end()), labels.end());
    return labels.size();
}

std::vector<std::string> UEigenfaces::getFacesNames() {
    std::vector<std::string> labels;

    BOOST_FOREACH(FacePair p, faces) {
        labels.push_back(p.second);
    }
    std::sort(labels.begin(), labels.end());
    labels.erase(std::unique(labels.begin(), labels.end()), labels.end());
    return labels;
}

int UEigenfaces::getFaceImagesCount(const std::string& name) {
    int index = 0;
    urbi::UImage result;
    result.imageFormat = IMAGE_UNKNOWN;

    BOOST_FOREACH(FacePair p, faces) {
        if (p.second == name)
            index++;
    }
    return index;
}

urbi::UImage UEigenfaces::getFaceImage(const std::string& name, int number) {
    int index = 0;
    urbi::UImage result;
    result.imageFormat = IMAGE_UNKNOWN;

    BOOST_FOREACH(FacePair p, faces) {
        if (p.second == name) {
            if (index == number) {
                urbi::UImage mBinImage;
                mBinImage.imageFormat = IMAGE_GREY8;
                mBinImage.width = p.first.cols;
                mBinImage.height = p.first.rows;
                mBinImage.size = mBinImage.width * mBinImage.height;
                mBinImage.data = new uint8_t[mBinImage.size];
                memcpy(mBinImage.data, p.first.data, mBinImage.size);
                result = mBinImage;
                return result;
            }
            index++;
        }
    }
    return result;
}

UStart(UEigenfaces);
