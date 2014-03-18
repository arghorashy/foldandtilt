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

// Function headers
void scaleImage(InputArray _src, OutputArray _dst);
void getFoldAndTiltParams(const Mat& image, bool showResult);

/** Global variables */
String face_cascade_name = "../../../../../assets/haarcascade_frontalface_alt.xml";
String eyeleft_cascade_name = "../../../../../assets/haarcascade_mcs_lefteye.xml";
String eyeright_cascade_name = "../../../../../assets/haarcascade_mcs_righteye.xml";
String mouth_cascade_name = "../../../../../assets/haarcascade_mcs_mouth.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyesleft_cascade;
CascadeClassifier eyesright_cascade;
CascadeClassifier mouth_cascade;
// RNG rng(12345);

int main(int argc, char** argv)
{
    Mat image;
    
    // Load cascades
    if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading cascade\n"); return -1; };
    if( !eyesleft_cascade.load( eyeleft_cascade_name ) ){ printf("--(!)Error loading cascade\n"); return -1; };
    if( !eyesright_cascade.load( eyeright_cascade_name ) ){ printf("--(!)Error loading cascade\n"); return -1; };
    if( !mouth_cascade.load( mouth_cascade_name ) ){ printf("--(!)Error loading cascade\n"); return -1; };
    
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
    
    // Scale image
    scaleImage(image, image);
    
    //
    getFoldAndTiltParams(image, true);

    
    return 0;
}

void scaleImage(InputArray _src, OutputArray _dst)
{
    int maxdim = 500;
    double scalefactor = 1;
    
    Mat src = _src.getMat();
    
    Size srcSize = src.size();
    if (srcSize.width > maxdim || srcSize.height > maxdim)
    {
        if (srcSize.width > srcSize.height)
        {
            scalefactor = double(maxdim) / double(srcSize.width);
        }
        else
        {
            scalefactor = double(maxdim) / double(srcSize.height);
        }
    }
    
    if (scalefactor != 1)
    {
        Size newSize = Size(double(srcSize.width) * scalefactor, double(srcSize.height) * scalefactor);
        _dst.create(newSize, src.type());
        Mat dst = _dst.getMat();
        
        resize(src, dst, dst.size(), 0, 0, INTER_AREA);
    }
    
    
    
    
    
}

void getFoldAndTiltParams(const Mat& image, bool showResult)
{
    std::vector<Rect> faces;
    
    // Apply Haar cascade filter for face
    // Try playing with these parameters...
    // Why must it be 1.1 and not 1.0
    face_cascade.detectMultiScale(image, faces, 1.1, 3, 0, Size(30,30), Size(30,30));
    
    
    
    
    if (showResult)
    {
        namedWindow("Display Image", WINDOW_AUTOSIZE );
        imshow("Display Image", image);
        
        waitKey(0);
    }
    
    
}