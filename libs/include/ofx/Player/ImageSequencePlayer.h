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

    ImageSequencePlayer(const std::string& path);

    ImageSequencePlayer(std::shared_ptr<ImageSequence> data);

    /// \brief Destroy the ImageSequencePlayer.
    virtual ~ImageSequencePlayer();

    bool load(const std::string& path);

    bool load(std::shared_ptr<ImageSequence> data);

    void close() override;

    bool isFrameNew() const;

    float getWidth() const;

    float getHeight() const;

    const ofPixels& getPixels() const;

    const ofTexture& getTexture() const;

    static const ofPixels EMPTY_PIXELS;
    static const ofTexture EMPTY_TEXTURE;

    const BaseTimeIndexed* indexedData() const override;

protected:
    std::shared_ptr<ImageSequence> _data = nullptr;

//    bool _isUsingTexture = true;
//
//    ofPixels* _pixels = nullptr;
//    ofTexture* _texture = nullptr;

};




















} } // namespace ofx::Player
