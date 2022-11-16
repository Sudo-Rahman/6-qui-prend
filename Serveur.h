//
// Created by rahman on 16/11/22.
//


#ifndef SYSTEMES_ET_RESEAUX_PROJET_SERVEUR_H
#define SYSTEMES_ET_RESEAUX_PROJET_SERVEUR_H


#include<stdlib.h>
#include<stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;



#define CRLF        "\r\n"
#define PORT         1977
#define MAX_CLIENTS     100

#define BUF_SIZE    1024

#include "Client.h"


#endif //SYSTEMES_ET_RESEAUX_PROJET_SERVEUR_H
