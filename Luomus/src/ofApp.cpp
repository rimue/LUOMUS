#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    // Kinect setup
    ofSetLogLevel(OF_LOG_ERROR);
    ofSetFrameRate(24);
    kinect.setRegistration(false);
    kinect.init();
    kinect.open("A00365A12829045A");
    kinect.setCameraTiltAngle(15);
    
    grayimage.allocate(kinect.width, kinect.height);
    grayimage1.allocate(kinect.width, kinect.height);
    
    bothKinects.allocate(kinect.height*2, kinect.width);
    combinedVideo = (unsigned char*)malloc(640 * 480 * 2 * sizeof(unsigned char*));
    
    cvblobs.resize(100);
    
    // Box2d setup
    ofSetVerticalSync(false);
    box2d.init();
    box2d.setGravity(0, 0);
    box2d.registerGrabbing();
    box2d.setFPS(24.0);
    
}

//--------------------------------------------------------------
void ofApp::update(){
    kinect.update();
    box2d.update();
    
    if(kinect.isFrameNew()){
        // get depth image from kinect and add pixels to cvGrayImages
        grayimage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        grayimage1.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        grayimage.blur(1.5);    grayimage1.blur(1.5);
        
        GrayPixel = grayimage.getPixelsRef();
        GrayPixel.rotate90(1);
        GrayPixel1 = grayimage1.getPixelsRef();
        GrayPixel1.rotate90(-1);
        
        for(int i=0; i<640; i++){
            memcpy(combinedVideo + (i*960), GrayPixel.getPixels()+(i*480), 480);
            memcpy(combinedVideo + (i*960+480), GrayPixel1.getPixels()+(i*480), 480);
            bothKinects.setFromPixels(combinedVideo, 480*2, 640);
        }
        bothKinects.resize(960*kinectResize, 640*kinectResize);
        bothKinects.threshold(nearThreshold);
        
        // CV_RETR_CCOMP returns a hierarchy of outer contours and holes
        maxArea = (bothKinects.width * bothKinects.height)/2;
        contourfinder.findContours(bothKinects, minArea, maxArea, maxInput, CV_RETR_CCOMP);
        
        // Clear previous edges
        for(int i=0; i<edges.size(); i++){
            edges[i].get()->clear();
        }
        
        for(int i=0; i<contourfinder.blobs.size(); i++){
            
            cvblobs[i] = contourfinder.blobs.at(i); //contain blobs in <cvblobs>
            numOfPtsOfBlob = cvblobs[i].nPts;   // number of points(x,y) in each blob
            
            if(cvblobs[i].hole){
                ofPtr<ofxBox2dEdge> edge = ofPtr<ofxBox2dEdge>(new ofxBox2dEdge);
                ofPtr<ofPoint> lastpoint = ofPtr<ofPoint>(new ofPoint);
                
                for(int j=0; j<numOfPtsOfBlob; j+=4){   //get every 4th points
                    
                    ofPtr<ofPoint> point = ofPtr<ofPoint>(new ofPoint); //and put into point
                    point.get()->x = cvblobs[i].pts.at(j).x;
                    point.get()->y = cvblobs[i].pts.at(j).y;
                    
                    lastpoint.get()->x = cvblobs[i].pts.at(0).x;
                    lastpoint.get()->y = cvblobs[i].pts.at(0).y;
                    
                    edge.get()->addVertex(point.get()->x, point.get()->y);  //add pointX,pointY in edge
                }
                edge.get()->addVertex(lastpoint.get()->x, lastpoint.get()->y); //add to connect first&last points
                
                edge.get()->setPhysics(0.0, 0.0, 1.0);
                edge.get()->create(box2d.getWorld());   // set box2d world in edge
                edges.push_back(edge);  // add edge into <edges>
            }
        }
    }
    currentInput = contourfinder.nBlobs;    // current number of blobs
    grayimage.flagImageChanged();
    grayimage1.flagImageChanged();
    
    ofRemove(circles, ofxBox2dBaseShape::shouldRemoveOffScreen);
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    //cout << message;
    
    // Draw depth image and contour
    bothKinects.draw(0, 0, bothKinects.width,bothKinects.height);
    contourfinder.draw(0, 0, bothKinects.width,bothKinects.height);
    
    // Draw yellow circle in the center of each blob
    for(int i=0; i<currentInput; i++){
        if(cvblobs[i].hole){
            ofSetColor(255, 255, 0);
            ofCircle(cvblobs[i].centroid.x, cvblobs[i].centroid.y, 5);
            ofSetColor(255);
            message = "HOLE :";
        }else{
            message = "not hole //";
        }
    }
    
    // Draw green Box2dCircles
    for(int i=0; i<circles.size(); i++){
        ofFill();
        ofSetColor(0, 255, 100);
        circles[i].get()->draw();
        ofSetColor(255);
    }
    
    // Draw red Box2dEdge on the contour of each blob
    ofSetColor(255, 0, 0);
    ofSetLineWidth(3.0);
    for(int i=0; i<edges.size(); i++){
        edges[i].get()->draw();
    }
    
    // Draw screen guidlines
    ofSetColor(0, 255, 255);
    for(int i=1; i<8; i++){
        ofSetLineWidth(1.0);
        if(i%2==1){
            ofSetLineWidth(0.5);
        }
        ofLine(0, i*(screenHeight/8), screenWidth, i*(screenHeight/8));
        ofLine(i*(screenWidth/8), 0, i*(screenWidth/8), screenHeight);
    }
    ofSetColor(255);
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'c'){
        float radius = 15;
        circles.push_back(ofPtr<ofxBox2dCircle>(new ofxBox2dCircle));
        ofxBox2dCircle * circle = circles.back().get();
        circle -> setPhysics(5.0, 0.5, 1.0); // density, bounce, friction
        circle -> setup(box2d.getWorld(), mouseX, mouseY, radius);
    }
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
