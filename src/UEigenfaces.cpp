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

UEigenfaces::UEigenfaces(const std::string& name) : UObject(name), eigenfaces(NULL) {
    cerr << "[UEigenfaces]::UEigenfaces()" << endl;
    UBindFunction(UEigenfaces, init);
    faceWidth = 92;
    faceHeight = 112;
}

UEigenfaces::~UEigenfaces() {
    cerr << "[UEigenfaces]::~UEigenfaces()" << endl;
}

void UEigenfaces::init(int i) {
    cerr << "[UEigenfaces]::init()" << endl;

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
    UBindFunction(UEigenfaces, getImageWidth);
    UBindFunction(UEigenfaces, getImageHeight);
    UBindFunction(UEigenfaces, getTestFace);
    UBindFunction(UEigenfaces, getThreshold);
    UBindFunction(UEigenfaces, setThreshold);
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
    std::string predicted = eigenfaces->predict(eigenfaces->mean(), distMean);
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
    } else if (src.imageFormat == IMAGE_RGB) {
        channel_type = CV_8UC3;
    } else {
        throw std::runtime_error("[UEigenfaces]::find() : Unsupported image format: ");
    }
    cv::Mat testSample(cv::Size(src.width, src.height), channel_type, src.data);
    if (channel_type == CV_8UC3)
        cvtColor(testSample, testSample, CV_RGB2GRAY);
    cv::resize(testSample, testSample, cv::Size(faceWidth, faceHeight));
    FacePair newFace = make_pair(testSample, name);
    faces.push_back(newFace);
    //    images.push_back(testSample);
    //    labels.push_back(name);
}

bool UEigenfaces::updateDatabase(int components) {
    std::vector<std::string> labels;
    std::vector<cv::Mat> images;

    numComponents = components;
    if (eigenfaces)
        delete eigenfaces;

    BOOST_FOREACH(FacePair p, faces) {
        cv::Mat tmp = Mat(cv::Size(p.first.cols, p.first.rows), p.first.type(), p.first.data);
        images.push_back(tmp);
        labels.push_back(p.second);
    }
    eigenfaces = new Eigenfaces(images, labels, numComponents);
    eigenfaces->predict(eigenfaces->mean(), distMean);
    thresh = distMean;
    cout << "distMean = " << distMean << endl;
}

std::string UEigenfaces::find(urbi::UImage src) const {
    uchar channel_type;
    double dist;
    std::string predicted;
    if (src.imageFormat == IMAGE_GREY8) {
        channel_type = CV_8UC1;
    } else if (src.imageFormat == IMAGE_RGB) {
        channel_type = CV_8UC3;
    } else {
        throw std::runtime_error("[UEigenfaces]::find() : Unsupported image format: ");
    }
    cv::Mat testSample(cv::Size(src.width, src.height), channel_type, src.data);
    if (channel_type == CV_8UC3)
        cvtColor(testSample, testSample, CV_RGB2GRAY);
    cv::resize(testSample, testSample, cv::Size(faceWidth, faceHeight));
    predicted = eigenfaces->predict(testSample, dist);
    if (dist > thresh) {
        cerr << "! predicted = " << predicted << " dist = " << dist << " thresh =  " << thresh << endl;
        predicted = "";
    }
    return predicted;
}

int UEigenfaces::getFacesCount() const {
    std::vector<std::string> labels2;

    BOOST_FOREACH(FacePair p, faces) {
        labels2.push_back(p.second);
    }
    std::sort(labels2.begin(), labels2.end());
    labels2.erase(std::unique(labels2.begin(), labels2.end()), labels2.end());
    return labels2.size();
}

std::vector<std::string> UEigenfaces::getFacesNames() {
    std::vector<std::string> labels2;

    BOOST_FOREACH(FacePair p, faces) {
        labels2.push_back(p.second);
    }
    std::sort(labels2.begin(), labels2.end());
    labels2.erase(std::unique(labels2.begin(), labels2.end()), labels2.end());
    return labels2;
}

int UEigenfaces::getFaceImagesCount(const std::string& label) {
    int index = 0;
    urbi::UImage result;
    result.imageFormat = IMAGE_UNKNOWN;

    BOOST_FOREACH(FacePair p, faces) {
        if (p.second == label)
            index++;
    }

    //    BOOST_FOREACH(std::string p, labels) {
    //        if (p == label)
    //            index++;
    //    }
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

    //    std::vector<cv::Mat>::iterator imgIter = images.begin();
    //    BOOST_FOREACH(std::string p, labels) {
    //        if (p == name) {
    //            if (index == number) {
    //                urbi::UImage mBinImage;
    //                mBinImage.imageFormat = IMAGE_GREY8;
    //                mBinImage.width = imgIter->cols;
    //                mBinImage.height = imgIter->rows;
    //                mBinImage.size = mBinImage.width * mBinImage.height;
    //                mBinImage.data = new uint8_t[mBinImage.size];
    //                memcpy(mBinImage.data, imgIter->data, mBinImage.size);
    //                result = mBinImage;
    //                return result;
    //            }
    //            index++;
    //        }
    //        imgIter++;
    //    }
    throw std::runtime_error("[UEigenfaces]::find() : Invalid image number");
    return result;
}

int UEigenfaces::getImageWidth() {
    return faceWidth;
}

int UEigenfaces::getImageHeight() {
    return faceHeight;
}

urbi::UImage UEigenfaces::getTestFace(std::string fileName) {
    Mat image = imread(fileName, 0);
    resize(image, image, Size(faceWidth, faceHeight));
    if (image.flags == CV_8UC3)
        cvtColor(image, image, CV_RGB2GRAY);
    if (image.flags != CV_8UC1)
        std::runtime_error("[UEigenfaces]::getTestFace() : Invalid image format");
    urbi::UImage mBinImage;
    mBinImage.imageFormat = IMAGE_GREY8;
    mBinImage.width = image.cols;
    mBinImage.height = image.rows;
    mBinImage.size = mBinImage.width * mBinImage.height;
    mBinImage.data = new uint8_t[mBinImage.size];
    memcpy(mBinImage.data, image.data, mBinImage.size);
    return mBinImage;
}

double UEigenfaces::getThreshold() {
    return thresh;
}

void UEigenfaces::setThreshold(double t) {
    thresh = t;
}


UStart(UEigenfaces);
