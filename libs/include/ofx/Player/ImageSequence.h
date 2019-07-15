//
// Copyright (c) 2013 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier: MIT
//


#pragma once


#include "ofJson.h"
#include "ofx/Player/BasePlayerTypes.h"
#include "ofx/Player/IndexedFile.h"
#include "ofx/Cache/LRUMemoryCache.h"


namespace ofx {
namespace Player {


/// \brief A class describing an index that was cached.
class IndexCachedEventArgs
{
public:
    /// \brief Cache the given index.
    /// \param index The index of the frame that was cached.
    IndexCachedEventArgs(std::size_t index): _index(index)
    {
    }

    /// \brief Destroy the IndexCachedEventArgs.
    virtual ~IndexCachedEventArgs()
    {
    }

    /// \brief Get the index that was cached.
    /// \returns the index that was cached.
    std::size_t index() const;

private:
    /// \brief The index that was cached.
    std::size_t _index;

};


class IndexCachedErrorEventArgs: public IndexCachedEventArgs
{
public:
    IndexCachedErrorEventArgs(std::size_t index, const std::string& error):
        IndexCachedEventArgs(index),
        _error(error)
    {
    }

private:
    std::string _error;

};


class TextureLoadedEventArgs: public IndexCachedEventArgs
{
public:
    TextureLoadedEventArgs(std::size_t index, const ofTexture* texture):
        IndexCachedEventArgs(index),
        _texture(texture)
    {
    }

    /// \returns a const pointer to the cached texture.
    const ofTexture* texture() const
    {
        return _texture;
    }

private:
    /// \brief A const pointer to the cached texture.
    const ofTexture* _texture;

};


class PixelsLoadedEventArgs: public IndexCachedEventArgs
{
public:
    PixelsLoadedEventArgs(std::size_t index, const ofPixels* pixels):
        IndexCachedEventArgs(index),
        _pixels(pixels)
    {
    }

    /// \returns a const pointer to the cached pixels.
    const ofPixels* pixels() const
    {
        return _pixels;
    }

private:
    /// \brief A const pointer to the cached pixels.
    const ofPixels* _pixels;

};

/// \brief An image sequence.
class ImageSequence: public BaseTimeIndexed
{
public:
    ImageSequence();

    ImageSequence(const ImageSequence& sequence);

    /// \brief Create an image sequence from a directory.
    ///
    /// If the name if the image sequence is empty, then the directory name will
    /// be used.
    ///
    /// \param directory The directory for the file resources.
    /// \param filePattern The regex file pattern to load.
    /// \param stamper The timestamper responsible for converting a file to a
    ///        timestamp.
    /// \param resources The timestamped URIs to fill.
    /// \param makeFilesRelativeToDirectory True if the image sequence should be
    ///        stored paths relative to the directory.
    /// \param stamper The timestamper to use.
    ImageSequence(const std::string& directory,
                  const std::string& filePattern = DEFAULT_FILE_PATTERN,
                  bool makeFilesRelativeToDirectory = true,
                  const AbstractURITimestamper& stamper = SequenceTimestamper::makeWithFrameRate(30));

    /// \brief Destroy the ImageSequence.
    virtual ~ImageSequence() override;

    double timeForIndex(std::size_t index) const override;

    std::size_t size() const override;

    /// \returns the sequence width.
    float getWidth() const;

    /// \brief The sequence width.
    /// \param width The sequence width.
    void setWidth(float width);

    /// \returns the sequence height.
    float getHeight() const;

    /// \brief Set the sequence height.
    /// \param height The sequence height.
    void setHeight(float height);

    /// \brief Get the pixels for a given frame index.
    ///
    /// This call will block on read if the frame isn't cached.This pointer is
    /// managed by the ImageSequence and should not be deleted or managed by
    /// the caller.
    ///
    /// \throws std::out_of_range for invalid indices.
    /// \throws std::runtime_error if the image can't be loaded.
    /// \param index The frame index to get.
    /// \returns a const reference to the pixels, or nullptr on failure.
    const ofPixels& getPixels(std::size_t index) const;

    /// \brief Get the texture for a given frame index.
    ///
    /// This call will block on read if the frame isn't cached. This pointer is
    /// managed by the ImageSequence and should not be deleted or managed by
    /// the caller.
    ///
    /// \throws std::out_of_range for invalid indices.
    /// \throws std::runtime_error if the image can't be loaded.
    /// \param index The frame index to get.
    /// \returns a const reference to the pixels, or nullptr on failure
    const ofTexture& getTexture(std::size_t index) const;

    /// \returns the name of the image sequence.
    std::string getName() const;

    /// \brief Set the name of the image sequence.
    /// \param name The name of the image sequence.
    void setName(const std::string& name);

    /// \returns the base directory for the URIs.
    std::string baseDirectory() const;

    /// \brief Resolve a relative URI against the base directory.
    /// \returns resolved URI against the base directory from the given URI.
    std::string resolve(const TimestampedURI& uri) const;

    /// \brief Create a list of timestamped URIs.
    ///
    /// If the name if the image sequence is empty, then the directory name will
    /// be used.
    ///
    /// \param sequence The sequence to load.
    /// \param directory The directory for the file resources.
    /// \param filePattern The regex file pattern to load.
    /// \param resources The timestamped URIs to fill.
    /// \param makeFilesRelativeToDirectory True if the image sequence should be
    ///        stored as paths relative to the directory.
    /// \param stamper The timestamper responsible for converting a file to a
    ///        timestamp.
    /// \returns true if loading is successful.
    static bool fromDirectory(ImageSequence& sequence,
                              const std::string& directory,
                              const std::string& filePattern = DEFAULT_FILE_PATTERN,
                              bool makeFilesRelativeToDirectory = true,
                              const AbstractURITimestamper& stamper = SequenceTimestamper::makeWithFrameRate(30));

    /// \brief Load an ImageSequence from a json file.
    /// \param filename The json file to load.
    /// \param sequence The ImageSequence to load.
    /// \returns true if the ImageSequence was loaded successfully.
    static bool fromJson(const std::string& filename, ImageSequence& sequence);

    /// \brief Save an ImageSequence to a json file.
    /// \param sequence The ImageSequence to load.
    /// \param filename The json file to load.
    /// \returns true if the ImageSequence was saved successfully.
    static bool toJson(const ImageSequence& sequence,
                       const std::string& filename = "");

    /// \returns a const reference to the timestamped image URIs.
    const std::vector<TimestampedURI>& images() const;

    /// \brief Set the size of the pixel cache.
    ///
    /// Setting the cache size resets the cache.
    ///
    /// \param size The maximum number of pixels frames to cache.
    void setPixelCacheSize(std::size_t size);

    /// \brief Clear the pixel cache.
    void clearPixelCache();

    /// \brief Set the size of the texture cache.
    ///
    /// Setting the cache size resets the cache.
    ///
    /// \param size The maximum number of textures to cache.
    void setTextureCacheSize(std::size_t size);

    /// \brief Clear the texture cache.
    void clearTextureCache();

    enum
    {
        /// \brief The default number of frame pixels to cache.
        DEFAULT_PIXEL_CACHE_SIZE = 256,
        /// \brief The default number of frame textures to cache.
        DEFAULT_TEXTURE_CACHE_SIZE = 256
    };

    /// \brief The default regex file pattern for loading directories.
    static const std::string DEFAULT_FILE_PATTERN;

private:
    /// \brief A typedef for a pixel cache.
    typedef Cache::LRUMemoryCache<std::size_t, ofPixels> PixelCache;

    /// \brief A typedef for a texture cache.
    typedef Cache::LRUMemoryCache<std::size_t, ofTexture> TextureCache;

    /// \brief The sequnce name, if set.
    std::string _name;

    /// \brief The base directory for the timestamped images.
    std::string _baseDirectory;

    /// \brief A collection of timestamped images.
    std::vector<TimestampedURI> _images;

    /// \brief The image width.
    float _width = 0;

    /// \brief The image height.
    float _height = 0;

    /// \brief A cache for pixels.
    mutable std::unique_ptr<PixelCache> _pixelCache;

    /// \brief A cache for textures.
    mutable std::unique_ptr<TextureCache> _textureCache;

};


} } // namespace ofx::Player
