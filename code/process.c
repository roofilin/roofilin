#ifdef _CH_
#pragma package <opencv>
#endif

#define CV_NO_BACKWARD_COMPATIBILITY

#ifndef _EiC
#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#endif

IplImage *frame = 0;
IplImage *image = 0, *red = 0, *red_edge = 0, *green = 0, *green_edge = 0, *edge = 0;
IplImage *final = 0;
float low = 110.0f, high = 150.0f;
float cog_x = 0.0, cog_y = 0.0;

IplImage *gray = 0, *bw = 0;
int flag = 0;

void doShit(IplImage*);

void COG(float*, float*);

void bullshit()
{
    //IplImage *gray = 0, *bw = 0;
    gray = cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 1);
    cvCvtColor(frame, gray, CV_BGR2GRAY);
    cvThreshold(gray, bw, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

    
    float intensity = 0.0, accum = 0.0;
    int x, y;
    for(x = 0; x < final->width; x++)
    {
        for(y = 0; y < final->height; y++)
        {
            intensity = (float)(cvGetReal2D(final, y, x));
            accum += intensity;
        }
    }

    if(accum > 2000) flag = 1;
    else flag = 0;
}

int main( int argc, char** argv )
{
    int c = 0, fps = 0;

    //capture from camera
    CvCapture *capture = cvCaptureFromCAM(1);
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
    cvMoveWindow("edges", 400, 0);


        CvFont font;
        cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0, 0, 2, CV_AA);

    //keep capturing frames until escape
    while(c != 27) //27 is escape key
    {
        //quit if can't grab frame
        if(!(frame = cvQueryFrame(capture))) break;

        //show the default image
//        cvShowImage("stream", frame);

        //edge detection - todo: HSV color filtering
        doShit(frame);


        //edge detection
        bullshit(frame);

        if(flag)
            cvPutText(frame, "edge", cvPoint(10, 400), &font, cvScalar(0, 255, 0, 0));

        //display center of gravity in coordinates
        COG(&cog_x, &cog_y);
        char x_coord[10];
        char y_coord[10];
        sprintf(x_coord, "%1.2f", cog_x);
        sprintf(y_coord, "%1.2f", cog_y);
        printf("%s\n", x_coord);
        printf("%s\n", y_coord);
        
        //CvFont font;
        //cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0, 0, 2, CV_AA);

        cvPutText(frame, x_coord, cvPoint(10,300), &font, cvScalar(0, 255, 0, 0));
        cvPutText(frame, y_coord, cvPoint(10,350), &font, cvScalar(0, 255, 0, 0));


        //avoid memory leaks
        /*
        cvReleaseImage(&image);
        cvReleaseImage(&red);
        cvReleaseImage(&green);
        cvReleaseImage(&red_edge);
        cvReleaseImage(&green_edge);
        cvReleaseImage(&edge);
        */
        
        cvShowImage("stream", frame);
        c = cvWaitKey(10);
    }

    cvReleaseCapture(&capture);
        //avoid memory leaks
        cvReleaseImage(&image);
        cvReleaseImage(&red);
        cvReleaseImage(&green);
        cvReleaseImage(&red_edge);
        cvReleaseImage(&green_edge);
        cvReleaseImage(&edge);
        cvReleaseImage(&final);
        cvReleaseImage(&frame);

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

void COG(float *X, float *Y)
{
    float intensity = 0.0, accum = 0.0;
    int x, y;
    *X = 0.0;
    *Y = 0.0;
    for(x = 0; x < final->width; x++)
    {
        for(y = 0; y < final->height; y++)
        {
            intensity = (float)(cvGetReal2D(final, y, x));
            *X += x*intensity;
            *Y += y*intensity;
            accum += intensity;
        }
    }

    *X /= accum;
    *Y /= accum;
}

#ifdef _EiC
main(1,"chalk.c");
#endif
