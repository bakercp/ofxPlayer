//
// Copyright (c) 2013 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:    MIT
//


#include "ofx/Player/BasePlayerTypes.h"
#include "ofx/Player/PlayerUtils.h"


namespace ofx {
namespace Player {


BaseTimeIndexed::~BaseTimeIndexed()
{
}


double BaseTimeIndexed::startTime() const
{
    return size() > 0 ? timeForIndex(0) : -1;
}


double BaseTimeIndexed::endTime() const
{
    return size() > 0 ? timeForIndex(size() - 1) : -1;
}


double BaseTimeIndexed::duration() const
{
    return size() > 0 ? (endTime() - startTime()) : 0;
}


std::size_t BaseTimeIndexed::indexForPosition(double position,
                                              bool increasing,
                                              std::size_t indexHint) const
{
    return indexForTime(timeForPosition(position), increasing, indexHint);
}


double BaseTimeIndexed::timeForPosition(double position) const
{
    return startTime() + (position / duration());
}


double BaseTimeIndexed::positionForIndex(std::size_t index) const
{
    return (timeForIndex(index) - startTime()) / duration();
}


std::size_t BaseTimeIndexed::indexForTime(double time,
                                          bool increasing,
                                          std::size_t indexHint) const
{
    std::size_t index = 0;

    if (time >= endTime())
    {
        index = size() - 1;
    }
    else if (time <= startTime())
    {
        index = 0;
    }
    else
    {
        std::size_t indexMin = 0;
        std::size_t indexMax = size();

        // Going forward?
        if (increasing)
        {
            if (timeForIndex(indexHint) <= time)
            {
                indexMin = indexHint;
                indexMax = size();
            }
            else
            {
                indexMin = 0;
                indexMax = indexHint;
            }

            for (std::size_t i = indexMin; i < indexMax; ++i)
            {
                if (timeForIndex(i) > time)
                {
                    break;
                }
                else
                {
                    index = i;
                }
            }
        }
        else
        {
            if (timeForIndex(indexHint) >= time)
            {
                indexMin = indexHint;
                indexMax = 0;
            }
            else
            {
                indexMin = size() - 1;
                indexMax = indexHint;
            }

            for (std::size_t i = indexMin; i != static_cast<std::size_t>(indexMax - 1); --i)
            {
                if (timeForIndex(i) < time)
                {
                    break;
                }
                else
                {
                    index = i;
                }
            }
        }
    }

    return index;

//    std::function<std::size_t (std::size_t, std::size_t, double, bool)> search;
//    search = [this, &search](std::size_t start,
//                             std::size_t end,
//                             double timestamp,
//                             bool searchForward)->std::size_t
//    {
//        // Find the middle element of the vector and use that for splitting
//        // the array into two pieces.
//        const int middle = start + ((end - start) / 2);
//
//        if (timeAtIndex(middle) == timestamp)
//        {
//            return middle;
//        }
//        else if (timeAtIndex(middle) > timestamp)
//        {
//            return search(start, middle - 1, timestamp);
//        }
//
//        return search(middle + 1, end, timestamp);
//    };

}


double BaseTimeIndexed::positionForTime(double time, bool clamp) const
{
    if (clamp)
    {
        return std::max(0.0, std::min((time - startTime()) / duration(), 1.0));
    }
    else
    {
        return (time - startTime()) / duration();
    }
}


double DefaultBufferAdapter::timestamp(const AbstractTimestamped& input)
{
    return input.timestamp();
}


BasePlayer::~BasePlayer()
{
}


void BasePlayer::update()
{
    // If it's not loaded, no data, or not playing there is nothing to do.
    if (!isLoaded() || indexedData()->size() == 0 || !isPlaying())
    {
        return;
    }

    auto now = std::chrono::high_resolution_clock::now();

    // Begin calculating frame updates.
    if (_isFirstUpdate)
    {
        _firstUpdateTime = now;
        _lastUpdateTime = now;

        if (_time < 0)
        {
            _time = startTime();
        }

        if (_lastTime < 0)
        {
            _lastTime = endTime();
        }

        _lastFrameIndex = std::numeric_limits<std::size_t>::max();

        _isFirstUpdate = false;
    }

    // Calculate the elapsed real-time.
    double elapsedRealTime = std::chrono::duration_cast<micros_duration>(now - _lastUpdateTime).count();

    _lastUpdateTime = now;

    // Calculate the elapsed time. Can be negative.
    double elapsedTime = _speed * elapsedRealTime;

    if (!_playingForward)
    {
        elapsedTime *= -1.0;
    }

    // Are we increasing?
    bool increasing = (elapsedTime > 0);

    // Set the uncorrected time.
    _time += elapsedTime;

    double loopStartTime = getLoopStartTime();
    double loopEndTime = getLoopEndTime();

    if (loopStartTime < 0)
    {
        loopStartTime = startTime();
    }

    if (loopEndTime < 0)
    {
        loopEndTime = endTime();
    }

    double loopDuration = loopEndTime - loopStartTime;

    // TODO: determine if there are any frames available during our loop points ...

    if (_loopType == OF_LOOP_NORMAL)
    {
        auto wrap = [](double time, double fromTime, double toTime)
        {
            if (fromTime > toTime)
            {
                std::swap(fromTime, toTime);
            }

            double cycle = toTime - fromTime;

            if (ofxIsFloatEqual(cycle, 0.0))
            {
                return toTime;
            }

            return time - cycle * std::floor((time - fromTime) / cycle);
        };

        // Add the elapsed time and wrap it.
        _time = wrap(_time + elapsedTime, loopStartTime, loopEndTime);

    }
    else if (_loopType == OF_LOOP_PALINDROME)
    {
        // Check if we are outside of the range and palindrome wrap if needed.
        if (_time < loopStartTime || _time > loopEndTime)
        {
            double overshoot = 0;

            if (_time > loopEndTime)
            {
                overshoot = (_time - loopEndTime);
            }
            else
            {
                overshoot = (loopStartTime - _time);
            }

            // Reduce overshoot by folding.
            while (overshoot > loopDuration)
            {
                overshoot -= loopDuration;
                _playingForward = !_playingForward;
            }

            if (_playingForward)
            {
                _time = (loopEndTime - overshoot);
            }
            else
            {
                _time = (loopStartTime + overshoot);
            }

            _playingForward = !_playingForward;
        }
    }
    else
    {
        // Add the elapsed time and clamp it.
        _time = std::max(loopStartTime, std::min(_time + elapsedTime, loopEndTime));
    }

    _frameIndex = indexForTime(_time, increasing, _lastFrameIndex);
    _isFrameIndexNew = (_lastFrameIndex != _frameIndex);
    _lastFrameIndex = _frameIndex;
}


bool BasePlayer::isFrameIndexNew() const
{
    return _isFrameIndexNew;
}


double BasePlayer::getSpeed() const
{
    return _speed;
}


void BasePlayer::setSpeed(double speed)
{
    _speed = speed;
}


double BasePlayer::getPosition() const
{
    return positionForTime(getTime(), false);
}


void BasePlayer::setPosition(double position)
{
    setTime(timeForPosition(position));
}


double BasePlayer::getTime() const
{
    return _time;
}


void BasePlayer::setTime(double time)
{
    _time = std::max(startTime(), std::min(time, endTime()));
}


std::size_t BasePlayer::getFrameIndex() const
{
    return _frameIndex;
}


void BasePlayer::setFrameIndex(std::size_t index)
{
    setTime(timeForIndex(index));
}


void BasePlayer::setLoopStartPosition(double position)
{
    setLoopStartTime(timeForPosition(position));
}


void BasePlayer::setLoopEndPosition(double position)
{
    setLoopStartTime(timeForPosition(position));
}


double BasePlayer::getLoopStartPosition() const
{
    return positionForTime(getLoopStartTime(), false);
}


double BasePlayer::getLoopEndPosition() const
{
    return positionForTime(getLoopEndTime(), false);
}


void BasePlayer::setLoopStartTime(double time)
{
    _loopStartTime = std::max(startTime(), std::min(time, endTime()));

    if (_loopStartTime > _loopEndTime)
    {
        std::swap(_loopStartTime, _loopEndTime);
    }
}


void BasePlayer::setLoopEndTime(double time)
{
    _loopEndTime = std::max(startTime(), std::min(time, endTime()));

    if (_loopStartTime > _loopEndTime)
    {
        std::swap(_loopStartTime, _loopEndTime);
    }
}


double BasePlayer::getLoopStartTime() const
{
    return _loopStartTime;
}


double BasePlayer::getLoopEndTime() const
{
    return _loopEndTime;
}


void BasePlayer::setLoopStartFrameIndex(std::size_t index)
{
    setLoopStartTime(timeForIndex(index));
}


void BasePlayer::setLoopEndFrameIndex(std::size_t index)
{
    setLoopEndTime(timeForIndex(index));
}


std::size_t BasePlayer::getLoopStartFrameIndex() const
{
    return indexForTime(getLoopStartTime(), 1, 0);
}


std::size_t BasePlayer::getLoopEndFrameIndex() const
{
    return indexForTime(getLoopEndTime(), -1, size());
}


void BasePlayer::clearLoopPoints()
{
    _loopStartTime = -1;
    _loopEndTime = -1;
}


ofLoopType BasePlayer::getLoopType() const
{
    return _loopType;
}


void BasePlayer::setLoopType(ofLoopType loopType)
{
    switch (loopType)
    {
        case OF_LOOP_PALINDROME:
            _playingForward = (getSpeed() > 0); // reset
            break;
        case OF_LOOP_NORMAL:
        case OF_LOOP_NONE:
        default:
            break;
    }

    _loopType = loopType;

}


void BasePlayer::setPaused(bool paused)
{
    return _paused;
}


void BasePlayer::play()
{
    _playing = true;
}


void BasePlayer::stop()
{
    _playing = false;
}


bool BasePlayer::isPaused() const
{
    return _paused;
}


bool BasePlayer::isLoaded() const
{
    return indexedData() != nullptr;
}


bool BasePlayer::isPlaying() const
{
    return _playing;
}


double BasePlayer::startTime() const
{
    if (isLoaded())
    {
        return indexedData()->startTime();
    }
    else
    {
        ofLogError("BasePlayer::startTime") << "The data is not loaded.";
        return 0;
    }
}


double BasePlayer::endTime() const
{
    if (isLoaded())
    {
        return indexedData()->endTime();
    }
    else
    {
        ofLogError("BasePlayer::endTime") << "The data is not loaded.";
        return 0;
    }
}

double BasePlayer::duration() const
{
    if (isLoaded())
    {
        return indexedData()->duration();
    }
    else
    {
        ofLogError("BasePlayer::duration") << "The data is not loaded.";
        return 0;
    }
}

std::size_t BasePlayer::indexForPosition(double position,
                                         bool increasing,
                                         std::size_t indexHint) const
{
    if (isLoaded())
    {
        return indexedData()->indexForPosition(position, increasing, indexHint);
    }
    else
    {
        ofLogError("BasePlayer::indexForPosition") << "The data is not loaded.";
        return 0;
    }
}


double BasePlayer::timeForPosition(double position) const
{
    if (isLoaded())
    {
        return indexedData()->timeForPosition(position);
    }
    else
    {
        ofLogError("BasePlayer::timeForPosition") << "The data is not loaded.";
        return 0;
    }
}


double BasePlayer::positionForIndex(std::size_t index) const
{
    if (isLoaded())
    {
        return indexedData()->positionForIndex(index);
    }
    else
    {
        ofLogError("BasePlayer::positionForIndex") << "The data is not loaded.";
        return 0;
    }
}


std::size_t BasePlayer::indexForTime(double time,
                                     bool increasing,
                                     std::size_t indexHint) const
{
    if (isLoaded())
    {
        return indexedData()->indexForTime(time, increasing, indexHint);
    }
    else
    {
        ofLogError("BasePlayer::indexForPosition") << "The data is not loaded.";
        return 0;
    }
}

double BasePlayer::timeForIndex(std::size_t index) const
{
    if (isLoaded())
    {
        return indexedData()->timeForIndex(index);
    }
    else
    {
        ofLogError("BasePlayer::timeAtIndex") << "The data is not loaded.";
        return 0;
    }
}


double BasePlayer::positionForTime(double time, bool clamp) const
{
    if (isLoaded())
    {
        return indexedData()->positionForTime(time, clamp);
    }
    else
    {
        ofLogError("BasePlayer::positionForTime") << "The data is not loaded.";
        return 0;
    }
}


std::size_t BasePlayer::size() const
{
    if (isLoaded())
    {
        return indexedData()->size();
    }
    else
    {
        ofLogError("BasePlayer::size") << "The data is not loaded.";
        return 0;
    }
}


} } // namespace ofx::Player
