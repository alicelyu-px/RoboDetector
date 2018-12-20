#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <iomanip>
#include <sstream> // stringstream

using namespace cv;
using namespace std;


class canvas {
public:
   bool setupQ;
   cv::Point origin;
   cv::Point corner;
   int minDims,maxDims;
   double scale;
   int rows, cols;
   cv::Mat img;
   cv::Mat mask;

   void init(int minD, int maxD, Mat originalImg){
      // Initialise the canvas with minimum and maximum rows and column sizes.
      minDims = minD; maxDims = maxD;
      origin = cv::Point(0,0);
      corner = cv::Point(0,0);
      scale = 1.0;
      rows = originalImg.rows;
      cols = originalImg.cols;
      setupQ = false;
      originalImg.copyTo(img);
      Mat mask(img.rows, img.cols, CV_8UC4);
      mask=cv::Scalar(255,255,255,0);
      // mask = Scalar(255, 255, 255, 0);
   }

   void stretch(cv::Point2f min, cv::Point2f max){
      // Stretch the canvas to include the points min and max.
      if(setupQ){
         if(corner.x < max.x){corner.x = (int)(max.x + 1.0);};
         if(corner.y < max.y){corner.y = (int)(max.y + 1.0);};
         if(origin.x > min.x){origin.x = (int) min.x;};
         if(origin.y > min.y){origin.y = (int) min.y;};
      } else {
         origin = cv::Point((int)min.x, (int)min.y);
         corner = cv::Point((int)(max.x + 1.0), (int)(max.y + 1.0));
      }

      int c = (int)(scale*((corner.x + 1.0) - origin.x));
      if(c<minDims){
         scale = scale * (double)minDims/(double)c;
      } else {
         if(c>maxDims){
            scale = scale * (double)maxDims/(double)c;
         }
      }
      int r = (int)(scale*((corner.y + 1.0) - origin.y));
      if(r<minDims){
         scale = scale * (double)minDims/(double)r;
      } else {
         if(r>maxDims){
            scale = scale * (double)maxDims/(double)r;
         }
      }
      cols = (int)(scale*((corner.x + 1.0) - origin.x));
      rows = (int)(scale*((corner.y + 1.0) - origin.y));
      setupQ = true;
   }

   void stretch(vector<Point2f> pts)
   {   // Stretch the canvas so all the points pts are on the canvas.
      cv::Point2f min = pts[0];
      cv::Point2f max = pts[0];
      for(size_t i=1; i < pts.size(); i++){
         Point2f pnt = pts[i];
         if(max.x < pnt.x){max.x = pnt.x;};
         if(max.y < pnt.y){max.y = pnt.y;};
         if(min.x > pnt.x){min.x = pnt.x;};
         if(min.y > pnt.y){min.y = pnt.y;};
      };
      stretch(min, max);
   }
   void stretch(cv::RotatedRect box)
   {   // Stretch the canvas so that the rectangle box is on the canvas.
      cv::Point2f min = box.center;
      cv::Point2f max = box.center;
      cv::Point2f vtx[4];
      box.points(vtx);
      for( int i = 0; i < 4; i++ ){
         cv::Point2f pnt = vtx[i];
         if(max.x < pnt.x){max.x = pnt.x;};
         if(max.y < pnt.y){max.y = pnt.y;};
         if(min.x > pnt.x){min.x = pnt.x;};
         if(min.y > pnt.y){min.y = pnt.y;};
      }
      stretch(min, max);
   }

   void drawEllipseWithBox(cv::RotatedRect box, cv::Scalar color, int lineThickness) {
      box.center = scale * cv::Point2f(box.center.x - origin.x, box.center.y - origin.y);
      box.size.width  = (float)(scale * box.size.width);
      box.size.height = (float)(scale * box.size.height);

      ellipse(img, box, color, lineThickness, LINE_AA);
   }

   void drawPoints(vector<Point2f> pts, cv::Scalar color) {
      for(size_t i=0; i < pts.size(); i++){
         Point2f pnt = scale * cv::Point2f(pts[i].x - origin.x, pts[i].y - origin.y);
         img.at<cv::Vec3b>(int(pnt.y), int(pnt.x))[0] = (uchar)color[0];
         img.at<cv::Vec3b>(int(pnt.y), int(pnt.x))[1] = (uchar)color[1];
         img.at<cv::Vec3b>(int(pnt.y), int(pnt.x))[2] = (uchar)color[2];
      };
   }

   void drawLabels( std::vector<std::string> text, std::vector<cv::Scalar> colors) {
      int vPos = 0;
      // cv::Scalar color = Scalar(100,  100,  100, 200);
      for (size_t i=0; i < text.size(); i++) {
         cv::Scalar color = Scalar(100,  100,  100, 200);
         std::string txt = text[i];
         Size textsize = getTextSize(txt, FONT_HERSHEY_COMPLEX, 1, 1, 0);
         vPos += (int)(1.3 * textsize.height);
         Point org((img.cols - textsize.width), vPos);
         cv::putText(img, txt, org, FONT_HERSHEY_COMPLEX, 1, color, 1, LINE_8);
      }
   }
};

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
   canvas paper;
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
   canvas paper;

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
