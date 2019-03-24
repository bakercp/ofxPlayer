//
// Copyright (c) 2013 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:    MIT
//


#pragma once


#include "ofx/Player/AbstractPlayerTypes.h"


namespace ofx {
namespace Player {


/// \brief A base time indexed class.
class BaseTimeIndexed: public AbstractTimeIndexed
{
public:
    /// \brief Destroy the BaseTimeIndexed.
    virtual ~BaseTimeIndexed();

    double startTime() const override;
    double endTime() const override;
    double duration() const override;

    std::size_t indexForPosition(double position,
                                 bool increasing,
                                 std::size_t indexHint) const override;

    double timeForPosition(double position) const override;

    double positionForIndex(std::size_t index) const override;

    std::size_t indexForTime(double time,
                             bool increasing,
                             std::size_t indexHint) const override;

    double positionForTime(double time, bool clamp) const override;

};

//
///// \brief A base playable buffer.
//class BasePlayableBuffer: public BasePlayable
//{
//public:
//    /// \brief Destroy the BasePlayableBuffer.
//    virtual ~BasePlayableBuffer();
//
//    std::size_t indexForTime(double time,
//                             double timeStep,
//                             std::size_t indexHint) const override;
//    double startTime() const override;
//    double endTime() const override;
//    double duration() const override;
//
//
//
//};







/// \brief A default buffer adapter.
class DefaultBufferAdapter
{
public:
    /// \brief Get a timestamp from the given input.
    /// \param input The input type to adapt.
    /// \returns the timestamp in microseconds.
    static double timestamp(const AbstractTimestamped& input);

};


/// \brief A playable buffer to handle any buffered data type.
///
/// The AdapterType must have a static function called ::timestamp that takes
/// a piece of data in the buffer container and returns the timestamp, ideally
/// without any copies, etc.
///
/// \tparam BufferType The buffer with the playable data.
/// \tparam AdapterType The adapter type.
template<typename BufferType, typename AdapterType>
class PlayableBufferHandle: public BaseTimeIndexed
{
public:
    /// \brief Create a PlayableBufferHandle_ with the given \p buffer.
    /// \param buffer The buffer to create a handle for.
    PlayableBufferHandle(BufferType& buffer): _buffer(buffer)
    {
    }


    /// \brief Destroy the PlayableBufferHandle.
    virtual ~PlayableBufferHandle()
    {
    }


    virtual double timeForIndex(std::size_t index) const override
    {
        return AdapterType::timestamp(_buffer[index]);
    }


    virtual std::size_t size() const override
    {
        return _buffer.size();
    }


private:
    /// \brief A reference to the buffer.
    BufferType& _buffer;

};



/// \brief A base class for playing arbitrary timestamped data.
class BasePlayer: public AbstractPlayer
{
public:
    /// \brief Destroy the AbstractPlayer.
    virtual ~BasePlayer();
    void update() override;
    bool isFrameIndexNew() const override;
    double getSpeed() const override;
    void setSpeed(double speed) override;
    double getPosition() const override;
    void setPosition(double position) override;
    double getTime() const override;
    void setTime(double time) override;
    std::size_t getFrameIndex() const override;
    void setFrameIndex(std::size_t index) override;
    void setLoopStartPosition(double position) override;
    void setLoopEndPosition(double position) override;
    double getLoopStartPosition() const override;
    double getLoopEndPosition() const override;
    void setLoopStartTime(double time) override;
    void setLoopEndTime(double time) override;
    double getLoopStartTime() const override;
    double getLoopEndTime() const override;
    void setLoopStartFrameIndex(std::size_t index) override;
    void setLoopEndFrameIndex(std::size_t index) override;
    std::size_t getLoopStartFrameIndex() const override;
    std::size_t getLoopEndFrameIndex() const override;
    void clearLoopPoints() override;
    ofLoopType getLoopType() const override;
    void setLoopType(ofLoopType loopType) override;
    void setPaused(bool paused) override;
    void play() override;
    void stop() override;
    bool isPaused() const override;
    bool isLoaded() const override;
    bool isPlaying() const override;

    double startTime() const override;
    double endTime() const override;
    double duration() const override;
    std::size_t indexForPosition(double position,
                                 bool increasing,
                                 std::size_t indexHint) const override;
    double timeForPosition(double position) const override;
    double positionForIndex(std::size_t index) const override;
    std::size_t indexForTime(double time,
                             bool increasing,
                             std::size_t indexHint) const override;
    double timeForIndex(std::size_t index) const override;
    double positionForTime(double time, bool clamp) const override;
    std::size_t size() const override;

protected:
    /// \brief A type definition for double microseconds.
    typedef std::chrono::duration<double, std::micro> micros_duration;

    /// \brief Get a pointer to the time indexed data.
    ///
    /// When data is loaded, this function should return a pointer to that
    /// data. If this returns a nullptr the data player is not considered to
    /// be loaded.
    ///
    /// \returns a const pointer to the indexed data or nullptr if not loaded.
    virtual const BaseTimeIndexed* indexedData() const = 0;

    /// \brief Reset internal variables.
//    virtual void close() override;

    /// \brief True if the frame is new.
    bool _isFrameIndexNew = true;

    /// \brief The playback speed.
    double _speed = 1;

    /// \brief The playing direction.
    bool _playingForward = true;

    /// \brief The playback time.
    double _time = -1;

    /// \brief The last playback time.
    double _lastTime = -1;

    /// \brief The current frame index.
    ///
    /// This is used to check if the frame is new.
    std::size_t _frameIndex = 0;

    /// \brief The last frame index.
    ///
    /// This is used to check if the frame is new.
    std::size_t _lastFrameIndex = 0;

    /// \brief The last update time in microseconds.
    std::chrono::high_resolution_clock::time_point _lastUpdateTime;

    /// \brief The first update time in microseconds.
    std::chrono::high_resolution_clock::time_point _firstUpdateTime;

    /// \brief A flag to determine if this is the first update.
    bool _isFirstUpdate = true;

    /// \brief The playback loop type.
    ofLoopType _loopType = OF_LOOP_NONE;

    /// \brief The loop start time in microseconds.
    double _loopStartTime = -1;

    /// \brief The loop end time in microseconds.
    double _loopEndTime = -1;

    /// \brief True if there are frames betwen the loop points.
    ///
    /// This is used to short circuit the search if it is known that there are
    /// no frames within a given loop.
    ///
    // bool _framesInLoop = true;

    /// \brief True if the playback is paused.
    bool _paused = false;

    /// \brief True if is playing.
    bool _playing = false;

};


} } // namespace ofx::Player
