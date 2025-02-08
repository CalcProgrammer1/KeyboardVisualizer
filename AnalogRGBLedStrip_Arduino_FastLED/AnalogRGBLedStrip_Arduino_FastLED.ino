/*---------------------------------------------------------*\
|                                                           |
|   Analog RGB (FastLED) LED Strip Driver                   |
|   for use with Keyboard Visualizer VCUI                   |
|                                                           |
|   Adam Honse (calcprogrammer1) 12/9/2016                  |
|   Onur Coskun (onurcskun) 28/4/2019                       |
|                                                           |
\*---------------------------------------------------------*/
#include "FastLED.h"

#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PACKET_SZ 6
#define REDPIN   A0
#define GREENPIN A1
#define BLUEPIN  A2

unsigned char serial_buffer[PACKET_SZ];
unsigned int head = 0;
unsigned int start;
unsigned int checksum_1;
unsigned int checksum_0;

void showAnalogRGB(int r, int g, int b){
  analogWrite(REDPIN,   r );
  analogWrite(GREENPIN, g );
  analogWrite(BLUEPIN,  b );
}

void setup(){
  Serial.begin(115200);
}

void loop(){
  if (Serial.available()) {
    serial_buffer[head] = Serial.read();
 
    if (head >= (PACKET_SZ - 1)) {
      start = 0;
      checksum_1 = head;
      checksum_0 = head - 1;
      head = 0;
    }
    else {
      start = head + 1;
      checksum_1 = head;
      if ( head == 0 ){
        checksum_0 = PACKET_SZ - 1;
      }
      else {
        checksum_0 = head - 1;
      }
      head++;
    }
  
    if (serial_buffer[start] == 0xAA) {
      unsigned short sum = 0;

      for (int i = 0; i < checksum_0; i++){
        sum += serial_buffer[i];
      }

      if (start > 0) {
        for (int i = start; i < PACKET_SZ; i++) {
          sum += serial_buffer[i];
        }
      }

      //Test if valid write packet
      if ((((unsigned short)serial_buffer[checksum_0] << 8 ) | serial_buffer[checksum_1] ) == sum) {
        noInterrupts();
        int idx = start + 1;
        int g = serial_buffer[idx];
        int r = serial_buffer[idx+1];
        int b = serial_buffer[idx+2];
        showAnalogRGB(255-r, 255-g, 255-b);
        interrupts();
      }
    }
  }
}
