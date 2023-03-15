/*
 ********************************************************************
 * maxqfft.c 
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

// INCLUDE STATEMENTS ---------------------------
#include "maxqfft.h"

// DEFINE STATEMENTS ----------------------------

/*
 * Windowing: Uncomment one of the following define
 *   statements to enable the corresponding windowing
 *   function on input samples. Comment all to disable
 *   windowing.
 */
//#define WINDOWING_HAMMING
//#define WINDOWING_HANN

/*
 * x_n_re
 *
 * This array will store the FFT input samples, x(n),
 * and the real part of the spectrum, X(n).
 */
// short x_n_re[N];

/*
 * Hardware Multiplier Macros
 *
 * These macros are used to access the hardware multiplier. For
 * the MAXQ, registers MA and MB are the hardware multiplier
 * operands while MC1:MC0 store the hardware multiplier result.
 *
 * (1) MUL_1(A,B,C) : C=A*B  (result converted to Q8.7 notation)
 * (2) MUL_2(A,C)   : C=A*MB (result converted to Q8.7 notation)
 * (3) MUL_INIT(B)  : MB=B
 * (4) MUL_NC(A,C)  : C=A*MB (result not converted to Q8.7 notation)
 */

short MB;

#define MUL_1(A,B,C) MB=B;C=(A*B)>>7
#define MUL_2(A,C)   C=(A*MB)>>7
#define MUL_INIT(B)  MB=B
#define MUL_NC(A,C)  C=A*MB

/*
 * main()
 */
void fft(short* x_n_re)
{
   /* 4. FFT Loop */
      /* 4.0. Variable Declaration and Initialization */
      short i;      // Misc index
      
      int  n_of_b      = N_DIV_2; // Number of butterflies
      int  s_of_b      = 1;       // Size   of butterflies
      int  a_index     = 0;       // fft data index
      int  a_index_ref = 0;       // fft data index reference
      char stage       = 0;       // Stage of the fft, 0 to (Log2(N)-1)
      
      int  nb_index;    // Number of butterflies index
      int  sb_index;    // Size   of butterflies index

      short x_n_im[N] = {0x0000};   // Imaginary part of x(n) and X(n),
                                  // initialized to 0 before every fft
   
      /* 4.2. Perform Bit-Reversal: Uses an unrolled loop that was created with
                                    the following C code:
      
            #include <stdio.h>
            #define  N               256
            #define  LOG_2_N           8

            int bitRev(int a, int nBits)
            {
               int rev_a = 0;
               for (int i=0; i<nBits; i++)
               {
                  rev_a = (rev_a << 1) | (a & 1);
                  a     = a >> 1;
               }
               return rev_a;
            }

            int main(int argc, char* argv[])
            {
               printf("   unsigned int i;\n");
               for(int i=0; i<N; i++)
                  if (bitRev(i,LOG_2_N) > i)
                  {
                     printf("   i=x_n_re[%3d]; "       ,i);
                     printf("x_n_re[%3d]=x_n_re[%3d]; ",i,bitRev(i,LOG_2_N));
                     printf("x_n_re[%3d]=i;\n"         ,bitRev(i,LOG_2_N));
                  }

               return 0;
            }
      
      */
      i=x_n_re[  1]; x_n_re[  1]=x_n_re[128]; x_n_re[128]=i;
      i=x_n_re[  2]; x_n_re[  2]=x_n_re[ 64]; x_n_re[ 64]=i;
      i=x_n_re[  3]; x_n_re[  3]=x_n_re[192]; x_n_re[192]=i;
      i=x_n_re[  4]; x_n_re[  4]=x_n_re[ 32]; x_n_re[ 32]=i;
      i=x_n_re[  5]; x_n_re[  5]=x_n_re[160]; x_n_re[160]=i;
      i=x_n_re[  6]; x_n_re[  6]=x_n_re[ 96]; x_n_re[ 96]=i;
      i=x_n_re[  7]; x_n_re[  7]=x_n_re[224]; x_n_re[224]=i;
      i=x_n_re[  8]; x_n_re[  8]=x_n_re[ 16]; x_n_re[ 16]=i;
      i=x_n_re[  9]; x_n_re[  9]=x_n_re[144]; x_n_re[144]=i;
      i=x_n_re[ 10]; x_n_re[ 10]=x_n_re[ 80]; x_n_re[ 80]=i;
      i=x_n_re[ 11]; x_n_re[ 11]=x_n_re[208]; x_n_re[208]=i;
      i=x_n_re[ 12]; x_n_re[ 12]=x_n_re[ 48]; x_n_re[ 48]=i;
      i=x_n_re[ 13]; x_n_re[ 13]=x_n_re[176]; x_n_re[176]=i;
      i=x_n_re[ 14]; x_n_re[ 14]=x_n_re[112]; x_n_re[112]=i;
      i=x_n_re[ 15]; x_n_re[ 15]=x_n_re[240]; x_n_re[240]=i;
      i=x_n_re[ 17]; x_n_re[ 17]=x_n_re[136]; x_n_re[136]=i;
      i=x_n_re[ 18]; x_n_re[ 18]=x_n_re[ 72]; x_n_re[ 72]=i;
      i=x_n_re[ 19]; x_n_re[ 19]=x_n_re[200]; x_n_re[200]=i;
      i=x_n_re[ 20]; x_n_re[ 20]=x_n_re[ 40]; x_n_re[ 40]=i;
      i=x_n_re[ 21]; x_n_re[ 21]=x_n_re[168]; x_n_re[168]=i;
      i=x_n_re[ 22]; x_n_re[ 22]=x_n_re[104]; x_n_re[104]=i;
      i=x_n_re[ 23]; x_n_re[ 23]=x_n_re[232]; x_n_re[232]=i;
      i=x_n_re[ 25]; x_n_re[ 25]=x_n_re[152]; x_n_re[152]=i;
      i=x_n_re[ 26]; x_n_re[ 26]=x_n_re[ 88]; x_n_re[ 88]=i;
      i=x_n_re[ 27]; x_n_re[ 27]=x_n_re[216]; x_n_re[216]=i;
      i=x_n_re[ 28]; x_n_re[ 28]=x_n_re[ 56]; x_n_re[ 56]=i;
      i=x_n_re[ 29]; x_n_re[ 29]=x_n_re[184]; x_n_re[184]=i;
      i=x_n_re[ 30]; x_n_re[ 30]=x_n_re[120]; x_n_re[120]=i;
      i=x_n_re[ 31]; x_n_re[ 31]=x_n_re[248]; x_n_re[248]=i;
      i=x_n_re[ 33]; x_n_re[ 33]=x_n_re[132]; x_n_re[132]=i;
      i=x_n_re[ 34]; x_n_re[ 34]=x_n_re[ 68]; x_n_re[ 68]=i;
      i=x_n_re[ 35]; x_n_re[ 35]=x_n_re[196]; x_n_re[196]=i;
      i=x_n_re[ 37]; x_n_re[ 37]=x_n_re[164]; x_n_re[164]=i;
      i=x_n_re[ 38]; x_n_re[ 38]=x_n_re[100]; x_n_re[100]=i;
      i=x_n_re[ 39]; x_n_re[ 39]=x_n_re[228]; x_n_re[228]=i;
      i=x_n_re[ 41]; x_n_re[ 41]=x_n_re[148]; x_n_re[148]=i;
      i=x_n_re[ 42]; x_n_re[ 42]=x_n_re[ 84]; x_n_re[ 84]=i;
      i=x_n_re[ 43]; x_n_re[ 43]=x_n_re[212]; x_n_re[212]=i;
      i=x_n_re[ 44]; x_n_re[ 44]=x_n_re[ 52]; x_n_re[ 52]=i;
      i=x_n_re[ 45]; x_n_re[ 45]=x_n_re[180]; x_n_re[180]=i;
      i=x_n_re[ 46]; x_n_re[ 46]=x_n_re[116]; x_n_re[116]=i;
      i=x_n_re[ 47]; x_n_re[ 47]=x_n_re[244]; x_n_re[244]=i;
      i=x_n_re[ 49]; x_n_re[ 49]=x_n_re[140]; x_n_re[140]=i;
      i=x_n_re[ 50]; x_n_re[ 50]=x_n_re[ 76]; x_n_re[ 76]=i;
      i=x_n_re[ 51]; x_n_re[ 51]=x_n_re[204]; x_n_re[204]=i;
      i=x_n_re[ 53]; x_n_re[ 53]=x_n_re[172]; x_n_re[172]=i;
      i=x_n_re[ 54]; x_n_re[ 54]=x_n_re[108]; x_n_re[108]=i;
      i=x_n_re[ 55]; x_n_re[ 55]=x_n_re[236]; x_n_re[236]=i;
      i=x_n_re[ 57]; x_n_re[ 57]=x_n_re[156]; x_n_re[156]=i;
      i=x_n_re[ 58]; x_n_re[ 58]=x_n_re[ 92]; x_n_re[ 92]=i;
      i=x_n_re[ 59]; x_n_re[ 59]=x_n_re[220]; x_n_re[220]=i;
      i=x_n_re[ 61]; x_n_re[ 61]=x_n_re[188]; x_n_re[188]=i;
      i=x_n_re[ 62]; x_n_re[ 62]=x_n_re[124]; x_n_re[124]=i;
      i=x_n_re[ 63]; x_n_re[ 63]=x_n_re[252]; x_n_re[252]=i;
      i=x_n_re[ 65]; x_n_re[ 65]=x_n_re[130]; x_n_re[130]=i;
      i=x_n_re[ 67]; x_n_re[ 67]=x_n_re[194]; x_n_re[194]=i;
      i=x_n_re[ 69]; x_n_re[ 69]=x_n_re[162]; x_n_re[162]=i;
      i=x_n_re[ 70]; x_n_re[ 70]=x_n_re[ 98]; x_n_re[ 98]=i;
      i=x_n_re[ 71]; x_n_re[ 71]=x_n_re[226]; x_n_re[226]=i;
      i=x_n_re[ 73]; x_n_re[ 73]=x_n_re[146]; x_n_re[146]=i;
      i=x_n_re[ 74]; x_n_re[ 74]=x_n_re[ 82]; x_n_re[ 82]=i;
      i=x_n_re[ 75]; x_n_re[ 75]=x_n_re[210]; x_n_re[210]=i;
      i=x_n_re[ 77]; x_n_re[ 77]=x_n_re[178]; x_n_re[178]=i;
      i=x_n_re[ 78]; x_n_re[ 78]=x_n_re[114]; x_n_re[114]=i;
      i=x_n_re[ 79]; x_n_re[ 79]=x_n_re[242]; x_n_re[242]=i;
      i=x_n_re[ 81]; x_n_re[ 81]=x_n_re[138]; x_n_re[138]=i;
      i=x_n_re[ 83]; x_n_re[ 83]=x_n_re[202]; x_n_re[202]=i;
      i=x_n_re[ 85]; x_n_re[ 85]=x_n_re[170]; x_n_re[170]=i;
      i=x_n_re[ 86]; x_n_re[ 86]=x_n_re[106]; x_n_re[106]=i;
      i=x_n_re[ 87]; x_n_re[ 87]=x_n_re[234]; x_n_re[234]=i;
      i=x_n_re[ 89]; x_n_re[ 89]=x_n_re[154]; x_n_re[154]=i;
      i=x_n_re[ 91]; x_n_re[ 91]=x_n_re[218]; x_n_re[218]=i;
      i=x_n_re[ 93]; x_n_re[ 93]=x_n_re[186]; x_n_re[186]=i;
      i=x_n_re[ 94]; x_n_re[ 94]=x_n_re[122]; x_n_re[122]=i;
      i=x_n_re[ 95]; x_n_re[ 95]=x_n_re[250]; x_n_re[250]=i;
      i=x_n_re[ 97]; x_n_re[ 97]=x_n_re[134]; x_n_re[134]=i;
      i=x_n_re[ 99]; x_n_re[ 99]=x_n_re[198]; x_n_re[198]=i;
      i=x_n_re[101]; x_n_re[101]=x_n_re[166]; x_n_re[166]=i;
      i=x_n_re[103]; x_n_re[103]=x_n_re[230]; x_n_re[230]=i;
      i=x_n_re[105]; x_n_re[105]=x_n_re[150]; x_n_re[150]=i;
      i=x_n_re[107]; x_n_re[107]=x_n_re[214]; x_n_re[214]=i;
      i=x_n_re[109]; x_n_re[109]=x_n_re[182]; x_n_re[182]=i;
      i=x_n_re[110]; x_n_re[110]=x_n_re[118]; x_n_re[118]=i;
      i=x_n_re[111]; x_n_re[111]=x_n_re[246]; x_n_re[246]=i;
      i=x_n_re[113]; x_n_re[113]=x_n_re[142]; x_n_re[142]=i;
      i=x_n_re[115]; x_n_re[115]=x_n_re[206]; x_n_re[206]=i;
      i=x_n_re[117]; x_n_re[117]=x_n_re[174]; x_n_re[174]=i;
      i=x_n_re[119]; x_n_re[119]=x_n_re[238]; x_n_re[238]=i;
      i=x_n_re[121]; x_n_re[121]=x_n_re[158]; x_n_re[158]=i;
      i=x_n_re[123]; x_n_re[123]=x_n_re[222]; x_n_re[222]=i;
      i=x_n_re[125]; x_n_re[125]=x_n_re[190]; x_n_re[190]=i;
      i=x_n_re[127]; x_n_re[127]=x_n_re[254]; x_n_re[254]=i;
      i=x_n_re[131]; x_n_re[131]=x_n_re[193]; x_n_re[193]=i;
      i=x_n_re[133]; x_n_re[133]=x_n_re[161]; x_n_re[161]=i;
      i=x_n_re[135]; x_n_re[135]=x_n_re[225]; x_n_re[225]=i;
      i=x_n_re[137]; x_n_re[137]=x_n_re[145]; x_n_re[145]=i;
      i=x_n_re[139]; x_n_re[139]=x_n_re[209]; x_n_re[209]=i;
      i=x_n_re[141]; x_n_re[141]=x_n_re[177]; x_n_re[177]=i;
      i=x_n_re[143]; x_n_re[143]=x_n_re[241]; x_n_re[241]=i;
      i=x_n_re[147]; x_n_re[147]=x_n_re[201]; x_n_re[201]=i;
      i=x_n_re[149]; x_n_re[149]=x_n_re[169]; x_n_re[169]=i;
      i=x_n_re[151]; x_n_re[151]=x_n_re[233]; x_n_re[233]=i;
      i=x_n_re[155]; x_n_re[155]=x_n_re[217]; x_n_re[217]=i;
      i=x_n_re[157]; x_n_re[157]=x_n_re[185]; x_n_re[185]=i;
      i=x_n_re[159]; x_n_re[159]=x_n_re[249]; x_n_re[249]=i;
      i=x_n_re[163]; x_n_re[163]=x_n_re[197]; x_n_re[197]=i;
      i=x_n_re[167]; x_n_re[167]=x_n_re[229]; x_n_re[229]=i;
      i=x_n_re[171]; x_n_re[171]=x_n_re[213]; x_n_re[213]=i;
      i=x_n_re[173]; x_n_re[173]=x_n_re[181]; x_n_re[181]=i;
      i=x_n_re[175]; x_n_re[175]=x_n_re[245]; x_n_re[245]=i;
      i=x_n_re[179]; x_n_re[179]=x_n_re[205]; x_n_re[205]=i;
      i=x_n_re[183]; x_n_re[183]=x_n_re[237]; x_n_re[237]=i;
      i=x_n_re[187]; x_n_re[187]=x_n_re[221]; x_n_re[221]=i;
      i=x_n_re[191]; x_n_re[191]=x_n_re[253]; x_n_re[253]=i;
      i=x_n_re[199]; x_n_re[199]=x_n_re[227]; x_n_re[227]=i;
      i=x_n_re[203]; x_n_re[203]=x_n_re[211]; x_n_re[211]=i;
      i=x_n_re[207]; x_n_re[207]=x_n_re[243]; x_n_re[243]=i;
      i=x_n_re[215]; x_n_re[215]=x_n_re[235]; x_n_re[235]=i;
      i=x_n_re[223]; x_n_re[223]=x_n_re[251]; x_n_re[251]=i;
      i=x_n_re[239]; x_n_re[239]=x_n_re[247]; x_n_re[247]=i;


      /* 4.3. FFT: loop through the 0 to log2(N) stages of
                   the butterfly computations. When the FFT
                   begins, the input samples (x(n)) are stored
                   in x_n_re/x_n_im. When the FFT is done,
                   the spectrum (X(n)) has replaced the input
                   stored in x_n_re/x_n_im.
                   
      */
      for(stage=0; stage<LOG_2_N; stage++)
      {
         for(nb_index=0; nb_index<n_of_b; nb_index++)
         {
            int tf_index = 0; // The twiddle factor index
            for(sb_index=0; sb_index<s_of_b; sb_index++)
            {
               short resultMulReCos;
               short resultMulImCos;
               short resultMulReSin;
               short resultMulImSin;            
                         int b_index = a_index+s_of_b; // 2nd fft data index             
                          
               MUL_1(cosLUT[tf_index],x_n_re[b_index],resultMulReCos);
               MUL_2(sinLUT[tf_index],resultMulReSin);
               MUL_1(cosLUT[tf_index],x_n_im[b_index],resultMulImCos);
               MUL_2(sinLUT[tf_index],resultMulImSin);
         
               x_n_re[b_index] = x_n_re[a_index]-resultMulReCos+resultMulImSin;
               x_n_im[b_index] = x_n_im[a_index]-resultMulReSin-resultMulImCos;
               x_n_re[a_index] = x_n_re[a_index]+resultMulReCos-resultMulImSin;
               x_n_im[a_index] = x_n_im[a_index]+resultMulReSin+resultMulImCos;
            
               if (((sb_index+1) & (s_of_b-1)) == 0)
                  a_index = a_index_ref;
               else
                  a_index++;

               tf_index += n_of_b;
            }
            a_index     = ((s_of_b<<1) + a_index) & N_MINUS_1;
            a_index_ref = a_index;
         }
         n_of_b >>= 1;
         s_of_b <<= 1;
      }


      /* 4.4. abs(X(n)): Loop through N/2+1 (0 to N/2) FFT results (stored in
                         x_n_re and x_n_im) and make all the values positive.
                         This will be needed for the algorithm used to compute
                         the magnitude of X(n).
      */
      MUL_INIT(-1);
      for(i=0; i<N_DIV_2_PLUS_1; i++)
      {
         // If Re{X(n)} is negative, multiply by -1
         if ((x_n_re[i] & 0x8000)!=0x0000)
         {
            MUL_NC(x_n_re[i],x_n_re[i]);
         }
         // If Im{X(n)} is negative, multiply by -1
         if ((x_n_im[i] & 0x8000)!=0x0000)
         {
            MUL_NC(x_n_im[i],x_n_im[i]);
         }    
      }   


      /* 4.5. |X(n)|: Compute the magniture of X(n) using a LUT. This is
                      possible only because the values of Re{X(n)} and
                      Im{X(n)} are all positive. Note that Im{X(0)} and
                      Im{X(N/2)} contain no data and therefore computing
                      |X(0)| and |X(N/2)| requires only the real part
                      of X(n).
                      
                      The magniture LUT is declared as:
                      
                         const unsigned char magnLUT[16][16] = {...};
                      
                      where the first index is abs(Re{X(n)}) and the second
                      index is abs(Im{X(n)}). Since X(n) is stored as Q8.7,
                      and the 4 most significant bits (2^4=16) are to be used
                      as the indexes, the Re{X(n)} and Im{X(n)} must be right
                      shifted 11 positions.
      */
      x_n_re[0] = magnLUT[x_n_re[0]>>11][0];
      for(i=1; i<N_DIV_2; i++)
         x_n_re[i] = magnLUT[x_n_re[i]>>11][x_n_im[i]>>11];
      x_n_re[N_DIV_2] = magnLUT[x_n_re[N_DIV_2]>>11][0];
}
