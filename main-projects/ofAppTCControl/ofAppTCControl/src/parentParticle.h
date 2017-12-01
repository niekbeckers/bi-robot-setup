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
        parentParticleMode _mode = PARENTPARTICLE_MODE_NORMAL;
        ofPoint _pos;
        ofColor _color = ofColor::deepSkyBlue;
        float _startTime;
        ofFillFlag _fillMode = OF_OUTLINE;

		float _tickPeriod = 1.0f / 60.0f; // 1 second refresh
		float _timeTick;
    
        vector<childParticle> _particles;
		deque<ofPoint> _tailPoints;
		int _numTailPoints = 60;
		ofPolyline _line;

		int h = ofGetScreenHeight();
		int w = ofGetScreenWidth();

		float dots_per_m = sqrt((double)(h * h) + (double)(w * w)) / 27.0 * 100.0 / 2.54; //dots per meter
    
		

        void resetPositions();
    public:
        parentParticle();
    
    
        float radius = 10.0f;
        float mean = 0.0f;
        float stdev = 0.5*sqrt(0.0001f) * dots_per_m; // pixels
		
		
		float velExplode = 400.0f;



		bool drawTail = false;
    
        void reset();
        void update();
        void draw();
        void setFillMode(ofFillFlag fillFlag);
        void setMode(parentParticleMode newMode);
        void setPosition(ofPoint p);
        void setColor(ofColor c);
    
};
