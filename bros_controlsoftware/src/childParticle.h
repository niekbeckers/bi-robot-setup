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
        
        float uniqueVal;
        float alpha = 1.0;    
		ofFillFlag _fillMode = OF_OUTLINE;
    
        particleDisplayMode dispMode = PARTICLE_DISPLAY_MODE_ALWAYSON;
		particleRepetitionMode repMode = PARTICLE_REPETITION_MODE_ONCE;// PARTICLE_REPETITION_MODE_REPEAT;
    
    public:
        childParticle();
    

		ofPoint absPos0 = ofPoint(0.0, 0.0);
		bool useAbsPos0 = false;

		ofPoint relPos = ofPoint(0.0, 0.0);
		ofPoint parentPos = ofPoint(0.0, 0.0);

		ofPoint vel = ofPoint(0.0, 0.0);
		ofPoint frc = ofPoint(0.0, 0.0);

		ofColor color = ofColor::deepSkyBlue;

		float scale = 1.0;
		float radius = 4.0f; 
		float lineWidth = 2.0f; 

		bool addNoise = false;
		bool isActive = false;

		bool doDraw = true;
		float startTime = 0.0;
		float resetTime = 0.0;
		float drag = 0.0;

		// 
		// functions
		//
        void reset();
        void update();
        void draw();

        void setDisplayMode(particleDisplayMode newMode);
        void setRepetitionMode(particleRepetitionMode newMode);
		void setFillMode(ofFillFlag fillFlag);
        void setPeriod(float newPeriod);

		inline ofPoint getAbsPos() { return relPos + parentPos; };
    

};
