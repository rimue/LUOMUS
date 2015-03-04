#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(100);
    ofSetFrameRate(30);
    
    // Kinect setup
    ofSetLogLevel(OF_LOG_ERROR);
    kinect.setRegistration(false);
    kinect.init();
    kinect.open();
    //kinect.listDevices();
    //kinect.open("A00365917784047A");
    //kinect.setCameraTiltAngle(0);
    
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
    background.loadImage("background_bFly.png");
    butterfly.setup("bf_notrim.xml", "bf");

    // palyer
    br = ofPtr<ofxBox2dRect>(new ofxBox2dRect);
    br.get()->setPhysics(0.01, 0.1, 0.1);
    ofRectangle brec = ofRectangle(butterfly.animalX, butterfly.animalY, butterfly.collisionW, butterfly.collisionH);
    br.get()->setup(box2d.getWorld(), brec);
    br.get()->body->SetType(b2_dynamicBody);
    br.get()->body->ResetMassData();
    rects.push_back(br);
    
    // Patches
    animalIsCaught = false;
    animalIsOverPatch = false;
    patch = ofPtr<animalPatch>(new animalPatch);
    patch.get()->setup( &box2d, "medow_patch.png", 400.0, 250.0, 248.0, 248.0 );
    
    // Syphon for madmapper
    mainOutputSyphonServer.setName("Screen Output");
    
    return;
}


bool ofApp::isInsideLine(ofxBox2dRect* rect){
    
    // Get position of rectangle
    ofVec2f pos = rect->getPosition();
    
    // Loop edges
    for (int i = 0; i < edges.size(); i++) {
        ofPtr<ofxBox2dEdge> edge = edges[i];
        
        // Test if the rectangle is inside this edge
        edge.get()->inside(pos);
        if ( edge->inside(pos.x, pos.y) ) {
            // Rectangle is inside an edge
            return true;
        }
    };
    // Rectangle is not inside any edge
    return false;
}

void ofApp::animalCaught(ofxBox2dRect* rect) {
    b2Body* body = rect->body;
    
    // Enable moving
    // Set dynamic type and density for the animal body
    body->SetType(b2_dynamicBody);
    body->GetFixtureList()->SetDensity(0.001);
    body->ResetMassData();
    
    // TODO randomly select position somehow?
//    float patchX = 250.0;
//    float patchY = 600.0;
//    patch->setPosition(patchX, patchY);
//    
    // Show patch
    animalIsCaught = true;

    return;
}

void ofApp::animalReleased( ofxBox2dRect* rect ) {
    
    b2Body* body = rect->body;
    
    // Disable moving
    // Set static type and remove density for the animal body
    body->SetType(b2_staticBody);
    body->GetFixtureList()->SetDensity(0.0);
    body->ResetMassData();
    
    // Hide patch
    animalIsCaught = false;
    
    return;
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
                edge.get()->close();
                edge.get()->update();
                edges.push_back(edge); // Add edge into end of <edges>
            }
        }
    }
    
        
    currentInput = contourfinder.nBlobs; // Current number of blobs
    grayimage.flagImageChanged();
    grayimage1.flagImageChanged();

    
    /////////////////////////////////////////////////////////////////////
    
    
    // Check if animal is caught
    for ( int i=0; i < rects.size(); i++ ) {
        ofxBox2dRect *rect = rects[i].get();
        
        rect->update();
        ofSetColor(255);
        
        if ( isInsideLine(rect) ) {
            // Animal caught
            animalCaught(rect);
            
        }
        else {
            // Animal released
            animalReleased(rect);
        }
    }
    // Check if animal is over patch
    if (animalIsCaught) {
        if ( patch->contains( br ) ) {
            // Animal is over patch
            animalIsOverPatch = true;
        }
        else {
            // Animal not in patch
            animalIsOverPatch = false;
        }
        
    }

    animalCurrentPos = br.get()->getPosition();

    // Animation handling
    if (!animalIsCaught) {
        aniplay = true;
        butterfly.animalmove();
    }
    else {
        butterfly.animalcaught(animalCurrentPos);
    }
    
    butterfly.update();
    
    br.get()->setPosition(butterfly.animalX, butterfly.animalY);
    br.get()->setRotation(butterfly.brAngle);


    return;
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    background.draw(0, 0);
    
    // Draw kinect depth image
    bothKinects.draw(0, TOP_MARGIN, bothKinects.width,bothKinects.height);
    //contourfinder.draw(0, TOP_MARGIN);

    // Draw player edges
    ofSetColor(200, 150, 0);
    ofSetLineWidth(2.0);
    for(int i=0; i<edges.size(); i++){
        edges[i].get()->updateShape();
        edges[i].get()->draw();
    }
    
    // Draw animal
    butterfly.draw();
    
    // Draw animal collision area
    for(int i=0; i<rects.size(); i++){
        ofNoFill();
        ofSetLineWidth(3.0);
        ofSetColor(0, 0, 255);
        if (animalIsCaught && patch->contains(br) ) {
            ofSetColor(100,255,0);
        }
        rects[i].get()->draw();
        ofSetColor(255);
    }
    
    // Draw patch
    if ( animalIsCaught ) {
        patch->draw();
        
        if (patch->contains(br) ) {
            ofVec2f pos = patch->getPosition();
            ofCircle( pos.x+124.0, pos.y+124.0, 50);
        }
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
    
    // Syphon for madmapper
    mainOutputSyphonServer.publishScreen();
    
    
    return;
}
//--------------------------------------------------------------
void ofApp::contactStart( ofxBox2dContactArgs &e ){
    
    if ( e.a != NULL && e.b != NULL ) {
        if (e.a -> GetType() == b2Shape::e_edge && e.b->GetType() == b2Shape::e_polygon){
            // Contact with user and something
            aniplay = false;
        }
    }
    return;
}

//--------------------------------------------------------------
void ofApp::contactEnd(ofxBox2dContactArgs &e){
    
    if(e.a != NULL && e.b != NULL) {}
    return;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
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
