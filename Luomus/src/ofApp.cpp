#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(100);
    ofSetFrameRate(25);
    
    // Kinect setup
    ofSetLogLevel(OF_LOG_ERROR);
    kinect.setRegistration(false);
    
    kinect.listDevices();
    
    kinect.init();

    kinect.open();

    //kinect.open("A00365A12829045A");
    kinect.setCameraTiltAngle(0);
    
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
    raccoon.loadImage("raccoon.png");
}

//--------------------------------------------------------------
bool ofApp::isCircleInsideLine(ofxBox2dCircle* circle){
    ofVec2f pos = circle->getPosition();

    for ( int i = 0; i < edges.size(); i++ ) {
        ofPtr<ofxBox2dEdge> edge = edges[i];
        edge.get()->inside(pos);
        if ( edge->inside( pos.x, pos.y ) ) {
            return true;
        }
    };
    return false;
}

void ofApp::animalCaught() {
    
    
    
    // Create a patch somewhere
    // And enable it
    
    // Display feedback
    
}

void ofApp::animalReleased() {
    // Hide the patch ?
}

void ofApp::box2dTestUpdate() {
    
    box2d.update();
    
    // World
    b2World* world = box2d.getWorld();
    
    // Create a body
    b2BodyDef myBodyDef;
    myBodyDef.type = b2_dynamicBody;
    myBodyDef.position.Set(0, 20);
    myBodyDef.angle = 0;
    
    b2Body* dynamicBody = world->CreateBody(&myBodyDef);
    
    // Create shape
    b2PolygonShape boxShape;
    boxShape.SetAsBox(1,1);
    
    // Create fixture
    b2FixtureDef boxFixtureDef;
    boxFixtureDef.shape = &boxShape;
    boxFixtureDef.density = 1;
    dynamicBody->CreateFixture(&boxFixtureDef);
    
    

    return;

}

//--------------------------------------------------------------
void ofApp::update(){
    
    //return box2dTestUpdate();
    
    box2d.update();
    kinect.update();
    
    
    if ( kinect.isFrameNew() ) {
        
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
        for ( int i=0; i < contourfinder.blobs.size(); i++ ) {
            
            cvblobs[i] = contourfinder.blobs.at(i); // Store found blob in <cvblobs>
            
            int numOfPtsOfBlob = cvblobs[i].nPts;   // Number of points(x,y) in each blob
            
            // If there is a hole (the blob is "closed")
            if ( cvblobs[i].hole ) {
                
                // Create an edge for capture area
                ofPtr<ofxBox2dEdge> edge = ofPtr<ofxBox2dEdge>(new ofxBox2dEdge);
                
                // Loop points in blob
                // Get every 4th point
                for ( int j=0; j < numOfPtsOfBlob; j += 4 ) {
                    
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
    
    ofRemove(circles, ofxBox2dBaseShape::shouldRemoveOffScreen);
    
    
    // Set circle densities
    for ( int i=0; i < circles.size(); i++ ) {
        ofxBox2dCircle *circle = circles[i].get();
        
        circlePos = circle->getPosition();
        circle->update();
        ofSetColor(255);
        
        b2Body* body = circle->body;
        
        if ( isCircleInsideLine(circle) ) {
            
            body->SetType(b2_dynamicBody);
            body->GetFixtureList()->SetDensity(0.05);
            body->ResetMassData();
            cout << "setting circle density to 0.05 " << endl;
        }
        else {
            
            body->SetType(b2_staticBody);
            body->GetFixtureList()->SetDensity(0.0);
            body->ResetMassData();
            
            cout << "setting circle density to 0.0" << endl;
        }
        
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    
    // Draw kinect depth image
    bothKinects.draw(0, TOP_MARGIN, bothKinects.width,bothKinects.height);
    
    // Draw yellow circle in the center of each blob
    for(int i=0; i<currentInput; i++){
        if(cvblobs[i].hole){
            ofSetColor(255, 255, 0);
            ofCircle(cvblobs[i].centroid.x, cvblobs[i].centroid.y+TOP_MARGIN, 5);
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
        ofDrawBitmapString(ofToString(int(ofMap(i*(screenHeight/8), 0, screenHeight, 0, 250)))+"cm", 0, i*(screenHeight/8)+9);
        ofLine(i*(screenWidth/8), 0, i*(screenWidth/8), screenHeight);
        ofDrawBitmapString(ofToString(int(ofMap(i*(screenWidth/8), 0, screenWidth, 0, 350)))+"cm", i*(screenWidth/8)+9, 10);
    }
    ofSetColor(255);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    
    // Place the animal image at mouse position
    if(key == 'c'){
        
        circles.clear();
        
        float radius = 43;
        circles.push_back(ofPtr<ofxBox2dCircle>(new ofxBox2dCircle));
        ofxBox2dCircle * circle = circles.back().get();

        // Initial density is 0
        circle -> setPhysics(0.05, 0.0, 0.0); // density, bounce, friction
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
