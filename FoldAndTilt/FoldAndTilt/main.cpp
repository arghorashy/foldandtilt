//
//  main.cpp
//  FoldAndTilt
//
//  Created by Afsheen Ghorashy on 3/16/2014.
//  Copyright (c) 2014 Afsheen Ghorashy. All rights reserved.
//


// Example showing how to read and write images
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cvaux.h>

// http://stackoverflow.com/questions/5025256/how-do-you-specify-command-line-arguments-in-xcode-4

int main(int argc, char** argv)
{
    IplImage * pInpImg = 0;
    
    
    // Expect image file as argument
    if (argc != 2)
    {
        printf("Expected usage: FoldAndTilt.out <picture.jpg>");
        return -1;
    }
    else
    {
        // Load an image from file - change this based on your image name
        pInpImg = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED);
        if(!pInpImg)
        {
            fprintf(stderr, "failed to load input image\n");
            return -1;
        }
    }
    
    // Write the image to a file with a different name,
    // using a different image format -- .png instead of .jpg
    if( !cvSaveImage("my_image_copy.png", pInpImg) )
    {
        fprintf(stderr, "failed to write image file\n");
    }
    
    // Remember to free image memory after using it!
    cvReleaseImage(&pInpImg);
    
    return 0;
}