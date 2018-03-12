//
// Copyright (c) 2013 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:    MIT
//


#pragma once


#include "ofFileUtils.h"
#include "Poco/DateTimeParser.h"
#include "ofx/IO/DirectoryUtils.h"
#include "ofx/IO/RegexPathFilter.h"
#include "ofx/Player/AbstractPlayerTypes.h"
#include "ofx/Player/BasePlayerTypes.h"


namespace ofx {
namespace Player {


/// \brief Create a timestamp for a given URI.
class AbstractURITimestamper
{
public:
    /// \brief Destroy the AbstractURITimestamper.
    virtual ~AbstractURITimestamper()
    {
    }

    /// \brief Determine a resource's timestamp based on its URI.
    /// \param uri The URI of the resource to analyze.
    /// \param timestamp The timestamp in microseconds.
    /// \returns true if the timestamp generation was successful.
    virtual bool createTimestamp(const std::string& uri,
                                 double& timestamp) const = 0;

};


/// \brief Represents a timestamped URI resource.
class TimestampedURI: public AbstractTimestamped
{
public:
    /// \brief Create a timestamped resource.
    /// \param uri The URI to timestamp.
    /// \param timestamp The timestamp in microseconds.
    TimestampedURI(const std::string& uri, double timestamp):
        _uri(uri),
        _timestamp(timestamp)
    {
    }

    /// \brief Destroy the TimestampedURI.
    virtual ~TimestampedURI()
    {
    }

    virtual double timestamp() const override
    {
        return _timestamp;
    }

    /// \returns the URI.
    std::string uri() const
    {
        return _uri;
    }

private:
    /// \brief The timestamp in microseconds.
    double _timestamp = 0;

    /// \brief The URI.
    std::string _uri;

};


///// \brief A playable TimestampedURIList.
/////
///// Should be sub-classed in order to load the timestamped resources into the
///// collection.
//class TimestampedURIList: public BasePlayableBuffer
//{
//public:
//    /// \brief Create an empty TimestampedResourceList.
//    TimestampedURIList();
//
//    /// \brief Destroy the TimestampedResourceList.
//    virtual ~TimestampedURIList();
//
//    virtual uint64_t timeAtIndex(std::size_t index) const override
//    {
//        return _buffer[index].timestamp();
//    }
//
//    virtual std::size_t size() const override
//    {
//        return _buffer.size();
//    }
//    /// \returns the timestamped resources.
//    const std::vector<TimestampedResource>& resources() const;
//
//private:
//    /// \brief A collection of timestamped resources.
//    std::vector<TimestampedResource> _resources;
//    
//};


/// \brief Determine a timestamp for file based on a timestamp in the filename.
class FilenameTimestamper: public AbstractURITimestamper
{
public:
    /// \brief Create a filename timestamper.
    /// \param timestampFormat The timestamp filename format used to store timestamps.
    FilenameTimestamper(const std::string& timestampFormat = DEFAULT_TIMESTAMP_FORMAT):
        _timestampFormat(timestampFormat)
    {
    }

    /// \brief Destroy the filename timestamper.
    virtual ~FilenameTimestamper()
    {
    }

    virtual bool createTimestamp(const std::string& uri,
                                 double& timestamp) const override
    {
        // Note, we might do this with std::get_time or similar, but this std-
        // based approach does not easily support fractional seconds, so for
        // now, we use Poco.

        Poco::DateTime dateTime;
        int tzd = 0;

        if (Poco::DateTimeParser::tryParse(_timestampFormat,
                                           ofFilePath::getBaseName(uri),
                                           dateTime,
                                           tzd))
        {
            timestamp = dateTime.timestamp().epochMicroseconds();
            return true;
        }
        else
        {
            ofLogError("FilenameTimestamper::createTimestamp") << "Unable to parse time: " << ofFilePath::getBaseName(uri) << " with " <<_timestampFormat;
            return false;
        }
    }

    /// \returns the timestamp format for the timestamper.
    std::string timestampFormat() const
    {
        return _timestampFormat;
    }

    /// \brief The default timestamp format.
    static constexpr const char* DEFAULT_TIMESTAMP_FORMAT = "%Y-%m-%d-%H-%M-%S-%i";

private:
    /// \brief The timestamp format to look for with filenames.
    std::string _timestampFormat;

};


/// \brief Determine a timestamp for URIs based on a given frame rate and offset.
class SequenceTimestamper: public AbstractURITimestamper
{
public:
    /// \brief Create a filename timestamper from a frame duration.
    /// \param frameDuration The duration of each frame in microseconds.
    /// \param offset The timestamp offset to start with in microseconds.
    SequenceTimestamper(double frameDuration,
                        double offset = 0):
        _frameDuration(frameDuration),
        _lastTimestamp(offset)
    {
    }

    /// \brief Destroy the SequenceTimestamper_.
    virtual ~SequenceTimestamper()
    {
    }

    virtual bool createTimestamp(const std::string& uri,
                                 double& timestamp) const override
    {
        timestamp = _lastTimestamp + _frameDuration;
        _lastTimestamp = timestamp;
        return true;
    }

    /// \brief Create a filename timestamper from a frame rate.
    /// \param frameRate Frames per second.
    /// \param offset The timestamp filename format used to store timestamps.
    /// \returns A SequenceTimestamper configured with the given parameters.
    static SequenceTimestamper makeWithFrameRate(double frameRate, double offset = 0)
    {
        return SequenceTimestamper(1000000.0 / frameRate, offset);
    }

private:
    /// \brief The duration of each frame.
    double _frameDuration = 0;

    /// \brief The last timestamp assigned to a URI.
    mutable double _lastTimestamp = 0;

};


/// \brief A collection of utilities for creating timestamped filenames.
class TimestampedFilenameUtils
{
public:
    /// \brief Create a list of timestamped URIs.
    /// \param directory The directory for the file resources.
    /// \param filePattern The regex file pattern to search for.
    /// \param makeRelativeToDirectory True if the image sequence should be
    ///        stored paths relative to the directory.
    /// \param stamper The timestamper responsible for converting a file to a
    ///        timestamp.
    /// \param resources The timestamped URIs to fill.
    /// \returns true if listing is successful.
    static bool list(const std::string& directory,
                     const std::string& filePattern,
                     bool makeFilesRelativeToDirectory,
                     const AbstractURITimestamper& stamper,
                     std::vector<TimestampedURI>& resources)
    {
        std::vector<std::string> files;

        IO::RegexPathFilter regexFilter(filePattern);

        IO::DirectoryUtils::list(directory,
                                 files,
                                 true,
                                 &regexFilter,
                                 makeFilesRelativeToDirectory);

        resources.clear();

        for (auto& file : files)
        {
            double timestamp = 0;

            if (stamper.createTimestamp(file, timestamp))
            {
                resources.push_back(TimestampedURI(file, timestamp));
            }
            else
            {
                return false;
            }
        }

        // Sort results in case they are not ordred by the file system.
        std::sort(resources.begin(),
                  resources.end(),
                  [](const TimestampedURI& lhs,
                     const TimestampedURI& rhs)
                  {
                      return lhs.timestamp() < rhs.timestamp();
                  });
        
        return true;

    }

};


} } // namespace ofx::Player
