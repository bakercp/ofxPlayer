// =============================================================================
//
// Copyright (c) 2013-2016 Christopher Baker <http://christopherbaker.net>
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


#pragma once


#include "ofTexture.h"
#include "ofPixels.h"
#include "ofx/Player/BasePlayerTypes.h"
#include "ofx/Player/ImageSequence.h"


namespace ofx {
namespace Player {


/// \brief An image sequence.
class ImageSequencePlayer: public BasePlayer
{
public:
    // \brief Create an ImageSequencePlayer.
    ImageSequencePlayer();

    ImageSequencePlayer(std::shared_ptr<ImageSequence> data);

    /// \brief Destroy the ImageSequencePlayer.
    virtual ~ImageSequencePlayer();

    bool load(std::shared_ptr<ImageSequence> data);
    
    void close();

    bool isFrameNew() const;

    float getWidth() const;

    float getHeight() const;

    const ofPixels& getPixels() const;

    const ofTexture& getTexture() const;

    static const ofPixels EMPTY_PIXELS;
    static const ofTexture EMPTY_TEXTURE;
    
//protected:
    const BaseTimeIndexed* indexedData() const override;

    std::shared_ptr<ImageSequence> _data;

//    bool _isUsingTexture = true;
//
//    ofPixels* _pixels = nullptr;
//    ofTexture* _texture = nullptr;

};




















} } // namespace ofx::Player
