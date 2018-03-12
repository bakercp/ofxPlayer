//
// Copyright (c) 2013 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:    MIT
//


#pragma once


#include "ofBaseTypes.h"


namespace ofx {
namespace Player {


/// \brief An abstract interface for timestamped data.
class AbstractTimestamped
{
public:
    /// \brief Destroy the AbstractTimestampedFrame.
    virtual ~AbstractTimestamped()
    {
    }

    /// \brief Get the timestamp in microseconds of the data.
    virtual double timestamp() const = 0;
    
};


/// \brief An abstract interface for time indexed data.
class AbstractTimeIndexed
{
public:
    /// \brief Destroy the AbstractTimeIndexed.
    virtual ~AbstractTimeIndexed()
    {
    }

    /// \brief Get the start time in microseconds.
    /// \returns the data's start time in microseconds.
    virtual double startTime() const = 0;

    /// \brief Get the end time in microseconds.
    /// \returns the data's end time in microseconds.
    virtual double endTime() const = 0;

    /// \brief Get the data's duration in microseconds.
    /// \returns The data's duration in microseconds.
    virtual double duration() const = 0;

    /// \brief Get data for the given position.
    ///
    /// Position is simply the normalized position within the indexed data.
    /// A position of 0 is the first data point. A position of 1 is the last
    /// data point.
    ///
    /// If playing forward (increasing == true), this will return the index of
    /// the frame with timestamp corresponding to a position <= position.
    ///
    /// If playing backward (increasing == false), this will return the index of
    /// the frame with a timestamp corresponding to a position >= position.
    ///
    /// The indexHint is optional and can be used to find make sequential
    /// access quicker by providing the last known index that met the
    /// requirements above.
    ///
    /// \param position The normalized time 0 - 1 to retrieve.
    /// \param increasing True if the position is increasing.
    /// \param indexHint An hint that may make it easier to find the index.
    virtual std::size_t indexForPosition(double position,
                                         bool increasing,
                                         std::size_t indexHint) const = 0;

    /// \brief Get a time for the given position.
    /// \param position The position to convert.
    /// \returns the calculated time in microseconds.
    virtual double timeForPosition(double position) const = 0;

    /// \brief Get the position value for the given index.
    /// \param index The index to query.
    /// \returns the position for the given index.
    virtual double positionForIndex(std::size_t index) const = 0;

    /// \brief Get data for the given time in microseconds.
    ///
    /// If playing forward (increasing == true), this will return the index of
    /// the frame with a timestamp <= time.
    ///
    /// If playing backward (increasing == false), this will return the index of
    /// the frame with a timestamp >= time.
    ///
    /// The indexHint is optional and can be used to find make sequential
    /// access quicker by providing the last known index that met the
    /// requirements above.
    ///
    /// \param time The time to query in microseconds.
    /// \param increasing True if the time is increasing.
    /// \param indexHint An hint that may make it easier to find the index.
    /// \returns The index corresponding to to the given time.
    virtual std::size_t indexForTime(double time,
                                     bool increasing,
                                     std::size_t indexHint) const = 0;

    /// \brief Get the timestamp at the given index in microseconds.
    /// \param index The frame index to query.
    /// \returns the timestamp at the index in microseconds.
    virtual double timeForIndex(std::size_t index) const = 0;

    /// \brief Map the time to a given position.
    ///
    /// If not clamped, the returned position may be outside the range of 0-1.
    /// If outside, this can be used to wrap the position, reflect, etc.
    ///
    /// \param time The time in microseconds.
    /// \param clamp If true, the position will be normalized betwen 0-1.
    /// \returns the position.
    virtual double positionForTime(double time, bool clamp) const = 0;

    /// \returns the number of indexed frames.
    virtual std::size_t size() const = 0;

};


/// \brief An abstract iterface for a data player.
class AbstractPlayer: public AbstractTimeIndexed
{
public:
    /// \brief Destroy the AbstractPlayer.
    virtual ~AbstractPlayer()
    {
    }

    /// \brief Update the player's internal state.
    virtual void update() = 0;

    /// \brief Query if the current frame index is new.
    /// \returns true if the frame index was updated in the last update cycle.
    virtual bool isFrameIndexNew() const = 0;

    /// \brief Query the playback speed.
    /// \returns The current speed playback multiplier.
    virtual double getSpeed() const = 0;

    /// \brief Set the playback speed.
    ///
    /// Sets the time base multiplier. A speed of 1 will play back the frames
    /// in "real time" based on timestamps. A speed of 2 will play them back at
    /// 2x the "real time" speed. A speed of -1 will play the frames in reverse
    /// at "real-time" speed.
    ///
    /// \param speed Set the playback speed.
    virtual void setSpeed(double speed) = 0;

    /// \brief Get the normalized position between 0 and 1.
    /// \returns the normalized position between 0 and 1.
    virtual double getPosition() const = 0;

    /// \brief Set the normalized position between 0 and 1.
    /// \param position Set the normalized position between 0 and 1.
    virtual void setPosition(double position) = 0;

    /// \brief Get the current time of the player in microseconds.
    ///
    /// This time is the timestamp at the current playhead position. This number
    /// is dependent upon the timestamps in playable media.
    ///
    /// \returns the current time of the player in microseconds.
    virtual double getTime() const = 0;

    /// \brief Set the current time of the player in microseconds.
    ///
    /// This time is the timestamp at the current playhead position. This number
    /// is dependent upon the timestamps in playable media.
    ///
    /// \param time The current time of the player in microseconds.
    virtual void setTime(double time) = 0;

    /// \brief Get the current frame index.
    ///
    /// A frame is the index of the closest data frame given the current
    /// playback time.
    ///
    /// \returns the current frame index.
    virtual std::size_t getFrameIndex() const = 0;

    /// \brief Set the current frame index.
    ///
    /// A frame is the index of the closest data frame given the current
    /// playback time.
    ///
    /// \param frame The current frame index.
    virtual void setFrameIndex(std::size_t index) = 0;

    /// \brief Set the loop start point by position.
    /// \param position Set the normalized start position between 0 and 1.
    virtual void setLoopStartPosition(double position) = 0;

    /// \brief Set the loop end point by position.
    /// \param position Set the normalized end position between 0 and 1.
    virtual void setLoopEndPosition(double position) = 0;

    /// \returns the start loop point position.
    virtual double getLoopStartPosition() const = 0;

    /// \returns the end loop point position.
    virtual double getLoopEndPosition() const = 0;

    /// \brief Set the loop start point time.
    /// \param time Set the absolute start time.
    virtual void setLoopStartTime(double time) = 0;

    /// \brief Set the loop end point.
    /// \param time Set the absolute end time.
    virtual void setLoopEndTime(double time) = 0;

    /// \returns the start loop point start time.
    virtual double getLoopStartTime() const = 0;

    /// \returns the end loop point end time.
    virtual double getLoopEndTime() const = 0;

    /// \brief Set the loop start frame index.
    /// \param frame The loop start frame index.
    virtual void setLoopStartFrameIndex(std::size_t index) = 0;

    /// \brief Set the loop point end frame index.
    /// \param frame The loop point end frame index.
    virtual void setLoopEndFrameIndex(std::size_t index) = 0;

    /// \returns the loop point start frame index.
    virtual std::size_t getLoopStartFrameIndex() const = 0;

    /// \returns the loop point end frame index.
    virtual std::size_t getLoopEndFrameIndex() const = 0;

    /// \brief Clear the loop points.
    virtual void clearLoopPoints() = 0;

    /// \brief Get the loop type.
    /// \returns the ofLoopType type.
    virtual ofLoopType getLoopType() const = 0;

    /// \brief Set the loop type.
    /// \param loopType The ofLoopType type.
    virtual void setLoopType(ofLoopType loopType) = 0;

    /// \brief Pause the frame playback.
    /// \param paused Set true to pause and false to unpause.
    virtual void setPaused(bool paused) = 0;

    /// \brief Begin frame playback.
    virtual void play() = 0;

    /// \brief Stop the frame playback.
    virtual void stop() = 0;

    /// \returns true iff is paused.
    virtual bool isPaused() const = 0;

    /// \returns true iff is loaded.
    virtual bool isLoaded() const = 0;

    /// \returns true iff is playing.
    virtual bool isPlaying() const = 0;

};


} } // namespace ofx::Player
