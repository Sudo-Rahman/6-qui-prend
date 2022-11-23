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

#include "Jeu.h"


int PORT = 65534;

#define BUF_SIZE 1024



typedef struct
{
    char *pseudo;
    int numero_joueur;
    int socket;
    int pret ;
    Joueur *joueur;
    int bot_or_not;

} client;

client *init_joueur();

void *listen_joueurs();

void send_all_joueurs(client **clients , int nb_clients, char *message);

void *joueur_pret(void *);

int all_joueur_pret();

void *listen_choix_carte_joueur(void *);

void *listen_client_quit(void *);

void listen_clients_quit();

void close_all_clients();



#endif //SYSTEMES_ET_RESEAUX_PROJET_SERVEUR_H
