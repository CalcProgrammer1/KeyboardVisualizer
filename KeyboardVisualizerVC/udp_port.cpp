/*---------------------------------------------------------*\
|  Cross Platform UDP Library for Windows and Linux         |
|  This library provides access to UDP ports with a         |
|  common API for both Windows and Linux systems.  It       |
|  features read and write                                  |
|                                                           |
|  Adam Honse (calcprogrammer1@gmail.com), 12/9/2016        |
\*---------------------------------------------------------*/

#include "udp_port.h"

#include <sys/types.h>
#include <memory.h>
#include <errno.h>
#include <stdlib.h>
#include <iostream>

//udp_port (constructor)
//	When created with port information, the constructor
//	will automatically open client address <client_name> on port <port>
udp_port::udp_port(const char * client_name, const char * port)
{
    udp_open(client_name, port);
}

bool udp_port::udp_open(const char * client_name, const char * port)
{
    int result = 0;
    sockaddr_in addrListen = {};
    addrListen.sin_family = AF_INET;
    WSADATA wsa;

    WSAStartup(MAKEWORD(2, 2), &wsa);

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    result = bind(sock, (sockaddr*)&addrListen, sizeof(addrListen));

    if (result == -1)
    {
        *(int *)(0x0000000) = 5;
        return(FALSE);
    }

    addrinfo* result_list = NULL;
    addrinfo hints = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    result = getaddrinfo(client_name, port, &hints, &result_list);

    if (result == 0)
    {
        memcpy(&addrDest, result_list->ai_addr, result_list->ai_addrlen);
        freeaddrinfo(result_list);
    }
    else
    {
        return(FALSE);
    }
}

int udp_port::udp_write(char * buffer, int length)
{
    return sendto(sock, buffer, length, 0, (sockaddr *)&addrDest, sizeof(addrDest));
}