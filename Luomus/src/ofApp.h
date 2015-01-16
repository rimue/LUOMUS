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
    
    // Kinect
    ofxKinect kinect;
    ofxCvGrayscaleImage grayimage;
    ofxCvGrayscaleImage graythresnear;
    ofxCvContourFinder contourfinder;
    vector<ofxCvBlob> cvblobs;
    int minArea = 1000;
    int maxArea = 25000;
    int maxInput = 5;
    int currentInput = 0;
    int numOfPtsOfBlob = 0;
    
    // Box2d
    ofxBox2d box2d;
    vector<ofPolyline> lines;
    vector <ofPtr<ofxBox2dCircle> > circles;
    vector <ofPtr<ofxBox2dEdge> > edges;
    
    
};
