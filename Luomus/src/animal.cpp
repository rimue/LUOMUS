//
//  animal.cpp
//  Luomus
//
//  Created by LeeNarim on 2015. 2. 27..
//
//

#include "animal.h"


void animal::setup(string filename, string spname){
    
    texture = new ofxTexturePacker();
    texture -> load("texture/" + filename);
    texture -> setDebugMode(true);
    animalAnimation = texture -> getAnimatedSprite( spname );
    
    if(animalAnimation != NULL){
        animalAnimation->setSpeed(30);
        animalAnimation->play();
    }else{
        ofLog(OF_LOG_FATAL_ERROR, "Could not load animated sprite");
    }
    
    animalW = animalAnimation->getWidth();
    animalH = animalAnimation->getHeight();
    collisionW = animalW*.65;
    collisionH = animalH*.25;
    
    animalX = ofGetScreenWidth()/4;
    animalY = ofGetScreenHeight()/2;
    
    groundSpeed = 0.02;
    groundTimer = 0;
    direction = -1;
    xSpeed = 5.0;

}


void animal::update(){
    
    animalAnimation->update();
    animalAnimation->play();

    animalAniX = animalX-animalW/2;
    animalAniY = animalY-animalH/2;
    
}

void animal::animalmove(){
    
    if (animalAnimation->getCurrentFrame()==6) {
        animalAnimation->setFrame(1);
    }
    
    // y-axis movement
    animalY += (xSpeed*direction);
    if(animalY <= 0) direction *= -1;
    if(animalY+animalH >= screenHeight ) direction *= -1;
    
    // x-axis movement
    float   t = ofGetElapsedTimef() * 0.6;
    float   x = ofSignedNoise(t) * 150 + (sin(t)* 50);
    if ( ofGetElapsedTimef() - groundTimer > groundSpeed ) {
        float newHeight = 200 + x;
        groundTimer = ofGetElapsedTimef();
        animalX = newHeight;
    }
    
}
void animal::animalcaught(ofVec2f currentPos){
    
    // bf stops at the current position
    animalX = currentPos.x;
    animalY = currentPos.y;
    
    // play 'being caught' animation
    animalAnimation->setFrame(7);
    if (animalAnimation->getCurrentFrame()==8) {
        animalAnimation->setFrame(7);
    }
    
}

void animal::draw(){

    
    if (animalAnimation) {
        animalAnimation->draw(animalAniX, animalAniY);
    }
    
}