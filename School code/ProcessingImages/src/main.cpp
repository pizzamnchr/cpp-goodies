#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "ImageProcessing.h"

using namespace std;

/*
1. Read in a number .TGA files in a binary format

2. Process the image data store within those files in a variety of ways

3. Write out new .TGA files in the same binary format
*/

// 1. Use the Multiply blending mode to combine “layer1.tga” (top layer) with “pattern1.tga" (bottom layer).
bool Part1Test()
{
    // The top layer
    ImageProcessing topLayer("input/layer1.tga");

    // The bottom layer
    ImageProcessing bottomLayer("input/pattern1.tga");

    // Example file to be compared to
    ImageProcessing compareTo("examples/EXAMPLE_part1.tga");

    // Use the Multiply blending mode
    topLayer.MultiplyBlending(bottomLayer);

    // If the files compared are a match, output it + return true + display success message, else, return false + display fail message
    if (topLayer.EqualTo(compareTo))
    {
        topLayer.OutputTo("output/part1.tga");

        return true;
    }
    else 
    {
        cout << "Task 1 failed" << endl;

        return false;
    }
}

/*
2. Use the Subtract blending mode to combine “layer2.tga” (top layer) with “car.tga” (bottom 
layer). This mode subtracts the top layer from the bottom layer. 
*/
bool Part2Test()
{
    // The top layer
    ImageProcessing topLayer("input/layer2.tga");

    // The bottom layer
    ImageProcessing bottomLayer("input/car.tga");

    // Example file to be compared to
    ImageProcessing compareTo("examples/EXAMPLE_part2.tga");

    // Use the Subtract blending mode
    bottomLayer.SubstractBlending(topLayer);

    // If the files compared are a match, output it + return true + display success message, else, return false + display fail message
    if (bottomLayer.EqualTo(compareTo))
    {
        bottomLayer.OutputTo("output/part2.tga");

        return true;
    }
    else 
    {
        cout << "Task 2 failed" << endl;

        return false;
    }
}

/*
3. Use the Multiply blending mode to combine “layer1.tga” with “pattern2.tga”, and store the
results temporarily. Load the image “text.tga” and, using that as the top layer, combine it with
the previous results of layer1/pattern2 using the Screen blending mode.
*/
bool Part3Test()
{
    // The top layer
    ImageProcessing topLayer("input/layer1.tga");

    // The bottom layer
    ImageProcessing bottomLayer("input/pattern2.tga");

    // The final top layer
    ImageProcessing finalTopLayer("input/text.tga");

    // Example file to be compared to
    ImageProcessing compareTo("examples/EXAMPLE_part3.tga");

    // Use the Multiply blending mode
    topLayer.MultiplyBlending(bottomLayer);

    // Use the Screen blending mode
    finalTopLayer.ScreenBlending(topLayer);

    // If the files compared are a match, output it + return true + display success message, else, return false + display fail message
    if (finalTopLayer.EqualTo(compareTo))
    {
        finalTopLayer.OutputTo("output/part3.tga");

        return true;
    }
    else 
    {
        cout << "Task 3 failed" << endl;
        return false;
    }
}

/*
4. Multiply “layer2.tga” with “circles.tga”, and store it. Load “pattern2.tga” and, using that as the
top layer, combine it with the previous result using the Subtract blending mode.
*/
bool Part4Test()
{
    // The top layer
    ImageProcessing topLayer("input/layer2.tga");

    // The bottom layer
    ImageProcessing bottomLayer("input/circles.tga");

    // The final top layer
    ImageProcessing finalTopLayer("input/pattern2.tga");

    // Example file to be compared to
    ImageProcessing compareTo("examples/EXAMPLE_part4.tga");

    // Use the Multiply blending mode
    bottomLayer.MultiplyBlending(topLayer);

    // Use the Subtract blending mode
    bottomLayer.SubstractBlending(finalTopLayer);

    // If the files compared are a match, output it + return true + display success message, else, return false + display fail message
    if (bottomLayer.EqualTo(compareTo))
    {
        bottomLayer.OutputTo("output/part4.tga");

        return true;
    }
    else 
    {
        cout << "Task 4 failed" << endl;

        return false;
    }
}

// 5. Combine “layer1.tga” (as the top layer) with “pattern1.tga” using the Overlay blending mode. 
bool Part5Test()
{
    // The top layer
    ImageProcessing topLayer("input/layer1.tga");

    // The bottom layer
    ImageProcessing bottomLayer("input/pattern1.tga");

    // Example file to be compared to
    ImageProcessing compareTo("examples/EXAMPLE_part5.tga");

    // Use the Overlay blending mode
    topLayer.OverlayBlending(bottomLayer);

    // If the files compared are a match, output it + return true + display success message, else, return false + display fail message
    if (topLayer.EqualTo(compareTo))
    {
        topLayer.OutputTo("output/part5.tga");

        return true;
    }
    else 
    {
        cout << "Task 5 failed" << endl;

        return false;
    }
}

// 6. Load “car.tga” and add 200 to the green channel.
bool Part6Test()
{
    // The top layer
    ImageProcessing topLayer("input/car.tga");

    // Example file to be compared to
    ImageProcessing compareTo("examples/EXAMPLE_part6.tga");

    // Add 200 to the green channel
    topLayer.AddChannel('G', 200);

    // If the files compared are a match, output it + return true + display success message, else, return false + display fail message
    if (topLayer.EqualTo(compareTo))
    {
        topLayer.OutputTo("output/part6.tga");

        return true;
    }
    else 
    {
        cout << "Task 6 failed" << endl;

        return false;
    }
}

/*
7. Load “car.tga” and scale (multiply) the red channel by 4, and the blue channel by 0. This will
increase the intensity of any red in the image, while negating any blue it may have.
*/
bool Part7Test()
{
    // The top layer
    ImageProcessing topLayer("input/car.tga");

    // Example file to be compared to
    ImageProcessing compareTo("examples/EXAMPLE_part7.tga");

    // Scale (multiply) the red channel by 4
    topLayer.ScaleChannel('R', 4);

    // Scale (multiply) the blue channel by 0
    topLayer.ScaleChannel('B', 0);

    // If the files compared are a match, output it + return true + display success message, else, return false + display fail message
    if (topLayer.EqualTo(compareTo)) 
    {
        topLayer.OutputTo("output/part7.tga");

        return true;
    }
    else 
    {
        cout << "Task 7 failed" << endl;

        return false;
    }
}

/*
8. Load “car.tga” and write each channel to a separate file: the red channel should be 
“part8_r.tga”, the green channel should be “part8_g.tga”, and the blue channel should be 
“part8_b.tga” 
*/
bool Part8Test()
{
    // The layer for the blue channel
    ImageProcessing blueLayer("input/car.tga");

    // The layer for the green channel
    ImageProcessing greenLayer("input/car.tga");

    // The layer for the red channel
    ImageProcessing redLayer("input/car.tga");

    // Example blue layer to be compared to
    ImageProcessing CompareToBlueLayer("examples/EXAMPLE_part8_b.tga");

    // Example green layer to be compared to
    ImageProcessing CompareToGreenLayer("examples/EXAMPLE_part8_g.tga");

    // Example red layer to be compared to
    ImageProcessing CompareToRedLayer("examples/EXAMPLE_part8_r.tga");

    // Clone the value of the blue channel to the other two channels
    blueLayer.CloneChannel('B');

    // Clone the value of the green channel to the other two channels
    greenLayer.CloneChannel('G');

    // Clone the value of the red channel to the other two channels
    redLayer.CloneChannel('R');

    // If the files compared are a match, output it + return true + display success message, else, return false + display fail message
    if (blueLayer.EqualTo(CompareToBlueLayer) && greenLayer.EqualTo(CompareToGreenLayer) && redLayer.EqualTo(CompareToRedLayer))
    {
        blueLayer.OutputTo("output/part8_b.tga");

        greenLayer.OutputTo("output/part8_g.tga");

        redLayer.OutputTo("output/part8_r.tga");

        return true;
    }
    else 
    {
        cout << "Task 8 failed" << endl;

        return false;
    }
}

/*
9. Load “layer_red.tga”, “layer_green.tga” and “layer_blue.tga”, and combine the three files into 
one file. The data from “layer_red.tga” is the red channel of the new image, layer_green is 
green, and layer_blue is blue.
*/
bool Part9Test()
{
    // The layer for the blue channel
    ImageProcessing blueLayer("input/layer_blue.tga");

    // The layer for the green channel
    ImageProcessing greenLayer("input/layer_green.tga");

    // The layer for the red channel
    ImageProcessing redLayer("input/layer_red.tga");

    // Example file to be compared to
    ImageProcessing compareTo("examples/EXAMPLE_part9.tga");

    // The data from “layer_red.tga” is the red channel of the new image
    blueLayer.SetChannel('R', redLayer);

    // The data from "layer_green.tga" is the green channel of the new image
    blueLayer.SetChannel('G', greenLayer);

    // If the files compared are a match, output it + return true + display success message, else, return false + display fail message
    if (blueLayer.EqualTo(compareTo)) 
    {
        blueLayer.OutputTo("output/part9.tga");

        return true;
    }
    else 
    {
        cout << "Task 9 failed" << endl;

        return false;
    }
}

/*
10. Load “text2.tga”, and rotate it 180 degrees, flipping it upside down. This is easier than you think! 
Try diagramming the data of an image (such as earlier in this document). What would the data 
look like if you flipped it? Now, how to write some code to accomplish that...? 
*/
bool Part10Test()
{
    // The layer for the text
    ImageProcessing textLayer("input/text2.tga");

    // Example file to be compared to
    ImageProcessing compareTo("examples/EXAMPLE_part10.tga");

    // Rotate the image 180 degrees, flipping it upside down
    textLayer.RotateImage();

    // If the files compared are a match, output it + return true + display success message, else, return false + display fail message
    if (textLayer.EqualTo(compareTo)) 
    {
        textLayer.OutputTo("output/part10.tga");

        return true;
    }
    else 
    {
        cout << "Task 10 failed" << endl;

        return false;
    }
}

// Run ze tests
int main()
{

    if (Part1Test())
        cout << "Task 1 done" << endl;

    if (Part2Test())
        cout << "Task 2 done" << endl;

    if (Part3Test())
        cout << "Task 3 done" << endl;

    if (Part4Test())
        cout << "Task 4 done" << endl;

    if (Part5Test())
        cout << "Task 5 done" << endl;

    if (Part6Test())
        cout << "Task 6 done" << endl;

    if (Part7Test())
        cout << "Task 7 done" << endl;

    if (Part8Test())
        cout << "Task 8 done" << endl;

    if (Part9Test())
        cout << "Task 9 done" << endl;

    if (Part10Test())
        cout << "Task 10 done" << endl;
}


