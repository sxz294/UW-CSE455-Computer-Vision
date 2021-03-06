#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"


/***********************************************************************
  We've been talking a lot about resizing and interpolation in class,
  now's your time to do it!
  In order to make grading easier, please only edit the files we mention to submit.
  You will submit the resize_image.c file on Canvas.
************************************************************************/


/******************************** Resizing *****************************
  To resize we'll need some interpolation methods and a function to create
  a new image and fill it in with our interpolation methods.
************************************************************************/

float nn_interpolate(image im, float x, float y, int c)
{
    // TODO
    /***********************************************************************
      This function performs nearest-neighbor interpolation on image "im"
      given a floating column value "x", row value "y" and integer channel "c",
      it interpolates and returns the interpolated value.
      Remember to use the closest "int", not just type-cast because in C that
      will truncate towards zero.
    ************************************************************************/
    int newx=round(x);
    if (newx<0){
        newx=0;
    }else if(newx>=im.w){
        newx=im.w-1;
    }
    int newy=round(y);
    if (newy<0){
        newy=0;
    }else if(newy>=im.h){
        newy=im.h-1;
    }
    float v=im.data[im.h*im.w*c+im.w*newy+newx];
    return v;
}

image nn_resize(image im, int w, int h)
{
    // TODO Fill in (also fix the return line)
    /***********************************************************************
      This function uses nearest-neighbor interpolation on image "im" to
      construct a new image of size "w x h". Hint:
      - Create a new image that is "w x h" and the same number of channels as "im"
      - Loop over the pixels and map back to the old coordinates.
      - Use nearest-neighbor interpolate to fill in the image.
    ************************************************************************/
    image newim=make_image(w,h,im.c);
    for (int i=0;i<im.c;i=i+1){
        for (int j=0;j<h;j=j+1){
            for (int k=0;k<w;k=k+1){
                float a=(float)im.w/w;
                float b=0.5*(a-1);
                float c=(float)im.h/h;
                float d=0.5*(c-1);
                float x=a*k+b;
                float y=c*j+d;
//                if (x<-0.5){
//                    x=-0.5;
//                }else if(x>im.w-0.5){
//                    x=im.w-0.5;
//                }
//                if (y<-0.5){
//                    y=-0.5;
//                }else if(y>im.h-0.5){
//                    y=im.h-0.5;
//                }
                set_pixel(newim,k,j,i,nn_interpolate(im, x, y, i));
            }
        }
    }
    return newim;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    // TODO
    /***********************************************************************
      This function performs bilinear interpolation on image "im" given
      a floating column value "x", row value "y" and integer channel "c".
      It interpolates and returns the interpolated value.
    ************************************************************************/
    float d1=0;
    float d3=0;
    if (x>=0 & x<im.w){
        d1=x-floor(x);
    }else if(x>=im.w){
        d1=1;
    }
    if (y>=0 & y<im.h){
        d3=y-floor(y);
    }else if(y>=im.h){
        d3=1;
    }
    int lowx=floor(x);
    int lowy=floor(y);
    if(lowx<0){
        lowx=0;
    }else if(lowx>=im.w){
        lowx=im.w-1;
    }
    if(lowy<0){
        lowy=0;
    }else if(lowy>=im.h){
        lowy=im.h-1;
    }
    float q=(1-d1)*(1-d3)*get_pixel(im,lowx,lowy,c)+d1*(1-d3)*get_pixel(im,lowx+1,lowy,c)+(1-d1)*d3*get_pixel(im,lowx,lowy+1,c)+d1*d3*get_pixel(im,lowx+1,lowy+1,c);
    return q;
}

//int cap_index(int index, int size) {
//  return index < size - 1 ? index >= 0 ? index : 0 : size - 1;
//}
//
//void find_bound(int *bound, float current_pos, int size) {
//  bound[0] = cap_index((int)current_pos, size);
//  bound[1] = cap_index((int)(current_pos + 1), size);
//}
//
//void find_distances(float *dis, float current_pos, int *bound) {
//  if (bound[0] == bound[1]) {
//    dis[0] = 0.5;
//    dis[1] = 0.5;
//  } else {
//    dis[0] = current_pos - bound[0];
//    dis[1] = bound[1] - current_pos;
//  }
//}
//
//float bilinear_interpolate(image im, float x, float y, int c) {
//  int rows[2], cols[2], row, col;
//  float d_rows[2], d_cols[2], color_value = 0;
//
//  find_bound(rows, y, im.h);
//  find_bound(cols, x, im.w);
//
//  find_distances(d_rows, y, rows);
//  find_distances(d_cols, x, cols);
//
//  for (row = 0; row < 2; row++) {
//    for (col = 0; col < 2; col++) {
//      color_value += get_pixel(im, cols[col], rows[row], c) * d_rows[1 - row] *
//                     d_cols[1 - col];
//    }
//  }
//  return color_value;
//}

image bilinear_resize(image im, int w, int h)
{
    // TODO
    /***********************************************************************
      This function uses bilinear interpolation on image "im" to construct
      a new image of size "w x h". Hint:
      - Create a new image that is "w x h" and the same number of channels as "im".
      - Loop over the pixels and map back to the old coordinates.
      - Use bilinear interpolate to fill in the image.
    ************************************************************************/
    image newim=make_image(w,h,im.c);
    for (int i=0;i<im.c;i=i+1){
        for (int j=0;j<h;j=j+1){
            for (int k=0;k<w;k=k+1){
                float a=(float)im.w/w;
                float b=0.5*(a-1);
                float c=(float)im.h/h;
                float d=0.5*(c-1);
                float x=a*k+b;
                float y=c*j+d;
                set_pixel(newim,k,j,i,bilinear_interpolate(im, x, y, i));
            }
        }
    }
    return newim;
}

/***************************** Box filter *******************************
  We want to create a box filter. We will only use square box filters.
  One way to do this is make an image,fill it in with all 1s, and then
  normalize it.That's what we'll do because the normalization function may
  be useful in the future!
************************************************************************/
