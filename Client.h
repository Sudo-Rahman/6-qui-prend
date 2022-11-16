//
// Created by rahman on 16/11/22.
//

#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/wait.h>
#include<signal.h>
#include <time.h>
#include <pcap/socket.h>

#ifndef SYSTEMES_ET_RESEAUX_PROJET_CLIENT_H
#define SYSTEMES_ET_RESEAUX_PROJET_CLIENT_H

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;


#endif //SYSTEMES_ET_RESEAUX_PROJET_CLIENT_H
