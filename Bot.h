//
// Created by Rahman  Yilmaz on 01/12/2022.
//

#ifndef SYSTEMES_ET_RESEAUX_PROJET_BOT_H
#define SYSTEMES_ET_RESEAUX_PROJET_BOT_H

#include<stdlib.h>
#include<stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include<errno.h>
#include <string.h>


int PORT;
char *hostname = "127.0.0.1";

long long int bot_type = 0xFFFF;
long long int carte_type = 0xFFFE;
long long int ligne_type = 0xFFFD;



#endif //SYSTEMES_ET_RESEAUX_PROJET_BOT_H
