#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxBox2d.h"
#include "ofxOpenCv.h"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    int screenWidth = 1280;
    int screenHeight = 854;
    
    // Kinect
    ofxKinect kinect;
    ofxCvGrayscaleImage grayimage;
    ofxCvGrayscaleImage grayimage1;
    ofxCvGrayscaleImage bothKinects;
    ofxCvContourFinder contourfinder;
    vector<ofxCvBlob> cvblobs;
    
    unsigned char* combinedVideo;
    ofPixels GrayPixel;
    ofPixels GrayPixel1;
    
    float kinectResize = 1.33;
    int nearThreshold = 80;
    int minArea = 500;
    int maxArea;
    int maxInput = 5;
    int currentInput = 0;
    int numOfPtsOfBlob = 0;
    string message = "not hole //";
    
    // Box2d
    ofxBox2d box2d;
    vector<ofPtr<ofxBox2dEdge> > edges;
    vector<ofPtr<ofxBox2dCircle> > circles;
    
    ofQTKitPlayer greendog;
    
};
