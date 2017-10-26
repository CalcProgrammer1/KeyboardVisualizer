/*---------------------------------------------------------*\
|                                                           |
|   RGB LED Serial Controlled LED Strip Driver              |
|     for use with Keyboard Visualizer VCUI                 |
|                                                           |
|   Adam Honse (calcprogrammer1) 12/9/2016                  |
|                                                           |
\*---------------------------------------------------------*/

#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

const char* ssid     = "";
const char* password = "";

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;

char packet_buf[1024];

#define LEDS 1
#define PACKET_SZ ( (LEDS * 3) + 3 )


void setup()
{ 
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Udp.begin(1234);
  
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
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
        for( int i = 0; i < LEDS; i++ )
        {
          int idx = 1 + ( 3 * i );

          analogWrite(3, 4*packet_buf[idx]);
          analogWrite(1, 4*packet_buf[idx+1]);
          analogWrite(2, 4*packet_buf[idx+2]);
        }
      }
    }
  }
}

