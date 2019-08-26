//
// Copyright (c) 2013 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier: MIT
//


#include "ofx/Player/BasePlayerTypes.h"
#include "ofx/Player/PlayerUtils.h"
#include "ofLog.h"
#include "ofMath.h"


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
                double timeAtI = timeForIndex(i);

                if (timeAtI > time)
                    break;
                else
                    index = i;
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


double BaseTimeIndexed::interpolatedIndexForTime(double time,
                                                 bool increasing,
                                                 std::size_t indexHint) const
{
    std::size_t index = indexForTime(time, increasing, indexHint);
    std::size_t nextIndex = index;

    if (increasing)
    {
        if (nextIndex < size() - 1)
            nextIndex++;
    }
    else
    {
        if (nextIndex > 0)
            nextIndex--;
    }

    double indexTime = timeForIndex(index);
    double nextIndexTime = timeForIndex(nextIndex);

//    if (index == nextIndexTime)
//        std::cout << "--------------------------------------" << std::endl;

    return ofMap(time, indexTime, nextIndexTime, index, nextIndex);

//    ofMap

//    if (fabs(inputMin - inputMax) < FLT_EPSILON){
//        return outputMin;
//    } else {
//        float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
//
//        if( clamp ){
//            if(outputMax < outputMin){
//                if( outVal < outputMax )outVal = outputMax;
//                else if( outVal > outputMin )outVal = outputMin;
//            }else{
//                if( outVal > outputMax )outVal = outputMax;
//                else if( outVal < outputMin )outVal = outputMin;
//            }
//        }
//        return outVal;
//    }
//
//
//    double fractionalTime = (time - indexTime) / std::abs(indexTime - nextIndexTime);
//
//    return index + fractionalTime;
}


double BaseTimeIndexed::positionForTime(double time, bool clamp) const
{
    if (clamp)
    {
        return std::max(0.0, std::min((time - startTime()) / duration(), 1.0));
    }

    return (time - startTime()) / duration();
}


double DefaultBufferAdapter::timestamp(const AbstractTimestamped& input)
{
    return input.timestamp();
}



BaseTimeIndexedInfo::BaseTimeIndexedInfo()
{
}


BaseTimeIndexedInfo::BaseTimeIndexedInfo(const BaseTimeIndexed& stats)
{
    load(stats);
}


bool BaseTimeIndexedInfo::load(const BaseTimeIndexed& stats)
{
    _isLoaded = false;
    _timestampMin = std::numeric_limits<double>::max();
    _timestampMax = std::numeric_limits<double>::lowest();
    _duration = 0;
    _samplingFrequencyMean = 0;
    _samplingFrequencyMin = 0;
    _samplingFrequencyMax = 0;
    _samplingFrequencyStdDev = 0;
    _isMonotonic = true;

    double samplingIntervalSum = 0;
    double samplingIntervalMin = std::numeric_limits<double>::max();
    double samplingIntervalMax = std::numeric_limits<double>::lowest();
    double samplingIntervalMean = 0;
    double samplingIntervalStd = 0;

    if (stats.size() > 0)
    {
        double lastTime = std::numeric_limits<double>::max();

        std::vector<double> samplingIntervals(stats.size() - 1, 0);

        for (std::size_t i = 0; i < stats.size(); ++i)
        {
            double time = stats.timeForIndex(i);

            _timestampMin = std::min(_timestampMin, time);
            _timestampMax = std::max(_timestampMax, time);

            if (i != 0)
            {
                if (time < lastTime)
                {
                    _isMonotonic = false;
                }

                double samplingInterval = time - lastTime;

//                std::cout << (time / 1000000.) << std::endl;

                samplingIntervalMin = std::min(samplingIntervalMin, samplingInterval);
                samplingIntervalMax = std::max(samplingIntervalMax, samplingInterval);

                samplingIntervalSum += samplingInterval;
                samplingIntervals[i - 1] = samplingInterval;
            }

            lastTime = time;
        }

        samplingIntervalMean =  samplingIntervalSum / samplingIntervals.size();

        for (auto interval: samplingIntervals)
        {
            samplingIntervalStd += std::pow(interval - samplingIntervalMean, 2);
        }

        samplingIntervalStd = std::sqrt(samplingIntervalStd / samplingIntervals.size());


        _duration = _timestampMax - _timestampMin;

        _samplingFrequencyMean = 1000000 / samplingIntervalMean;
        _samplingFrequencyMin = 1000000 / samplingIntervalMin;
        _samplingFrequencyMax = 1000000 / samplingIntervalMax;
        _samplingFrequencyStdDev = 1000000 / samplingIntervalStd;

    }

    _isLoaded = true;
    return isLoaded();
}


double BaseTimeIndexedInfo::timestampMin() const
{
    return _timestampMin;
}


double BaseTimeIndexedInfo::timestampMax() const
{
    return _timestampMin;
}


double BaseTimeIndexedInfo::duration() const
{
    return _duration;
}

double BaseTimeIndexedInfo::samplingFreqencyMean() const
{
    return _samplingFrequencyMean;
}


double BaseTimeIndexedInfo::samplingFrequencyMin() const
{
    return _samplingFrequencyMin;
}

double BaseTimeIndexedInfo::samplingFrequencyMax() const
{
    return _samplingFrequencyMax;
}

double BaseTimeIndexedInfo::samplingFrequencyStdDev() const
{
    return _samplingFrequencyStdDev;
}


bool BaseTimeIndexedInfo::isMonotonic() const
{
    return _isMonotonic;
}


bool BaseTimeIndexedInfo::isLoaded() const
{
    return _isLoaded;
}


BasePlayer::~BasePlayer()
{
}


void BasePlayer::update()
{
    // If it's not loaded, no data, or not playing there is nothing to do.
    if (!isLoaded() || indexedData()->size() == 0 || !isPlaying())
    {
        _isFrameIndexNew = false;
        return;
    }

    // Determine the current update time.
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

    // We use a local copy here.
    double loopStartTime = getLoopStartTime();
    double loopEndTime = getLoopEndTime();

    if (!_loopSet)
    {
        loopStartTime = startTime();
        loopEndTime = endTime();
    }
    else if (loopEndTime < loopStartTime)
    {
        std::swap(loopStartTime, loopEndTime);
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

            if (ofIsFloatEqual(cycle, 0.0))
            {
                return toTime;
            }

            return time - cycle * std::floor((time - fromTime) / cycle);
        };

        // Add the elapsed time and wrap it.
        _time = wrap(_time /* + elapsedTime */, loopStartTime, loopEndTime);

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
        _time = std::max(loopStartTime, std::min(_time /* + elapsedTime */, loopEndTime));
    }

    _interpolatedFrameIndex = interpolatedIndexForTime(_time, increasing, _lastFrameIndex);
    _frameIndex = indexForTime(_time, increasing, _lastFrameIndex);

    _isFrameIndexNew = (_lastFrameIndex != _frameIndex);
    _lastFrameIndex = _frameIndex;

    _nextFrameIndex = _frameIndex;
    _lastUpdateTime = now;

    // TODO - this will wrap if the loop mode
    if (increasing)
    {
        if (_nextFrameIndex < size() - 1)
            _nextFrameIndex++;
    }
    else
    {
        if (_nextFrameIndex > 0)
            _nextFrameIndex--;
    }
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


double BasePlayer::interpolatedFrameIndex() const
{
    return _interpolatedFrameIndex;
}


std::size_t BasePlayer::nextFrameIndex() const
{
    return _nextFrameIndex;
}


std::size_t BasePlayer::lastFrameIndex() const
{
    return _lastFrameIndex;
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
    _loopSet = true;
    _loopStartTime = time;
}


void BasePlayer::setLoopEndTime(double time)
{
    _loopSet = true;
    _loopEndTime = time;
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
    return indexForTime(getLoopStartTime(), true, 0);
}


std::size_t BasePlayer::getLoopEndFrameIndex() const
{
    return indexForTime(getLoopEndTime(), false, size());
}


void BasePlayer::clearLoopPoints()
{
    _loopSet = false;
    _loopStartTime = 0.0;
    _loopEndTime = 0.0;
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
            break;
    }

    _loopType = loopType;
}


void BasePlayer::setPaused(bool paused)
{
    _paused = paused;
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

    ofLogError("BasePlayer::startTime") << "The data is not loaded.";
    return 0;
}


double BasePlayer::endTime() const
{
    if (isLoaded())
    {
        return indexedData()->endTime();
    }

    ofLogError("BasePlayer::endTime") << "The data is not loaded.";
    return 0;
}

double BasePlayer::duration() const
{
    if (isLoaded())
    {
        return indexedData()->duration();
    }

    ofLogError("BasePlayer::duration") << "The data is not loaded.";
    return 0;
}


std::size_t BasePlayer::indexForPosition(double position,
                                         bool increasing,
                                         std::size_t indexHint) const
{
    if (isLoaded())
    {
        return indexedData()->indexForPosition(position, increasing, indexHint);
    }

    ofLogError("BasePlayer::indexForPosition") << "The data is not loaded.";
    return 0;
}


double BasePlayer::timeForPosition(double position) const
{
    if (isLoaded())
    {
        return indexedData()->timeForPosition(position);
    }

    ofLogError("BasePlayer::timeForPosition") << "The data is not loaded.";
    return 0;
}


double BasePlayer::positionForIndex(std::size_t index) const
{
    if (isLoaded())
    {
        return indexedData()->positionForIndex(index);
    }

    ofLogError("BasePlayer::positionForIndex") << "The data is not loaded.";
    return 0;
}


std::size_t BasePlayer::indexForTime(double time,
                                     bool increasing,
                                     std::size_t indexHint) const
{
    if (isLoaded())
    {
        return indexedData()->indexForTime(time, increasing, indexHint);
    }

    ofLogError("BasePlayer::indexForPosition") << "The data is not loaded.";
    return 0;
}


double BasePlayer::interpolatedIndexForTime(double time,
                                            bool increasing,
                                            std::size_t indexHint) const
{
    if (isLoaded())
    {
        return indexedData()->interpolatedIndexForTime(time, increasing, indexHint);
    }

    ofLogError("BasePlayer::interpolatedIndexForTime") << "The data is not loaded.";
    return 0;
}


double BasePlayer::timeForIndex(std::size_t index) const
{
    if (isLoaded())
    {
        return indexedData()->timeForIndex(index);
    }

    ofLogError("BasePlayer::timeAtIndex") << "The data is not loaded.";
    return 0;
}


double BasePlayer::positionForTime(double time, bool clamp) const
{
    if (isLoaded())
    {
        return indexedData()->positionForTime(time, clamp);
    }

    ofLogError("BasePlayer::positionForTime") << "The data is not loaded.";
    return 0;
}


std::size_t BasePlayer::size() const
{
    if (isLoaded())
    {
        return indexedData()->size();
    }

    ofLogError("BasePlayer::size") << "The data is not loaded.";
    return 0;
}


BaseTimeIndexedInfo BasePlayer::stats() const
{
    if (isLoaded())
    {
        if (!_stats.isLoaded())
        {
            _stats.load(*indexedData());
        }

        return _stats;
    }

    ofLogError("BasePlayer::stats") << "The data is not loaded.";
    return _stats;
}


void BasePlayer::close()
{
    reset();
}


void BasePlayer::reset()
{
    _isFrameIndexNew = false;
    _speed = 1;
    _playingForward = true;
    _time = -1;
    _lastTime = -1;
    _frameIndex = 0;
    _interpolatedFrameIndex = 0;
    _lastFrameIndex = -1;
    _nextFrameIndex = -1;
    // _lastUpdateTime;
    // _firstUpdateTime;
    _isFirstUpdate = true;
    _loopType = OF_LOOP_NONE;
    _loopSet = false;
    _loopStartTime = 0.0;
    _loopEndTime = 0.0;
    // bool _framesInLoop = true;
    _paused = false;
    _playing = false;
    _stats = BaseTimeIndexedInfo();
}


} } // namespace ofx::Player
