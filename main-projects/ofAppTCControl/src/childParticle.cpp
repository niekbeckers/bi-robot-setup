#include "childParticle.h"

//------------------------------------------------------------------
childParticle::childParticle()
{
    doDraw = false;
    isActive = false;
}

//------------------------------------------------------------------
void childParticle::reset()
{
    //the unique val allows us to set properties slightly differently for each particle
    uniqueVal = ofRandom(-10000, 10000);
    
    vel = ofPoint(0, 0);
    frc = ofPoint(0, 0, 0);
    drag  = ofRandom(0.95, 0.998);
    
    scale = ofRandom(0.5, 1.0);
    alpha = 1.0f;
    phase = 0.0f;
    
    startTime = ofGetElapsedTimef();
}

//------------------------------------------------------------------
void childParticle::update()
{
    // display updated
    float t = ofGetElapsedTimef() - startTime;
    switch (dispMode) {
        case PARTICLE_DISPLAY_MODE_ALWAYSON:
            alpha = 1.0f;
            doDraw = true;
            break;
        case PARTICLE_DISPLAY_MODE_ONOFF_CONSTANT:
            alpha = 1.0f;
            doDraw = (t <= 0.5f*period);
            break;
        case PARTICLE_DISPLAY_MODE_ONOFF_ALPHA:
            //alpha = abs(sin(2.0f*M_PI*t/period));
            alpha = pow(1.0f-cos(2.0f*M_PI*t/period + phase),2.0f);
            doDraw = (t <= period);
            break;
    }
    
    // repetition mode
    if (t >= period) {
        isActive = false;

        if (repMode == PARTICLE_REPETITION_MODE_REPEAT) {
            doDraw = true;
            isActive = true;
            startTime = ofGetElapsedTimef();
        }
    }
    
    // update kinematics
    float dt = 1.0/60.0f;
    if (ofGetFrameRate() > 30.0f)
        dt = 1.0f/ofGetFrameRate();
    
    // add a little bit of noise to the particle?
    ofPoint frcNoise;
    frcNoise.x = 2.0*ofSignedNoise(uniqueVal, relPos.y * 0.1, ofGetElapsedTimef()*0.2) * (float)addNoise;
    frcNoise.y = 2.0*ofSignedNoise(uniqueVal, relPos.x * 0.1, ofGetElapsedTimef()*0.2) * (float)addNoise;
    
    // integrate
    vel += (frc + frcNoise - vel*0.1*drag)*dt;
    relPos += vel*dt;
}

//------------------------------------------------------------------
void childParticle::draw()
{
    if (doDraw) {
        ofFill();
        ofEnableAlphaBlending();
        ofSetColor(color.r, color.g, color.b, (int)(alpha*255.0f));
        ofDrawCircle(relPos.x + parentPos.x, relPos.y + parentPos.y, radius*scale);
        ofDisableAlphaBlending();
    }
}

//------------------------------------------------------------------
void childParticle::setDisplayMode(particleDisplayMode newMode)
{
    dispMode = newMode;
}

//------------------------------------------------------------------
void childParticle::setRepetitionMode(particleRepetitionMode newMode)
{
    repMode = newMode;
}

//------------------------------------------------------------------
void childParticle::setPeriod(float newPeriod)
{
    period = newPeriod;
}

//------------------------------------------------------------------
void childParticle::setPhase(float newPhase)
{
    phase = newPhase;
}

