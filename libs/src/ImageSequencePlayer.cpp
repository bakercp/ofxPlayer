//
// Copyright (c) 2013 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier: MIT
//


#include "ofx/Player/ImageSequencePlayer.h"


namespace ofx {
namespace Player {


const ofPixels ImageSequencePlayer::EMPTY_PIXELS;
const ofTexture ImageSequencePlayer::EMPTY_TEXTURE;


ImageSequencePlayer::ImageSequencePlayer()
{
}


ImageSequencePlayer::ImageSequencePlayer(const std::string& path)
{
    load(path);
}


ImageSequencePlayer::ImageSequencePlayer(std::shared_ptr<ImageSequence> data)
{
    load(data);
}


ImageSequencePlayer::~ImageSequencePlayer()
{
}


bool ImageSequencePlayer::load(const std::string& path)
{
    auto data = std::make_shared<ImageSequence>();
    if (ImageSequence::fromDirectory(*data, path))
    {
        _data = data;
        return true;
    }

    return false;
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

    return 0;
}


float ImageSequencePlayer::getHeight() const
{
    if (isLoaded())
    {
        return _data->getHeight();
    }

    return 0;
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

    return EMPTY_PIXELS;
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

    return EMPTY_TEXTURE;
}


const BaseTimeIndexed* ImageSequencePlayer::indexedData() const
{
    return _data.get();
}


} } // namespace ofx::Player
