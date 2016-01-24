/*---------------------------------------------------------*\
|                                                           |
|   WS2812B (NeoPixel) Serial Controlled LED Strip Driver   |
|     for use with Keyboard Visualizer VCUI                 |
|                                                           |
|   Adam Honse (calcprogrammer1) 1/22/2016                  |
|                                                           |
\*---------------------------------------------------------*/
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(30, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.
unsigned char serial_buffer[92];

void setup()
{
  Serial.begin(115200);
  strip.begin();
  strip.show();
}

void loop()
{
if( Serial.available() )
  {
  for( int i = 0; i < 91; i++ )
  {
    serial_buffer[i] = serial_buffer[i+1];
  }
  serial_buffer[91] = Serial.read();

  unsigned char sum = 0;
  for( int i = 0; i < 91; i++ )
  {
    sum += serial_buffer[i];
  }
  //Test if valid write packet
  if(serial_buffer[0] == 0xAA && serial_buffer[91] == sum)
  {
    for( int i = 0; i < 30; i++ )
    {
      int idx = 1 + ( 3 * i );
      strip.setPixelColor(i, strip.Color(serial_buffer[idx], serial_buffer[idx+1], serial_buffer[idx+2]));
    }
    noInterrupts();
    strip.show();
    interrupts();
  }
  }
}
