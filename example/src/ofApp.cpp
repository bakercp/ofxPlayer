//
// Copyright (c) 2014 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:    MIT
//


#include "ofApp.h"


void ofApp::setup()
{
    ofSetFrameRate(30);
    ofEnableAlphaBlending();

//    ofx::SensorEventArgs::List events = ofx::SensorEventUtils::load(ofx::SensorEventArgs::TYPE_ROTATION_VECTOR, "Rotation.txt");
//
//    ofx::PlayableBufferHandle<std::vector<ofx::SensorEventArgs> > handle(events);


}


void ofApp::draw()
{
    ofBackgroundGradient(ofColor::white, ofColor::black);

}
