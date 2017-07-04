#pragma once
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <ofMain.h>

enum particleDisplayMode {
    PARTICLE_DISPLAY_MODE_ALWAYSON = 0,
    PARTICLE_DISPLAY_MODE_ONOFF_CONSTANT,
    PARTICLE_DISPLAY_MODE_ONOFF_ALPHA
};

enum particleRepetitionMode {
    PARTICLE_REPETITION_MODE_ONCE = 0,
    PARTICLE_REPETITION_MODE_REPEAT
};

class childParticle {
    private:
        float period = 1.0f;
        float phase = 0.0f;
        float startTime = 0.0;
        float uniqueVal;
        float alpha = 1.0;
        float drag;
    
        bool doDraw = false;
    
        particleDisplayMode dispMode = PARTICLE_DISPLAY_MODE_ONOFF_ALPHA;
        particleRepetitionMode repMode = PARTICLE_REPETITION_MODE_REPEAT;
    
    public:
        childParticle();
    
        void reset();
        void update();
        void draw();
        void setDisplayMode(particleDisplayMode newMode);
        void setRepetitionMode(particleRepetitionMode newMode);
        void setPeriod(float newPeriod);
        void setPhase(float newPhase = 0.0f);
    
        ofPoint relPos = ofPoint(0,0);
        ofPoint parentPos = ofPoint(0,0);
        ofPoint vel = ofPoint(0,0);
        ofPoint frc = ofPoint(0,0);
        
        ofColor color = ofColor::deepSkyBlue;
    
        float scale = 1.0;
        float radius = 2.0f;
    
        bool addNoise = false;
        bool isActive = false;
};
