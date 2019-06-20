#include "parentParticle.h"

//--------------------------------------------------------------
parentParticle::parentParticle()
{
	// toggle for first update run
	_firstUpdate = true;
}

//--------------------------------------------------------------
void parentParticle::reset()
{
    // first update tick
	_firstUpdate = true;

	// reset time
	_startTime = ofGetElapsedTimef();

    // clear _particles vector
    _particles.clear();

	// clear tail point vector
	_tailPoints.clear();
    
    switch (_mode) {
        case PARENTPARTICLE_MODE_NORMAL:
            // do nothing
            break;
        case PARENTPARTICLE_MODE_CLOUD: case PARENTPARTICLE_MODE_MOVINGCLOUD:

            // give all the _particles a relative position (random)
            _particles.assign(numberParticles, childParticle());
			

            for(unsigned int i = 0; i < _particles.size(); i++){
                _particles[i].reset();
				_particles[i].color = _color;
                _particles[i].addNoise = false;
				_particles[i].setDisplayMode(PARTICLE_DISPLAY_MODE_ALWAYSON);
                _particles[i].setRepetitionMode(PARTICLE_REPETITION_MODE_ONCE);
                _particles[i].setPeriod(_periodChildParticle);
				_particles[i].radius = particleRadius;
				_particles[i].setFillMode(_fillMode);
				_particles[i].startTime = ((float)i / (float)numberParticles)*_periodChildParticle;
				_particles[i].resetTime = ((float)i / (float)numberParticles)*_periodChildParticle;

				//cout << "particle[" << i << "]: " << _particles[i].startTime << endl;
                
				// resample position and velocity
				ofPoint p, v;
				resamplePosVel(p, v);

                _particles[i].relPos = p;
                _particles[i].vel = v;
            }
            
            break;
            
        case PARENTPARTICLE_MODE_EXPLODE:
            // make 2500 _particles
            _particles.assign(250, childParticle());
            
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

	double t = (double)ofGetElapsedTimeMillis() / 1000.0 - _startTime;
	//double t = ofGetElapsedTimef() - _startTime;

	//cout << ofGetElapsedTimef() << endl;
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
					ofPoint p, v;
					resamplePosVel(p, v);
					_particles[i].relPos = p;
					_particles[i].vel = ofPoint(0.0, 0.0);

					_particles[i].doDraw = true;
					_particles[i].isActive = true;
					_particles[i].startTime = t;
				}
            }
            
            // after some time, refresh (reseed)
            //if (ofGetElapsedTimef() - _startTime > 15.0) {
            //    resetPositions();
            //    _startTime = ofGetElapsedTimef();
            //}
            
            break;

		case PARENTPARTICLE_MODE_MOVINGCLOUD:
			// check if any of the points are not active anymore
			for (unsigned int i = 0; i < _particles.size(); i++) {
				_particles[i].parentPos = _pos;
				_particles[i].update();

				// determine when to reset the particles
				if (t >  _particles[i].resetTime) {

					// resample
					ofPoint p, v;
					resamplePosVel(p, v);

					_particles[i].relPos = p;
					_particles[i].absPos0 = p + _pos;
					_particles[i].useAbsPos0 = _useAbsPosChild;

					_particles[i].vel = v;

					_particles[i].resetTime = t + _periodChildParticle;

				}

				// check if any particles are outside of the workspace
				//if (_particles[i].getAbsPos().lengthSquared() > pow(workspaceRadius, 2.0)) { // used squared because taking a root takes up a lot of PC resource
				//_particles[i].relPos =  workspaceRadius*_particles[i].getAbsPos().normalize() - _pos;
				//_particles[i].vel = -_particles[i].vel;
				//}

			}
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
			ofFill();
			ofDrawCircle(_pos.x, _pos.y, radius );
			/*
			if (_fillMode == OF_FILLED) {
				ofSetLineWidth(2.0);
				ofNoFill();
				ofSetColor(ofColor(7, 54, 66));
				ofDrawCircle(_pos, radius + 1.0);
				ofSetColor(_color);
			}*/
			break;
		case PARTICLESHAPE_CIRCLE_OPEN:
			ofSetCircleResolution(200);
			ofSetLineWidth(6.0);
			ofNoFill();
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
			
			//stel je wil iets transparant maken
			ofEnableAlphaBlending();
			float alpha = 0.7 * 255; 
			ofColor mycolor = ofColor(255,255, 255,alpha);
			ofSetColor(mycolor);

			ofSetCircleResolution(120);
			// outer circle + streepjes
			ofSetLineWidth(5.0);  
			ofDrawCircle(_pos, radius);
			ofDrawLine(_pos + ofPoint(0.0, -radius), _pos + ofPoint(0.0, -radius - 0.5*radius));
			ofDrawLine(_pos + ofPoint(0.0, radius), _pos + ofPoint(0.0, radius + 0.5*radius));
			ofDrawLine(_pos + ofPoint(-radius, 0.0), _pos + ofPoint(-radius - 0.5*radius, 0.0));
			ofDrawLine(_pos + ofPoint(radius, 0.0), _pos + ofPoint(radius + 0.5*radius, 0.0));
			
			ofDisableAlphaBlending();


			// inner circle/dot
			ofFill();
			ofDrawCircle(_pos, 5.0);
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
    else if (_mode == PARENTPARTICLE_MODE_CLOUD || _mode == PARENTPARTICLE_MODE_EXPLODE || _mode == PARENTPARTICLE_MODE_MOVINGCLOUD) {
        for(unsigned int i = 0; i < _particles.size(); i++){
            _particles[i].draw();
        }

		// draw actual target for debug
		//ofSetColor(ofColor::burlyWood);
		//ofFill();
		//ofDrawCircle(_pos, 8.0);
		//setFillMode(_fillMode);
		
    }
}

//--------------------------------------------------------------
void parentParticle::resetPositions()
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    std::normal_distribution<float> distribution(_mean_pos,_sd_pos);

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

	for (unsigned int i = 0; i < _particles.size(); i++) {
		_particles[i].setFillMode(_fillMode);
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

//--------------------------------------------------------------
void parentParticle::resamplePosVel(ofPoint &p, ofPoint &v) {
	// position 
	/*
	unsigned seed_pos = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator_pos(seed_pos);
	std::normal_distribution<float> distribution_pos(_mean_pos, _sd_pos);
	// position (normal distribution)
	double x = distribution_pos(generator_pos);
	double y = distribution_pos(generator_pos);
	p = ofPoint((float)x, (float)y);

	// velocity
	unsigned seed_vel = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator_vel(seed_vel);
	std::normal_distribution<float> distribution_vel(_mean_vel, _sd_vel);
	// position (normal distribution)
	double vx = distribution_vel(generator_vel);
	double vy = distribution_vel(generator_vel);
	v = ofPoint((float)vx, (float)vy);
	*/

	double x = _sd_pos*randomGaussian(_mean_pos,1.0)*dots_per_m;
	double y = _sd_pos * randomGaussian(_mean_pos, 1.0)*dots_per_m;
	p = ofPoint(x, y);

	double vx = _sd_vel * randomGaussian(_mean_vel, 1.0)*dots_per_m;
	double vy = _sd_vel * randomGaussian(_mean_vel, 1.0)*dots_per_m;
	v = ofPoint(vx, vy);
}

//--------------------------------------------------------------
float parentParticle::randomGaussian(float mean, float stddev) {
	// normal dist rand var sample
	std::normal_distribution<float>::param_type _param(mean, stddev);
	distribution.param(_param);
	return distribution(generator);
}