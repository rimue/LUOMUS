//
//  luomusUtils.h
//  Luomus
//
//  Created by Tom Engström on 19/02/15.
//
//

#ifndef Luomus_luomusUtils_h
#define Luomus_luomusUtils_h

#include "ofxBox2d.h"

static bool rectOverlap( ofPtr<ofxBox2dRect> r_a, ofPtr<ofxBox2dRect> r_b ) {

    float a_height = r_a.get()->getHeight();
    float a_width = r_a.get()->getWidth();
    float a_x = r_a.get()->getPosition().x - a_width/2;
    float a_y = r_a.get()->getPosition().y - a_height/2;
    
    float b_height = r_b.get()->getHeight();
    float b_width = r_b.get()->getWidth();
    float b_x = r_b.get()->getPosition().x - b_width/2;
    float b_y = r_b.get()->getPosition().y - b_height/2;

    return !(a_x + a_width < b_x || a_y + a_height < b_y || a_x > b_x + b_width || a_y > b_y + b_height);
}


#endif
