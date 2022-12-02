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
void GestionSignauxClient(int signal_recu) {

    switch (signal_recu) {

        //SIGNAL CTRL + C
        case SIGINT:
            printf(BOLD_MAGENTA"\nVous avez quitté la partie\nAu revoir\n"RESET);
            exit(0);

            //SIGNAL POUR ARRETER PROGRAMME
        case SIGTERM:
            printf(BOLD_YELLOW"\nSIGNAL SIGTERM RECU\n"RESET);
            printf(BOLD_MAGENTA"\nVous avez quitté la partie\nAu revoir\n"RESET);
            exit(0);

        default:
            printf(BOLD_YELLOW"\nSIGNAL RECU > %d\n"RESET, signal_recu);
            break;
    }
}

#endif //SYSTEMES_ET_RESEAUX_PROJET_CLIENT_H
