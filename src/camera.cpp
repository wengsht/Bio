// =====================================================================================
// 
//       Filename:  camera.cpp
// 
//    Description:  
// 
//        Version:  0.01
//        Created:  2014/11/10 16时49分55秒
//       Revision:  none
//       Compiler:  clang 3.5
// 
//         Author:  
//         chenss(SYSU-CMU), 
//         wengsht (SYSU-CMU), wengsht.sysu@gmail.com
//        Company:  
// 
// =====================================================================================

#include <iostream>
#include <fstream>
#include <cstdio>
#include <unistd.h>
#include "feature.h"
#include "SiftExtractor.h"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/objdetect/objdetect.hpp>

#include "SiftMatcher.h"
#include <cstdlib>

#include "configure.h"

#include "tool.h"

using namespace std;

using namespace bio;

using namespace cv;

char templateDir[MAX_FILE_NAME_LEN] = "./img/camera_data";

char *face_cascade_name = "./haarcascades/haarcascade_frontalface_default.xml";

CascadeClassifier face_cascade;

bool dealOpts(int argc, char **argv);
void drawFeatures(Mat &frame, vector<Feature> &feats);
void drawRects(Mat &frame, vector<Rect> &rects);

void detectFace(Mat &frame, vector<Rect> &faces);

int main(int argc, char **argv) {
    if(!dealOpts(argc, argv))
        return -1;

    if(!face_cascade.load(face_cascade_name)) {
        Warn("Error when loading cascade_name from [%s]\n", face_cascade_name);

        return -1;
    }

    VideoCapture cap(0);
    cap.set( CV_CAP_PROP_FRAME_WIDTH,CAP_WIDTH);
    cap.set( CV_CAP_PROP_FRAME_HEIGHT,CAP_HEIGHT);

    SiftExtractor extractor;
    SiftMatcher matcher;

    matcher.loadDir( templateDir );

    matcher.setup();

    vector<Feature> feats;
    vector<Feature> face;

    int cnt = FRAME_INTERVAL;

    Mat frame;

    vector<Rect> faces;
    vector<string> results;

    while(true) {
        cap >> frame;
        Mat showFrame = frame;

        if(cnt --) {
        }
        else {
            detectFace(frame, faces);

            for(int idx = 0;idx < faces.size(); idx ++) {
                Log("%d %d %d %d\n", faces[idx].x, faces[idx].y, faces[idx].width, faces[idx].height);
            }
            /*  
        face.clear();
            extractor.sift(& frame, feats);

            for(int idx = 0;idx < feats.size(); idx ++) {
                pair<Feature *, Feature *> newMatch = matcher.match(feats[idx]);

                Feature *bestFeat = newMatch.first;
                Feature *secFeat  = newMatch.second;

                double bestDist = * bestFeat - feats[idx];
                double secBestDist = *secFeat - feats[idx];

                if(bestDist / secBestDist < DEFAULT_MATCH_RATIO) {
                    printf("%s\n", (char *) (bestFeat->getContainer()));
                    face.push_back(feats[idx]);
                }
            }

            */
            cnt = FRAME_INTERVAL;
        }

        drawFeatures(showFrame, face);
        drawRects(showFrame, faces);


        for(int idx = 0; idx < SHOW_PYR_TIMES; idx++) {
            pyrUp( showFrame, showFrame, Size((showFrame.cols)*2, (showFrame.rows)*2));
        }

        imshow( "Capture", showFrame);
        if( waitKey(30)>=0 ) break;
    }

    return 0;
}

bool dealOpts(int argc, char **argv) {
    int c;
    while((c = getopt(argc, argv, "ht:")) != -1) {
        switch(c) {
            case 'h':
                printf("usage: \n \
                        -t template dir \n\
                        \n");

                return false;
                break;
            case 't':
                strcpy(templateDir, optarg);
                break;
            default:
                break;
        }
    }
    return true;
}

void drawFeatures(Mat &frame, vector<Feature> &feats) {
    Point p1;
    for(int idx = 0; idx < feats.size(); idx ++) {
        p1.x = feats[idx].originLoc.x;
        p1.y = feats[idx].originLoc.y;

        circle(frame, p1, 2, Scalar(0,0,255));
    }
}

void detectFace(Mat &frame, vector<Rect> &faces) {
    Mat frame_gray;
    cvtColor(frame, frame_gray, CV_BGR2GRAY);
    equalizeHist(frame_gray, frame_gray);

    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
}

void drawRects(Mat &frame, vector<Rect> &rects) {
    int idx;
    for(idx = 0; idx < rects.size(); idx ++) {
        rectangle(frame, rects[idx], Scalar(0,0,255));
    }
}
