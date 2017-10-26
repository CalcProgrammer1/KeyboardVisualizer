/*---------------------------------------------------------*\
|                                                           |
|   WS2812B (NeoPixel) Serial Controlled LED Strip Driver   |
|     for use with Keyboard Visualizer VCUI                 |
|                                                           |
|   Adam Honse (calcprogrammer1) 12/9/2016                  |
|                                                           |
\*---------------------------------------------------------*/

#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include "FastLED.h"

const char* ssid     = "";
const char* password = "";

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;

char packet_buf[1024];

#define LEDS_PER_STRIP 50
#define STRIPS 4
#define PACKET_SZ ( (LEDS_PER_STRIP * STRIPS * 3) + 3 )

CRGB leds[STRIPS][LEDS_PER_STRIP];

void setup()
{
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

  FastLED.addLeds<WS2811, 0> (leds[0], LEDS_PER_STRIP);  
  FastLED.addLeds<WS2811, 1> (leds[1], LEDS_PER_STRIP);
  FastLED.addLeds<WS2811, 3> (leds[2], LEDS_PER_STRIP);
  FastLED.addLeds<WS2811, 2> (leds[3], LEDS_PER_STRIP);

leds[0][1].r = 0;
leds[0][1].g = 0;
leds[0][1].b = 0;

leds[1][1].r = 0;
leds[1][1].g = 0;
leds[1][1].b = 0;

leds[2][1].r = 255;
leds[2][1].g = 255;
leds[2][1].b = 0;

leds[3][1].r = 255;
leds[3][1].g = 255;
leds[3][1].b = 255;

FastLED.show();
  delay(10);

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Udp.begin(1234);
}

int value = 0;

void loop()
{
  int noBytes = Udp.parsePacket();

  if( noBytes )
  {
    Udp.read(packet_buf, noBytes);

    if( noBytes == PACKET_SZ && packet_buf[0] == 0xAA )
      {
      unsigned short sum = 0;
      int checksum_0 = PACKET_SZ - 2;
      int checksum_1 = PACKET_SZ - 1;

      for( int i = 0; i < checksum_0; i++ )
        {
          sum += packet_buf[i];
        }
        
      //Test if valid write packet
      if( ( ( (unsigned short)packet_buf[checksum_0] << 8 ) | packet_buf[checksum_1] ) == sum )
      {
        for( int i = 0; i < STRIPS; i++ )
        {
          for( int j = 0; j < LEDS_PER_STRIP; j++ )
          {
          int idx = 1 + ( i * LEDS_PER_STRIP * 3 ) + ( 3 * j );
          
          leds[i][j].r = packet_buf[idx];
          leds[i][j].g = packet_buf[idx+1];
          leds[i][j].b = packet_buf[idx+2];
          }
        FastLED.show();
        }
      }
    }
  }
}

