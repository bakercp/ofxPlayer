// =============================================================================
//
// Copyright (c) 2014-2016 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


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
