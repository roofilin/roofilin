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
IplImage *thresh = 0;
float low = 60.0f, high = 100.0f;
float cog_x = 0.0, cog_y = 0.0;

IplImage *gray = 0, *bw = 0;
int flag = 0;


void findLine(IplImage*);
void findEdge(IplImage*);
void COG(float*, float*);



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
    //cvNamedWindow("edges", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("hue", CV_WINDOW_NORMAL);
    cvResizeWindow("hue", 320, 240);
    cvMoveWindow("hue", 640, 0);


    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0, 0, 2, CV_AA);

    //keep capturing frames until escape
    while(c != 27) //27 is escape key
    {
        //quit if can't grab frame
        if(!(frame = cvQueryFrame(capture))) break;

        //show the default image
        //cvShowImage("stream", frame);

        //edge detection - todo: HSV color filtering
        findLine(frame);

        //edge detection
        findEdge(frame);

        if(flag == 2)
            cvPutText(frame, "right edge", cvPoint(30, 400), &font, cvScalar(255, 0, 0, 0));
        else if(flag == 1)
            cvPutText(frame, "left edge", cvPoint(30, 400), &font, cvScalar(255, 0, 0, 0));
        //display center of gravity in coordinates
        COG(&cog_x, &cog_y);
        char x_coord[10];
        char y_coord[10];
        sprintf(x_coord, "%1.2f", cog_x);
        sprintf(y_coord, "%1.2f", cog_y);
        //printf("%s\n", x_coord);
        //printf("%s\n", y_coord);
        //CvFont font;
        //cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0, 0, 2, CV_AA);
        //cvPutText(frame, "x:", cvPoint(0,300), &font, cvScalar(255, 0, 0, 0));
        cvPutText(frame, "y:", cvPoint(0,350), &font, cvScalar(255, 0, 0, 0));

        //cvPutText(frame, x_coord, cvPoint(30,300), &font, cvScalar(255, 0, 0, 0));
        cvPutText(frame, y_coord, cvPoint(30,350), &font, cvScalar(255, 0, 0, 0));

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


void findLine(IplImage *image)
{

    IplImage *hsv = cvCreateImage(cvGetSize(image), 8, 3);
    IplImage *hue = cvCreateImage(cvGetSize(image), 8, 1);

    cvCvtColor(image, hsv, CV_BGR2HSV);

    cvSplit(hsv, hue, 0, 0, 0);

    thresh = cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 1);
    cvThreshold(hue, thresh, 15, 255, CV_THRESH_BINARY_INV);

    //cvCanny(hue, hue, low, high, 3);
    cvShowImage("hue", thresh);

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

    cvPyrDown(edge, final, CV_GAUSSIAN_5x5);   //
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
    //cvShowImage("edges", final);

}


void findEdge(IplImage *image)
{

    gray = cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 1);
    bw = cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 1);

    cvCvtColor(frame, gray, CV_BGR2GRAY);
    cvThreshold(gray, bw, 200, 255, CV_THRESH_BINARY);

    int intensity = 0, right_accum = 0, left_accum = 0, accum = 0;
    int x, y;
    for(x = 0; x < bw->width; x++)
    {
        for(y = 0; y < bw->height; y++)
        {
            //printf("%d\n", intensity);
            intensity = (int)(cvGetReal2D(bw, y, x)/255);
            if(x >= 320)
                right_accum += intensity;
            else
                left_accum += intensity;
            //accum += intensity;
        }
    }
    //printf("%d\n", accum);
    //printf("width: %d, height:%d\n", image->width, image->height);
    accum = left_accum + right_accum;
    if(accum > 10000)
    {
        if(right_accum > left_accum)
            flag = 2; //edge on right
        else
            flag = 1; //edge on left;
    }
    else flag = 0;
}


void COG(float *X, float *Y)
{
    float intensity = 0.0, accum = 0.0;
    int x, y;
    *X = 0.0;
    *Y = 0.0;
    for(x = 0; x < thresh->width; x++)
    {
        for(y = 0; y < thresh->height; y++)
        {
            intensity = (float)(cvGetReal2D(thresh, y, x));
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
