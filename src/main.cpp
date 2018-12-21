#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <iomanip>
#include <sstream> // stringstream

#ifndef PUPILDETECOR_CANVAS_H
#include "Canvas.h"
#endif

using namespace cv;
using namespace std;

int sliderPos = 35;
Mat image;
Mat image_gray;

cv::Scalar fitEllipseColor       = Scalar(255,  0,  0, 200);
cv::Scalar fitEllipseAMSColor    = Scalar(0,  255,  0, 200);
cv::Scalar fitEllipseDirectColor = Scalar(0,  0,  255, 200);

void processImage(int, void*);

int main( int argc, char** argv ) {
   Mat image_gray;
   cv::CommandLineParser parser(argc, argv,"{help h||}{@image|ellipses.jpg|}");
   string filename = parser.get<string>("@image");
   image = imread(samples::findFile(filename), 1); // 0 = IMREAD_GRAYSCALE
   // image = imread(samples::findFile(filename), IMREAD_COLOR); // 0 = IMREAD_GRAYSCALE
   if( image.empty() ) {
      cout << "Couldn't open image " << filename << "\n";
      return 0;
   }

   namedWindow("result", WINDOW_NORMAL );
   cvtColor(image, image_gray, COLOR_RGB2GRAY);
   Canvas paper;
   paper.init(int(0.8*MIN(image.rows, image.cols)), int(1.2*MAX(image.rows, image.cols)), image);
   Mat x;
   createTrackbar( "threshold", "result", &sliderPos, 255, processImage );
   processImage(0, 0);

   waitKey();
   return 0;
}

bool comp(vector<Point> i, vector<Point> j) {
    return i.size() < j.size();
}

void processImage(int /*h*/, void*) {
   RotatedRect box, boxAMS, boxDirect;
   int margin = 2;
   vector<vector<Point> > contours;
   cvtColor(image, image_gray, COLOR_RGB2GRAY);
   Mat bimage = image_gray >= sliderPos;

   findContours(bimage, contours, RETR_LIST, CHAIN_APPROX_NONE);

   // sort(contours.begin(), contours.end(), comp);
   // contours = vector<vector<Point>>(contours.begin(), contours.begin() + contours.size() * 0.6);
   Canvas paper;

   paper.init(int(1*MIN(bimage.rows, bimage.cols)), int(1*MAX(bimage.rows, bimage.cols)), image);
   // paper.stretch(cv::Point2f(0.0f, 0.0f), cv::Point2f((float)(bimage.cols+2.0), (float)(bimage.rows+2.0)));

   std::vector<std::string> text;
   std::vector<cv::Scalar> color;

   vector< vector<Point2f> > points;
   for(size_t i = 0; i < contours.size(); i++) {
      size_t count = contours[i].size();
      if( count < 6 )
         continue;

      Mat pointsf;
      Mat(contours[i]).convertTo(pointsf, CV_32F);

      vector<Point2f>pts;
      for (int j = 0; j < pointsf.rows; j++) {
         Point2f pnt = Point2f(pointsf.at<float>(j,0), pointsf.at<float>(j,1));
         if ((pnt.x > margin && pnt.y > margin && pnt.x < bimage.cols-margin && pnt.y < bimage.rows-margin)) {
            if(j%20==0){
               pts.push_back(pnt);
            }
         }
      }
      points.push_back(pts);
   }

   for(size_t i = 0; i < points.size(); i++) {
      vector<Point2f> pts = points[i];

      if (pts.size()<=6)
         continue;

     box = fitEllipse(pts);
     int dimension = MIN(bimage.rows, bimage.cols);
     if( MAX(box.size.width, box.size.height) > MIN(box.size.width, box.size.height)*1.4 ||
        MAX(box.size.width, box.size.height) <= 0.4*dimension ||
        MAX(box.size.width, box.size.height) >= 0.8*dimension ||
        MIN(box.size.width, box.size.height) <= 0) continue;


       float area = 0.25 * box.size.width * M_PI * box.size.height;
       stringstream ss;
       ss << fixed << std::setprecision(5) << area;
       string area_str = ss.str();
       text.push_back("Area= " + area_str);
       color.push_back(cv::Scalar(255, 255, 255, 155));
       ellipse(paper.img, box, fitEllipseColor, 3, cv::LineTypes::LINE_AA);
       paper.drawLabels(text, color);

       paper.drawEllipseWithBox(box, fitEllipseColor, 3);
       paper.drawPoints(pts, cv::Scalar(255,255,255));
     // paper.drawEllipseWithBox(box, fitEllipseAMSColor, 2);
     // paper.drawEllipseWithBox(box, fitEllipseDirectColor, 1);
     // paper.drawPoints(pts, cv::Scalar(255, 0 ,0 , 255));
   }

   imshow("result", paper.img);
}
