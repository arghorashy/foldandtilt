//
//  main.cpp
//  FoldAndTilt
//
//  Created by Afsheen Ghorashy on 3/16/2014.
//  Copyright (c) 2014 Afsheen Ghorashy. All rights reserved.
//



#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;


int main(int argc, char** argv)
{
    Mat image;
    
    
    // Expect image file as argument
    // Good resource for getting Xcode 4 to put in command-line arguments on-the-fly
    // http://stackoverflow.com/questions/5025256/how-do-you-specify-command-line-arguments-in-xcode-4
    if (argc != 2)
    {
        printf("Expected usage: FoldAndTilt.out <picture.jpg>");
        return -1;
    }
    else
    {
        // Load an image from file - change this based on your image name
        image = imread( argv[1], 1 );
        if ( !image.data )
        {
            printf("No image data \n");
            return -1;
        }
    }
    
    //namedWindow("Display Image", WINDOW_AUTOSIZE );
    //imshow("Display Image", image);
    
    //waitKey(0);
    

    
    return 0;
}