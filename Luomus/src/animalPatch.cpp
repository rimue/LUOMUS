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
    //hitRectangle = ofPtr<ofxBox2dRect>(new ofxBox2dRect);
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


void animalPatch::setup( ofxBox2d* box2d, string filename, float x, float y, float width, float height) {
    
    this->width = width;
    this->height = height;
    this->x = x;
    this->y = y;
    
    
    // Setup hit rectangle
//    hitRectangle.get()->setup( box2d->getWorld(), x, y, width, height );
//    hitRectangle.get()->setPhysics( 0.0, 0.0, 0.0 );
//    hitRectangle.get()->body->SetType(b2_kinematicBody);
//    hitRectangle.get()->body->ResetMassData();
    
    // Init patch image
    image.loadImage(filename);

    // Setup contact listeners
//    ofAddListener( box2d->contactStartEvents, this, &animalPatch::contactStart );
//    ofAddListener( box2d->contactEndEvents, this, &animalPatch::contactEnd );

    
    return;
}

void animalPatch::setPosition(float x, float y) {
    this->x = x;
    this->y = y;
//    hitRectangle.get()->setPosition(x,y);
}
void animalPatch::setPosition(ofVec2f p) {
    this->x = p.x;
    this->y = p.y;
//    hitRectangle.get()->setPosition(p);
}

//------------------------------------------------
ofVec2f animalPatch::getPosition() {
    return *new ofVec2f(x, y);
}

//------------------------------------------------
void animalPatch::destroy() {
    //hitRectangle.get()->destroy();
}

//------------------------------------------------
void animalPatch::update() {
//    hitRectangle.get()->update();
}

bool animalPatch::contains( ofPtr<ofxBox2dRect> rect) {
    return rectOverlap( this->x, this->y, this->width, this->height, rect );
}

void animalPatch::draw() {
    // Draw rectangle
//    ofSetColor(150, 200, 0);
//    ofSetLineWidth(2.0);
//    ofRect(this->x, this->y, this->width, this->height);

    // Draw the image at the current position
    image.draw( this->x, this->y, this->width, this->height );
}


