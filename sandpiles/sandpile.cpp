
#include <cv.hpp>
#include <highgui.h>
#include <stdio.h>

#define CLAMP(x,l,u) (x < l ? l : (x > u ? u : x) )

using namespace cv;

// BGR                          black    red        orange       yellow
const Vec3b color_table[20] = { {0,0,0}, {0,0,255}, {0,128,255}, {0,255,255} };

int main(int argc, char** argv) {
    if(argc < 4 ) {
        printf("Usage: %s width height output_image [sand]\n",argv[0]);
        printf("Example: %s 640 360 frame_%%d.png 100000\n",argv[0]);
        return -1;
    }
    int W = atoi(argv[1]);
    int H = atoi(argv[2]);
    int frame = 1;
    char fn[1000];
    Mat im(H, W, CV_32SC1, Scalar(0));
    int sand = 10000;
    if (argc > 4) sand = atoi(argv[4]);
    // make two sandpiles, in these locations so there is room on the right for zooms
    im.at<int>(H/2,W/4) = sand;
    im.at<int>(H/2,W/2) = sand;

    // could also do the toppling in-place, but using the auxiliary matrix im_plus makes it symmetric
    // (doing it in-place means you do more downwards toppling than upwards toppling per iteration if you scan from top to bottom)
    Mat im_plus(H, W, CV_32SC1, Scalar(0));
    for (int it=0; it < sand; it++) {
     bool did_something = false;
     for (int y = 0; y < H; y++) {
      for (int x = 0; x < W; x++) {
        if (im.at<int>(y,x) > 3) {
            int amount = im.at<int>(y,x)/4;
            im.at<int>(y,x) -= 4*amount;
            if (y>0) im_plus.at<int>(y-1,x) += amount;
            if (y+1<H) im_plus.at<int>(y+1,x) += amount;
            if (x>0) im_plus.at<int>(y,x-1) += amount;
            if (x+1<W) im_plus.at<int>(y,x+1) += amount;
            did_something = true;
        }
      }
     }
     im += im_plus;
     im_plus = 0;

     // when to render a frame (skipping lots of frames to keep things interesting)
     // if (true) {   // use this if you want to render all frames

     if (it < 500 || it % 64 == 0 || (it > 10000 && it < 10500) || !did_something) {
     // render first 500 iterations, and from then on, only 1 in 64 except between iteration 10000 and 10500 where we slow down again; also render very last frame

          Mat im2(H, W, CV_8UC3);
          // render sandpile image using color table + a hardcoded gradient for the still-to-be-toppled cells
          for (int i = 0; i < W*H; i++) {
             int x = im.at<int>(i);
             im2.at<Vec3b>(i) = (x < 4 ? color_table[x] : Vec3b(128+CLAMP(x,0,127),CLAMP(x*4,0,255),CLAMP(x,0,255)) );
          }
          // zoom in on some regions
          for (int y = 0; y < H/2; y++) {
           for (int x = 0; x < W/4; x++) {
             im2.at<Vec3b>(y,x+3*W/4) = im2.at<Vec3b>(y/4 + 7*H/16,x/4 + 7*W/32);
           }
          }
          for (int y = 0; y < H/2; y++) {
           for (int x = 0; x < W/4; x++) {
             im2.at<Vec3b>(y+H/2,x+3*W/4) = im2.at<Vec3b>(y/4 + 7*H/16,x/4 + 11*W/32);
           }
          }
          sprintf(fn,argv[3],frame++);
          imwrite(fn,im2);
          printf("%i iterations\n",it);
     }
     if (!did_something) break; // stop iterating when no more toppling happens
    }
    return 0;
}

