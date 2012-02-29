#ifdef _CH_
#pragma package <opencv>
#endif

#define CV_NO_BACKWARD_COMPATIBILITY

#ifndef _EiC
#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#endif

//james sucks

IplImage *image = 0, *red = 0, *red_edge = 0, *green = 0, *green_edge = 0, *edge = 0;
IplImage *final = 0;
float low = 110.0f, high = 150.0f;

void doShit(IplImage*);

int main( int argc, char** argv )
{
    int c = 0, fps = 0;
    IplImage *frame = 0;

    //capture from camera
    CvCapture *capture = cvCaptureFromCAM(2);
    //quit if camera not found
    if(!capture) {
        printf("cannot init capture!\n");
        return -1;
    }

    //get fps from camera
    //fps = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);

    //display original video stream
    cvNamedWindow("stream", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("edges", CV_WINDOW_AUTOSIZE);
    cvMoveWindow("edges", 0, 400);

    //keep capturing frames until escape
    while(c != 27) //27 is escape key
    {
        //quit if can't grab frame
        if(!(frame = cvQueryFrame(capture))) break;

        //show the default image
        cvShowImage("stream", frame);

        doShit(frame);

        cvReleaseImage(&image);
        cvReleaseImage(&red);
        cvReleaseImage(&green);
        cvReleaseImage(&red_edge);
        cvReleaseImage(&green_edge);
        cvReleaseImage(&edge);

        c = cvWaitKey(10);
    }

    cvReleaseCapture(&capture);

    return 0;
}


void doShit(IplImage *image)
{

    //cvNamedWindow("orig", CV_WINDOW_AUTOSIZE);
    //cvShowImage("orig", image);

    // Extract red channel of image
    red = cvCreateImage(cvSize(image->width,image->height), IPL_DEPTH_8U, 1);
    green = cvCreateImage(cvSize(image->width,image->height), IPL_DEPTH_8U, 1);
    cvSplit(image, NULL, green, red, NULL);
    
    red_edge = cvCreateImage(cvSize(image->width,image->height), IPL_DEPTH_8U, 1);
    green_edge = cvCreateImage(cvSize(image->width,image->height), IPL_DEPTH_8U, 1);
    cvCanny(red, red_edge, low, high, 3);
    cvCanny(green, green_edge, low, high, 3);

    edge = cvCreateImage(cvSize(image->width,image->height), IPL_DEPTH_8U, 1);

    cvAbsDiff(red_edge, green_edge, edge);


    final = cvCreateImage(cvSize((image->width&-2)/2,(image->height&-2)/2), IPL_DEPTH_8U, 1);

    cvPyrDown(edge, final, CV_GAUSSIAN_5x5);
    cvSmooth(final, final, CV_BLUR, 3, 0, 0, 0);
    cvCanny(final, final, 1.0f, 250.0f, 3);
    cvPyrUp(final, edge, CV_GAUSSIAN_5x5);
    //cvResize(edge, final, CV_INTER_AREA);
    //cvCanny(final, final, 100.0f, 200.0f, 3);
    //cvResize(final, edge, CV_INTER_AREA);
    //cvErode(edge, edge, cvCreateStructuringElementEx(1,1,0,0,CV_SHAPE_RECT,NULL), 1);
    //cvSmooth(edge, edge, CV_GAUSSIAN, 3, 0, 0, 0);
    //cvCanny(edge, edge, low, high, 3);

    //cvNamedWindow("red channel", CV_WINDOW_AUTOSIZE);
    //cvShowImage("red channel", red);

    //cvNamedWindow("edges", CV_WINDOW_AUTOSIZE);
    //cvShowImage("edges", edge);

    //cvNamedWindow("final", CV_WINDOW_AUTOSIZE);
    cvShowImage("edges", final);

    //cvWaitKey(0);
    /*
    cvReleaseImage(&image);
    cvReleaseImage(&red);
    cvReleaseImage(&green);
    cvReleaseImage(&red_edge);
    cvReleaseImage(&green_edge);
    cvReleaseImage(&edge);
    */
}

#ifdef _EiC
main(1,"chalk.c");
#endif
