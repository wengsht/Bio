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
#include "ImgFileName.h"
#include <cstdlib>

#include "configure.h"

#include "tool.h"

#include <pthread.h>

using namespace std;

using namespace bio;

using namespace cv;

char templateDir[MAX_FILE_NAME_LEN] = "./img/camera_data";

char face_cascade_name[MAX_FILE_NAME_LEN] = "./haarcascades/haarcascade_frontalface_alt.xml";

CascadeClassifier face_cascade;

bool dealOpts(int argc, char **argv);
void drawFeatures(Mat &frame, vector<Feature> &feats);
void drawRects(Mat &frame, vector<Rect> &rects, vector<string> names);

void *detectFaceThread(void *);
void detectFace(Mat frame, vector<Rect> &faces);
void reconition(Mat frame, vector<Rect> &faces, vector<string > &results);


VideoCapture cap(0);

SiftExtractor extractor;
SiftMatcher matcher;

vector<Feature> feats;
vector<Feature> face;

int cnt = FRAME_INTERVAL;

Mat frame;

vector<Rect> faces;
vector<string> names;

pthread_mutex_t detect_lock = PTHREAD_MUTEX_INITIALIZER;

Mat temp;

double matchRatio = DEFAULT_MATCH_RATIO;
int kdBackTrackRatio = KD_BACKTRACK_RATIO;

int main(int argc, char **argv) {
    if(!dealOpts(argc, argv))
        return -1;

    if(!face_cascade.load(face_cascade_name)) {
        Warn("Error when loading cascade_name from [%s]\n", face_cascade_name);

        return -1;
    }

    cap.set( CV_CAP_PROP_FRAME_WIDTH,CAP_WIDTH);
    cap.set( CV_CAP_PROP_FRAME_HEIGHT,CAP_HEIGHT);

    matcher.loadDir( templateDir );
    matcher.setMatchRatio(matchRatio);
    matcher.setKdBackTrackRatio(kdBackTrackRatio);
    matcher.setup();

    while(true) {
        cap >> frame;
        Mat showFrame = frame;

        if(cnt --) {
        }
        else {
            pthread_t thread;
            pthread_create(&thread, NULL, detectFaceThread, NULL);

            cnt = FRAME_INTERVAL;
        }


        pthread_mutex_lock(&detect_lock);

        drawFeatures(showFrame, face);
        drawRects(showFrame, faces, names);

        pthread_mutex_unlock(&detect_lock);

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
    while((c = getopt(argc, argv, "ht:b:k:")) != -1) {
        switch(c) {
            case 'h':
                printf("usage: \n \
                        -b Match ratio [0.8 is reasonable] \n\
                        -k kd bracktrack ratio, lower value means higher accuracy and lower speed\n\
                        -t template dir \n\
                        \n");

                return false;
                break;
            case 't':
                strcpy(templateDir, optarg);
                break;
            case 'b':
                sscanf(optarg, "%lf", &matchRatio);
                break;
            case 'k':
                kdBackTrackRatio = atoi(optarg);
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

void detectFace(Mat frame, vector<Rect> &faces) {
    Mat frame_gray;
    cvtColor(frame, frame_gray, CV_BGR2GRAY);
    equalizeHist(frame_gray, frame_gray);

    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
}

void drawRects(Mat &frame, vector<Rect> &rects, vector<string> names) {
    int idx;
    for(idx = 0; idx < rects.size(); idx ++) {
        rectangle(frame, rects[idx], Scalar(0,0,255));
        putText(frame, names[idx], Point(rects[idx].x, rects[idx].y), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,255));
    }
}

void reconition(Mat frame, vector<Rect> &faces, vector<string > &results) {
    int faceIdx;
    vector<Feature> feats;
    results.clear();

    SiftExtractor extractor;

    for(faceIdx = 0; faceIdx < faces.size(); faceIdx ++) {
        Mat face(frame, faces[faceIdx]);

        feats.clear();

        extractor.sift(&face, feats);

        unsigned long matchTag = matcher.match(feats);

        results.push_back(ImgFileName::descriptor(matchTag));
    }
}

void *detectFaceThread(void *data) {
    vector<Rect> newFaces;
    vector<string> newNames;

    detectFace(frame, newFaces);
    reconition(frame, newFaces, newNames);

    pthread_mutex_lock(&detect_lock);
    faces = newFaces;
    names = newNames;
    pthread_mutex_unlock(&detect_lock);
    pthread_exit(NULL);
}
