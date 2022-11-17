#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#include "Serveur.h"

joueur joueurs[8];
int compteur_joueur = 0;

int serveur_socket;

struct sockaddr_in serveur_addr = {0};

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        PORT = atoi(argv[1]);
    }

    serveur_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (serveur_socket == -1)
    {
        perror("erreur création socket");
        exit(errno);
    }
    printf("création socket\n");


    serveur_addr.sin_family = AF_INET;
    serveur_addr.sin_addr.s_addr = INADDR_ANY;
    serveur_addr.sin_port = htons(PORT);

    if (bind(serveur_socket, (struct sockaddr *) &serveur_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("erreur socket liaison");
        exit(errno);
    }

    printf("socket lié avec succès au port %i\n", PORT);

    listen(serveur_socket, 5);

    pthread_t thread;

    // lancement du thread qui gere les connections des joueurs.
    pthread_create(&thread, NULL, &listen_joueurs, NULL);


    while (1)
    {

    }

    return EXIT_SUCCESS;
}


void *joueur_pret(void *argv)
{
    joueur  *j = (joueur *) argv;
    char *buffer = (char *) calloc(128, sizeof(char *));
    char *message = (char *) calloc(128, sizeof(char *));

    while (1)
    {
        recv(j->socket, buffer, sizeof(buffer) - 1, 0);
        printf("%s\n", buffer);
        if (strcmp(buffer, "pret") ==0)
        {
            j->pret = 1;
            strcpy(message, "Le joueur ");
            strcat(message, j->pseudo);
            strcat(message, " à mis pret");
            send_all_joueurs(joueurs, compteur_joueur, message);
            break;
        }
    }
    free(buffer);
    free(message);

    return 0;
}

void *listen_joueurs()
{
    while (1)
    {
        printf("Attente client\n");

        if (compteur_joueur == MAX_JOUEURS)
        {
            send_all_joueurs(joueurs, compteur_joueur, "nombre max de joueur atteint la partie va commencé.");
            break;
        }

        struct sockaddr_in client_addr = {0};

        int taille = (int) sizeof(serveur_addr);

        int client = accept(serveur_socket, (struct sockaddr *) &serveur_addr, (socklen_t * ) & taille);

        joueur j = init_joueur();
        j.socket = client;

        char *buffer = (char *) calloc(1024, sizeof(char *));
        char *message = (char *) calloc(1024, sizeof(char *));

        if (recv(client, buffer, sizeof(buffer) - 1, 0) < 0)
        {
            perror("bug reception");
            exit(-1);
        }
        strcpy(j.pseudo, buffer);

        printf("Connection réaliser avec le joueur %s\n", j.pseudo);
        strcpy(message,"Vous avez rejoint le serveur, vous etes le joueur n°");
        char nb[8];
        sprintf(nb, "%i", compteur_joueur + 1);
        strcat(message, nb);
        send(client, message, strlen(message), 0);

        printf("%s\n",message);

        strcpy(message, "Le joueur ");
        strcat(message, j.pseudo);
        strcat(message, " vient de se connecter");

        send_all_joueurs(joueurs, compteur_joueur, message);

        joueurs[compteur_joueur] = j;

        compteur_joueur++;

        free(buffer);
        free(message);

        pthread_t pthread;

        pthread_create(&pthread,NULL,&joueur_pret,(void *)&j);
    }

    while (1)
    {
        struct sockaddr_in client_addr = {0};

        int taille = (int) sizeof(serveur_addr);

        int client = accept(serveur_socket, (struct sockaddr *) &serveur_addr, (socklen_t * ) & taille);

        char *mes = "Le nombre de joueurs maximum a été atteint";

        send(client, mes, strlen(mes), 0);

        close(client);
    }
}

void send_all_joueurs(joueur *joueurs, int nb_joueur, char *message)
{
    for (int i = 0; i < nb_joueur; ++i)
        send(joueurs[i].socket, message, strlen(message), 0);
}

joueur init_joueur()
{
    joueur j;
    j.pseudo = (char *) calloc(128, sizeof(char *));
    j.pret = 0;

    return j;
}