#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

class ImageProcessing
{
public:

    //Header containing all of the relevant info
    struct Header
    {
        // Size of the Image ID field
        char idLength;
        // Is a color map included?
        char colorMapType;
        // Compressed? True Color? Grayscale?
        char dataTypeCode;
        // Color Map Origin – 0 in our case
        short colorMapOrigin;
        // Color Map Length – 0 in our case
        short colorMapLength;
        // Color Map Depth – 0 in our case 
        char colorMapDepth;
        // X-Origin – 0 in our case
        short xOrigin;
        // Y-Origin – 0 in our case 
        short yOrigin;
        // Image Width
        short width;
        // Image Height 
        short height;
        // Pixel depth – typically 8, 16, 24 or 32
        char bitsPerPixel;
        // Image Descriptor
        char imageDescriptor;
    };

    /*
    In a .TGA file, the colors are stored in reverse order, such that the first byte is the 
    blue component, the second byte is the green component, and the third byte is the red component.

    The pixel data is stored in unsigned char variables, with values from 0-255.
    */
    struct Pixel 
    {
        // The blue component
        unsigned char blue;
        // The green component
        unsigned char green;
        // The red component
        unsigned char red;
    };

    // For some operations (like multiplication), they work based on a normalized value, from 0-1.
    struct NormalizedPixel 
    {
        // The blue component
        float blue;
        // The green component
        float green;
        // The red component
        float red;
    };

    // File constructor
    ImageProcessing(string fileName);

    // Output the results to a new file
    void OutputTo(string fileName);

    // Checks if the file are a match
    bool EqualTo(ImageProcessing& file);

    // Blending modes
    void MultiplyBlending(ImageProcessing& file);

    void SubstractBlending(ImageProcessing& file);

    void ScreenBlending(ImageProcessing& file);

    void OverlayBlending(ImageProcessing& file);

    // Modify the individual channels
    void AddChannel(char channel, unsigned char value);

    void ScaleChannel(char channel, unsigned char value);

    void CloneChannel(char channel);

    void SetChannel(char channel, ImageProcessing& file); 

    // Normalize the pixels
    void Normalize();

    // Return the pixels back to normal
    void DeNormalize();

    // Rotate the image 180 degrees
    void RotateImage();

private:
    // Where the header information will be stored
    Header _header;
    
    // Number of pixels
    unsigned int _numberOfPixels;

    // Vector to store pixels
    vector<Pixel> _pixels;

    // Vector to store normalized pixels
    vector<NormalizedPixel> _normalizedPixels;
};
