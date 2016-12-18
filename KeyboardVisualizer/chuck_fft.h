 //-----------------------------------------------------------------------------
// name: chuck_fft.h
// desc: fft impl - based on CARL distribution
//
// authors: code from San Diego CARL package
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
// date: 11.27.2003
//-----------------------------------------------------------------------------
#ifndef __CHUCK_FFT_H__
#define __CHUCK_FFT_H__


// complex type
typedef struct { float re ; float im ; } complex;

// complex absolute value
#define cmp_abs(x) ( sqrt( (x).re * (x).re + (x).im * (x).im ) )

#define FFT_FORWARD 1
#define FFT_INVERSE 0

// c linkage
#if ( defined( __cplusplus ) || defined( _cplusplus ) )
  extern "C" {
#endif

// make the window
void hanning( float * window, unsigned long length );
void hamming( float * window, unsigned long length );
void blackman( float * window, unsigned long length );
// apply the window
void apply_window( float * data, float * window, unsigned long length );

// real fft, N must be power of 2
void rfft( float * x, long N, unsigned int forward );
// complex fft, NC must be power of 2
void cfft( float * x, long NC, unsigned int forward );

// c linkage
#if ( defined( __cplusplus ) || defined( _cplusplus ) )
  }
#endif

#endif

