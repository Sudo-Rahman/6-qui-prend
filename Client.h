//**********//
// Client.h //
//**********//


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
#include <string.h>
#include "Color.h"

char *hostname = "127.0.0.1";

int PORT = 65534;

/**
 * @brief Fonction qui écoute à l'infini sur le socket du serveur.
 * @param argv
 * @return
 */
void *listen_all_time(void *);

/**
 * @details Fonction de gestion des signaux du programme
 * @param signal_recu
 */
void gestion_signaux_client(int signal_recu);

#endif //SYSTEMES_ET_RESEAUX_PROJET_CLIENT_H
