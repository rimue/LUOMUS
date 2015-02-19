//
//  animalPatch.cpp
//  Luomus
//
//  Created by Tom Engström on 19/02/15.
//
//

#include "animalPatch.h"

//----------------------------------------
animalPatch::animalPatch() {
    hitRectangle = ofPtr<ofxBox2dRect>(new ofxBox2dRect);
}

//----------------------------------------
animalPatch::~animalPatch() {
    // TODO destructor stuff
}

//void animalPatch::contactStart(ofxBox2dContactArgs &e) {
//    cout << "contactStart" << endl;
//    
//    if ( e.a != NULL && e.b != NULL ) {
//        
//        if(e.a -> GetType() == b2Shape::e_edge && e.b->GetType() == b2Shape::e_polygon) {
//            
//        }
//    }
//    
//    return;
//}
//
//void animalPatch::contactEnd(ofxBox2dContactArgs &e) {
//    
//    cout << "contactEnd" << endl;
//    
//    if ( e.a != NULL && e.b != NULL ) {
//        
//    }
//    
//    return;
//
//}


void animalPatch::setup(  ofxBox2d* box2d) {
        
    // Setup hit rectangle
    hitRectangle.get()->setPhysics( 0.0, 0.0, 0.0 );
    hitRectangle.get()->setup( box2d->getWorld(), 0.0, 0.0, 300.0, 300.0 );

    // Init patch image
    image.loadImage("dog.png");

    // Setup contact listeners
//    ofAddListener( box2d->contactStartEvents, this, &animalPatch::contactStart );
//    ofAddListener( box2d->contactEndEvents, this, &animalPatch::contactEnd );

    
    return;
}

void animalPatch::setPosition(float x, float y) {
    hitRectangle.get()->setPosition(x,y);
}
void animalPatch::setPosition(ofVec2f p) {
    hitRectangle.get()->setPosition(p);
}

//------------------------------------------------
ofVec2f animalPatch::getPosition() {
    return hitRectangle.get()->getPosition();
}

//------------------------------------------------
void animalPatch::destroy() {
    hitRectangle.get()->destroy();
}

//------------------------------------------------
void animalPatch::update() {
    hitRectangle.get()->update();
}

void animalPatch::draw() {
    // Draw rectangle
    hitRectangle.get()->draw();
    
    // Draw the image at the current position
    ofVec2f pos = getPosition();
    image.draw( pos.x, pos.y, 200, 200 );
}


