#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include "image.h"
#include "matrix.h"

/*********************************************
**********************************************
FILE CONTAINING ALL TODOs FOR THE HOMEWORK
**********************************************
*********************************************/


image make_1d_gaussian(float sigma)
/**************************************************
Create a separable (1-D) Gaussian filter of size given by input "sigma"
TODO: 
Fill this for extra credit
**************************************************/
{
    return make_image(1,1,1);
}


image structure_matrix(image im, float sigma)
/**************************************************
Create the structure matrix for the image "im".
TODO:
1) Create Ix using make_gx_filter() and convolve_image()
2) Create Iy using make_gy_filter() and convolve_image()
3) Create an empty image, say "S" of size im.w x im.h x 3
4) While looping over each pixel position of "S", assign the 1st channel to Ix^2, 2nd channel to Iy^2 and 3rd channel to IxIy
5) Smooth the image "S" using make_gaussian_filter() and convolve_image(); use input "sigma" for gaussian filter size
	You may use make_1d_gaussian() instead of make_gaussian_filter() if you want
6) Free the temporary variables
7) Return the smoothed image
**************************************************/
{
    image gx_filter=make_gx_filter();
    image Ix=convolve_image(im,gx_filter,0);
    image gy_filter=make_gy_filter();
    image Iy=convolve_image(im,gy_filter,0);
    image SInit = make_image(im.w, im.h, 3);
    for (int i=0;i<im.h;i++){
        for (int j=0;j<im.w;j++){
            set_pixel(SInit,j,i,0,pow(get_pixel(Ix,j,i,0),2));
            set_pixel(SInit,j,i,1,pow(get_pixel(Iy,j,i,0),2));
            set_pixel(SInit,j,i,2,get_pixel(Ix,j,i,0)*get_pixel(Iy,j,i,0));
        }
    }
    image filter=make_gaussian_filter(sigma);
    image S=convolve_image(SInit,filter,1);
    return S;
}


image cornerness_response(image S)
/**************************************************
Estimate the cornerness response "R" of each pixel given a structure matrix "S".
Return response map of the same size as structure matrix
TODO:
Loop over each pixel position in "S"
    compute determinant = Ix^2 * Iy^2 - IxIy * IxIy
    compute trace = Ix^2 + Iy^2
    compute R = determinant - alpha * trace^2, alpha = .06
**************************************************/
{
    image R = make_image(S.w, S.h, 1);
    for (int i=0;i<S.h;i++){
        for(int j=0;j<S.w;j++){
            float determinant=get_pixel(S,j,i,0)*get_pixel(S,j,i,1)-pow(get_pixel(S,j,i,2),2);
            float trace=get_pixel(S,j,i,0)+get_pixel(S,j,i,1);
            set_pixel(R,j,i,0,determinant-0.06*pow(trace,2));
        }
    }
    return R;
}


image nms_image(image R, int w)
/**************************************************
Perform non-max supression on the response map "R".
TODO:
for every pixel in "R":
	for every neighboring pixel within "w":
		if neighboring pixel value > current pixel value:
			set current pixel value to be very low (= -999999)
**************************************************/
{
    image r = copy_image(R);
    for (int i=0;i<R.h;i++){
        for (int j=0;j<R.w;j++){
            float p=get_pixel(R,j,i,0);
            for (int k1=0;k1<2*w;k1++){
                for (int k2=0;k2<2*w;k2++){
                    float q=get_pixel(R,j-w+k2,i-w+k1,0);
                    if (q>p){
                        p=-999999;
                        set_pixel(r,j,i,0,p);
                        break;
                    }
                }
                if (p==-999999){
                    break;
                }
            }
        }
    }
    return r;
}


descriptor *harris_corner_detector(image im, float sigma, float thresh, int nms, int *n)
/**************************************************
Perform Harris corner detection by computing a feature descriptor for each corner in image "im".
float sigma: std. dev for gaussian filter used in structure matrix
float thresh: threshold for detecting corners from response map
int nms: size of the neighborhood to look for local-maxes in response map
int *n: pointer to number of corners detected
TODO:
Return array d of descriptors of corners in the input image. Details inline.
The "descriptor" structure is defined in "image.h"
**************************************************/
{
    // Calculate structure matrix
    image S = structure_matrix(im, sigma);
    // Estimate cornerness
    image R = cornerness_response(S);
    // Run NMS on the responses
    image Rnms = nms_image(R, nms);

    // TODO: change the value of "count" to the number of corners
    // Initialize the count to 0
    // For each pixel in Rnms:
    //      if pixel value > "thresh":
    //          increase value of count
    int count = 0;
    for (int i=0;i<Rnms.h;i++){
        for (int j=0;j<Rnms.w;j++){
            if (get_pixel(Rnms,j,i,0)>thresh){
                count++;
            }
        }
    }

    
    // TODO: fill in array "d" of descriptors of corners
    descriptor *d = calloc(count, sizeof(descriptor));
    // For each pixel in Rnms:
    //      if pixel value > "thresh":
    //          get descriptor for the current pixel using make_descriptor() defined in helpers.c
    //          update the array "d" with this descriptor
    count=0;
    for (int i=0;i<Rnms.h;i++){
        for (int j=0;j<Rnms.w;j++){
            if (get_pixel(Rnms,j,i,0)>thresh){
                d[count]=make_descriptor(im, i*im.w+j);
                count++;
            }
        }
    }

    free_image(S);
    free_image(R);
    free_image(Rnms);
    // set *n equal to number of corners in image "im"
    *n = count;
    return d;
}


match *match_descriptors(descriptor *a, int an, descriptor *b, int bn, int *mn)
/**************************************************
Finds best matches between descriptors of two images.
descriptor *a, *b: array of descriptors in two images
int an, bn: number of descriptors in arrays "a" and "b"
int *mn: pointer to number of matches found
TODO:
Return an array of matches. Details inline.
The "match" structure is defined in "image.h"
**************************************************/
{
    int i,j,k;

    // Initialize array of matches. We will have at most "an" matches
    *mn = an;
    match *m = calloc(an, sizeof(match));

    // For each descriptor in a..
    for(j = 0; j < an; ++j){
        // Initialize best index having minimum distance
        float min_distance = 99999999;
        int best_index = 0;
        // For each descriptor in b..
        for(i = 0; i < bn; ++i){
            // TODO: Compute L1 distance between the two descriptors
            // and update min_distance and best_index as necessary
            float distance_L1=0;
            for (k=0;k<a[j].n;k++){
                distance_L1=distance_L1+fabsf(a[j].data[k]-b[i].data[k]);
            }
            if (distance_L1<min_distance){
                min_distance=distance_L1;
                best_index=i;
            }
        }

        // Update the match
        m[j].ai = j;
        m[j].bi = best_index;
        m[j].p = a[j].p;
        m[j].q = b[best_index].p;
        m[j].distance = min_distance;
    }

    int count = 0;

    // Initialize an array "seen" to all zeros
    // It keeps track of whether a descriptor in "*b" is already matched or not
    int *seen = calloc(bn, sizeof(int));

    // Sort matches based on distance
    qsort(m, *mn, sizeof(match), match_compare);

    // TODO: Find one-to-one matches.
    // Note: Each point should only be a part of one match. Some points will not be in a match.
    // Loop over all the *mn matches
    //      if the b-index of the current match is not seen:
    //          set it to seen (make the corresponding value in "seen" to 1)
    //          assign the current match to m[count] and then update count
    //      else continue
    for (int h=0;h<*mn;h++){
        if (seen[m[h].bi]==0){
            seen[m[h].bi]=1;
            m[count]=m[h];
            count++;
        }
    }

    // Update the number of final matches
    *mn = count;
    free(seen);
    return m;
}


point project_point(matrix H, point p)
/**************************************************
Apply a projective transformation using homography "H" to the point "p".
The matrix functions are defined in src/matrix.c
TODO:
Return the projected point "q". Details inline.
**************************************************/
{
    // Create c matrix using point "p"
    matrix c = make_matrix(3, 1);
    // TODO: Fill in "c" matrix with x-coordinate of p, y-coordinate of p, and 1
    c.data[0][0]=p.x;
    c.data[1][0]=p.y;
    c.data[2][0]=1;

    // Multiply "H" with the created c matrix
    matrix m = matrix_mult_matrix(H, c);

    point q;
    // TODO: Assign x,y coordinates of "q" using homogenous coordinates from "m"
    q.x=m.data[0][0]/m.data[2][0];
    q.y=m.data[1][0]/m.data[2][0];

    free_matrix(c);
    free_matrix(m);
    return q;
}

int model_inliers(matrix H, match *m, int n, float thresh)
/**************************************************
Count number of inliers in an array "m" of "n" matches.
Also bring inliers to the front of array "m".
matrix H: homography between coordinate systems
float thresh: threshold for a match to be an inlier
TODO:
initialize count to 0
Loop over all matches starting from the end (since matches are already sorted by distance)
    Project the point "p" in the current match using "H" (use project_point())
    compute L2 distance between the projected point and the point "q" in the current match
    if L2 distance < thresh:
        swap m[count] with the current match
        update count
Return count (i.e. number of matches that are inliers)
Note: In this way, you are sorting the matches so that the inliers are the first #'count' elements.
**************************************************/
{
    int count = 0;
    for (int i=n-1;i>=count;i--){
        point project_p=project_point(H,m[i].p);
        float distance_L2=pow(pow(project_p.x-m[i].q.x,2)+pow(project_p.y-m[i].q.y,2),0.5);
        if (distance_L2<thresh){
            match temp=m[count];
            m[count]=m[i];
            m[i]=temp;
            count++;
            i++;

        }
    }
    return count;
}


matrix RANSAC(match *m, int n, float thresh, int iter, int cutoff)
/**************************************************
Performs RANdom SAmple Consensus to calculate homography for noisy matches.
Returns matrix representing most common homography between matches.
Inputs:
array "m" of "n" matches
"thresh": threshold for inlier modeling
"iter": number of iterations
"cutoff": inlier cutoff to exit early
TODO:
For "iter" iterations:
    compute a homography with 4 matches using compute_homography() defined in panorama_helpers.c
    if no homography, continue, else
    compute #inliers in the matches using model_inliers() and the computed homography
    if #inliers > max_inliers (i.e. new homography is better than old):
        compute updated homography using all inliers
        if no homography, continue, else
        compute updated #inliers using updated homography and assign it to max_inliers
        if updated #inliers > cutoff:
            return the best homography
If we get to the end, return the best homography
**************************************************/
{
    // Initializations
    int max_inliers = 0;
    matrix best_H = make_translation_homography(256, 0);
    for(int i=0;i<iter;i++){
        matrix candidate = compute_homography(m,n,4);
        if (candidate.data){
            int inliers=model_inliers(candidate,m,n,thresh);
            if(inliers>max_inliers){
                candidate = compute_homography(m,n, inliers);
                if (candidate.data){
                    inliers=model_inliers(candidate,m,n,thresh);
                    if (inliers>max_inliers){
                        best_H=candidate;
                        max_inliers=inliers;
                        if (max_inliers>cutoff){
                            return best_H;
                        }
                    }
                }
            }
        }
    }
    return best_H;
}



image combine_images(image a, image b, matrix H)
/**************************************************
Stitches two images "a" and "b" together using a homography "H" and returns the stitched image.
TODO:
Paste image a and projected image b into a combined_image and return it.
More details are provided inline.
**************************************************/
{
    matrix Hinv = matrix_invert(H);

    // Project the corners of image b into image a coordinates.
    point c1 = project_point(Hinv, make_point(0,0));
    point c2 = project_point(Hinv, make_point(b.w-1, 0));
    point c3 = project_point(Hinv, make_point(0, b.h-1));
    point c4 = project_point(Hinv, make_point(b.w-1, b.h-1));

    // Find top left and bottom right corners of image b warped into image a.
    point topleft, botright;
    botright.x = MAX(c1.x, MAX(c2.x, MAX(c3.x, c4.x)));
    botright.y = MAX(c1.y, MAX(c2.y, MAX(c3.y, c4.y)));
    topleft.x = MIN(c1.x, MIN(c2.x, MIN(c3.x, c4.x)));
    topleft.y = MIN(c1.y, MIN(c2.y, MIN(c3.y, c4.y)));

    // Find how big our new image should be and the offsets from image a.
    int dx = MIN(0, topleft.x);
    int dy = MIN(0, topleft.y);
    int w = MAX(a.w, botright.x) - dx;
    int h = MAX(a.h, botright.y) - dy;

    // Can disable this if you are making very big panoramas.
    // Usually this means there was an error in calculating H.
    if(w > 7000 || h > 7000){
        fprintf(stderr, "output too big, stopping\n");
        return copy_image(a);
    }

    int i,j,k;
    image combined_img = make_image(w, h, a.c);

    // TODO:
    // Loop over all pixels in image a
    //      get current pixel value
    //      assign it to corresponding pixel offset by "dx" and "dy" in "combined_img"

    // Loop over all pixels lying between "topleft" and "botright"
    //      create a point with the current x,y coordinates using make_point() defined in panorama_helpers.c
    //      project the point to image b coordinate system using homography "H"
    //      if the projected point's coordinates lie within the bounds for image b:
    //          estimate the value at the projected point location using bilinear_interpolate()
    //          assign it to the corresponding pixel offset by "dx" and "dy" in "combined_img"
    for (i=0;i<a.c;i++){
        for(j=0;j<a.h;j++){
            for(k=0;k<a.w;k++){
                set_pixel(combined_img,k-dx,j-dy,i,get_pixel(a,k,j,i));
            }
        }
    }

    for (i=0;i<combined_img.c;i++){
        for(j=topleft.y;j<botright.y;j++){
            for(k=topleft.x;k<botright.x;k++){
                point p=make_point(k,j);
                point project_p=project_point(H,p);
                if (project_p.x<b.w && project_p.x>=0 && project_p.y<b.h && project_p.y>=0){
                    float v=bilinear_interpolate(b,project_p.x,project_p.y,i);
                    set_pixel(combined_img,k-dx,j-dy,i,v);
                }
            }
        }
    }

    return combined_img;
}


image cylindrical_project(image im, float f)
/**************************************************
Project an image "im" onto a cylinder using focal length "f"
The formulas are given in the lecture slides
TODO:
Attempt for extra credit. Return the projected image.
**************************************************/
{
    image c = copy_image(im);
    return c;
}


