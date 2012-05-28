# UrbiEigenfaces #

## INFO ##

Face recognition based on Eigenfaces for Urbi
Copyright (C) 2012  Lukasz Malek
 
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
  
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

## REQUIREMENTS ##

## INSTALATION ##

This library is prepared to work with the cmake.
Create new directory
```
mkdir build
```
then run cmake, ccmake or cmake-gui to configure project
```
ccmake ../
```
after configuring project type
```
make
```

## MODULE FUNCTIONS ##

**UEigenfaces.init(1);**            - initialize module  
**UEigenfaces.loadData("fileName.xml");**  - load database from file  
**UEigenfaces.saveData("fileName.xml");**  - save database to file  
**UEigenfaces.train(image, "label");**              - add image with label to database  
**UEigenfaces.updateDatabase(componentsCount);** - update database by added images, PCA componetsCount stays for the number of significant elements that should be taken into the consideration   
**UEigenfaces.find(image);**        - recognize image in database, returns label  
**UEigenfaces.getFacesCount();**    - return number of labels availabe in the database  
**UEigenfaces.getFacesNames();**    - return labels available in database      
**UEigenfaces.getFaceImagesCount(const std::string& name);**    - return number of images for given label  
**UEigenfaces.getFaceImage(const std::string& name, int number);**  - return image with given label and number  
**UEigenfaces.getImageWidth();**    - return image width  
**UEigenfaces.getImageHeight();**   - return image height  
**UEigenfaces.getTestFace(std::string fileName);**  - load image from file  
**UEigenfaces.getThreshold();**     - returns threshold level used for finding label for given image  
**UEigenfaces.setThreshold(double t);**     - sets threshold level for finding label for given image  

## USAGE ##
```
urbi -i -m UEigenfaces -P 54000

var Global.Faces=UEigenfaces.new(1);
```
## NOTES ##

The test of the application can be done in the folowing way
```
var Global.Faces=UEigenfaces.new(1);
var Global.face1 = Faces.getTestFace("/tmp/s1/1.pgm");
var Global.face2 = Faces.getTestFace("/tmp/s1/2.pgm");
var Global.face3 = Faces.getTestFace("/tmp/s1/3.pgm");


Faces.train(face1, "s1");
Faces.train(face2, "s1");

Faces.updateDatabase();

Faces.getFacesCount();

Faces.getFacesNames();

Faces.getFaceImagesCount("s1");

Faces.getFaceImage("s1",0);

Faces.find(face1);
Faces.find(face2);
Faces.find(face3);
```
```
var Global.Faces=UEigenfaces.new(1);
Faces.train(Faces.getTestFace("/tmp/s1/1.pgm"),"s1");
Faces.train(Faces.getTestFace("/tmp/s1/2.pgm"),"s1");
Faces.train(Faces.getTestFace("/tmp/s1/3.pgm"),"s1");
Faces.train(Faces.getTestFace("/tmp/s1/4.pgm"),"s1");
Faces.train(Faces.getTestFace("/tmp/s1/5.pgm"),"s1");
Faces.train(Faces.getTestFace("/tmp/s1/6.pgm"),"s1");
Faces.train(Faces.getTestFace("/tmp/s1/7.pgm"),"s1");
Faces.train(Faces.getTestFace("/tmp/s1/8.pgm"),"s1");
Faces.train(Faces.getTestFace("/tmp/s2/1.pgm"),"s2");
Faces.train(Faces.getTestFace("/tmp/s2/2.pgm"),"s2");
Faces.train(Faces.getTestFace("/tmp/s2/3.pgm"),"s2");
Faces.train(Faces.getTestFace("/tmp/s2/4.pgm"),"s2");
Faces.train(Faces.getTestFace("/tmp/s2/5.pgm"),"s2");
Faces.train(Faces.getTestFace("/tmp/s2/6.pgm"),"s2");
Faces.train(Faces.getTestFace("/tmp/s2/7.pgm"),"s2");
Faces.train(Faces.getTestFace("/tmp/s2/8.pgm"),"s2");
Faces.train(Faces.getTestFace("/tmp/s3/1.pgm"),"s3");
Faces.train(Faces.getTestFace("/tmp/s3/2.pgm"),"s3");
Faces.train(Faces.getTestFace("/tmp/s3/3.pgm"),"s3");
Faces.train(Faces.getTestFace("/tmp/s3/4.pgm"),"s3");
Faces.train(Faces.getTestFace("/tmp/s3/5.pgm"),"s3");
Faces.train(Faces.getTestFace("/tmp/s3/6.pgm"),"s3");
Faces.train(Faces.getTestFace("/tmp/s3/7.pgm"),"s3");
Faces.train(Faces.getTestFace("/tmp/s3/8.pgm"),"s3");
Faces.updateDatabase(80);

Faces.saveData("test.xml");

Faces.find(Faces.getTestFace("/tmp/s1/1.pgm"));
Faces.find(Faces.getTestFace("/tmp/s1/9.pgm"));
Faces.find(Faces.getTestFace("/tmp/s1/10.pgm"));
Faces.find(Faces.getTestFace("/tmp/s2/1.pgm"));
Faces.find(Faces.getTestFace("/tmp/s2/9.pgm"));
Faces.find(Faces.getTestFace("/tmp/s2/10.pgm"));
Faces.find(Faces.getTestFace("/tmp/s3/1.pgm"));
Faces.find(Faces.getTestFace("/tmp/s3/9.pgm"));
Faces.find(Faces.getTestFace("/tmp/s3/10.pgm"));
Faces.find(Faces.getTestFace("/tmp/s4/9.pgm"));
Faces.find(Faces.getTestFace("/tmp/s4/10.pgm"));

Faces.setThreshold(4000);
```
