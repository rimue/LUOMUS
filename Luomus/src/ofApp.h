#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxBox2d.h"
#include "ofxOpenCv.h"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    
    // For testing box2d stuff
    void box2dTestUpdate();
    
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
    int screenHeight = 914;
    
    // Kinect
    ofxKinect kinect, kinect1;
    
    ofxCvGrayscaleImage grayimage;
    ofxCvGrayscaleImage grayimage1;
    ofxCvGrayscaleImage bothKinects;
    ofxCvContourFinder contourfinder;
    
    // Closed contours
    vector<ofxCvBlob> cvblobs;
    
    unsigned char* combinedVideo;
    ofPixels GrayPixel;
    ofPixels GrayPixel1;
    
    float kinectResize = 1.33;
    int nearThreshold = 200;
    int minArea = 500;
    int maxArea;
    int maxInput = 5;
    int currentInput = 0;
    const int TOP_MARGIN = 25;
    
    string message = "not hole //";
    
    // Box2d
    ofxBox2d box2d;
    vector<ofPtr<ofxBox2dEdge> > edges;
    vector<ofPtr<ofxBox2dCircle> > circles;
    
    // Animal
    int raWidth = 155;
    int raHeight = 100;
    float circlePosX=0;
    float circlePosY=0;
    ofxBox2dCircle raCircle;
    ofVec2f circlePos;
    ofImage raccoon;
    

private:
    bool isCircleInsideLine(ofxBox2dCircle* circle);
    

};
