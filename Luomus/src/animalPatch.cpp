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

void animalPatch::setup( b2World* world) {
        
    // Setup hit rectangle
    hitRectangle.get()->setup( world, 0.0, 0.0, 100.0, 100.0);
    
    // Init patch image
    image.loadImage("dog.png");
    
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

