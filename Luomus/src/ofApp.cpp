#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(100);
    ofSetFrameRate(30);
    
    // Kinect setup
    ofSetLogLevel(OF_LOG_ERROR);
    kinect.setRegistration(false);
//    kinect.listDevices();
    kinect.init();
//    kinect.open("A00365917784047A");
    kinect.open();
    kinect.setCameraTiltAngle(0);
    
    grayimage.allocate(kinect.width, kinect.height);
    grayimage1.allocate(kinect.width, kinect.height);
    
    bothKinects.allocate(kinect.height*2, kinect.width);
    
    combinedVideo = (unsigned char*)malloc(640 * 480 * 2 * sizeof(unsigned char*));
    
    cvblobs.resize(100);
    
    // Box2d setup
    ofSetVerticalSync(false);
    box2d.init();
    box2d.setGravity(0, 0.0);
    box2d.registerGrabbing();
    //box2d.createBounds();
    box2d.enableEvents();
    
    ofAddListener(box2d.contactStartEvents, this, &ofApp::contactStart);
    ofAddListener(box2d.contactEndEvents, this, &ofApp::contactEnd);
    
    // animal
    background.loadImage("back1.jpg");
//    raccoon.loadImage("raccoon.png");
    
    bird = new ofxTexturePacker();
    bird->load("texture/notrim.xml");
    birdAnimation = bird->getAnimatedSprite("bird");
    //bird->setDebugMode(true);
    if(birdAnimation != NULL){
        birdAnimation->setSpeed(30);
        birdAnimation->play();
    }else{
        ofLog(OF_LOG_FATAL_ERROR, "Could not load animated sprite");
    }
    
    birdW = birdAnimation->getWidth();
    birdH = birdAnimation->getHeight();
    birdX = 300;
    birdY = screenHeight/2;
    
    birdRectW = birdW*.25;
    birdRectH = birdH*.65;
    
    groundSpeed = 0.02;
    groundTimer = 0;
    
    xSpeed = 5.0;
    direction = -1;
    
    br = ofPtr<ofxBox2dRect>(new ofxBox2dRect);
    br.get()->setPhysics(0.01, 0.1, 0.1);
    ofRectangle brec = ofRectangle(birdX, birdY, birdRectW, birdRectH);
    br.get()->setup(box2d.getWorld(), brec);
    rects.push_back(br);
    
}

bool ofApp::isInsideLine(ofxBox2dRect* rect){
    ofVec2f pos = rect->getPosition();
    
    for (int i = 0; i < edges.size(); i++) {
        ofPtr<ofxBox2dEdge> edge = edges[i];
        edge.get()->inside(pos);
        if ( edge->inside(pos.x, pos.y) ) {
            return true;
        }
    };
    return false;
}


//--------------------------------------------------------------
void ofApp::update(){
    
    box2d.update();
    kinect.update();
    
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
        for ( int i=0; i<640; i++ ) {
            memcpy(combinedVideo + (i*960), GrayPixel.getPixels()+(i*480), 480);
//            memcpy(combinedVideo + (i*960+480), GrayPixel1.getPixels()+(i*480), 480);
            bothKinects.setFromPixels(combinedVideo, 480*2, 640);
        }
        bothKinects.resize(960*kinectResize, 640*kinectResize);
        bothKinects.threshold(nearThreshold);
        
        // CV_RETR_CCOMP returns a hierarchy of outer contours and holes
        maxArea = (bothKinects.width * bothKinects.height)*2;
        contourfinder.findContours(bothKinects, minArea, maxArea, maxInput, CV_RETR_CCOMP);
        
        // Clear previous edges
        for ( int i=0; i<edges.size(); i++ ) {
            edges[i].get()->clear();
        }
        
        // Loop all found blobs
        for(int i=0; i<contourfinder.blobs.size(); i++){
            
            cvblobs[i] = contourfinder.blobs.at(i); // Store found blob in <cvblobs>
            int numOfPtsOfBlob = cvblobs[i].nPts;   // Number of points(x,y) in each blob
            
            // If there is a hole (the blob is "closed")
            if(cvblobs[i].hole){
                
                // Create an edge for capture area
                ofPtr<ofxBox2dEdge> edge = ofPtr<ofxBox2dEdge>(new ofxBox2dEdge);
                
                // Loop points in blob
                // Get every 4th point
                for ( int j=0; j < numOfPtsOfBlob; j += 8 ) {
                    
                    int pointX = cvblobs[i].pts.at(j).x;
                    int pointY = cvblobs[i].pts.at(j).y + TOP_MARGIN;
                    
                    // Add point to edge
                    edge.get()->addVertex( pointX, pointY );
                }
                
                // Final point of the edge
                int lastPointX = cvblobs[i].pts.at(0).x;
                int lastPointY = cvblobs[i].pts.at(0).y + TOP_MARGIN;
                
                // Add last point to edge
                edge.get()->addVertex( lastPointX, lastPointY );
                
                edge.get()->setPhysics( 20.0, 0.0, 0.0 );
                edge.get()->create( box2d.getWorld() ); // Set box2d world in edge
                edges.push_back(edge); // Add edge into end of <edges>
                edge.get()->close();
                
                edge.get()->update();
                
                
            }
        }
    }
    
    currentInput = contourfinder.nBlobs; // Current number of blobs
    grayimage.flagImageChanged();
    grayimage1.flagImageChanged();
    
    /////////////////////////////////////////////////////////////////////
    
    
    birdCurrentPos = br.get()->getPosition();
    birdAnimation->update();
    
//    if(birdAnimation){}
    
    for(int i=0; i<rects.size(); i++){
        if (!(isInsideLine(rects[i].get()))) {
            aniplay = true;
            birdAnimation->play();
            birdY += (xSpeed*direction);
            if(birdY <= 0) direction *= -1;
            if(birdY+birdRectH >= screenHeight) direction *= -1;
            
            float   t = ofGetElapsedTimef() * 0.6;
            float   x = ofSignedNoise(t) * 150 + (sin(t)* 50);
            if(ofGetElapsedTimef() - groundTimer > groundSpeed) {
                float newHeight = 200 + x;
                groundTimer = ofGetElapsedTimef();
                birdX = newHeight;
            }
        }
    }
    
    
    if (!aniplay) {
        birdAnimation->stop();
        birdX = birdCurrentPos.x;
        birdY = birdCurrentPos.y;
    }
    
    br.get()->setPosition(birdX, birdY);
    br.get()->setRotation(brAngle);
    
    birdAniX = birdX-birdW/2;
    birdAniY = birdY-birdH/2;
    
    /////////////////////////////
    // Set rect densities
    for ( int i=0; i < rects.size(); i++ ) {
        ofxBox2dRect *rect = rects[i].get();
        
        birdRectPos = rect->getPosition();
        rect->update();
        ofSetColor(255);
        
        b2Body* body = rect->body;
        
        if ( isInsideLine(rect) ) {
            
            body->SetType(b2_dynamicBody);
            body->GetFixtureList()->SetDensity(0.001);
            body->ResetMassData();
            //cout << "setting circle density to 0.05 " << endl;
        }
        else {
            
            body->SetType(b2_staticBody);
            body->GetFixtureList()->SetDensity(0.0);
            body->ResetMassData();
            
            //cout << "setting circle density to 0.0" << endl;
        }
        
    }

    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    //cout << kinect.getSerial() << "\n";
    //background.draw(0, 0);
    // Draw kinect depth image
    bothKinects.draw(0, TOP_MARGIN, bothKinects.width,bothKinects.height);
    //contourfinder.draw(0, TOP_MARGIN);
    
    // Draw yellow circle in the center of each blob
    for(int i=0; i<currentInput; i++){
        if(cvblobs[i].hole){
            blobCenterX = cvblobs[i].centroid.x;
            blobCenterY = cvblobs[i].centroid.y;
            ofSetColor(255, 255, 0);
            ofCircle(blobCenterX, blobCenterY+TOP_MARGIN, 5);
            ofSetColor(255);
        }
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
    
//    // Draw raccoon collision area: green Box2dCircles
//    for(int i=0; i<circles.size(); i++){
//        ofNoFill();
//        ofSetColor(0, 255, 100);
//        circles[i].get()->draw();
//        circlePos = circles[i].get()->getPosition();
//        ofSetColor(255);
//    }
//    
//    // Draw raccoon image
//    circlePosX = circlePos.x;
//    circlePosY = circlePos.y;
//    raccoon.draw(circlePosX-55, circlePosY-30, raWidth*.65, raHeight*.65);
//    ofFill();
    
    
    ///////////////////////////////////////////////////////////////////////////
    
    // Draw bird collision area
    for(int i=0; i<rects.size(); i++){
        ofNoFill();
        ofSetLineWidth(3.0);
        ofSetColor(0, 0, 255);
        rects[i].get()->draw();
        ofSetColor(255);
    }
    
    // draw red rect - mouse click
    for(int i=0; i<testRects.size(); i++){
        ofSetColor(255, 0, 0);
        testRects[i].get()->draw();
//        if (isInsideLine(testRects[i].get())) {
//            cout << " RECTANGLE IS INSIDE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
//        }
        ofSetColor(255);
    }
    
    // Draw bird animation
    if (birdAnimation) {
        birdAnimation->draw(birdAniX, birdAniY);
    }
    
}
//--------------------------------------------------------------
void ofApp::contactStart(ofxBox2dContactArgs &e){
    
    if(e.a != NULL && e.b != NULL) {
        if(e.a -> GetType() == b2Shape::e_edge && e.b->GetType() == b2Shape::e_polygon){
            for(int i=0; i<rects.size(); i++){
                if (isInsideLine(rects[i].get())) {
                    
                                cout << " contactStart......" << endl;
                    aniplay = false;
                    //birdAnimation->stop();
                }
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::contactEnd(ofxBox2dContactArgs &e){
    
    if(e.a != NULL && e.b != NULL) {
//        for(int i=0; i<rects.size(); i++){
//            if (!(isInsideLine(rects[i].get()))) {
//                
//                cout << ".....................contactEnd" << endl;
//                aniplay = true;
//                birdAnimation->play();
//            }
//        }
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    ofPtr <ofxBox2dRect> r = ofPtr<ofxBox2dRect>(new ofxBox2dRect);
    r.get()->setPhysics(1, 0.5, 0.9);
    ofRectangle rec = ofRectangle(x, y, 50, 50);
    r.get()->setup(box2d.getWorld(), rec);
    testRects.push_back(r);
    
}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == 's') {
        aniplay=false;
        birdAnimation->stop();
    }
    
    if (key == 'p') {
        aniplay=true;
        birdAnimation->play();
    }
    
    // Place the animal image at mouse position
//    if(key == 'c'){
//        
//        circles.clear();
//        
//        float radius = 43;
//        circles.push_back(ofPtr<ofxBox2dCircle>(new ofxBox2dCircle));
//        ofxBox2dCircle * circle = circles.back().get();
//        
//        // Initial density is 0
//        circle -> setPhysics(0.05, 0.0, 0.0); // density, bounce, friction
//        circle -> setup(box2d.getWorld(), mouseX, mouseY, radius);
//        
//    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
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
