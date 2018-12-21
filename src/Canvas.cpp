//
// Created by Alice lol on 2018-12-20.
//

#include "Canvas.h"

void Canvas::init(int minD, int maxD, Mat originalImg){
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

void Canvas::stretch(cv::Point2f min, cv::Point2f max){
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

void Canvas::stretch(vector<Point2f> pts)
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
void Canvas::stretch(cv::RotatedRect box)
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

void Canvas::drawEllipseWithBox(cv::RotatedRect box, cv::Scalar color, int lineThickness) {
    box.center = scale * cv::Point2f(box.center.x - origin.x, box.center.y - origin.y);
    box.size.width  = (float)(scale * box.size.width);
    box.size.height = (float)(scale * box.size.height);

    ellipse(img, box, color, lineThickness, LINE_AA);
}

void Canvas::drawPoints(vector<Point2f> pts, cv::Scalar color) {
    for(size_t i=0; i < pts.size(); i++){
        Point2f pnt = scale * cv::Point2f(pts[i].x - origin.x, pts[i].y - origin.y);
        img.at<cv::Vec3b>(int(pnt.y), int(pnt.x))[0] = (uchar)color[0];
        img.at<cv::Vec3b>(int(pnt.y), int(pnt.x))[1] = (uchar)color[1];
        img.at<cv::Vec3b>(int(pnt.y), int(pnt.x))[2] = (uchar)color[2];
    };
}

void Canvas::drawLabels( std::vector<std::string> text, std::vector<cv::Scalar> colors) {
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
};