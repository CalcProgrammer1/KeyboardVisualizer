/*---------------------------------------------------------*\
|  Cross Platform UDP Library for Windows and Linux         |
|  This library provides access to UDP ports with a         |
|  common API for both Windows and Linux systems.  It       |
|  features read and write                                  |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/9/2016        |
\*---------------------------------------------------------*/

#ifdef WIN32
#include <winsock2.h>
#include <winsock.h>
#include <WS2tcpip.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <net/if.h>
#endif

//UDP Port Class
//The reason for this class is that UDP ports are treated differently
//on Windows and Linux.  By creating a class, those differences can be
//made invisible to the program and make cross-platform usage easy

class udp_port
{
public:
    udp_port();
    udp_port(const char * client_name, const char * port);

    ~udp_port();

    //Function to open the port
    bool udp_open(const char* client_name, const char * port);

    //Function to close the port
    void udp_close();

    //Function to read data from the port buffer
    int udp_read(char * buffer, int length);

    //Function to write data to the serial port
    int udp_write(char * buffer, int length);

private:
    SOCKET sock;
    sockaddr_storage addrDest = {};
};