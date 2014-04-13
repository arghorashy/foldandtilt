//
//  main.cpp
//  FoldAndTilt
//
//  Created by Afsheen Ghorashy on 3/16/2014.
//  Copyright (c) 2014 Afsheen Ghorashy. All rights reserved.
//

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <string>

using namespace cv;

// Function headers
void scaleImage(InputArray _src, OutputArray _dst);
void getFoldAndTiltParams(Mat& image, bool showResult, Rect& face, Rect& lefteye, Rect& righteye, Rect& mouth);
void returnNBiggestRectangles(std::vector<Rect>& in, std::vector<Rect>& out, int num);
void getReliefAfterFold(Mat image, Mat& relief, Rect face, Rect lefteye, Rect righteye, Rect mouth, bool debugdisplay);
void extendLinePointsToEdges(Rect face, Point& p1, Point& p2);
void foldAndTilt(Mat imageIn, Mat& imageOut, Mat relief, Rect face, double d, double alph, int intensity);

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
    string outPath;
    int intensity;
    
    // Load cascades
    if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading cascade\n"); return -1; };
    if( !eyesleft_cascade.load( eyeleft_cascade_name ) ){ printf("--(!)Error loading cascade\n"); return -1; };
    if( !eyesright_cascade.load( eyeright_cascade_name ) ){ printf("--(!)Error loading cascade\n"); return -1; };
    if( !mouth_cascade.load( mouth_cascade_name ) ){ printf("--(!)Error loading cascade\n"); return -1; };
    
    // Expect image file as argument
    // Good resource for getting Xcode 4 to put in command-line arguments on-the-fly
    // http://stackoverflow.com/questions/5025256/how-do-you-specify-command-line-arguments-in-xcode-4
    if (argc != 4)
    {
        printf("Expected usage: FoldAndTilt.out <picture.jpg> <outpath.jpg> <intensity value [-100,100]>");
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
        
        outPath = argv[2];
        
        string strIntensity = argv[3];
        intensity = atoi(strIntensity.c_str());
        
        
        
        
    }
    
    //printf("%i\n", image.type());
    
    // Scale image
    scaleImage(image, image);
    
    Rect mouth, face, lefteye, righteye;
    getFoldAndTiltParams(image, false, face, lefteye, righteye, mouth);
    
    Mat relief;
    getReliefAfterFold(image, relief, face, lefteye, righteye, mouth, false);
    
    foldAndTilt(image, image, relief, face, 1000.0, 0.75, intensity);
    
    
    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    compression_params.push_back(95);
    imwrite(outPath, image, compression_params);

    
    return 0; 
}

void foldAndTilt(Mat imageIn, Mat& imageOut, Mat relief, Rect face, double d, double alph, int intensity)
{
    imageOut = imageIn.clone();
    
    double lrfactor = 0;
    double minVal;
    double maxVal;
    Point minLoc;
    Point maxLoc;
    minMaxLoc( abs(relief-127), &minVal, &maxVal, &minLoc, &maxLoc );
    // normalised for relative values of relief mask and for size of face.
    lrfactor =  ((4.0 / maxVal) / 188) * face.height * intensity / -100;
    
    for (int x = 0; x < imageIn.cols; x++)
    {
        for (int y = 0; y < imageIn.rows; y++)
        {
            /*
            double lr = relief.at<uchar>(y,x) - 127; // local relief
            int yfc  = int(imageOut.cols * 0.5 - y); // the centre being 0
            double yorig = yfc * d * cos(alph) / (d + lr);
            int yft = int(yorig + imageOut.cols * 0.5); // back to measuring from top
            printf("%i\n", yft);
            imageOut.at<uchar>(y,x) = imageIn.at<uchar>(int(yft),x);
            */
            
            int lr = relief.at<uchar>(y,x) - 127; // local relief
            lr = int(lr*lrfactor);
            if (y+lr < imageIn.cols && y+lr > 0)
            {
                //printf("%i\n", imageIn.at<Vec3f>(y+lr, x)[0]);
                imageOut.at<Vec3b>(y,x)[0] = imageIn.at<Vec3b>(y+lr, x)[0];
                imageOut.at<Vec3b>(y,x)[1] = imageIn.at<Vec3b>(y+lr, x)[1];
                imageOut.at<Vec3b>(y,x)[2] = imageIn.at<Vec3b>(y+lr, x)[2];
            }
            
            
            
        }
    }
    
    
    namedWindow("Display Image", WINDOW_AUTOSIZE );
    imshow("Display Image", imageOut);
    
    waitKey(0);
    
}

void extendLinePointsToEdges(Rect face, Point& p1, Point& p2)
{
    // diff in dir of P1
    Point diff = p1 - p2;
    Point temp;
    
    temp = p1;
    while (temp.x > 0 && temp.x < face.width && temp.y > 0 && temp.y < face.height)
    {
        temp += diff * 0.01;
    }
    p1 = temp;
    
    temp = p2;
    while (temp.x > 0 && temp.x < face.width && temp.y > 0 && temp.y < face.height)
    {
        temp -= diff * 0.01;
    }
    p2 = temp;
    
    

}

void getReliefAfterFold(Mat image, Mat& relief, Rect face, Rect lefteye, Rect righteye, Rect mouth, bool debugdisplay)
{
    
    relief = Mat::ones(image.size(), CV_8U);
    relief *= 127;
    
    
    Point lefteyePt(lefteye.x + lefteye.width*0.5, lefteye.y + lefteye.height*0.5);
    Point leftmouthPt(mouth.x, mouth.y + mouth.height*0.5);
    Point righteyePt(righteye.x + righteye.width*0.5, righteye.y + righteye.height*0.5);
    Point rightmouthPt(mouth.x + mouth.width, mouth.y + mouth.height*0.5);
    Point ctreyePt = (lefteyePt + righteyePt) * 0.5;
    Point ctrmouthPt = (leftmouthPt + rightmouthPt) * 0.5;
    
    extendLinePointsToEdges(face, lefteyePt, leftmouthPt);
    extendLinePointsToEdges(face, righteyePt, rightmouthPt);
    extendLinePointsToEdges(face, ctreyePt, ctrmouthPt);
    
    Point facePt(face.x, face.y);
    lefteyePt += facePt;
    leftmouthPt += facePt;
    righteyePt += facePt;
    rightmouthPt += facePt;
    ctreyePt += facePt;
    ctrmouthPt += facePt;
    
    
    //line(image, lefteyePt, leftmouthPt, Scalar( 0, 0, 0 ), 3, 8, 0);
    //line(image, righteyePt, rightmouthPt, Scalar( 0, 0, 0 ), 3, 8, 0);
    //line(image, ctreyePt, ctrmouthPt, Scalar( 255, 255, 255 ), 3, 8, 0);
    
    line(relief, lefteyePt, leftmouthPt, Scalar( 0, 0, 0 ), int(lefteye.width * 0.1), 8, 0);
    line(relief, righteyePt, rightmouthPt, Scalar( 0, 0, 0 ), int(righteye.width * 0.1), 8, 0);
    //line(relief, ctreyePt, ctrmouthPt, Scalar( 255, 255, 255 ), 3, 8, 0);
    

    if (debugdisplay)
    {
    
        //namedWindow("Display Image", WINDOW_AUTOSIZE );
        //imshow("Display Image", image);
    
        //waitKey(0);
        
        namedWindow("Display Image", WINDOW_AUTOSIZE );
        imshow("Display Image", relief);
        
        waitKey(0);
        
    }
    
    Mat blurfilter = Mat::ones(Size(5,1), CV_32F) * 0.2;
    
    for (int j = 0; j < 40; j++) filter2D(relief, relief, -1 , blurfilter, Point(-1,-1), 0, BORDER_DEFAULT );
    
    
    if (debugdisplay)
    {
        
        //namedWindow("Display Image", WINDOW_AUTOSIZE );
        //imshow("Display Image", image);
        
        //waitKey(0);
        
        namedWindow("Display Image", WINDOW_AUTOSIZE );
        imshow("Display Image", relief);
        
        waitKey(0);
        
    }
    

    
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

void returnNBiggestRectangles(std::vector<Rect>& in, std::vector<Rect>& out, int num)
{
    
    Rect r = Rect(0,0,0,0);
    
    if (num >= in.size())
    {
        out = in;
        return;
    }
    else if (num == 0)
    {
        return;
    }
    else
    {
        for (int j = 0; j < num; j++)
        {
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
            out.push_back(r);
            in.erase(in.begin() + ind);
        }
    }
}


// Can speed this up by doing some sort of binary search...
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
                if (i != 0)
                {
                    i--;
                    cascade.detectMultiScale(im, found, 1.1, i, 0, Size(0,0), Size(0,0));
                
                    // Resort to getting n biggest ones, if cannot find a value of i
                    // for which n instances are found.
                    std::vector<Rect> biggest;
                    returnNBiggestRectangles(found, biggest, num);
                    found = biggest;
                    return;
                }
                else return;
                
            }
        }
    }
}

void getFoldAndTiltParams(Mat& image, bool showResult, Rect& face, Rect& lefteye, Rect& righteye, Rect& mouth)
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
    if (faces.size() == 1)
    {
        face = faces[0];
    }
    else
    {
        printf("Found %i face(s); expected 1.\n", int(faces.size()));
        exit(-1);
    }
    
    Mat face_image = image_gray(faces[0]);
    
    if (showResult)
    {
        for( size_t i = 0; i < faces.size(); i++ )
        {
            Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
            ellipse( image, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
        }
    }
    

    // Apply Haar cascade filter for eyes  *******************************************************
    
    std::vector<Rect> eyes;
    detectN(face_image, eyes, eyesright_cascade, 2);
    if (eyes.size() != 2)
    {
        printf("Found %i eye(s); expected 2.\n", int(eyes.size()));
        exit(-1);
    }
    else
    {
        if (eyes[0].x < eyes[1].x)
        {
            lefteye = eyes[0];
            righteye = eyes[1];
        }
        else
        {
            righteye = eyes[0];
            lefteye = eyes[1];
        }
    }
    
    
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
    detectN(face_image, mouths, mouth_cascade, 1);
    if (mouths.size() != 1)
    {
        printf("Found %i mouth(s); expected 1.\n", int(mouths.size()));
        exit(-1);
    }
    else
    {
        mouth = mouths[0];
    }
    
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