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

#ifndef __MAXQ_FFT_H__
#define __MAXQ_FFT_H__

/* DEFINE STATEMENTS */
#define N                 256
#define N_DIV_2           128
#define N_DIV_2_PLUS_1    129
#define N_MINUS_1         255
#define LOG_2_N             8

void fft(short*);
#endif
