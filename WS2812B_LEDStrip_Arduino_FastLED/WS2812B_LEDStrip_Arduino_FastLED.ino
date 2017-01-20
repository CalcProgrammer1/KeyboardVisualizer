/*---------------------------------------------------------*\
|                                                           |
|   WS2812B (FastLED) Serial Controlled LED Strip Driver    |
|     for use with Keyboard Visualizer VCUI                 |
|                                                           |
|   Adam Honse (calcprogrammer1) 12/9/2016                  |
|                                                           |
\*---------------------------------------------------------*/
#include "FastLED.h"
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6
#define LEDS 30
#define PACKET_SZ ( (LEDS * 3) + 3 )

CRGB leds[LEDS];
unsigned char serial_buffer[PACKET_SZ];
unsigned int head = 0;
unsigned int start;
unsigned int checksum_1;
unsigned int checksum_0;

void setup()
{
  FastLED.addLeds<WS2812B, PIN, RGB>(leds, LEDS);
  for( int i = 0; i < LEDS; i++ )
  {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
  
  Serial.begin(115200);
}

void loop()
{
  if( Serial.available() )
  {
    serial_buffer[head] = Serial.read();
  
    if( head >= (PACKET_SZ - 1) )
    {
      start = 0;
      checksum_1 = head;
      checksum_0 = head - 1;
      head = 0;
    }
    else
    {
      start = head + 1;
      checksum_1 = head;
      if( head == 0 )
      {
        checksum_0 = PACKET_SZ - 1;
      }
      else
      {
        checksum_0 = head - 1;
      }
      head++;
    }
  
    if( serial_buffer[start] == 0xAA )
    {
      unsigned short sum = 0;

      for( int i = 0; i < checksum_0; i++ )
      {
        sum += serial_buffer[i];
      }

      if( start > 0 )
      {
        for( int i = start; i < PACKET_SZ; i++ )
        {
          sum += serial_buffer[i];
        }
      }
      
      //Test if valid write packet
      if( ( ( (unsigned short)serial_buffer[checksum_0] << 8 ) | serial_buffer[checksum_1] ) == sum )
      {
        noInterrupts();
        for( int i = 0; i < LEDS; i++ )
        {
          int idx = start + 1 + ( 3 * i );
  
          if( idx >= (PACKET_SZ - 1) )
          {
            idx = idx - PACKET_SZ;
          }
          
          leds[i].g = serial_buffer[idx];
          leds[i].r = serial_buffer[idx+1];
          leds[i].b = serial_buffer[idx+2];
        }
        
        FastLED.show();
        interrupts();
      }
    }
  }
}
