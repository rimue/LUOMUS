#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(100);
    ofSetFrameRate(30);
    
    // Kinect setup
    ofSetLogLevel(OF_LOG_ERROR);
    kinect.setRegistration(false);
    kinect.init();
    kinect.open("A00365A12829045A");
    kinect.setCameraTiltAngle(15);
    
//    kinect1.setRegistration(false);
//    kinect1.init();
//    kinect1.open("A00365917784047A");
//    kinect1.setCameraTiltAngle(0);
    
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
    //box2d.setFPS(24.0);
    
    // animal
    background.loadImage("back.jpg");
    raccoon.loadImage("raccoon.png");
    
    bird = new ofxTexturePacker();
    bird->load("texture/notrim.xml");
    birdAnimation = bird->getAnimatedSprite("bird");
    if(birdAnimation != NULL){
        birdAnimation->setSpeed(30);
        birdAnimation->play();
    }else{
        ofLog(OF_LOG_FATAL_ERROR, "Could not load animated sprite");
    }
}

//void ofApp::exit(){ }
//--------------------------------------------------------------
void ofApp::update(){
    kinect.update();
    box2d.update();
    if(kinect.isFrameNew()){
        // Get depth image from kinect and add pixels to cvGrayImages
        grayimage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        grayimage1.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        grayimage.blur(1.5);    grayimage1.blur(1.5);
        
        GrayPixel = grayimage.getPixelsRef();
        GrayPixel.rotate90(1);
        GrayPixel1 = grayimage1.getPixelsRef();
        GrayPixel1.rotate90(-1);
        
        // Combine two kinect images
        for(int i=0; i<640; i++){
            memcpy(combinedVideo + (i*960), GrayPixel.getPixels()+(i*480), 480);
            //memcpy(combinedVideo + (i*960+480), GrayPixel1.getPixels()+(i*480), 480);
            bothKinects.setFromPixels(combinedVideo, 480*2, 640);
        }
        bothKinects.resize(960*kinectResize, 640*kinectResize);
        bothKinects.threshold(nearThreshold);
        
        // CV_RETR_CCOMP returns a hierarchy of outer contours and holes
        maxArea = (bothKinects.width * bothKinects.height)*2;
        contourfinder.findContours(bothKinects, minArea, maxArea, maxInput, CV_RETR_CCOMP);
        
        // Clear previous edges
        for(int i=0; i<edges.size(); i++){
            edges[i].get()->clear();
        }
        
        // Get points of Blob and Add to Edge
        for(int i=0; i<contourfinder.blobs.size(); i++){
            
            cvblobs[i] = contourfinder.blobs.at(i); //contain blobs in <cvblobs>
            numOfPtsOfBlob = cvblobs[i].nPts;   // number of points(x,y) in each blob
            
            if(cvblobs[i].hole){
                ofPtr<ofxBox2dEdge> edge = ofPtr<ofxBox2dEdge>(new ofxBox2dEdge);
                ofPtr<ofPoint> lastpoint = ofPtr<ofPoint>(new ofPoint);
                
                for(int j=0; j<numOfPtsOfBlob; j+=4){   //get every 4th points
                    
                    ofPtr<ofPoint> point = ofPtr<ofPoint>(new ofPoint); //and put into point
                    point.get()->x = cvblobs[i].pts.at(j).x;
                    point.get()->y = cvblobs[i].pts.at(j).y+topMargin;
                    
                    lastpoint.get()->x = cvblobs[i].pts.at(0).x;
                    lastpoint.get()->y = cvblobs[i].pts.at(0).y+topMargin;
                    
                    edge.get()->addVertex(point.get()->x, point.get()->y);  //add pointX,pointY in edge
                }
                edge.get()->addVertex(lastpoint.get()->x, lastpoint.get()->y); //add to connect first&last points
                
                edge.get()->setPhysics(20.0, 0.0, 0.0);
                edge.get()->create(box2d.getWorld());   // set box2d world in edge
                edges.push_back(edge);  // add edge into <edges>
            }
        }
    }
    currentInput = contourfinder.nBlobs;    // current number of blobs
    grayimage.flagImageChanged();
    grayimage1.flagImageChanged();
    
    ofRemove(circles, ofxBox2dBaseShape::shouldRemoveOffScreen);
    
    if(birdAnimation){
        birdAnimation->update();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    background.draw(0, 0);
    // Draw kinect depth image
    //bothKinects.draw(0, topMargin, bothKinects.width,bothKinects.height);
    contourfinder.draw(0, topMargin);
    
    // Draw yellow circle in the center of each blob
    for(int i=0; i<currentInput; i++){
        if(cvblobs[i].hole){
            ofSetColor(255, 255, 0);
            ofCircle(cvblobs[i].centroid.x, cvblobs[i].centroid.y+topMargin, 5);
            ofSetColor(255);
        }
    }
    
    // Draw raccoon collision area: green Box2dCircles
    for(int i=0; i<circles.size(); i++){
        ofNoFill();
        ofSetColor(0, 255, 100);
        circles[i].get()->draw();
        circlePos = circles[i].get()->getPosition();
        ofSetColor(255);
    }
    
    // Draw raccoon image
    circlePosX = circlePos.x;
    circlePosY = circlePos.y;
    raccoon.draw(circlePosX-55, circlePosY-30, raWidth*.65, raHeight*.65);
    ofFill();
    
    // Draw bird collision area
    for(int i=0; i<rects.size(); i++){
        ofNoFill();
        ofSetColor(100, 150, 200);
        rects[i].get()->draw();
        rectPos = rects[i].get()->getPosition();
        ofSetColor(255);
    }
    
    // Draw bird animation
    if (birdAnimation) {
        birdX = rectPos.x;
        birdY = rectPos.y;
        birdAnimation->draw(birdX-birdW*.6, birdY-birdH*.5);
    }
    
    // Draw red Box2dEdge on the contour of each blob
    ofSetColor(255, 0, 0);
    ofSetLineWidth(3.0);
    for(int i=0; i<edges.size(); i++){
        edges[i].get()->draw();
    }
    
    // Draw screen guidlines
    ofSetColor(0, 255, 255, 100);
    for(int i=1; i<8; i++){
        ofSetLineWidth(1.0);
        if(i%2==1){
            ofSetLineWidth(0.5);
        }
        ofLine(0, i*(screenHeight/8), screenWidth, i*(screenHeight/8));
        ofDrawBitmapString(ofToString(int(ofMap(i*(screenHeight/8), 0, screenHeight, 0, 250)))+"cm", 0, i*(screenHeight/8)+9);
        ofLine(i*(screenWidth/8), 0, i*(screenWidth/8), screenHeight);
        ofDrawBitmapString(ofToString(int(ofMap(i*(screenWidth/8), 0, screenWidth, 0, 350)))+"cm", i*(screenWidth/8)+9, 10);
    }
    ofSetColor(255);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if(key == 'c'){
        circles.clear();
        float radius = 43;
        circles.push_back(ofPtr<ofxBox2dCircle>(new ofxBox2dCircle));
        ofxBox2dCircle * circle = circles.back().get();
        circle -> setPhysics(0.001, 0.0, 0.0); // density, bounce, friction
        circle -> setup(box2d.getWorld(), mouseX, mouseY, radius);
    }
    
    if(key == 'b'){
        rects.clear();
        rects.push_back(ofPtr<ofxBox2dRect>(new ofxBox2dRect));
        ofxBox2dRect * rect = rects.back().get();
        ofRectangle rec;
        rec = ofRectangle(mouseX, mouseY, birdW*.75, birdH*.3);
        rect -> setPhysics(0.001, 0.0, 0.0);
        rect -> setup(box2d.getWorld(), rec);
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
