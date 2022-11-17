//
// Created by rahman on 16/11/22.
//

#ifndef SYSTEMES_ET_RESEAUX_PROJET_CLIENT_H
#define SYSTEMES_ET_RESEAUX_PROJET_CLIENT_H


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

char *hostname = "127.0.0.1";

int PORT = 65534;


#endif //SYSTEMES_ET_RESEAUX_PROJET_CLIENT_H
