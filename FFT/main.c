#include <math.h>
#include <stdio.h>
#include "fft.h"

void usb_write(short c)
{printf("%hd\n",c);}

/*
 * getSamplesFromADC()
 *
 * Captures N 8-bit samples (in 2's complement format) from
 * the ADC and stores them in the array x_n_re. If windowing
 * is enabled, the data will be multiplied by the
 * appropriate function.
 */
void getSamplesFromADC(short *x_n_re)
{int i;
 for (i=0;i<N;i++)
    x_n_re[i]=(short)(127*sin((float)(i)/4.));
}

void main()
{  int i;
   short x_n_re[N];

   getSamplesFromADC(x_n_re);
   fft(x_n_re);

   /* 4.6. Xmit |X(n)|: Transmits the magnitude of X(n) using UART0 */
   for(i=0; i<N_DIV_2_PLUS_1; i++)
      {
         usb_write(x_n_re[i]); // Send magn
      }
//   while (1) {}
}
