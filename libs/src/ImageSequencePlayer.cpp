//
// Copyright (c) 2013 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:    MIT
//


#include "ofx/Player/ImageSequencePlayer.h"


namespace ofx {
namespace Player {


const ofPixels ImageSequencePlayer::EMPTY_PIXELS;
const ofTexture ImageSequencePlayer::EMPTY_TEXTURE;


ImageSequencePlayer::ImageSequencePlayer(): ImageSequencePlayer(nullptr)
{
}


ImageSequencePlayer::ImageSequencePlayer(std::shared_ptr<ImageSequence> data): _data(data)
{
}


ImageSequencePlayer::~ImageSequencePlayer()
{
}


bool ImageSequencePlayer::load(std::shared_ptr<ImageSequence> data)
{
    _data = data;
    return true;
}


void ImageSequencePlayer::close()
{
    _data.reset();
}


float ImageSequencePlayer::getWidth() const
{
    if (isLoaded())
    {
        return _data->getWidth();
    }
    else
    {
        return 0;
    }
}


float ImageSequencePlayer::getHeight() const
{
    if (isLoaded())
    {
        return _data->getHeight();
    }
    else
    {
        return 0;
    }
}


bool ImageSequencePlayer::isFrameNew() const
{
    return isFrameIndexNew();
}


const ofPixels& ImageSequencePlayer::getPixels() const
{
    if (isLoaded())
    {
        try
        {
            return _data->getPixels(getFrameIndex());
        }
        catch (const std::exception& exc)
        {
            ofLogError("ImageSequencePlayer::getPixels") << exc.what();
            return EMPTY_PIXELS;
        }
    }
    else
    {
        return EMPTY_PIXELS;
    }
}


const ofTexture& ImageSequencePlayer::getTexture() const
{
    if (isLoaded())
    {
        try
        {
            return _data->getTexture(getFrameIndex());
        }
        catch (const std::exception& exc)
        {
            ofLogError("ImageSequencePlayer::getTexture") << exc.what();
            return EMPTY_TEXTURE;
        }
    }
    else
    {
        return EMPTY_TEXTURE;
    }
}


const BaseTimeIndexed* ImageSequencePlayer::indexedData() const
{
    return _data.get();
}


} } // namespace ofx::Player
