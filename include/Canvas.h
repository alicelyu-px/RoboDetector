//
// Created by Alice lol on 2018-12-20.
//
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>

#ifndef CANVAS_H
#define CANVAS_H

using namespace cv;
using namespace std;

class Canvas {
public:
    bool setupQ;
    cv::Point origin;
    cv::Point corner;
    int minDims,maxDims;
    double scale;
    int rows, cols;
    cv::Mat img;
    cv::Mat mask;

    void init(int minD, int maxD, Mat originalImg);

    void stretch(cv::Point2f min, cv::Point2f max);

    void stretch(vector<Point2f> pts);

    void stretch(cv::RotatedRect box);

    void drawEllipseWithBox(cv::RotatedRect box, cv::Scalar color, int lineThickness);

    void drawPoints(vector<Point2f> pts, cv::Scalar color);

    void drawLabels( std::vector<std::string> text, std::vector<cv::Scalar> colors);
};


#endif
