//
//  animalPatch.h
//  Luomus
//
//  Created by Tom Engström on 19/02/15.
//
//
#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "b2Collision.h"
#include "ofxBox2dUtils.h"
#include "luomusUtils.h"

class animalPatch {
    
public:
    
    ofPtr<ofxBox2dRect> hitRectangle;
    ofImage image;
    
    ofxBox2d* box2d;
        
    animalPatch();
    ~animalPatch();
    
    //------------------------------------------------
    virtual void setPosition(float x, float y);
    virtual void setPosition(ofVec2f p);
    
    virtual void setup( ofxBox2d* box2d );
    
    //------------------------------------------------
    ofVec2f getPosition();
    ofVec2f getB2DPosition();
    
    //------------------------------------------------
    virtual void destroy();
        
    //------------------------------------------------
    virtual void update();
    virtual void draw();
    
    virtual bool contains( ofPtr<ofxBox2dRect> rect);
    
private:

    void contactStart(ofxBox2dContactArgs &e);
    void contactEnd(ofxBox2dContactArgs &e);
    
};
