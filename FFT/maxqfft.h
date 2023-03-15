/*
 ********************************************************************
 * maxqfft.h
 *
 * July 01, 2005
 *
 * Paul Holden (Paul_Holden@maximhq.com)
 * Maxim Integrated Products
 *
 * SOFTWARE COMPILES USING IAR EMBEDDED WORKBENCH FOR MAXQ
 * 
 * NOTE: All fft input/outputs are signed and in Q8.7 notation
 *
 * Copyright (C) 2005 Maxim/Dallas Semiconductor Corporation,
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM/DALLAS SEMICONDUCTOR BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim/Dallas Semiconductor
 * shall not be used except as stated in the Maxim/Dallas Semiconductor
 * Branding Policy.
 *
 ********************************************************************
 */

#include "fft.h"

/*
   cosine Look-Up Table: An LUT for the cosine function in Q8.7. The
                         table was created with the following program:
                         
     #include <stdio.h>
     #include <math.h>
     #define  N 256
     void main(int argc, char* argv[])
     {
        printf("const int cosLUT[%d] =\n{\n",N/2);
        for(int i=0; i<N/2; i++)
        {
           printf("%+4d",(int)(128*cos(2*M_PI*i/N)));
           if (i<(N/2-1))     printf(",");
           if ((i+1)%16 == 0) printf("\n");
        }
        printf("};\n");
     }
*/
const short cosLUT[N_DIV_2] =
{
+128,+127,+127,+127,+127,+127,+126,+126,+125,+124,+124,+123,+122,+121,+120,+119,
+118,+117,+115,+114,+112,+111,+109,+108,+106,+104,+102,+100, +98, +96, +94, +92,
 +90, +88, +85, +83, +81, +78, +76, +73, +71, +68, +65, +63, +60, +57, +54, +51,
 +48, +46, +43, +40, +37, +34, +31, +28, +24, +21, +18, +15, +12,  +9,  +6,  +3,
  +0,  -3,  -6,  -9, -12, -15, -18, -21, -24, -28, -31, -34, -37, -40, -43, -46,
 -48, -51, -54, -57, -60, -63, -65, -68, -71, -73, -76, -78, -81, -83, -85, -88,
 -90, -92, -94, -96, -98,-100,-102,-104,-106,-108,-109,-111,-112,-114,-115,-117,
-118,-119,-120,-121,-122,-123,-124,-124,-125,-126,-126,-127,-127,-127,-127,-127
};

/*
   sine Look-Up Table: An LUT for the sine function in Q8.7. The
                       table was created with the following program:

     #include <stdio.h>
     #include <math.h>
     #define  N 256
     void main(int argc, char* argv[])
     {
        printf("const int sinLUT[%d] =\n{\n",N/2);
        for(int i=0; i<N/2; i++)
        {
           printf("%+4d",(int)(128*sin(2*M_PI*i/N)));
           if (i<(N/2-1))     printf(",");
           if ((i+1)%16 == 0) printf("\n");
        }
        printf("};\n");
     }
*/
const short sinLUT[N_DIV_2] =
{
  +0,  +3,  +6,  +9, +12, +15, +18, +21, +24, +28, +31, +34, +37, +40, +43, +46,
 +48, +51, +54, +57, +60, +63, +65, +68, +71, +73, +76, +78, +81, +83, +85, +88,
 +90, +92, +94, +96, +98,+100,+102,+104,+106,+108,+109,+111,+112,+114,+115,+117,
+118,+119,+120,+121,+122,+123,+124,+124,+125,+126,+126,+127,+127,+127,+127,+127,
+128,+127,+127,+127,+127,+127,+126,+126,+125,+124,+124,+123,+122,+121,+120,+119,
+118,+117,+115,+114,+112,+111,+109,+108,+106,+104,+102,+100, +98, +96, +94, +92,
 +90, +88, +85, +83, +81, +78, +76, +73, +71, +68, +65, +63, +60, +57, +54, +51,
 +48, +46, +43, +40, +37, +34, +31, +28, +24, +21, +18, +15, +12,  +9,  +6,  +3
};

/*
   Hamming Window Look-Up Table: An LUT for the Hamming Window function
                                 in Q8.7. The table was created with
                                 the following program:
                                 
     #include <stdio.h>
     #include <math.h>
     #define  N 256
     void main(int argc, char* argv[])
     {
        printf("const char hammingLUT[%d] =\n{\n",N);
        for(int i=0; i<N; i++)
        {
           printf("%+4d",(int)(128*(0.54-0.46*cos(2*M_PI*i/(N-1)))));
           if (i<(N-1))       printf(",");
           if ((i+1)%16 == 0) printf("\n");
        }
        printf("};\n");
     }
*/
const char hammingLUT[N] =
{
 +10, +10, +10, +10, +10, +10, +10, +11, +11, +11, +12, +12, +12, +13, +13, +14,
 +14, +15, +15, +16, +17, +17, +18, +19, +20, +21, +21, +22, +23, +24, +25, +26,
 +27, +28, +29, +30, +31, +33, +34, +35, +36, +37, +39, +40, +41, +42, +44, +45,
 +46, +48, +49, +50, +52, +53, +55, +56, +57, +59, +60, +62, +63, +65, +66, +68,
 +69, +70, +72, +73, +75, +76, +78, +79, +81, +82, +83, +85, +86, +88, +89, +90,
 +92, +93, +94, +96, +97, +98, +99,+101,+102,+103,+104,+105,+106,+107,+109,+110,
+111,+112,+113,+114,+114,+115,+116,+117,+118,+119,+119,+120,+121,+121,+122,+123,
+123,+124,+124,+125,+125,+126,+126,+126,+126,+127,+127,+127,+127,+127,+127,+127,
+127,+127,+127,+127,+127,+127,+127,+126,+126,+126,+126,+125,+125,+124,+124,+123,
+123,+122,+121,+121,+120,+119,+119,+118,+117,+116,+115,+114,+114,+113,+112,+111,
+110,+109,+107,+106,+105,+104,+103,+102,+101, +99, +98, +97, +96, +94, +93, +92,
 +90, +89, +88, +86, +85, +83, +82, +81, +79, +78, +76, +75, +73, +72, +70, +69,
 +68, +66, +65, +63, +62, +60, +59, +57, +56, +55, +53, +52, +50, +49, +48, +46,
 +45, +44, +42, +41, +40, +39, +37, +36, +35, +34, +33, +31, +30, +29, +28, +27,
 +26, +25, +24, +23, +22, +21, +21, +20, +19, +18, +17, +17, +16, +15, +15, +14,
 +14, +13, +13, +12, +12, +12, +11, +11, +11, +10, +10, +10, +10, +10, +10, +10
};

/*
   Hann Window Look-Up Table: An LUT for the Hann Window function
                              in Q8.7. The table was created with
                              the following program:
                                 
     #include <stdio.h>
     #include <math.h>
     #define  N 256
     void main(int argc, char* argv[])
     {
        printf("const char hannLUT[%d] =\n{\n",N);
        for(int i=0; i<N; i++)
        {
           printf("%+4d",(int)(128*(0.5-0.5*cos(2*M_PI*i/(N-1)))));
           if (i<(N-1))       printf(",");
           if ((i+1)%16 == 0) printf("\n");
        }
        printf("};\n");
     }
*/
const char hannLUT[N] =
{
  +0,  +0,  +0,  +0,  +0,  +0,  +0,  +0,  +1,  +1,  +1,  +2,  +2,  +3,  +3,  +4,
  +4,  +5,  +6,  +6,  +7,  +8,  +9, +10, +10, +11, +12, +13, +14, +15, +16, +17,
 +18, +20, +21, +22, +23, +24, +26, +27, +28, +29, +31, +32, +34, +35, +36, +38,
 +39, +41, +42, +44, +45, +47, +48, +50, +51, +53, +54, +56, +58, +59, +61, +62,
 +64, +65, +67, +69, +70, +72, +73, +75, +76, +78, +79, +81, +83, +84, +86, +87,
 +88, +90, +91, +93, +94, +95, +97, +98,+100,+101,+102,+103,+105,+106,+107,+108,
+109,+110,+111,+112,+113,+114,+115,+116,+117,+118,+119,+120,+120,+121,+122,+122,
+123,+123,+124,+124,+125,+125,+126,+126,+126,+127,+127,+127,+127,+127,+127,+127,
+127,+127,+127,+127,+127,+127,+127,+126,+126,+126,+125,+125,+124,+124,+123,+123,
+122,+122,+121,+120,+120,+119,+118,+117,+116,+115,+114,+113,+112,+111,+110,+109,
+108,+107,+106,+105,+103,+102,+101,+100, +98, +97, +96, +94, +93, +91, +90, +88,
 +87, +86, +84, +83, +81, +79, +78, +76, +75, +73, +72, +70, +69, +67, +65, +64,
 +62, +61, +59, +58, +56, +54, +53, +51, +50, +48, +47, +45, +44, +42, +41, +39,
 +38, +36, +35, +34, +32, +31, +29, +28, +27, +26, +24, +23, +22, +21, +20, +18,
 +17, +16, +15, +14, +13, +12, +11, +10, +10,  +9,  +8,  +7,  +6,  +6,  +5,  +4,
  +4,  +3,  +3,  +2,  +2,  +1,  +1,  +1,  +0,  +0,  +0,  +0,  +0,  +0,  +0,  +0
};

/*
   Magnitude Look-Up Table: A LUT for determining the magnitude of X(n). The
                            actual magnitude is calculated using the following
                            equation:
                            
                                  |X(n)| = sqrt(Re{X(n)}^2 + Im{X(n)}^2)
                            
                            The first LUT index corresponds to the real part
                            of X(n) while the second index corresponds to the
                            imaginary part of X(n). If X(n) is in Q8.7, the
                            absolut value of the real and imaginary parts
                            must be shifted to the right by 11 positions to 
                            obtain the 4-MSb (Most Significant bits) needed
                            for each of the two indexed (2^4 = 16). The table
                            was created with the following program: 
              
     #include <stdio.h>
     #include <math.h>
     #define  N 256
     long magn[16][16] = {{0}};
     void main(int argc, char* argv[])
     {
        printf("const unsigned char magnLUT[16][16] =\n{\n");
        for(int i=0; i<16; i++)
        {
           printf("{");
           for(int j=0; j<16; j++)
           {
                magn[i][j] = (long)sqrt(pow(i<<12,2) + pow(j<<12,2));
                if (magn[i][j] > 0xffff)
                   magn[i][j] = 0xffff;
                printf("0x%02x",(magn[i][j] >> 8) & 0x000000ff);
                if (j<15)
                   printf(",");
           }
           if (i<15) printf("},\n");
           else      printf("}\n");
        }
        printf("};\n");
     }
*/
const unsigned char magnLUT[16][16] =
{
{0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,0x90,0xa0,0xb0,0xc0,0xd0,0xe0,0xf0},
{0x10,0x16,0x23,0x32,0x41,0x51,0x61,0x71,0x80,0x90,0xa0,0xb0,0xc0,0xd0,0xe0,0xf0},
{0x20,0x23,0x2d,0x39,0x47,0x56,0x65,0x74,0x83,0x93,0xa3,0xb2,0xc2,0xd2,0xe2,0xf2},
{0x30,0x32,0x39,0x43,0x50,0x5d,0x6b,0x79,0x88,0x97,0xa7,0xb6,0xc5,0xd5,0xe5,0xf4},
{0x40,0x41,0x47,0x50,0x5a,0x66,0x73,0x80,0x8f,0x9d,0xac,0xbb,0xca,0xd9,0xe8,0xf8},
{0x50,0x51,0x56,0x5d,0x66,0x71,0x7c,0x89,0x96,0xa4,0xb2,0xc1,0xd0,0xde,0xed,0xfc},
{0x60,0x61,0x65,0x6b,0x73,0x7c,0x87,0x93,0xa0,0xad,0xba,0xc8,0xd6,0xe5,0xf3,0xff},
{0x70,0x71,0x74,0x79,0x80,0x89,0x93,0x9e,0xaa,0xb6,0xc3,0xd0,0xde,0xec,0xfa,0xff},
{0x80,0x80,0x83,0x88,0x8f,0x96,0xa0,0xaa,0xb5,0xc0,0xcc,0xd9,0xe6,0xf4,0xff,0xff},
{0x90,0x90,0x93,0x97,0x9d,0xa4,0xad,0xb6,0xc0,0xcb,0xd7,0xe3,0xf0,0xfc,0xff,0xff},
{0xa0,0xa0,0xa3,0xa7,0xac,0xb2,0xba,0xc3,0xcc,0xd7,0xe2,0xed,0xf9,0xff,0xff,0xff},
{0xb0,0xb0,0xb2,0xb6,0xbb,0xc1,0xc8,0xd0,0xd9,0xe3,0xed,0xf8,0xff,0xff,0xff,0xff},
{0xc0,0xc0,0xc2,0xc5,0xca,0xd0,0xd6,0xde,0xe6,0xf0,0xf9,0xff,0xff,0xff,0xff,0xff},
{0xd0,0xd0,0xd2,0xd5,0xd9,0xde,0xe5,0xec,0xf4,0xfc,0xff,0xff,0xff,0xff,0xff,0xff},
{0xe0,0xe0,0xe2,0xe5,0xe8,0xed,0xf3,0xfa,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
{0xf0,0xf0,0xf2,0xf4,0xf8,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}
};

