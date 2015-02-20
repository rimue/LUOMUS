#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxBox2d.h"
#include "ofxOpenCv.h"
#include "ofxXmlSettings.h"
#include "ofxTexturePacker.h"
#include "animalPatch.h"
#include "ofxSyphon.h"

class ofApp : public ofBaseApp {
    
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
    
    void contactStart(ofxBox2dContactArgs &e);
    void contactEnd(ofxBox2dContactArgs &e);
    
//    int screenWidth = ofGetScreenWidth();
//    int screenHeight = ofGetScreenHeight();

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
    int nearThreshold = 190;
    int minArea = 5000;
    int maxArea;
    int maxInput = 5;
    int currentInput = 0;
    const int TOP_MARGIN = 25;
    string message = "not a hole";
    
    
    // Box2d
    ofxBox2d box2d;
    vector<ofPtr<ofxBox2dEdge> > edges;
    vector<ofPtr<ofxBox2dRect> > rects;
    vector<ofPtr<ofxBox2dRect> > testRects;
    vector<ofPtr<ofxBox2dCircle> > circles;
    
    ////// bird
    ofxTexturePacker * bird;
    ofxTPAnimatedSprite * birdAnimation;
    
    // starting pos
    float birdX, birdY;
    // bird ani pos
    float birdAniX, birdAniY;
    // original sprite size
    float birdW, birdH;
    
    // box2d sprite size
    float birdRectW, birdRectH;

    
    ofVec2f birdCurrentPos;
    ofVec2f birdRectPos;
    
    float blobCenterX, blobCenterY;
    
    float groundSpeed;
    float groundTimer;
    float xSpeed = 5.0;
    int direction;
    
    // background
    ofImage background;
    
    
    // bird
    ofPtr<ofxBox2dRect> br;
    float brAngle=0;
    bool aniplay=true;
    
    ofxSyphonServer mainOutputSyphonServer;
    
private:
    
    ofPtr<animalPatch> patch;
    
    bool animalIsOverPatch;
    
    bool animalIsCaught;
    
    bool isInsideLine(ofxBox2dRect* rect);
    
    
    void animalCaught(ofxBox2dRect* rect);
    void animalReleased(ofxBox2dRect* rect);
    
    
};
