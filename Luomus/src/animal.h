//
//  animal.h
//  Luomus
//
//  Created by LeeNarim on 2015. 2. 27..
//
//

#ifndef __Luomus__animal__
#define __Luomus__animal__

#include <stdio.h>

#endif /* defined(__Luomus__animal__) */

#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxTexturePacker.h"


class animal {
    
    
public:
    void setup(string filename, string spname);
    void update();
    void draw();
    void animalmove();
    void animalcaught(ofVec2f currentPos);
    
    ofxTexturePacker * texture;
    ofxTPAnimatedSprite * animalAnimation;
    
    int screenWidth = 1280;
    int screenHeight = 914;
    
    // starting pos
    float animalX, animalY;
    // animal ani pos
    float animalAniX, animalAniY;
    // original sprite size
    float animalW, animalH;
    // box2d sprite size
    float collisionW, collisionH;
    
    float groundSpeed;
    float groundTimer;
    float xSpeed = 5.0;
    int direction;
    float brAngle = 0;
    
private:
    
};
