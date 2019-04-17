#pragma once
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <cmath>
#include <chrono>
#include <random>
#include "ofMain.h"
#include "childParticle.h"
#include "myCommon.h"


enum parentParticleMode {
    PARENTPARTICLE_MODE_NORMAL = 0,
    PARENTPARTICLE_MODE_CLOUD,
	PARENTPARTICLE_MODE_MOVINGCLOUD,
    PARENTPARTICLE_MODE_EXPLODE
};

static std::default_random_engine generator;
static std::normal_distribution<float> distribution(0, 1);

class parentParticle {
    private:

        parentParticleMode _mode = PARENTPARTICLE_MODE_NORMAL;
		ParticleShape _shape = PARTICLESHAPE_CIRCLE;
        ofPoint _pos;
        ofColor _color = ofColor::deepSkyBlue;
        float _startTime;
        ofFillFlag _fillMode = OF_OUTLINE;

		float _tickPeriod = 1.0f / 120.0f; // 1 second refresh
		float _timeTick;

		bool _firstUpdate = true;

		float _periodChildParticle = 0.5f;
		bool _useAbsPosChild = false;
    
        vector<childParticle> _particles;
		deque<ofPoint> _tailPoints;
		int _numTailPoints = 60;
		ofPolyline _line;

		int h = ofGetScreenHeight();
		int w = ofGetScreenWidth();

		float dots_per_m = sqrt((double)(h * h) + (double)(w * w)) / 27.0 * 100.0 / 2.54; //dots per meter
    
		// normal distribution for target cloud position and velocity
		float _mean_pos = 0.0f;
		float _sd_pos = 0.02; // m
		float _mean_vel = 0.0f;
		float _sd_vel = 0.005; // m


		//
		// functions
		//
        void resetPositions();
		void resamplePosVel(ofPoint &p, ofPoint &v);
		float randomGaussian(float mean, float stddev);
    public:
        
		// setting (size, random sampling)
        float radius = 8.0f;

		float velExplode = 400.0f;
		bool doDraw = true;
		bool drawTail = false;
		int numberParticles = 10;
		float particleRadius = 4.0f;
    
		float workspaceRadius = 0.1*dots_per_m;

		//
		// functions
		//
		parentParticle();

        void reset();
        void update();
        void draw();
        void setFillMode(ofFillFlag fillFlag);
        void setMode(parentParticleMode newMode);
        void setPosition(ofPoint p);
		ofPoint getPosition();
        void setColor(ofColor c);
		inline void setShape(ParticleShape t) { _shape = t; };


		inline parentParticleMode getMode() { return _mode; }
		inline void setNormalDistPos(float m, float vr) { _mean_pos = m; _sd_pos = vr; };
		inline void setNormalDistVel(float m, float vr) { _mean_vel = m; _sd_vel = vr; };
		inline void setPeriodChildParticle(float p) { _periodChildParticle = p; };
   
};
