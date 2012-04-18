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
#include <stdint.h>
#include <string.h>
#include <unistd.h>

//serial io stuff
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <getopt.h>

#endif

//images
IplImage *frame = 0;
IplImage *thresh = 0;
IplImage *gray = 0, *bw = 0;

//heuristics
float low = 60.0f, high = 100.0f;

//centers of line, each edge
float cog_x = 0.0, cog_y = 0.0;
float left_y = 0.0, right_y = 0.0;

//set on detection of left (1) or right (2) edge
int flag = 0;

//set on detection of offset slant
int offset = 0;
int slant = 0;
int ROL, ROR = 0;

//count tells us which shingle should be placed next
/*
 _ _ _
| | | |
3 6 5 4
|2|1|0|
 - - -
*/
int count = 0;

//function prototypes
void findLine(IplImage*);
void findEdge(IplImage*);
void findSlant(IplImage*);
void COG(float*, float*);
void COG_edges(float*, float*);
int serialport_init(const char*, int);


int main( int argc, char** argv )
{
    /*
     * serial stuff
     */
    int fd = 0;
    char serialport[256];
    int baudrate = B19200;
    //FIXME
    fd = serialport_init("/dev/ttyUSB0", baudrate);
            if(fd==-1) return -1;
    usleep(3000 * 1000);
    ///////////////////////////////////////////////////////


    int c = 0, fps = 0;

    //capture from camera
    CvCapture *capture = cvCaptureFromCAM(1);
    //quit if camera not found
    if(!capture) {
        printf("cannot init capture!\n");
        return -1;
    }


    //display original video stream
    cvNamedWindow("stream", CV_WINDOW_NORMAL);
    cvResizeWindow("stream", 320, 240);
    cvNamedWindow("hue", CV_WINDOW_NORMAL);
    cvResizeWindow("hue", 320, 240);
    cvMoveWindow("hue", 320, 0);

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
		//printf("%1.2f\n", left_y);
		//printf("%1.2f\n", right_y);

		//find slant
		findSlant(frame);
		char write_slant[15];
		if(slant && offset) sprintf(write_slant, "%s", "crooked slant!");		//eventually invokes s movement
        else sprintf(write_slant, "%s", "\0");

        
        //TODO: FIXME
		//motor1 is closer to bottom, motor 2 is closer to top (in starting config)
        uint8_t b = 0b00000000;
        //motor logic
        char motor1[10];
        char motor2[10];
		
		//handles any errors - move 0.5 second for now
        //might have to flip these
        if(flag == 1)
        {
            b = 0b00000111;	
            sprintf(motor1, "%s", "backward");
            sprintf(motor2, "%s", "backward");
            write(fd,&b,1);
        }
        else if(flag == 2)
        {
            b = 0b00000100;
            sprintf(motor1, "%s", "forward");
            sprintf(motor2, "%s", "forward");
            write(fd,&b,1);
        }
        else if(ROR) //rotate right (eventually look at angle and change timing appropriately)
        {
            b = 0b00000101;
            sprintf(motor1, "%s", "forward");
            sprintf(motor2, "%s", "backward");
            write(fd,&b,1);
        }
        else if(ROL) //rotate left (eventually look at angle and change timing appropriately)
        {
            b = 0b00000110;
            sprintf(motor1, "%s", "backward");
            sprintf(motor2, "%s", "forward");
            write(fd,&b,1);
        }
		//NO ERROR - THIS HANDLES LOGIC TO DRIVE ROBOT IN CORRECT SEQUENCE
		//determines the amount of time and directions in which to move the motors.
		//the arduino is responsible for sending stop command to wheels!
        else
        {
			switch(count)
			{
				//first shingle - already in place
				case 0:
				b = 0b00000000;	//do nothing
				write(fd,&b,1);
				sleep(6); //however long it takes to place shingle, or wait until arduino says go
                printf("%s\n", "case 0");
				break;
				
				//second/third shingle - drive forward one foot (for now 2 secs, figure this out!)
				case 1:
				case 2:
				b = 0b01000000;	//forward 1 foot
				write(fd,&b,1);
				sleep(6);
                printf("%s\n", "case 1/2");
				break;
				
				//left (viewing from bottom) overhang
				case 3:
				b = 0b00100011;	//backward 1/2 foot
				write(fd,&b,1);
				b = 0b00110001;	//ROR 90 degrees
				write(fd,&b,1);
				b = 0b00100011;	//backward 1/2 foot
				write(fd,&b,1);
				sleep(12);
                printf("%s\n", "case 3");
				break;
				
				//right overhang
				case 4:
				b = 0b00110001;	//ROR 90 degrees
				write(fd,&b,1);
				b = 0b01000000;	//forward 1 foot
				write(fd,&b,1);
				b = 0b00110001;	//ROR 90 degrees
				write(fd,&b,1);
				sleep(14);
                printf("%s\n", "case 4");
				break;
				
				//next to right overhang
				case 5:
				b = 0b01000011;	//backward 1 foot
				write(fd,&b,1);
				b = 0b00110001;	//ROR 90 degrees
				write(fd,&b,1);
				sleep(8);
				break;
				
				//next to left overhang
				case 6:
				b = 0b01000000;	//forward 1 foot
				write(fd,&b,1);
				sleep(6);
				break;
				
				//reposition at beginning position 
				case 7:
				b = 0b00110001;	//ROR 90 degrees
				write(fd,&b,1);
				b = 0b00100000;	//forward 1/2 foot
				write(fd,&b,1);
				b = 0b00110010; //ROL 90 degrees
				write(fd,&b,1);
				b = 0b01100011;	//backward 1+1/2 foot
				write(fd,&b,1);
                sleep(14);
                break;
				
				//should not get here
				default:
				b = 0; //error
				write(fd,&b,1);
				sleep(20);
				break;
			}
			
			count = (count+1)%8;
        }
            
        
        //SERIAL - motor logic
        //xxxx_xxyy -> y = motor1/motor2, 0 = forward, 1 = backward, x = time in ms
        //edges

        //write(fd,&b,1);

        //cvPutText(frame, x_coord, cvPoint(30,300), &font, cvScalar(255, 0, 0, 0));
        cvPutText(frame, "y:", cvPoint(0,350), &font, cvScalar(255, 0, 0, 0));
        cvPutText(frame, y_coord, cvPoint(30,350), &font, cvScalar(255, 0, 0, 0));
        cvPutText(frame, "motor1:", cvPoint(0,150), &font, cvScalar(255, 0, 0, 0));
        cvPutText(frame, motor1, cvPoint(150,150), &font, cvScalar(255, 0, 0, 0));
        cvPutText(frame, "motor2:", cvPoint(0,200), &font, cvScalar(255, 0, 0, 0));
        cvPutText(frame, motor2, cvPoint(150,200), &font, cvScalar(255, 0, 0, 0));

        cvPutText(frame, write_slant, cvPoint(0,50), &font, cvScalar(255, 0,
                    0, 0));

        cvShowImage("stream", frame);
        c = cvWaitKey(10);


        //release images

	    //cvReleaseImage(&image);
	    //cvReleaseImage(&edge);
	    //cvReleaseImage(&final);

	    //cvReleaseImage(&frame);
	    //cvReleaseImage(&bw);
	    //cvReleaseImage(&gray);
    }

	//release created images
	cvReleaseCapture(&capture);

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
    cvThreshold(hue, thresh, 20, 255, CV_THRESH_BINARY_INV);	//play with these thresholds
    cvShowImage("hue", thresh);

	//CHECKTHIS
    cvReleaseImage(&hsv);
    cvReleaseImage(&hue);
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
 * findSlant
 * use calculated centers to determine rotations, offset
 */
void findSlant(IplImage* image)
{
	offset = 0;
	slant = 0;
	ROR = 0;
	ROL = 0;
	
	//offset
	if((int)cog_y < 230 || (int)cog_y > 250) {
		offset = 1;
	}
	
	//rotate right
    if(left_y - right_y < -15){
	//if((int)(left_y/10.0) - (int)(right_y/10.0) < -2) {
		slant = 1;
		ROR = 1;
	}
	//rotate left
    else if(right_y - left_y < -15){
	//else if((int)(right_y/10.0) - (int)(left_y/10.0) < -2) {
		slant = 1;
		ROL = 1;
	}
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
main(1,"new-serial.c");
#endif

