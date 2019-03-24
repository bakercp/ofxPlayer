//
// Copyright (c) 2013 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:    MIT
//


#include "ofx/Player/ImageSequence.h"
#include "ofx/IO/ImageUtils.h"
#include "ofImage.h"


namespace ofx {
namespace Player {


const std::string ImageSequence::DEFAULT_FILE_PATTERN = ".*.png|.*.jpg";


ImageSequence::ImageSequence():
    _pixelCache(std::make_unique<PixelCache>(DEFAULT_PIXEL_CACHE_SIZE)),
    _textureCache(std::make_unique<TextureCache>(DEFAULT_TEXTURE_CACHE_SIZE))
{
}


ImageSequence::ImageSequence(const ImageSequence& sequence):
    _name(sequence._name),
    _baseDirectory(sequence._baseDirectory),
    _images(sequence._images),
    _width(sequence._width),
    _height(sequence._height),
    _pixelCache(std::make_unique<PixelCache>(sequence._pixelCache->size())),
    _textureCache(std::make_unique<TextureCache>(sequence._textureCache->size()))
{
}


//ImageSequence::ImageSequence(const std::string& filename):
//    _pixelCache(std::make_unique<PixelCache>(DEFAULT_PIXEL_CACHE_SIZE)),
//    _textureCache(std::make_unique<TextureCache>(DEFAULT_TEXTURE_CACHE_SIZE))
//{
//    if (!fromJson(filename, *this))
//    {
//        ofLogError("ImageSequence::ImageSequence") << "Error loading from json.";
//    }
//}


ImageSequence::ImageSequence(const std::string& directory,
                             const std::string& filePattern,
                             bool makeFilesRelativeToDirectory,
                             const AbstractURITimestamper& stamper):
    _pixelCache(std::make_unique<PixelCache>(DEFAULT_PIXEL_CACHE_SIZE)),
    _textureCache(std::make_unique<TextureCache>(DEFAULT_TEXTURE_CACHE_SIZE))
{
    if (!fromDirectory(*this,
                       directory,
                       filePattern,
                       makeFilesRelativeToDirectory,
                       stamper))
    {
        ofLogError("ImageSequence::ImageSequence") << "Error loading from json.";
    }
}


ImageSequence::~ImageSequence()
{
}


double ImageSequence::timeForIndex(std::size_t _index) const
{
    std::size_t index = _index % size();

    return _images[_index % size()].timestamp();
}


std::size_t ImageSequence::size() const
{
    return _images.size();
}


float ImageSequence::getWidth() const
{
    return _width;
}


void ImageSequence::setWidth(float width)
{
    _width = width;
}


float ImageSequence::getHeight() const
{
    return _height;
}


void ImageSequence::setHeight(float height)
{
    _height = height;
}


const ofPixels& ImageSequence::getPixels(std::size_t _index) const
{
    std::size_t index = _index % size();

    if (index < size())
    {
        auto pixels = _pixelCache->get(index);

        if (pixels)
            return *pixels;

        pixels = std::make_shared<ofPixels>();

        auto path = resolve(_images[index]);

        if (ofLoadImage(*pixels, path))
        {
            _pixelCache->add(index, pixels);
            return *pixels;
        }

        throw std::runtime_error("Unable to load image " + path);
    }

    throw std::out_of_range("Index out of range: " + std::to_string(index));
}


const ofTexture& ImageSequence::getTexture(std::size_t _index) const
{
    std::size_t index = _index % size();

    if (index < size())
    {
        auto texture = _textureCache->get(index);

        if (texture)
            return *texture;

        auto pixels = getPixels(index);

        texture = std::make_shared<ofTexture>();

        texture->loadData(pixels);

        if (texture->isAllocated())
        {
            _textureCache->add(index, texture);
            return *texture;
        }

        throw std::runtime_error("Unable to load texture " + resolve(_images[index]));
    }

    throw std::out_of_range("Index out of range: " + std::to_string(index));
}


std::string ImageSequence::getName() const
{
    return _name;
}


void ImageSequence::setName(const std::string& name)
{
    _name = name;
}


std::string ImageSequence::baseDirectory() const
{
    return _baseDirectory;
}


std::string ImageSequence::resolve(const TimestampedURI& uri) const
{
    return (std::filesystem::path(_baseDirectory) / std::filesystem::path(uri.uri())).string();
}


bool ImageSequence::fromDirectory(ImageSequence& sequence,
                                  const std::string& directory,
                                  const std::string& filePattern,
                                  bool makeFilesRelativeToDirectory,
                                  const AbstractURITimestamper& stamper)
{
    sequence._baseDirectory = makeFilesRelativeToDirectory ? directory : "";

    if (sequence._name.empty())
    {
        sequence._name = std::filesystem::basename(directory);
    }

    if (TimestampedFilenameUtils::list(sequence._images,
                                       directory,
                                       filePattern,
                                       makeFilesRelativeToDirectory,
                                       stamper) && sequence.size() > 0)
    {

        // Load size info from header.
        IO::ImageUtils::ImageHeader header;

        if (IO::ImageUtils::loadHeader(header,
                                       sequence.resolve(sequence.images()[0])))
        {
            sequence._width = header.width;
            sequence._height = header.height;
            return true;
        }

        return false;
    }

    return false;
}


bool ImageSequence::fromJson(const std::string& filename, ImageSequence& sequence)
{
    ofFile file(filename);

    if (file.exists())
    {
        ofJson json;
        file >> json;

        if (!json["base_directory"].is_null())
        {
            sequence._baseDirectory = json["base_directory"];
        }

        if (!json["name"].is_null())
        {
            sequence._name = json["name"];
        }

        if (!json["width"].is_null())
        {
            sequence._width = json["width"];
        }

        if (!json["height"].is_null())
        {
            sequence._height = json["height"];
        }

        if (json["images"].is_array())
        {
            for (auto& image: json["images"])
            {
                sequence._images.push_back(TimestampedURI(image["uri"], image["ts"]));
            }
        }

        return true;
    }
    else
    {
        ofLogError("ImageSequence_::fromJson") << "File not found : " << filename;
        return false;
    }
}


bool ImageSequence::toJson(const ImageSequence& sequence,
                           const std::string& filename)
{
    ofJson json;

    json["name"] = sequence.getName();
    json["base_directory"] = sequence._baseDirectory;
    json["width"] = sequence.getWidth();
    json["height"] = sequence.getHeight();

    ofJson images;

    for (auto& image : sequence._images)
    {
        json["images"].push_back({
            { "uri", image.uri() },
            { "ts", image.timestamp() }
        });
    }

    std::string _filename = filename;

    if (_filename.empty())
    {
        _filename += sequence._baseDirectory;
        _filename += "/";
        _filename += sequence.getName();
        _filename += ".json";
    }

    return ofSaveJson(_filename, json);
}


const std::vector<TimestampedURI>& ImageSequence::images() const
{
    return _images;
}


void ImageSequence::setTextureCacheSize(std::size_t size)
{
    _textureCache = std::make_unique<TextureCache>(size);
}


void ImageSequence::clearTextureCache()
{
    _textureCache->clear();
}


void ImageSequence::setPixelCacheSize(std::size_t size)
{
    _pixelCache = std::make_unique<PixelCache>(size);
}


void ImageSequence::clearPixelCache()
{
    _pixelCache->clear();
}


} } // namespace ofx::Player
