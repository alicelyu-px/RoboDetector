#include "utils.h"

// threshold the 
// static void thresh(Mat src, Mat dst, CV_8U small, CV_8U large) {
    

// }

static void peek_hsv(Mat src) {
    Mat hue, image_hsv;
    vector<Mat> channels;
    cvtColor(src, image_hsv, COLOR_RGB2HSV);
    Mat empty(src.size(), CV_8U, Scalar(0));
    split(image_hsv, channels);
    vector<Mat> h, s, v;
    for (int i = 0; i<3; i++) {
      if (i==0) 
         h.push_back(channels[0]);
      else
         h.push_back(empty);
      if (i==1) 
         s.push_back(channels[1]);
      else
         s.push_back(empty);
      if (i==2) 
         v.push_back(channels[2]);
      else
         v.push_back(empty);
   }
   merge(h, hue);

   imshow("h", hue);
}
