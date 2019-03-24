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

    std::vector<std::string> patterns = {
        ".*.jpg"
    };

    for (auto& pattern: patterns)
    {
        auto sequence = std::make_shared<ofxPlayer::ImageSequence>();

        if (ofx::Player::ImageSequence::fromDirectory("/Users/bakercp/Desktop/of_v0.10.1_osx_release/apps/Drawings/ARPlayer/bin/data/videos/dress/45904_complete",
                                                      *sequence,
                                                      pattern))
        {
            auto player = ofx::Player::ImageSequencePlayer();
            player.load(sequence);
            player.play();
            //player.setLoopType(OF_LOOP_PALINDROME);
            players.push_back(player);
        }
    }

}


void ofApp::update()
{
    for (auto& player: players)
    {
        float speed = ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, 10);

        player.setSpeed(speed);
        player.update();
    }
}


void ofApp::draw()
{
    ofBackgroundGradient(ofColor::white, ofColor::black);

    int x = 0;
    int y = 0;

    for (auto& player: players)
    {
        player.getTexture().draw(x, y);
        x += 100;
        y += 100;
    }

}
