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
void getFoldAndTiltParams(Mat& image, bool showResult);
Rect returnNBiggestRectangles(std::vector<Rect>& in, std::vector<Rect>& out, int num);

/** Global variables */
String face_cascade_name = "../../../../../assets/haarcascade_frontalface_default.xml";
String eyeleft_cascade_name = "../../../../../assets/haarcascade_mcs_lefteye.xml";
String eyeright_cascade_name = "../../../../../assets/haarcascade_mcs_righteye.xml";
String mouth_cascade_name = "../../../../../assets/Mouth.xml";
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

Rect returnNBiggestRectangles(std::vector<Rect>& in, std::vector<Rect>& out, int num)
{
    Rect r = Rect(0,0,0,0);
    
    if (num >= in.size())
    {
        out = in;
        return r;
    }
    else if (num > 0)
    {
        if (num > 1)
        {
            r = returnNBiggestRectangles(in, out, num-1);
            out.push_back(r);
        }
        
        // Do comparison and return largest
        int w = 0, h = 0, ind;
        for( size_t i = 0; i < in.size(); i++ )
        {
            if (in[i].width > w || in[i].height > h)
            {
                w = in[i].width;
                h = in[i].height;
                r = in[i];
                ind = int(i);
            }
        }
        in.erase(in.begin() + ind + 1);
        return r;
        
    }
    else
    {
        printf("Invalid value for parameter num, for function returnNBiggestRectangles");
        exit(-1);
    }

    
}

void detectN(InputArray _src, std::vector<Rect>& found, CascadeClassifier cascade, int num)
{
    Mat im = _src.getMat();
    
    // repeat cascade with stricter rejection until only 1 face is found.
    for (int i = 0; i < 200; i++)
    {
        cascade.detectMultiScale(im, found, 1.1, i, 0, Size(0,0), Size(0,0));
        if (found.size() == num)
        {
            return;
        }
        else if (found.size() < num)
        {
            if (i == 0)
            {
                return;
            }
            else
            {
                i--;
                cascade.detectMultiScale(im, found, 1.1, i, 0, Size(0,0), Size(0,0));
                
                std::vector<Rect> biggest;
                
                
                returnNBiggestRectangles(found, biggest, num);
                found = biggest;
                return;
                
            }
        }
    }
}

void getFoldAndTiltParams(Mat& image, bool showResult)
{
    Mat image_gray;
    
    cvtColor( image, image_gray, CV_BGR2GRAY );
    equalizeHist( image_gray, image_gray );
    
    
    // Apply Haar cascade filter for face *******************************************************
    
    std::vector<Rect> faces;
    // Don't know why 3rd parameter must be 1.1, and not 1
    // If 4th parameter higher: more rejection
    // Set 5th and 6th parameter to Size(0,0) to use default
    detectN(image_gray, faces, face_cascade, 1);
    Rect face = faces[0];
    Mat face_image = image_gray(faces[0]);
    
    if (showResult)
    {
        for( size_t i = 0; i < faces.size(); i++ )
        {
            Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
            ellipse( image, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
        }
    }
    
    // Pick biggest face *************************************************************************
    /*
    Mat face_image;
    Rect face;
    int w = 0, h = 0;
    for( size_t i = 0; i < faces.size(); i++ )
    {
        if (faces[i].width > w || faces[i].height > h)
        {
            w = faces[i].width;
            h = faces[i].height;
            face = faces[i];
        }
    }
    if (w ==0 || h == 0)
    {
        printf("No faces found!");
        return;
    }
    else
    {
        face_image = image_gray(face);
    }
    */
    // Apply Haar cascade filter for eyes  *******************************************************
    
    std::vector<Rect> eyes;
    detectN(face_image, eyes, eyesright_cascade, 2);
    //eyesright_cascade.detectMultiScale(face_image, eyes, 1.1, 20, 0, Size(0,0), Size(0,0));
    if (showResult)
    {
        for( size_t i = 0; i < eyes.size(); i++ )
        {
            Point center( face.x + eyes[i].x + eyes[i].width*0.5, face.y + eyes[i].y + eyes[i].height*0.5 );
            ellipse( image, center, Size( eyes[i].width*0.5, eyes[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
        }
    }
    

    // Apply Haar cascade filter for mouth  *******************************************************
    
    std::vector<Rect> mouths;
    //mouth_cascade.detectMultiScale(face_image, mouths, 1.1, 10, 0, Size(0,0), Size(0,0));
    detectN(face_image, mouths, mouth_cascade, 1);
    if (showResult)
    {
        for( size_t i = 0; i < mouths.size(); i++ )
        {
            Point center( face.x + mouths[i].x + mouths[i].width*0.5, face.y + mouths[i].y + mouths[i].height*0.5 );
            ellipse( image, center, Size( mouths[i].width*0.5, mouths[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
        }
    }
    
    
    
    if (showResult)
    {
        namedWindow("Display Image", WINDOW_AUTOSIZE );
        imshow("Display Image", image);
        
        waitKey(0);
    }
    
    
}