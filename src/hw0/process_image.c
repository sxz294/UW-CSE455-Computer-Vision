#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    // TODO Fill this in
    if (x>=im.w){
        x=im.w-1;
    }else if(x<0){
        x=0;
    }
    if (y>=im.h){
        y=im.h-1;
    }else if(y<0){
        y=0;
    }
    if (c>=im.c){
        c=c-1;
    }else if(c<0){
        c=0;
    }
    float v=im.data[im.h*im.w*c+im.w*y+x];
    return v;
}

void set_pixel(image im, int x, int y, int c, float v)
{
    // TODO Fill this in
    if (x>=im.w){
        x=im.w-1;
    }else if(x<0){
        x=0;
    }
    if (y>=im.h){
        y=im.h-1;
    }else if(y<0){
        y=0;
    }
    if (c>=im.c){
        c=c-1;
    }else if(c<0){
        c=0;
    }
    im.data[im.h*im.w*c+im.w*y+x]=v;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    // TODO Fill this in
    for (int i=0;i<im.c;i=i+1){
        for (int j=0;j<im.h;j=j+1){
            for (int k=0;k<im.w;k=k+1){
                set_pixel(copy,k,j,i,get_pixel(im,k,j,i));
            }
        }
        
    }
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    // TODO Fill this in
    for (int i=0;i<im.c;i=i+1){
        for (int j=0;j<im.h;j=j+1){
            for (int k=0;k<im.w;k=k+1){
                set_pixel(gray,k,j,1,0.299*get_pixel(im,k,j,0)+0.587*get_pixel(im,k,j,1)+0.114*get_pixel(im,k,j,2));
            }
        }
        
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    // TODO Fill this in
    for (int j=0;j<im.h;j=j+1){
        for (int k=0;k<im.w;k=k+1){
            set_pixel(im,k,j,c,get_pixel(im,k,j,c)+v);
        }
    }
}

void clamp_image(image im)
{
    // TODO Fill this in
    for (int i=0;i<im.c;i=i+1){
        for (int j=0;j<im.h;j=j+1){
            for (int k=0;k<im.w;k=k+1){
                if (get_pixel(im,k,j,i)>1){
                    set_pixel(im,k,j,i,1);
                }else if(get_pixel(im,k,j,i)<0){
                    set_pixel(im,k,j,i,0);
                }
            }
        }
    }
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    // TODO Fill this in
    for (int j=0;j<im.h;j=j+1){
        for (int k=0;k<im.w;k=k+1){
            float R=get_pixel(im,k,j,0);
            float G=get_pixel(im,k,j,1);
            float B=get_pixel(im,k,j,2);
            float V=three_way_max(R,G,B);
            float m=three_way_min(R,G,B);
            float C=V-m;
            float S=0;
            if (V!=0){
                S=C/V;
            }
            float Hstar=0;
            if (C!=0){
                if (V==R){
                    Hstar=(G-B)/C;
                }else if (V==G){
                    Hstar=(B-R)/C+2;
                }else{
                    Hstar=(R-G)/C+4;
                }
            }
            float H=0;
            if (Hstar<0){
                H=Hstar/6+1;
            }else{
                H=Hstar/6;
            }
            while(H<0){
                H=H+1;
            }
            while(H>=1){
                H=H-1;
            }
            set_pixel(im,k,j,0,H);
            set_pixel(im,k,j,1,S);
            set_pixel(im,k,j,2,V);
        }
    }
}

void hsv_to_rgb(image im)
{
    // TODO Fill this in
    for (int j=0;j<im.h;j=j+1){
        for (int k=0;k<im.w;k=k+1){
            float H=get_pixel(im,k,j,0);
            while(H<0){
                H=H+1;
            }
            while(H>=1){
                H=H-1;
            }
            H=H*6;
            float S=get_pixel(im,k,j,1);
            float V=get_pixel(im,k,j,2);
            float Hi=floor(H);
            float F=H-Hi;
            float P=V*(1-S);
            float Q=V*(1-F*S);
            float T=V*(1-(1-F)*S);
            float R=H/6;
            float G=S;
            float B=V;
            if (Hi==0){
                R=V;
                G=T;
                B=P;
            }else if(Hi==1){
                R=Q;
                G=V;
                B=P;
            }else if(Hi==2){
                R=P;
                G=V;
                B=T;
            }else if(Hi==3){
                R=P;
                G=Q;
                B=V;
            }else if(Hi==4){
                R=T;
                G=P;
                B=V;
            }else if(Hi==5){
                R=V;
                G=P;
                B=Q;
            }
//            switch (Hi) {
//                case 0:
//                    R=V;
//                    G=T;
//                    B=P;
//                    break;
//                case 1:
//                    R=Q;
//                    G=V;
//                    B=P;
//                case 2:
//                    R=P;
//                    G=V;
//                    B=T;
//                    break;
//                case 3:
//                    R=P;
//                    G=Q;
//                    B=V;
//                case 4:
//                    R=T;
//                    G=P;
//                    B=V;
//                    break;
//                case 5:
//                    R=V;
//                    G=P;
//                    B=Q;
//                default:
//                    break;
//            }
            set_pixel(im,k,j,0,R);
            set_pixel(im,k,j,1,G);
            set_pixel(im,k,j,2,B);
        }
    }
}

void scale_image(image im, int c, float v){
    for (int j=0;j<im.h;j=j+1){
        for (int k=0;k<im.w;k=k+1){
            set_pixel(im,k,j,c,get_pixel(im,k,j,c)*v);
        }
    }
}
