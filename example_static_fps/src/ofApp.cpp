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
//        ".*_full.jpg",
        ".*_net.png",
//        ".*_thumb.jpg"
    };

    for (auto& pattern : patterns)
    {
        auto sequence = std::make_shared<ofx::ImageSequence>();

        if (ofx::ImageSequence::fromDirectory("plc_seq", *sequence, pattern))
        {
            auto player = std::make_shared<ofx::ImageSequencePlayer>();

            player->load(sequence);
            player->play();
            player->setLoopType(OF_LOOP_PALINDROME);

            players.push_back(player);
        }
    }

}


void ofApp::update()
{
    for (auto& player: players)
    {
        float speed = 100;//= ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, 10);

        player->setSpeed(speed);
        player->update();
    }
}


void ofApp::draw()
{
    ofBackgroundGradient(ofColor::white, ofColor::black);

    int x = 0;
    int y = 0;

    for (auto& player: players)
    {
        ofPixels p = player->getPixels();

//        ofPixels stable = stabilizer.stabilize(p);

        ofTexture tex;
        tex.loadData(p);
        tex.draw(x, y);

        //        player->getTexture().draw(x, y);

        x += 100;
        y += 100;
    }

}
