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
    
    // clear _particles vector
    _particles.clear();

	// clear tail point vector
	_tailPoints.clear();
    
    switch (_mode) {
        case PARENTPARTICLE_MODE_NORMAL:
            // do nothing
            break;
        case PARENTPARTICLE_MODE_CLOUD:
            // give all the _particles a relative position (random)
            _particles.assign(30, childParticle());
            _startTime = ofGetElapsedTimef();
            for(unsigned int i = 0; i < _particles.size(); i++){
                _particles[i].reset();
				_particles[i].color = _color;
                _particles[i].addNoise = false;
                _particles[i].setDisplayMode(PARTICLE_DISPLAY_MODE_ONOFF_ALPHA);
                _particles[i].setRepetitionMode(PARTICLE_REPETITION_MODE_ONCE);
                _particles[i].setPeriod(0.25f + ofRandom(0.75f));
                _particles[i].setPhase(2.0*M_PI*ofRandom(0.5f));

                // position (normal distribution)
                double x = distribution(generator);
                double y = distribution(generator);
                ofPoint p = ofPoint((float)x,(float)y);
                _particles[i].relPos = p;
                _particles[i].vel = ofPoint(0.0,0.0);
            }
            
            break;
            
        case PARENTPARTICLE_MODE_EXPLODE:
            // make 2500 _particles
            _particles.assign(1250, childParticle());
            
            // all _particles in a circle, give them a velocity
            for(unsigned int i = 0; i < _particles.size(); i++){
                _particles[i].reset();
                _particles[i].addNoise = true;
                _particles[i].setDisplayMode(PARTICLE_DISPLAY_MODE_ALWAYSON);
                
                // position
                float theta = 2*M_PI*(float)i/(float)_particles.size();
                ofPoint p = ofPoint(radius*sin(theta),radius*cos(theta));
                _particles[i].relPos = p;
                ofPoint v = p.normalize()*ofRandom(0.01,1.0)*velExplode;
                _particles[i].vel = v;
            }
            break;
    }
    
}

//--------------------------------------------------------------
void parentParticle::update()
{
    switch (_mode) {
        case PARENTPARTICLE_MODE_NORMAL:
            // position is updated externally

			if (ofGetElapsedTimef() - _timeTick > _tickPeriod) {
				_timeTick = ofGetElapsedTimef();
				_tailPoints.push_back(_pos);

				// add position to _tailPoints deque
				if (_tailPoints.size() > _numTailPoints)
					_tailPoints.pop_front();
			}
			

            break;
        case PARENTPARTICLE_MODE_CLOUD:
            // check if any of the points are not active anymore
            for(unsigned int i = 0; i < _particles.size(); i++){
                _particles[i].parentPos = _pos;
                _particles[i].update();

				// if particle is not active (and repeat is off, resample position and activate)
				if (!_particles[i].isActive) {
					unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
					std::default_random_engine generator(seed);
					std::normal_distribution<float> distribution(mean, stdev);
					// position (normal distribution)
					double x = distribution(generator);
					double y = distribution(generator);
					ofPoint p = ofPoint((float)x, (float)y);
					_particles[i].relPos = p;
					_particles[i].vel = ofPoint(0.0, 0.0);

					_particles[i].doDraw = true;
					_particles[i].isActive = true;
					_particles[i].startTime = ofGetElapsedTimef();
				}
            }
            
            // after some time, refresh (reseed)
            //if (ofGetElapsedTimef() - _startTime > 15.0) {
            //    resetPositions();
            //    _startTime = ofGetElapsedTimef();
            //}
            
            break;
        case PARENTPARTICLE_MODE_EXPLODE:
            for(unsigned int i = 0; i < _particles.size(); i++){
                //_particles[i].parentPos = pos;
                _particles[i].update();
            }
            break;
    }
}

//--------------------------------------------------------------
void parentParticle::draw()
{
	if (!doDraw)
		return;

    setFillMode(_fillMode);
    ofSetColor(_color);
    
    if (_mode == PARENTPARTICLE_MODE_NORMAL) {

		// check if cursor is out of the screen, cap at screen boundary
		if (_pos.x > ofGetWidth() / 2.0) _pos.x = ofGetWidth() / 2.0;
		if (_pos.x < -ofGetWidth() / 2.0) _pos.x = -ofGetWidth() / 2.0;
		if (_pos.y > ofGetHeight() / 2.0) _pos.y = ofGetHeight() / 2.0;
		if (_pos.y < -ofGetHeight() / 2.0) _pos.y = -ofGetHeight() / 2.0;


		switch (_shape) {
		case PARTICLESHAPE_CIRCLE:
			ofSetCircleResolution(200);
			ofSetLineWidth(6.0);
			ofDrawCircle(_pos.x, _pos.y, radius);
			/*
			if (_fillMode == OF_FILLED) {
				ofSetLineWidth(2.0);
				ofNoFill();
				ofSetColor(ofColor(7, 54, 66));
				ofDrawCircle(_pos, radius + 1.0);
				ofSetColor(_color);
			}*/
			break;
		case PARTICLESHAPE_LINE:
			ofSetLineWidth(8.0);
			ofDrawLine(_pos.x, -radius, _pos.x, -0.25*radius);
			ofDrawLine(_pos.x, -6.0, _pos.x, 6.0);
			ofDrawLine(_pos.x, 0.25*radius, _pos.x, radius);
			

			break;
		case PARTICLESHAPE_CROSS:
			ofSetLineWidth(4);
			//ofPushMatrix();
			//ofTranslate(_pos);
			ofDrawLine(_pos + ofPoint(0.0, radius), _pos - ofPoint(0.0, radius));
			ofDrawLine(_pos + ofPoint(radius, 0.0), _pos - ofPoint(radius, 0.0));
			//ofPopMatrix();
			break;

		case PARTICLESHAPE_CROSSHAIR:
			ofSetLineWidth(5.0);
			ofSetCircleResolution(120);
			ofDrawCircle(_pos, radius);
			ofDrawLine(_pos + ofPoint(0.0, -radius), _pos + ofPoint(0.0, -radius - 0.5*radius));
			ofDrawLine(_pos + ofPoint(0.0, radius), _pos + ofPoint(0.0, radius + 0.5*radius));
			ofDrawLine(_pos + ofPoint(-radius, 0.0), _pos + ofPoint(-radius - 0.5*radius, 0.0));
			ofDrawLine(_pos + ofPoint(radius, 0.0), _pos + ofPoint(radius + 0.5*radius, 0.0));
			ofFill();
			ofDrawCircle(_pos, 4.0);
			break;
		}
        

		// draw tail
		_line.clear();
		if (drawTail) {
			ofSetLineWidth(2);
			for (auto point : _tailPoints) {
				// don't add vertex if it's inside the circle of the target
				if (ofDist(_pos.x, _pos.y, point.x, point.y) > radius)
					_line.addVertex(point.x, point.y);
				_line.draw();
			}
		}
    }
    else if (_mode == PARENTPARTICLE_MODE_CLOUD || _mode == PARENTPARTICLE_MODE_EXPLODE) {
        for(unsigned int i = 0; i < _particles.size(); i++){
            _particles[i].draw();
        }
    }
}

//--------------------------------------------------------------
void parentParticle::resetPositions()
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    std::normal_distribution<float> distribution(mean,stdev);

    for(unsigned int i = 0; i < _particles.size(); i++){
        double x = distribution(generator);
        double y = distribution(generator);
        ofPoint p = ofPoint((float)x,(float)y);
        _particles[i].relPos = p;
    }
    
}

//--------------------------------------------------------------
void parentParticle::setMode(parentParticleMode newMode)
{
    _mode = newMode;
    reset();
}

//--------------------------------------------------------------
void parentParticle::setPosition(ofPoint p)
{
    _pos = p;
}

//--------------------------------------------------------------
ofPoint parentParticle::getPosition()
{
	return _pos;
}

//--------------------------------------------------------------
void parentParticle::setFillMode(ofFillFlag fillFlag)
{
    _fillMode = fillFlag;
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
    _color = c;
    for(unsigned int i = 0; i < _particles.size(); i++) {
        _particles[i].color = c;
    }
}
