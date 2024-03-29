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
 * File:   UEigenfaces.h
 * Author: lmalek
 *
 * Created on 21 marzec 2012, 09:55
 */

#ifndef UEIGENFACES_H
#define	UEIGENFACES_H

#include <urbi/uobject.hh>
#include <string>
#include <vector>

#ifdef _WIN32 // note the underscore: without it, it's not msdn official!
#include <opencv.hpp>
#elif __unix__ // all unices
#include <cv.h>
#endif

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/binary_object.hpp>

#include "eigenfaces.hpp"

typedef std::pair<cv::Mat, std::string> FacePair;

namespace boost {
    namespace serialization {

        template<class Archive>
        void serialize(Archive & ar, cv::Mat & g, const unsigned int version) {
            using boost::serialization::make_nvp;
            using boost::serialization::make_binary_object;
            ar & make_nvp("cols", g.cols);
            ar & make_nvp("rows", g.rows);
            ar & make_nvp("flags", g.flags);
            if (Archive::is_loading::value) {
                if (g.data)
                    delete [] g.data;
                g.data = new uchar[g.cols * g.rows];
            }
            ar & make_nvp("data", make_binary_object(g.data, g.cols * g.rows));
        }
    } // namespace serialization
} // namespace boost

class UEigenfaces : public urbi::UObject {
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive& ar, const unsigned int /* version */) {
        using boost::serialization::make_nvp;
        ar & make_nvp("faceWidth", faceWidth);
        ar & make_nvp("faceHeight", faceHeight);
        ar & make_nvp("numComponents", numComponents);
        ar & make_nvp("threshold", thresh);
        ar & make_nvp("faces", faces);
    }

public:
    UEigenfaces(const std::string& name);
    virtual ~UEigenfaces();

    /** 
     * Initialization function
     * Responsible for initalization
     */
    void init(int);


    bool loadData(const std::string& fileName);

    bool saveData(const std::string& fileName) const;

    // Train
    bool train(urbi::UImage src, const std::string& name);

    bool updateDatabase(int components);

    // Find
    std::string find(urbi::UImage src) const;

    int getFacesCount() const;

    std::vector<std::string> getFacesNames();

    int getFaceImagesCount(const std::string& name);

    urbi::UImage getFaceImage(const std::string& name, int number);

    int getImageWidth();
    int getImageHeight();

    urbi::UImage getTestFace(std::string fileName);

    double getThreshold();
    void setThreshold(double t);

private:
    int faceWidth;
    int faceHeight;
    std::vector<FacePair> faces;
    Eigenfaces* eigenfaces;
    double distMean;
    int numComponents;

    double thresh;
};

#endif	/* UEIGENFACES_H */

