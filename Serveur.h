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


int PORT = 65534;
#define MAX_JOUEURS 8

#define BUF_SIZE 1024

typedef struct
{
    char *pseudo;
    int numero_joueur;
    int socket;
    int pret ;

} joueur;

joueur init_joueur();

void *listen_joueurs();

void send_all_joueurs(joueur *joueurs , int nb_joueur, char *message);

void *joueur_pret(void *);


#endif //SYSTEMES_ET_RESEAUX_PROJET_SERVEUR_H
