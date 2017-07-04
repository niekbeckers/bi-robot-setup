#include "parentParticle.h"

//--------------------------------------------------------------
parentParticle::parentParticle()
{
    
}

//--------------------------------------------------------------
void parentParticle::reset()
{
    // random
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    std::normal_distribution<float> distribution(mean,stdev);
    
    // clear particles vector
    particles.clear();
    
    switch (mode) {
        case PARENTPARTICLE_MODE_NORMAL:
            // do nothing
            break;
        case PARENTPARTICLE_MODE_CLOUD:
            // give all the particles a relative position (random)
            particles.assign(25, childParticle());
            startTime = ofGetElapsedTimef();
            for(unsigned int i = 0; i < particles.size(); i++){
                particles[i].reset();
                particles[i].addNoise = false;
                particles[i].setDisplayMode(PARTICLE_DISPLAY_MODE_ONOFF_ALPHA);
                particles[i].setRepetitionMode(PARTICLE_REPETITION_MODE_REPEAT);
                particles[i].setPeriod(1.0f + ofRandom(3.0f));
                particles[i].setPhase(2.0*M_PI*ofRandom(0.5f));

                // position (normal distribution)
                double x = distribution(generator);
                double y = distribution(generator);
                ofPoint p = ofPoint((float)x,(float)y);
                particles[i].relPos = p;
                particles[i].vel = ofPoint(0.0,0.0);
            }
            
            break;
            
        case PARENTPARTICLE_MODE_EXPLODE:
            // make 2500 particles
            particles.assign(2500, childParticle());
            
            // all particles in a circle, give them a velocity
            for(unsigned int i = 0; i < particles.size(); i++){
                particles[i].reset();
                particles[i].addNoise = true;
                particles[i].setDisplayMode(PARTICLE_DISPLAY_MODE_ALWAYSON);
                
                // position
                float theta = 2*M_PI*(float)i/(float)particles.size();
                ofPoint p = ofPoint(radius*sin(theta),radius*cos(theta));
                particles[i].relPos = p;
                ofPoint v = p.normalize()*ofRandom(0.01,1.0)*velExplode;
                particles[i].vel = v;
            }
            break;
    }
    
}

//--------------------------------------------------------------
void parentParticle::update()
{
    switch (mode) {
        case PARENTPARTICLE_MODE_NORMAL:
            // do nothing, position is updated externally
            break;
        case PARENTPARTICLE_MODE_CLOUD:
            // check if any of the points are not active anymore
            for(unsigned int i = 0; i < particles.size(); i++){
                particles[i].parentPos = pos;
                particles[i].update();
            }
            
            // after some time, refresh (reseed)
            //if (ofGetElapsedTimef() - startTime > 15.0) {
            //    resetPositions();
            //    startTime = ofGetElapsedTimef();
            //}
            
            break;
        case PARENTPARTICLE_MODE_EXPLODE:
            for(unsigned int i = 0; i < particles.size(); i++){
                particles[i].parentPos = pos;
                particles[i].update();
            }
            break;
    }
}

//--------------------------------------------------------------
void parentParticle::draw()
{
    setFillMode(fillMode);
    ofSetColor(color);
    
    if (mode == PARENTPARTICLE_MODE_NORMAL) {
        ofSetCircleResolution(120);
        ofSetLineWidth(6);
        ofDrawCircle(pos.x,pos.y,radius);
    }
    else if (mode == PARENTPARTICLE_MODE_CLOUD || mode == PARENTPARTICLE_MODE_EXPLODE) {
        for(unsigned int i = 0; i < particles.size(); i++){
            particles[i].draw();
        }
    }
}

//--------------------------------------------------------------
void parentParticle::resetPositions()
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    std::normal_distribution<float> distribution(mean,stdev);

    for(unsigned int i = 0; i < particles.size(); i++){
        double x = distribution(generator);
        double y = distribution(generator);
        ofPoint p = ofPoint((float)x,(float)y);
        particles[i].relPos = p;
    }
    
}

//--------------------------------------------------------------
void parentParticle::setMode(parentParticleMode newMode)
{
    mode = newMode;
    reset();
}

//--------------------------------------------------------------
void parentParticle::setPosition(ofPoint p)
{
    pos = p;
}

//--------------------------------------------------------------
void parentParticle::setFillMode(ofFillFlag fillFlag)
{
    fillMode = fillFlag;
    if (fillFlag == OF_OUTLINE) {
        ofNoFill();
    }
    else if (fillFlag == OF_FILLED) {
        ofFill();
    }
}

//--------------------------------------------------------------
void parentParticle::setColor(ofColor c)
{
    color = c;
    for(unsigned int i = 0; i < particles.size(); i++) {
        particles[i].color = c;
    }
}
