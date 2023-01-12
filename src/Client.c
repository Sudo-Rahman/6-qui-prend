//**********//
// Client.c //
//**********//

#include "Color.h"
#include "Client.h"
#include "Jeu.h"
#include <pthread.h>

int sock;
char pseudo[64];


// ******** //
//   MAIN   //
// ******** //
int main(int argc, char **argv)
{

    if (argc == 3)
    {
        PORT = atoi(argv[1]);
        strcpy(argv[2], hostname);
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror(BOLD_RED"Impossible de créer le socket"RESET);
        exit(errno);
    }

    struct sockaddr_in sin = {0}; /* initialise la structure avec des 0 */

    sin.sin_port = htons(PORT); /* on utilise htons pour le port */
    sin.sin_family = AF_INET;

    if (inet_pton(AF_INET, hostname, &sin.sin_addr) <= 0)
    {
        perror(BOLD_RED"\nAdresse invalide \n"RESET);
        exit(errno);
    }

    if (connect(sock, (struct sockaddr *) &sin, sizeof(struct sockaddr)) == -1)
    {
        perror(BOLD_RED"Connexion impossible"RESET);
        exit(errno);
    }

    //GESTION DES SIGNAUX pour fermer le programme correctement
    signal(SIGINT, gestion_signaux_client);
    signal(SIGTERM, gestion_signaux_client);


    printf(BOLD_HIGH_WHITE"BIENVENUE SUR LE JEU "RESET);
    printf(BOLD_GREEN"6"RESET);
    printf(BOLD_CYAN" QUI"RESET);
    printf(BOLD_MAGENTA" PREND!\n\n"RESET);

    printf("Choisissez votre pseudo : ");

    scanf("%s", pseudo);
    fflush(stdin);
    fflush(stdout);
    send(sock, pseudo, strlen(pseudo), 0);

    pthread_t pthread;
    pthread_create(&pthread, NULL, &listen_all_time, NULL);

    // Boucle pour envoyer des messages au serveur
    while (1)
    {
        char *message = (char *) malloc(512);
        scanf("%s", message);
        send(sock, message, strlen(message), 0);
        fflush(stdin);
        fflush(stdout);
        free(message);
    }

    return EXIT_SUCCESS;
}



// ********* //
// FONCTIONS //
// ********* //


void *listen_all_time(void *argv)
{
    while (1)
    {
        char buffer[2048];
        int n = 0;

        if ((n = recv(sock, buffer, sizeof buffer - 1, 0)) == 0)
        {
            perror(BOLD_RED"Erreur dans fonction listen_all_time()"RESET);
            exit(errno);
        }
        buffer[n] = '\0';
        printf("%s\n", buffer);
        fflush(stdout);
    }
}

void gestion_signaux_client(int signal_recu)
{

    switch (signal_recu)
    {

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
