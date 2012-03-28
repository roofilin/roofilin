#ifdef _CH_
#pragma package <opencv>
#endif

#define CV_NO_BACKWARD_COMPATIBILITY

#ifndef _EiC

//opencv stuff
#include "cv.h"
#include "highgui.h"

//c stuff
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>   /* Standard types */
#include <string.h>   /* String function definitions */
#include <unistd.h>   /* UNIX standard function definitions */

//serial io stuff
#include <fcntl.h>    /* File control definitions */
#include <errno.h>    /* Error number definitions */
#include <termios.h>  /* POSIX terminal control definitions */
#include <sys/ioctl.h>
#include <getopt.h>

#endif

IplImage *frame = 0;
IplImage *image = 0, *red = 0, *red_edge = 0, *green = 0, *green_edge = 0, *edge = 0;
IplImage *final = 0;
IplImage *thresh = 0;
float low = 60.0f, high = 100.0f;
float cog_x = 0.0, cog_y = 0.0;
float left_y = 0.0, right_y = 0.0;

IplImage *gray = 0, *bw = 0;
int flag = 0;

void findLine(IplImage*);
void findEdge(IplImage*);
void COG(float*, float*);
void COG_edges(float*, float*);


int main( int argc, char** argv )
{
    ///////////
    //serial stuff
    int fd = 0;
    char serialport[256];
    int baudrate = B19200;
    //int baudrate = B115200;  // default
    
    fd = serialport_init("/dev/ttyUSB0", baudrate);
            if(fd==-1) return -1;
    usleep(3000 * 1000 );
    ///////////
    
    
    int c = 0, fps = 0;

    //capture from camera
    CvCapture *capture = cvCaptureFromCAM(1);
    //quit if camera not found
    if(!capture) {
        printf("cannot init capture!\n");
        return -1;
    }

    //display original video stream
    cvNamedWindow("stream", CV_WINDOW_AUTOSIZE);
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
	


        
        //find center of y coordinate in left and right edge
        COG_edges(&left_y, &right_y);

printf("%1.2f\n", left_y);
printf("%1.2f\n", right_y);

        
        
        uint8_t b = 0b00000000;
        //motor logic
        char motor1[10];
        char motor2[10];
        if(flag == 1) 
        {
            b = 0b00010001;
            sprintf(motor1, "%s", "backward");
            sprintf(motor2, "%s", "backward");
        }
        else if(flag == 2)
        {
            b = 0b10011001;
            sprintf(motor1, "%s", "forward");
            sprintf(motor2, "%s", "forward");
        }
        else if((int)(left_y/10.0) - (int)(right_y/10.0) < -4) //rotate right
        {
            b = 0b10010001;
            sprintf(motor1, "%s", "forward");
            sprintf(motor2, "%s", "backward");
        }
        else if((int)(right_y/10.0) - (int)(left_y/10.0) < -4) //rotate left
        {
            b = 0b00011001;
            sprintf(motor1, "%s", "backward");
            sprintf(motor2, "%s", "forward");
        }
        else 
        {
            b = 0;
            sprintf(motor1, "%s", "STOP");
            sprintf(motor2, "%s", "STOP");
        }
            
        
        //SERIAL - motor logic
        //xxxx_xxxx = motor1_motor2, 1-15 -> -7,7 -> 8 = 0 = 1000
        //edges
        
        write(fd,&b,1);
        
        
        //cvPutText(frame, x_coord, cvPoint(30,300), &font, cvScalar(255, 0, 0, 0));
        cvPutText(frame, "y:", cvPoint(0,350), &font, cvScalar(255, 0, 0, 0));
        cvPutText(frame, y_coord, cvPoint(30,350), &font, cvScalar(255, 0, 0, 0));
        
        
        cvPutText(frame, "motor1:", cvPoint(0,150), &font, cvScalar(255, 0, 0, 0));
        cvPutText(frame, motor1, cvPoint(150,150), &font, cvScalar(255, 0, 0, 0));
        cvPutText(frame, "motor2:", cvPoint(0,200), &font, cvScalar(255, 0, 0, 0));
        cvPutText(frame, motor2, cvPoint(150,200), &font, cvScalar(255, 0, 0, 0));

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
	cvReleaseImage(&bw);
	cvReleaseImage(&gray);

    return 0;
}


/*
 * findLine
 * hsv line detection
 */
void findLine(IplImage *image)
{
    IplImage *hsv = cvCreateImage(cvGetSize(image), 8, 3);
    IplImage *hue = cvCreateImage(cvGetSize(image), 8, 1);
    cvCvtColor(image, hsv, CV_BGR2HSV);
    cvSplit(hsv, hue, 0, 0, 0);
    thresh = cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 1);
    cvThreshold(hue, thresh, 30, 255, CV_THRESH_BINARY_INV);
    cvShowImage("hue", thresh);
}

/*
 * findEdge
 * 1 for leftEdge, 2 for rightEdge
 */
void findEdge(IplImage *image)
{
    gray = cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 1);
    bw = cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 1);

    cvCvtColor(frame, gray, CV_BGR2GRAY);
    cvThreshold(gray, bw, 200, 255, CV_THRESH_BINARY);

    int intensity = 0, left_accum = 0, right_accum = 0;
    int x, y;

    for(x = 0; x < bw->width; x++)
    {
        for(y = 0; y < bw->height; y++)
        {
            intensity = (int)(cvGetReal2D(bw, y, x)/255);
            if(x <= 40) left_accum += intensity;
            else if(x >= bw->width-40) right_accum += intensity;
        }
    }

    //edge on left
    if((left_accum>5000) && (left_accum>right_accum)) flag = 1;
    //edge on right
    else if((right_accum>5000) && (right_accum>left_accum)) flag = 2;
    //neither edge
    else flag  = 0;
}

/*
 * COG
 * find x and y coordinates of center of image
 */
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


/*
 * COG_edges
 * find intersection of chalkline with either edge
 */
void COG_edges(float *Y_left, float *Y_right)
{
    float intensity = 0.0, accum = 0.0;
    int x, y;

    *Y_left = 0.0;
    *Y_right = 0.0;

    for(x = 0; x < 40; x++)
    {
        for(y = 0; y < thresh->height; y++)
        {
            intensity = (float)(cvGetReal2D(thresh, y, x));
            *Y_left += y*intensity;
            accum += intensity;
        }
    }
    *Y_left /= accum;

    accum = 0.0;
    for(x = thresh->width-40; x < thresh->width; x++)
    {
        for(y = 0; y < thresh->height; y++)
        {
            intensity = (float)(cvGetReal2D(thresh, y, x));
            *Y_right += y*intensity;
            accum += intensity;
        }
    }
    *Y_right /= accum;
}

/*
 * serialport_init
 * initializes serial port
 */
int serialport_init(const char* serialport, int baud)
{
    struct termios toptions;
    int fd;

    fd = open(serialport, O_RDWR | O_NOCTTY);
    if (fd == -1)  {
      perror("init_serialport: Unable to open port ");
      return -1;
    }

    if (tcgetattr(fd, &toptions) < 0) {
      perror("init_serialport: Couldn't get term attributes");
      return -1;
    }
    speed_t brate = baud; // let you override switch below if needed

/*
    switch(baud) {
    case 4800:   brate=B4800;   break;
    case 9600:   brate=B9600;   break;
// if you want these speeds, uncomment these and set #defines if Linux
//#ifndef OSNAME_LINUX
//    case 14400:  brate=B14400;  break;
//#endif
    case 19200:  brate=B19200;  break;
//#ifndef OSNAME_LINUX
//    case 28800:  brate=B28800;  break;
//#endif
    //case 28800:  brate=B28800;  break;
    case 38400:  brate=B38400;  break;
    case 57600:  brate=B57600;  break;
    case 115200: brate=B115200; break;
    }
*/

    cfsetispeed(&toptions, brate);
    cfsetospeed(&toptions, brate);

    // 8N1
    toptions.c_cflag &= ~PARENB;
    toptions.c_cflag &= ~CSTOPB;
    toptions.c_cflag &= ~CSIZE;
    toptions.c_cflag |= CS8;
    // no flow control
    toptions.c_cflag &= ~CRTSCTS;

    toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
    toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

    toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    toptions.c_oflag &= ~OPOST; // make raw

    // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
    toptions.c_cc[VMIN]  = 0;
    toptions.c_cc[VTIME] = 20;

    if( tcsetattr(fd, TCSANOW, &toptions) < 0) {
      perror("init_serialport: Couldn't set term attributes");
      return -1;
    }

    return fd;
}


#ifdef _EiC
main(1,"chalk.c");
#endif

