#include "ImageProcessing.h"

// Read in a number .TGA files in a binary forma
// File constructor
ImageProcessing::ImageProcessing(string fileName)
{
    // Reading in the file
    ifstream inFile(fileName, ios::binary);

    // Taking in the header values
    inFile.read((char*)&_header.idLength, sizeof(_header.idLength));

    inFile.read((char*)&_header.colorMapType, sizeof(_header.colorMapType));

    inFile.read((char*)&_header.dataTypeCode, sizeof(_header.dataTypeCode));

    inFile.read((char*)&_header.colorMapOrigin, sizeof(_header.colorMapOrigin));

    inFile.read((char*)&_header.colorMapLength, sizeof(_header.colorMapLength));

    inFile.read((char*)&_header.colorMapDepth, sizeof(_header.colorMapDepth));

    inFile.read((char*)&_header.xOrigin, sizeof(_header.xOrigin));

    inFile.read((char*)&_header.yOrigin, sizeof(_header.yOrigin));

    inFile.read((char*)&_header.width, sizeof(_header.width));

    inFile.read((char*)&_header.height, sizeof(_header.height));

    inFile.read((char*)&_header.bitsPerPixel, sizeof(_header.bitsPerPixel));

    inFile.read((char*)&_header.imageDescriptor, sizeof(_header.imageDescriptor));

    // The pixels
    unsigned char blue;

    unsigned char green;

    unsigned char red;

    // Total number of pixels
    _numberOfPixels = (_header.width) * (_header.height);

    // Add the pixels to the pixels vector
    for (unsigned int i = 0; i < _numberOfPixels; i++) 
    {
        inFile.read((char*)&blue, sizeof(blue));

        inFile.read((char*)&green, sizeof(green));

        inFile.read((char*)&red, sizeof(red));

        // Add the pixel to the pixel vector
        _pixels.push_back(Pixel{blue,green,red});
    }
}

// Write out new .TGA files in the same binary format
void ImageProcessing::OutputTo(string fileName)
{
    // Outputting a new file
    fstream outFile(fileName, ios::out | ios::binary);

    // Writing out header values
    outFile.write((char*)&_header.idLength, sizeof(_header.idLength));

    outFile.write((char*)&_header.colorMapType, sizeof(_header.colorMapType));

    outFile.write((char*)&_header.dataTypeCode, sizeof(_header.dataTypeCode));

    outFile.write((char*)&_header.colorMapOrigin, sizeof(_header.colorMapOrigin));

    outFile.write((char*)&_header.colorMapLength, sizeof(_header.colorMapLength));

    outFile.write((char*)&_header.colorMapDepth, sizeof(_header.colorMapDepth));

    outFile.write((char*)&_header.xOrigin, sizeof(_header.xOrigin));

    outFile.write((char*)&_header.yOrigin, sizeof(_header.yOrigin));

    outFile.write((char*)&_header.width, sizeof(_header.width));

    outFile.write((char*)&_header.height, sizeof(_header.height));

    outFile.write((char*)&_header.bitsPerPixel, sizeof(_header.bitsPerPixel));

    outFile.write((char*)&_header.imageDescriptor, sizeof(_header.imageDescriptor));

    // Writing data to output file
    for (unsigned int i = 0; i < _numberOfPixels; i++) 
    {
        outFile.write((char*)&_pixels[i].blue, sizeof(_pixels[i].blue));

        outFile.write((char*)&_pixels[i].green, sizeof(_pixels[i].green));

        outFile.write((char*)&_pixels[i].red, sizeof(_pixels[i].red));
    }

}

// Use to compare with the files in the examples folder
bool ImageProcessing::EqualTo(ImageProcessing& file)
{
    for (unsigned int i = 0; i < _numberOfPixels; i++) 
    {
        if (_pixels[i].blue != file._pixels[i].blue)   
            return false;

        if (_pixels[i].green != file._pixels[i].green) 
            return false;

        if (_pixels[i].red != file._pixels[i].red)     
            return false;
    }

    return true;
}

/*
Multiplies the standardized (based on the domain 0...1) tonal values of the fore- and background's pixels.
C = A * B
*/
void ImageProcessing::MultiplyBlending(ImageProcessing& file)
{
    // Before use, normalize first
    file.Normalize();

    this->Normalize();

    for (unsigned int i = 0; i < _numberOfPixels; i++) 
    {
        _normalizedPixels[i].blue *= file._normalizedPixels[i].blue;

        _normalizedPixels[i].green *= file._normalizedPixels[i].green;

        _normalizedPixels[i].red *= file._normalizedPixels[i].red;
    }

    // Once done, denormalize + clear normalized pixel vector
    this->DeNormalize();

    this->_normalizedPixels.clear();
}

/*
The tonal values of fore- and background that sum up to less than 255 (i.e. 1.0) become pure black.
C = A + B - 1
*/
void ImageProcessing::SubstractBlending(ImageProcessing& file)
{
    for (unsigned int i = 0; i < _numberOfPixels; i++) 
    {
        if (file._pixels[i].blue > _pixels[i].blue)
        {
            _pixels[i].blue = 0;

        }
        else                          
            _pixels[i].blue -= file._pixels[i].blue;

        if (file._pixels[i].green > _pixels[i].green) 
        {
            _pixels[i].green = 0;
        }
        else                        
            _pixels[i].green -= file._pixels[i].green;

        if (file._pixels[i].red > _pixels[i].red)
        {
            _pixels[i].red = 0;
        }
        else                            
            _pixels[i].red -= file._pixels[i].red;
    }
}

/*
Fore- and background are »negatively multiplied« and lighten each other (as if two slides are projected with different projectors onto the same screen).
C = 1 - (1 - A) * (1 - B)
*/
void ImageProcessing::ScreenBlending(ImageProcessing& file)
{
    // Before use, normalize first
    file.Normalize();

    this->Normalize();

    for (unsigned int i = 0; i < _numberOfPixels; i++) 
    {
        _normalizedPixels[i].blue = 1 - (1 - _normalizedPixels[i].blue) * (1 - file._normalizedPixels[i].blue);

        _normalizedPixels[i].green = 1 - (1 - _normalizedPixels[i].green) * (1 - file._normalizedPixels[i].green);

        _normalizedPixels[i].red = 1 - (1 - _normalizedPixels[i].red) * (1 - file._normalizedPixels[i].red);
    }

    // Once done, denormalize + clear normalized pixel vector
    this->DeNormalize();

    this->_normalizedPixels.clear();
}

/*
If the background is darker than 50% gray, the tonal values get multiplied, otherwise they get »screened« (and afterwards they are doubled in both cases).
B <= 0.5 -> C = 2 * A * B
B > 0.5 -> C = 1 - 2 * (1 - A) * (1 - B)
*/
void ImageProcessing::OverlayBlending(ImageProcessing& file)
{
    // Before use, normalize first
    file.Normalize();

    this->Normalize();

    for (unsigned int i = 0; i < _numberOfPixels; i++) 
    {
        if (file._normalizedPixels[i].blue <= 0.5)
        {
            _normalizedPixels[i].blue = 2 * _normalizedPixels[i].blue * file._normalizedPixels[i].blue;
        }
        else
            _normalizedPixels[i].blue = 1 - 2 * (1 - _normalizedPixels[i].blue) * (1 - file._normalizedPixels[i].blue);

        if (file._normalizedPixels[i].green <= 0.5)
        {
            _normalizedPixels[i].green = 2 * _normalizedPixels[i].green * file._normalizedPixels[i].green;
        }
        else
            _normalizedPixels[i].green = 1 - 2 * (1 - _normalizedPixels[i].green) * (1 - file._normalizedPixels[i].green);

        if (file._normalizedPixels[i].red <= 0.5)
        {
            _normalizedPixels[i].red = 2 * _normalizedPixels[i].red * file._normalizedPixels[i].red;
        }
        else
            _normalizedPixels[i].red = 1 - 2 * (1 - _normalizedPixels[i].red) * (1 - file._normalizedPixels[i].red);
    }

    // Once done, denormalize + clear normalized pixel vector
    this->DeNormalize();

    this->_normalizedPixels.clear();
}

/*
Just adding a value to a channel

If over 255, clamp it to 255, else, just add as usual
*/
void ImageProcessing::AddChannel(char channel, unsigned char value)
{
    if (channel == 'B') 
    {
        for (unsigned int i = 0; i < _numberOfPixels; i++) 
        {
            int range = (int)value + _pixels[i].blue;

            if (range > 255) 
                _pixels[i].blue = 255;
            else 
                _pixels[i].blue += value;
        }
    }

    if (channel == 'G') 
    {
        for (unsigned int i = 0; i < _numberOfPixels; i++) 
        {
            int range = (int)value + _pixels[i].green;

            if (range > 255) 
                _pixels[i].green = 255;
            else 
                _pixels[i].green += value;
        }
    }

    if (channel == 'R') 
    {
        for (unsigned int i = 0; i < _numberOfPixels; i++) 
        {
            int range = (int)value + _pixels[i].red;

            if (range > 255) 
                _pixels[i].red = 255;
            else 
                _pixels[i].red += value;
        }
    }
}

/*
Just scaling (multiplying) the channel

If over 255, clamp it to 255, else, just multiply as usual
*/
void ImageProcessing::ScaleChannel(char channel, unsigned char value)
{
    if (channel == 'B') 
    {
        for (unsigned int i = 0; i < _numberOfPixels; i++) 
        {
            int range = (int)value * _pixels[i].blue;

            if (range > 255) 
                _pixels[i].blue = 255;
            else 
                _pixels[i].blue *= value;
        }
    }

    if (channel == 'G') 
    {
        for (unsigned int i = 0; i < _numberOfPixels; i++) 
        {
            int range = (int)value * _pixels[i].green;

            if (range > 255) 
                _pixels[i].green = 255;
            else 
                _pixels[i].green *= value;
        }
    }

    if (channel == 'R') 
    {
        for (unsigned int i = 0; i < _numberOfPixels; i++) 
        {
            int range = (int)value * _pixels[i].red;

            if (range > 255) 
                _pixels[i].red = 255;
            else 
                _pixels[i].red *= value;
        }
    }
}

// Clone the value of the selected channel to the other two channels
void ImageProcessing::CloneChannel(char channel)
{
    if (channel == 'B')
    {
        for (unsigned int i = 0; i < _numberOfPixels; i++)
        {
            _pixels[i].green = _pixels[i].blue;

            _pixels[i].red = _pixels[i].blue;
        }
    }

    if (channel == 'G')
    {
        for (unsigned int i = 0; i < _numberOfPixels; i++)
        {
            _pixels[i].blue = _pixels[i].green;

            _pixels[i].red = _pixels[i].green;
        }
    }

    if (channel == 'R')
    {
        for (unsigned int i = 0; i < _numberOfPixels; i++)
        {
            _pixels[i].blue = _pixels[i].red;

            _pixels[i].green = _pixels[i].red;
        }
    }
}

// The data from the given layer is the channel of the new image
void ImageProcessing::SetChannel(char channel, ImageProcessing& file)
{
    if (channel == 'B') 
    {
        for (unsigned int i = 0; i < _numberOfPixels; i++)
            _pixels[i].blue = file._pixels[i].blue;
    }

    if (channel == 'G') 
    {
        for (unsigned int i = 0; i < _numberOfPixels; i++)
            _pixels[i].green = file._pixels[i].green;
    }

    if (channel == 'R') 
    {
        for (unsigned int i = 0; i < _numberOfPixels; i++)
            _pixels[i].red = file._pixels[i].red;
    }
}

// To normalize, convert your 0-255 value to a 0-1 value (dividing the original by the maximum)
void ImageProcessing::Normalize()
{
    for (unsigned int i = 0; i < _numberOfPixels; i++) 
    {
        float blue = (float)_pixels[i].blue / 255;

        float green = (float)_pixels[i].green / 255;

        float red = (float)_pixels[i].red / 255;

        // Add the newly converted pixels to the normalized pixel vector
        _normalizedPixels.push_back(NormalizedPixel{blue,green,red});
    }
}

// To denormalize, convert your 0-1 value to a 0-255 value (dividing the original by the maximum)
void ImageProcessing::DeNormalize()
{
    for (unsigned int i = 0; i < _numberOfPixels; i++) 
    {
        // To round, simply add 0.5f to the final value before assigning back to an unsigned char variable
        _normalizedPixels[i].blue = (_normalizedPixels[i].blue * 255) + 0.5f;

        _normalizedPixels[i].green = (_normalizedPixels[i].green * 255) + 0.5f;

        _normalizedPixels[i].red = (_normalizedPixels[i].red * 255) + 0.5f;

        // Add the newly converted pixels to the regular pixel vector
        _pixels[i].blue = (unsigned char)_normalizedPixels[i].blue;

        _pixels[i].green = (unsigned char)_normalizedPixels[i].green;

        _pixels[i].red = (unsigned char)_normalizedPixels[i].red;
    }
}

// Rotate the image 180 degrees
void ImageProcessing::RotateImage()
{
    // Placeholder pixel
    Pixel temporaryPixel;

    for (unsigned int i = 0; i < _numberOfPixels / 2; i++) 
    {
        temporaryPixel = _pixels[i];

        _pixels[i] = _pixels[_numberOfPixels - 1 - i];

        _pixels[_numberOfPixels - 1 - i] = temporaryPixel;
    }
}
