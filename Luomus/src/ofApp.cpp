#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    // Kinect setup
    ofSetLogLevel(OF_LOG_ERROR);
    ofSetFrameRate(12);
    kinect.setRegistration(true);
    kinect.init();
    kinect.open("A00365A12829045A");
    
    grayimage.allocate(kinect.width, kinect.height);
    graythresnear.allocate(kinect.width, kinect.height);
    cvblobs.resize(100);
    
    // Box2d setup
    ofSetVerticalSync(false);
    box2d.init();
    box2d.setGravity(0, 30);
    box2d.registerGrabbing();
    box2d.setFPS(12.0);
    
}

//--------------------------------------------------------------
void ofApp::update(){
    kinect.update();
    
    if(kinect.isFrameNew()){
        // get depth image from kinect and add pixels to cvGrayImages
        grayimage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        graythresnear = grayimage;
        graythresnear.threshold(80);
        // CV_RETR_CCOMP returns a hierarchy of outer contours and holes
        contourfinder.findContours(graythresnear, minArea, maxArea, maxInput, CV_RETR_CCOMP);
        
        for(int i=0; i<contourfinder.blobs.size(); i++){
            
            cvblobs[i] = contourfinder.blobs.at(i); //contain blobs in <cvblobs>
            numOfPtsOfBlob = cvblobs[i].nPts;   // number of points(x,y) in each blob
            
            if(cvblobs[i].hole){
                
                ofPtr<ofxBox2dEdge> edge = ofPtr<ofxBox2dEdge>(new ofxBox2dEdge);
                
                for(int j=0; j<numOfPtsOfBlob; j+=10){   //get every 10th points
                    
                    ofPtr<ofPoint> point = ofPtr<ofPoint>(new ofPoint); //and put into point
                    point.get()->x = cvblobs[i].pts.at(j).x;
                    point.get()->y = cvblobs[i].pts.at(j).y;
                    
                    edge.get()->addVertex(point.get()->x, point.get()->y);  //add pointX,pointY in edge
                }
                edge.get()->create(box2d.getWorld());   // set box2d world in edge
                edges.push_back(edge);  // add edge into <edges>
            }
            
            
        }
    }
    currentInput = contourfinder.nBlobs;    // current number of blobs
    grayimage.flagImageChanged();
    
    box2d.update();
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    // Draw depth image and contour
    graythresnear.draw(0, 0, kinect.width, kinect.height);
    contourfinder.draw(0, 0, kinect.width, kinect.height);
    
    // Draw yellow circle in the center of each blob
    for(int i=0; i<currentInput; i++){
        if(cvblobs[i].hole){
            ofSetColor(255, 255, 0);
            ofCircle(cvblobs[i].centroid.x, cvblobs[i].centroid.y, 5);
            ofSetColor(255);
        }
    }
    
    // Draw red Box2dEdge on the contour of each blob
    ofSetColor(255, 0, 0);
    for(int i=0; i<edges.size(); i++){
        edges[i].get()->draw();
    }
    ofSetColor(255);
    
    cout << numOfPtsOfBlob << " /";
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}
