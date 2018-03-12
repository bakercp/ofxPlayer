//
// Copyright (c) 2013 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:    MIT
//


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
