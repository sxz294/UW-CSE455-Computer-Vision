#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    // TODO
    /***********************************************************************
      This function divides each value in an image "im" by the sum of all the
      values in the image and modifies the image in place.
    ************************************************************************/
    float sum=0;
    for (int i=0;i<im.c;i=i+1){
        for (int j=0;j<im.h;j=j+1){
            for (int k=0;k<im.w;k=k+1){
                sum=sum+get_pixel(im,k,j,i);
            }
        }

    }
    for (int i=0;i<im.c;i=i+1){
        for (int j=0;j<im.h;j=j+1){
            for (int k=0;k<im.w;k=k+1){
                set_pixel(im,k,j,i,get_pixel(im,k,j,i)/sum);
            }
        }

    }
}

image make_box_filter(int w)
{
    // TODO
    /***********************************************************************
      This function makes a square filter of size "w x w". Hint:
      Change the "make_image" arguments so that it is a square image of
      width = height = w and number of channels = 1, with all entries equal
      to 1. Then use "l1_normalize" to normalize your filter.
    ************************************************************************/
    image filter=make_image(w,w,1);
    for (int i=0;i<w;i=i+1){
        for (int j=0;j<w;j=j+1){
            set_pixel(filter,j,i,0,1);
        }
    }
    l1_normalize(filter);
    return filter;
}



image convolve_image(image im, image filter, int preserve)
{
    // TODO
    /***********************************************************************
    im: an image with shape "h x w x c"
    filter: a convolution filter with shape "k1 x k2 x k3".
    Preserve: an integer, which is usually either 0 or 1.

    - If `filter` and `im` have the same number of channels then it's just a normal
    convolution. We sum over spatial and channel dimensions and produce a 1 channel image.
    UNLESS:
        If `preserve` is set to 1 we should produce an image with the same number of
        channels as the input. This is useful if, for example, we want to run a box
        filter over an RGB image and get out an RGB image. This means each channel in
        the image will be filtered by the corresponding channel in the filter.
    If the `filter` only has one channel but `im` has multiple channels we want to
    apply the filter to each of those channels. Then we either sum between channels
    or not depending on if `preserve` is set.

    Also, `filter` better have either the same number of channels as `im` or have one channel.
    I check this with an `assert`.
    ************************************************************************/

    assert(filter.c == 1 || filter.c == im.c);
    image newim=make_image(im.w,im.h,im.c);
    int w=filter.w;
    int h=filter.h;
    int c=filter.c;
    for (int i=0; i<im.c;i=i+1){
        for (int j=0;j<im.h;j=j+1){
            for (int k=0;k<im.w;k=k+1){
                float v=0;
                for (int k1=0;k1<filter.c;k1=k1+1){
                    for (int k2=0;k2<filter.h;k2=k2+1){
                        for (int k3=0;k3<filter.w;k3=k3+1){
                            v=v+get_pixel(filter,k3,k2,k1)*get_pixel(im,k-w/2+k3,j-h/2+k2,i-c/2+k1);
                        }
                    }
                }
                set_pixel(newim,k,j,i,v);
            }
        }
    }

    if(preserve==0){
//        image gray=rgb_to_grayscale(im);
        image gray=make_image(im.w,im.h,1);
        for (int j=0;j<im.h;j=j+1){
                for (int k=0;k<im.w;k=k+1){
                    float v=0;
                    for (int i=0;i<im.c;i=i+1){
                            v=v+get_pixel(newim,k,j,i);
                        }
                    set_pixel(gray,k,j,0,v);
                }
            }

        return gray;
    }

    return newim;
}

image make_highpass_filter()
{
    // TODO
    /***********************************************************************
    Create a 3x3 highpass filter and return it
    ************************************************************************/
    image filter=make_image(3,3,1);
    set_pixel(filter,0,0,0,0);
    set_pixel(filter,0,1,0,-1);
    set_pixel(filter,0,2,0,0);
    set_pixel(filter,1,0,0,-1);
    set_pixel(filter,1,1,0,4);
    set_pixel(filter,1,2,0,-1);
    set_pixel(filter,2,0,0,0);
    set_pixel(filter,2,1,0,-1);
    set_pixel(filter,2,2,0,0);
    return filter;
}

image make_sharpen_filter()
{
    // TODO
    /***********************************************************************
    Create a 3x3 sharpen filter and return it
    ************************************************************************/
    image filter=make_image(3,3,1);
    set_pixel(filter,0,0,0,0);
    set_pixel(filter,0,1,0,-1);
    set_pixel(filter,0,2,0,0);
    set_pixel(filter,1,0,0,-1);
    set_pixel(filter,1,1,0,5);
    set_pixel(filter,1,2,0,-1);
    set_pixel(filter,2,0,0,0);
    set_pixel(filter,2,1,0,-1);
    set_pixel(filter,2,2,0,0);
    return filter;
}

image make_emboss_filter()
{
    // TODO
    /***********************************************************************
    Create a 3x3 emboss filter and return it
    ************************************************************************/
    image filter=make_image(3,3,1);
    set_pixel(filter,0,0,0,-2);
    set_pixel(filter,0,1,0,-1);
    set_pixel(filter,0,2,0,0);
    set_pixel(filter,1,0,0,-1);
    set_pixel(filter,1,1,0,1);
    set_pixel(filter,1,2,0,1);
    set_pixel(filter,2,0,0,0);
    set_pixel(filter,2,1,0,1);
    set_pixel(filter,2,2,0,2);
    return filter;
}

// Question 2.2.1: Which of these filters should we use `preserve = 1` when we run our convolution and which ones should we not? Why?
// Answer: TODO
// sharpen and emboss filters should have preserve=1 while highpass filter should have preserve=0;

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: TODO

image make_gaussian_filter(float sigma)
{
    // TODO
    /***********************************************************************
    sigma: a float number for the Gaussian.

    Create a Gaussian filter with the given sigma. Note that the kernel size
    is the next highest odd integer from 6x sigma.

    Return the Gaussian filter.
    ************************************************************************/
    int d=ceil(sigma*6);
    int m=d%2;
    if (m==0){
        d=d+1;
    }
    image filter=make_image(d,d,1);
    for (int i=0;i<d;++i){
        for (int j=0;j<d;++j){
            int x=i-d/2;
            int y=j-d/2;
            float v=1.0/(2*TWOPI*pow(sigma,2))*exp(-(pow(x,2)+pow(y,2))/(2*pow(sigma,2)));
            set_pixel(filter,j,i,0,v);
        }
    }
    l1_normalize(filter);
    return filter;
}

image add_image(image a, image b)
{
    // TODO
    /***********************************************************************
    The input images a and image b have the same height, width, and channels.
    Sum the given two images and return the result.
    The result image should also have the same height, width, and channels as the inputs.
    ************************************************************************/
    assert(a.w==b.w && a.h==b.h && a.c==b.c);
    image newim=make_image(a.w,a.h,a.c);
    for (int i=0; i<newim.c;++i){
        for (int j=0;j<newim.h;++j){
            for (int k=0;k<newim.w;++k){
                float v=get_pixel(a,k,j,i)+get_pixel(b,k,j,i);
                set_pixel(newim,k,j,i,v);
            }
        }
    }
    return newim;
}

image sub_image(image a, image b)
{
    // TODO
    /***********************************************************************
    The input image a and image b have the same height, width, and channels.
    Subtract the given two images (a - b) and return the result.
    The result image should also have the same height, width, and channels as the inputs.
    ************************************************************************/
    assert(a.w==b.w && a.h==b.h && a.c==b.c);
    image newim=make_image(a.w,a.h,a.c);
    for (int i=0; i<newim.c;++i){
        for (int j=0;j<newim.h;++j){
            for (int k=0;k<newim.w;++k){
                float v=get_pixel(a,k,j,i)-get_pixel(b,k,j,i);
                set_pixel(newim,k,j,i,v);
            }
        }
    }
    return newim;
}

image make_gx_filter()
{
    // TODO
    /***********************************************************************
    Create a 3x3 Sobel Gx filter and return it
    ************************************************************************/
    image filter=make_image(3,3,1);
    set_pixel(filter,0,0,0,-1);
    set_pixel(filter,0,1,0,-2);
    set_pixel(filter,0,2,0,-1);
    set_pixel(filter,1,0,0,0);
    set_pixel(filter,1,1,0,0);
    set_pixel(filter,1,2,0,0);
    set_pixel(filter,2,0,0,1);
    set_pixel(filter,2,1,0,2);
    set_pixel(filter,2,2,0,1);
    return filter;
}

image make_gy_filter()
{
    // TODO
    /***********************************************************************
    Create a 3x3 Sobel Gy filter and return it
    ************************************************************************/
    image filter=make_image(3,3,1);
    set_pixel(filter,0,0,0,-1);
    set_pixel(filter,0,1,0,0);
    set_pixel(filter,0,2,0,1);
    set_pixel(filter,1,0,0,-2);
    set_pixel(filter,1,1,0,0);
    set_pixel(filter,1,2,0,2);
    set_pixel(filter,2,0,0,-1);
    set_pixel(filter,2,1,0,0);
    set_pixel(filter,2,2,0,1);
    return filter;
}

image *sobel_image(image im)
{
    // TODO
    /***********************************************************************
    im: the input image is either a "h x w x 3" RGB image or "h x w x 1" grayscale
    image.

    Apply Sobel filter to the given image, get the magnitude and gradient,
    and return the result.

    Hint: the "calloc" function can allocate the memory for your output. You can
    assess the first image (magnitute) by calling rst[0] and the second image
    by calling rst[1]
    ************************************************************************/
    image *rst = calloc(2, sizeof(image));
    image gx_filter=make_gx_filter();
    image Gx=convolve_image(im,gx_filter,0);
    image gy_filter=make_gy_filter();
    image Gy=convolve_image(im,gy_filter,0);
    image magnitude=make_image(im.w,im.h,1);
    image direction=make_image(im.w,im.h,1);
    for(int j=0;j<magnitude.h;++j){
            for(int k=0;k<magnitude.w;++k){
                float mag=pow(pow(get_pixel(Gx,k,j,0),2)+pow(get_pixel(Gy,k,j,0),2),0.5);
                set_pixel(magnitude,k,j,0,mag);
                float theta=atan2f(get_pixel(Gy,k,j,0),get_pixel(Gx,k,j,0));
                set_pixel(direction,k,j,0,theta);
            }
        }
    rst[0] = magnitude;
    rst[1] = direction;

    return rst;
}


void normalize_image(image im)
{
    /***********************************************************************
    Calculate minimum and maximum pixel values. Normalize the image by
    subtracting the minimum and dividing by the max-min difference.
    This is a helper function to visualize the sobel magnitude image better.
    No TODO here :)
    ***********************************************************************/
    int i;
    float min = im.data[0];
    float max = im.data[0];
    for(i = 0; i < im.w*im.h*im.c; ++i){
        if(im.data[i] > max) max = im.data[i];
        if(im.data[i] < min) min = im.data[i];
    }
    for(i = 0; i < im.w*im.h*im.c; ++i){
        im.data[i] = (im.data[i] - min)/(max-min);
    }
}


// EXTRA CREDITS BELOW
int compare_float(const void * a, const void * b)
{
    // This function is provided for your convenience
    float fa = *(const float*) a;
    float fb = *(const float*) b;
    return (fa > fb) - (fa < fb);
}

image apply_median_filter(image im, int kernel_size)
{
    image out = make_image(im.w, im.h, im.c);

    // TODO (EXTRA CREDIT)
    /***********************************************************************
    im is the input image.
    kernel_size is a positive odd number.

    We assume a median filter is a square, with the same height and width.
    The kernel size is always a positive odd number. We use "clamp" padding
    for borders and corners. The output image should have the same width,
    height, and channels as the input image. You should apply median filter
    to each channel of the input image `im`.

    Hint: use the qsort() function to sort an array. Make use of compare_float() as needed.
    ************************************************************************/

    return out;
}
