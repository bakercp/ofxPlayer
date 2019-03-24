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
    /// \param timestampMicros The timestamp in microseconds.
    /// \returns true if the timestamp generation was successful.
    virtual bool createTimestampMicros(const std::string& uri,
                                       double& timestampMicros) const = 0;

};


/// \brief Represents a timestamped URI resource.
class TimestampedURI: public AbstractTimestamped
{
public:
    /// \brief Create a timestamped resource.
    /// \param uri The URI to timestamp.
    /// \param timestampMicros The timestamp in microseconds.
    TimestampedURI(const std::string& uri, double timestampMicros):
        _uri(uri),
        _timestampMicros(timestampMicros)
    {
    }

    /// \brief Destroy the TimestampedURI.
    virtual ~TimestampedURI()
    {
    }

    virtual double timestamp() const override
    {
        return _timestampMicros;
    }

    /// \returns the URI.
    std::string uri() const
    {
        return _uri;
    }

private:
    /// \brief The timestamp in microseconds.
    double _timestampMicros = 0;

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

    virtual bool createTimestampMicros(const std::string& uri,
                                       double& timestampMicros) const override
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
            timestampMicros = dateTime.timestamp().epochMicroseconds();
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
    /// \param frameDurationMicros The duration of each frame in microseconds.
    /// \param offsetMicros The timestamp offset to start with in microseconds.
    SequenceTimestamper(double frameDurationMicros,
                        double offsetMicros = 0):
        _frameDurationMicros(frameDurationMicros),
        _lastTimestampMicros(offsetMicros)
    {
    }

    /// \brief Destroy the SequenceTimestamper_.
    virtual ~SequenceTimestamper()
    {
    }

    virtual bool createTimestampMicros(const std::string& uri,
                                       double& timestampMicros) const override
    {
        timestampMicros = _lastTimestampMicros + _frameDurationMicros;
        _lastTimestampMicros = timestampMicros;
        return true;
    }

    /// \brief Create a filename timestamper from a frame rate.
    /// \param frameRate Frames per second.
    /// \param offset The timestamp offset to start with in microseconds.
    /// \returns A SequenceTimestamper configured with the given parameters.
    static SequenceTimestamper makeWithFrameRate(double frameRate, double offsetMicros = 0)
    {
        return SequenceTimestamper(1000000.0 / frameRate, offsetMicros);
    }

private:
    /// \brief The duration of each frame in microseconds.
    double _frameDurationMicros = 0;

    /// \brief The last timestamp assigned to a URI in microseconds.
    mutable double _lastTimestampMicros = 0;

};


/// \brief A collection of utilities for creating timestamped filenames.
class TimestampedFilenameUtils
{
public:
    /// \brief Create a list of timestamped URIs.
    /// \param resources The timestamped URIs to fill.
    /// \param directory The directory for the file resources.
    /// \param filePattern The regex file pattern to search for.
    /// \param makeRelativeToDirectory True if the image sequence should be
    ///        stored paths relative to the directory.
    /// \param stamper The timestamper responsible for converting a file to a
    ///        timestamp.
    /// \returns true if listing is successful.
    static bool list(std::vector<TimestampedURI>& resources,
                     const std::string& directory,
                     const std::string& filePattern,
                     bool makeFilesRelativeToDirectory,
                     const AbstractURITimestamper& stamper)
    {
        std::vector<std::filesystem::path> files;

        IO::RegexPathFilter regexFilter(filePattern);

        IO::DirectoryUtils::list(std::filesystem::path(directory),
                                 files,
                                 true,
                                 &regexFilter,
                                 makeFilesRelativeToDirectory);

        resources.clear();

        for (auto& file: files)
        {
            double timestampMicros = 0;

            if (stamper.createTimestampMicros(file.string(), timestampMicros))
            {
                resources.push_back(TimestampedURI(file.string(), timestampMicros));
            }
            else
            {
                ofLogError("TimestampedFilenameUtils::list") << "Unable to make TimestampedURI for: " << file;
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
