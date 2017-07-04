#pragma once
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <cmath>
#include <chrono>
#include <random>
#include "ofMain.h"
#include "childParticle.h"


enum parentParticleMode {
    PARENTPARTICLE_MODE_NORMAL = 0,
    PARENTPARTICLE_MODE_CLOUD,
    PARENTPARTICLE_MODE_EXPLODE
};

class parentParticle {
    private:
        parentParticleMode mode = PARENTPARTICLE_MODE_NORMAL;
        ofPoint pos;
        ofColor color = ofColor::deepSkyBlue;
        float startTime;
        ofFillFlag fillMode = OF_OUTLINE;
    
        vector<childParticle> particles;
    
        void resetPositions();
    public:
        parentParticle();
    
    
        float radius = 10.0f;
        float mean = 0.0f;
        float stdev = 6.0f;
        float velExplode = 400.0f;
    
        void reset();
        void update();
        void draw();
        void setFillMode(ofFillFlag fillFlag);
        void setMode(parentParticleMode newMode);
        void setPosition(ofPoint p);
        void setColor(ofColor c);
    
};
