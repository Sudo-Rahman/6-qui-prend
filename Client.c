//**********//
// Client.c //
//**********//

#include "Color.h"
#include "Client.h"
#include "Jeu.h"
#include <pthread.h>

int sock;

//TODO Bug quand le client doit entrer une valeur genre 'pret' mais qu'il rentre autre chose


// ******** //
//   MAIN   //
// ******** //
int main(int argc, char **argv) {

    if (argc == 3) {
        PORT = atoi(argv[1]);
        strcpy(argv[2], hostname);
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror(BOLD_RED"Impossible de créer le socket"RESET);
        exit(errno);
    }

    struct sockaddr_in sin = {0}; /* initialise la structure avec des 0 */

    sin.sin_port = htons(PORT); /* on utilise htons pour le port */
    sin.sin_family = AF_INET;

    if (inet_pton(AF_INET, hostname, &sin.sin_addr) <= 0) {
        perror(BOLD_RED"\nAdresse invalide \n"RESET);
        exit(errno);
    }

    if (connect(sock, (struct sockaddr *) &sin, sizeof(struct sockaddr)) == -1) {
        perror(BOLD_RED"Connection impossible"RESET);
        exit(errno);
    }

    //GESTION DES SIGNAUX pour fermer le programme correctement
    signal(SIGINT, GestionSignauxClient);
    signal(SIGTERM, GestionSignauxClient);

    printf("Choisissez votre pseudo : ");

    char pseudo[64];
    scanf("%s", pseudo);
    fflush(stdin);
    fflush(stdout);
    send(sock, pseudo, strlen(pseudo), 0);

    pthread_t pthread;

    pthread_create(&pthread, NULL, &listen_all_time, NULL);


    // Boucle pour envoyer des messages au serveur
    while (1) {
        char *message = (char *) malloc(2048);
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


void *listen_all_time(void *argv) {
    while (1) {
        char buffer[1024];
        int n = 0;

        if ((n = recv(sock, buffer, sizeof buffer - 1, 0)) == 0) {
            perror(BOLD_RED"Erreur dans fonction listen_all_time()"RESET);
            exit(errno);
        }
        buffer[n] = '\0';
        printf("%s\n", buffer);
        fflush(stdout);
    }
}
