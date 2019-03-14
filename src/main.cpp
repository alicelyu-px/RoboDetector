//#include "/usr/local/include/opencv4/opencv2"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>

#include <iostream>
#include <iomanip>
#include <sstream>

#ifndef CANVAS_H
#include "Canvas.h"
#endif

#ifndef UTILS_H
#include "utils.h"
#endif 

using namespace cv;
using namespace std;

int sliderPos = 35;
Mat image;
Mat image_gray;
Mat src, dilation_dst, erosion_dst;
int erosion_elem = 0;
int erosion_size = 0;
int dilation_size = 0;
int dilation_elem = 0;
int const max_elem = 2;
int const max_kernel_size = 21;
//cv::Scalar fitEllipseColor       = Scalar(255,  0,  0, 200);

void processImage(int, void*);
void peek_val(Mat img);
static void onMouse(int event, int x, int y, int f, void* );
void mergeImg(Mat & dst,Mat &src1,Mat &src2);
void Dilation( int, void* );
void Erosion( int, void* );
void ero_dilate(Mat & src, int erosion_size, int dilation_dize);
// void select(Mat img);

int main( int argc, char** argv ) {
   Mat image_gray, image_hsv;
   CommandLineParser parser(argc, argv,"{help h||}{@image|1.jpg|}");
   string filename = parser.get<string>("@image");
   // VideoCapture cap(filename);
   image = imread(samples::findFile(filename), IMREAD_COLOR); // 0 = IMREAD_GRAYSCALE
   if( image.empty() ) {
      cout << "Couldn't open image " << filename << "\n";
      return 0;
   }
   peek_val(image);
   // namedWindow("peek", WINDOW_NORMAL );
   cvtColor(image, image_hsv, COLOR_RGB2HSV);
   blur(image_hsv, image_hsv, Size(3,3) );
   Mat light(image.rows, image.cols, CV_8UC1, Scalar(0));
   Mat red(image.rows, image.cols, CV_8UC1, Scalar(0));
   Mat dark(image.rows, image.cols, CV_8UC1, Scalar(0));
   int cols = image.cols;
   int rows = image.rows;
   // partition
   for (int i = 0; i<image.rows;i++) {
      for (int j = 0; j<image.cols; j++) {
         Vec3b hsv=image_hsv.at<Vec3b>(i,j);
         int H=hsv.val[0];
         int S=hsv.val[1];
         int V=hsv.val[2];
         // low satuation & low brightness
         if (S < 0.3 * 255 && V < 0.3 * 255 && i > rows*0.3)
            dark.at<uchar>(i, j) = 255;
         if (V > 0.9*255 && S < 0.1 * 255 && i > rows*0.3)
            light.at<uchar>(i, j) = 255;
      }
   } 
   erosion_size=1;
   dilation_size=8;
   ero_dilate(light, 1, 8);
   ero_dilate(dark, 1, 1);
   // namedWindow("Light after", WINDOW_AUTOSIZE );
   namedWindow("Dark after", WINDOW_AUTOSIZE );
   // moveWindow( "Dark after", cols, 0 );
   vector<vector<Point> > contours;
   findContours(dark, contours, RETR_LIST, CHAIN_APPROX_NONE);
   for(int i = 0; i < contours.size(); i++) {
      cout << "len_ctr= " << contours[i].size() << endl;
      if (contours[i].size() < 2000) {
         drawContours(dark, contours, i, Scalar(0), -1);
      }
   }
   imshow("Dark after", dark);
   
   // // peak emosion/dilation
   // namedWindow( "Erosion Demo", WINDOW_AUTOSIZE );
   // namedWindow( "Dilation Demo", WINDOW_AUTOSIZE );
   // moveWindow( "Dilation Demo", src.cols, 0 );
   // createTrackbar( "Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", "Erosion Demo",
   //       &erosion_elem, max_elem,
   //       Erosion );
   // createTrackbar( "Kernel size:\n 2n +1", "Erosion Demo",
   //       &erosion_size, max_kernel_size,
   //       Erosion );
   // createTrackbar( "Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", "Dilation Demo",
   //       &dilation_elem, max_elem,
   //       Dilation);
   // createTrackbar( "Kernel size:\n 2n+1", "Dilation Demo", &dilation_size, 21, Dilation);
   // Erosion(0, 0);
   // Dilation(0, 0);
   
   // // plot together
   // Mat three;
   // three.create(rows + 5, cols*3 + 25, light.type());
   // light.copyTo(three(Rect(0,0,cols,rows)));
   // dark.copyTo(three(Rect(cols+5, 0, cols, rows)));
   // red.copyTo(three(Rect(2*cols+10, 0, cols, rows)));
   // imshow("light, dark, red", three);

   // imshow("bright", light);
   // waitKey();
   // imshow("red", dark);
   // waitKey();
   // imshow("dark", red);
   waitKey();
   // setMouseCallback("peek", onMouse, 0);
//   imshow("peek", image_hsv);
   // src = image_hsv.clone();
   // setMouseCallback("peek", onMouse, 0);
   // imshow("peek", channels[2]);
   // cvtColor(image, image_gray, COLOR_RGB2GRAY);
   // Canvas paper;
   // paper.init(int(0.8*MIN(image.rows, image.cols)), int(1.2*MAX(image.rows, image.cols)), image);
   // Mat x;
   
   
   // createTrackbar( "threshold", "result", &sliderPos, 255, processImage );
   // processImage(0, 0);
   return 0;
}
void ero_dilate(Mat & src, int erosion_size, int dilation_dize) {
   Mat e_element = getStructuringElement( MORPH_ELLIPSE,
                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                       Point( erosion_size, erosion_size ) );
   erode( src, src, e_element );
   Mat d_element = getStructuringElement( MORPH_ELLIPSE,
                       Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                       Point( dilation_size, dilation_size ) );

   dilate( src, src, d_element );   
}

void Erosion( int, void* ) {
  int erosion_type = 0;
  if( erosion_elem == 0 ){ erosion_type = MORPH_RECT; }
  else if( erosion_elem == 1 ){ erosion_type = MORPH_CROSS; }
  else if( erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }
  Mat element = getStructuringElement( erosion_type,
                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                       Point( erosion_size, erosion_size ) );
  erode( src, erosion_dst, element );
  imshow( "Erosion Demo", erosion_dst );
}

void Dilation( int, void* ) {
  int dilation_type = 0;
  if( dilation_elem == 0 ){ dilation_type = MORPH_RECT; }
  else if( dilation_elem == 1 ){ dilation_type = MORPH_CROSS; }
  else if( dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }
  Mat element = getStructuringElement( dilation_type,
                       Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                       Point( dilation_size, dilation_size ) );
  Mat dilation_dst;
  dilate( src, dilation_dst, element );
  imshow( "Dilation Demo", dilation_dst );
}

void mergeImg(Mat & dst,Mat &src1,Mat &src2) {
    int rows = src1.rows+5+src2.rows;
    int cols = src1.cols+5+src2.cols;
    CV_Assert(src1.type () == src2.type ());
    dst.create (rows,cols,src1.type());
    src1.copyTo (dst(Rect(0,0,src1.cols,src1.rows)));
    src2.copyTo (dst(Rect(src1.cols+5,0,src2.cols,src2.rows)));
}

void peek_val(Mat img) {
   src = img.clone();
   imshow("peek", img);
   setMouseCallback("peek", onMouse, 0);
}
void static onMouse(int event, int x, int y, int f, void* ){
    Mat image=src.clone();
    Vec3b rgb=image.at<Vec3b>(y,x);
    int B=rgb.val[0];
    int G=rgb.val[1];
    int R=rgb.val[2];

    Mat HSV;
    Mat RGB=image(Rect(x,y,1,1));
    cvtColor(RGB, HSV,COLOR_BGR2HSV);

    Vec3b hsv=HSV.at<Vec3b>(0,0);
    int H=hsv.val[0];
    int S=hsv.val[1];
    int V=hsv.val[2];

    char name[30];
    sprintf(name,"B=%d",B);
    putText(image,name, Point(150,40) , FONT_HERSHEY_SIMPLEX, .7, Scalar(0,255,0), 2,8,false );

    sprintf(name,"G=%d",G);
    putText(image,name, Point(150,80) , FONT_HERSHEY_SIMPLEX, .7, Scalar(0,255,0), 2,8,false );

    sprintf(name,"R=%d",R);
    putText(image,name, Point(150,120) , FONT_HERSHEY_SIMPLEX, .7, Scalar(0,255,0), 2,8,false );

    sprintf(name,"H=%d",H);
    putText(image,name, Point(25,40) , FONT_HERSHEY_SIMPLEX, .7, Scalar(0,255,0), 2,8,false );

    sprintf(name,"S=%d",S);
    putText(image,name, Point(25,80) , FONT_HERSHEY_SIMPLEX, .7, Scalar(0,255,0), 2,8,false );

    sprintf(name,"V=%d",V);
    putText(image,name, Point(25,120) , FONT_HERSHEY_SIMPLEX, .7, Scalar(0,255,0), 2,8,false );

    sprintf(name,"X=%d",x);
    putText(image,name, Point(25,300) , FONT_HERSHEY_SIMPLEX, .7, Scalar(0,0,255), 2,8,false );

    sprintf(name,"Y=%d",y);
    putText(image,name, Point(25,340) , FONT_HERSHEY_SIMPLEX, .7, Scalar(0,0,255), 2,8,false );

//imwrite("hsv.jpg",image);
    imshow( "peek", image );
}

// bool comp(vector<Point> i, vector<Point> j) {
//     return i.size() < j.size();
// }

// void processImage(int /*h*/, void*) {
//    RotatedRect box, boxAMS, boxDirect;
//    int margin = 2;
//    vector<vector<Point> > contours;
//    cvtColor(image, image_gray, COLOR_RGB2GRAY);
//    Mat bimage = image_gray >= sliderPos;

//    // findContours(bimage, contours, RETR_LIST, CHAIN_APPROX_NONE);

//    // sort(contours.begin(), contours.end(), comp);
//    // contours = vector<vector<Point>>(contours.begin(), contours.begin() + contours.size() * 0.6);
//    Canvas paper;

//    paper.init(int(1*MIN(bimage.rows, bimage.cols)), int(1*MAX(bimage.rows, bimage.cols)), image);
//    // paper.stretch(cv::Point2f(0.0f, 0.0f), cv::Point2f((float)(bimage.cols+2.0), (float)(bimage.rows+2.0)));

//    std::vector<std::string> text;
//    std::vector<cv::Scalar> color;

//    vector< vector<Point2f> > points;
//    for(size_t i = 0; i < contours.size(); i++) {
//       size_t count = contours[i].size();
//       if( count < 6 )
//          continue;

//       Mat pointsf;
//       Mat(contours[i]).convertTo(pointsf, CV_32F);

//       vector<Point2f>pts;
//       for (int j = 0; j < pointsf.rows; j++) {
//          Point2f pnt = Point2f(pointsf.at<float>(j,0), pointsf.at<float>(j,1));
//          if ((pnt.x > margin && pnt.y > margin && pnt.x < bimage.cols-margin && pnt.y < bimage.rows-margin)) {
//             if(j%20==0){
//                pts.push_back(pnt);
//             }
//          }
//       }
//       points.push_back(pts);
//    }

//    for(size_t i = 0; i < points.size(); i++) {
//       vector<Point2f> pts = points[i];

//       if (pts.size()<=6)
//          continue;

//      box = fitEllipse(pts);
//      int dimension = MIN(bimage.rows, bimage.cols);
//      if( MAX(box.size.width, box.size.height) > MIN(box.size.width, box.size.height)*1.4 ||
//         MAX(box.size.width, box.size.height) <= 0.4*dimension ||
//         MAX(box.size.width, box.size.height) >= 0.8*dimension ||
//         MIN(box.size.width, box.size.height) <= 0) continue;


//        float area = 0.25 * box.size.width * M_PI * box.size.height;
//        stringstream ss;
//        ss << fixed << std::setprecision(5) << area;
//        string area_str = ss.str();
//        text.push_back("Area= " + area_str);
//        color.push_back(cv::Scalar(255, 255, 255, 155));
//        ellipse(paper.img, box, fitEllipseColor, 3, cv::LineTypes::LINE_AA);
//        paper.drawLabels(text, color);

//        paper.drawEllipseWithBox(box, fitEllipseColor, 3);
//        paper.drawPoints(pts, cv::Scalar(255,255,255));
//      // paper.drawEllipseWithBox(box, fitEllipseAMSColor, 2);
//      // paper.drawEllipseWithBox(box, fitEllipseDirectColor, 1);
//      // paper.drawPoints(pts, cv::Scalar(255, 0 ,0 , 255));
//    }

//    imshow("result", paper.img);
// }
