//
// Created by rahman on 16/11/22.
//

#include "Client.h"
#include <pthread.h>

int sock;


/**
 * @brief Main.
 * @param argc
 * @param argv
 * @return int
 */
int main(int argc, char **argv)
{

    if(argc == 3){
        PORT = atoi(argv[1]);
        strcpy(argv[2],hostname);
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("Impossible de créer le socket");
        exit(errno);
    }

    struct sockaddr_in sin = {0}; /* initialise la structure avec des 0 */

    sin.sin_port = htons(PORT); /* on utilise htons pour le port */
    sin.sin_family = AF_INET;

    if (inet_pton(AF_INET, hostname, &sin.sin_addr) <= 0)
    {
        perror("\naddresse invalide \n");
        exit(errno);
    }

    if (connect(sock, (struct  sockaddr* ) & sin, sizeof(struct  sockaddr)) == -1)
    {
        perror("connection impossible");
        exit(errno);
    }

    printf("Donner votre pseudo : ");

    char pseudo[1024];
    scanf( "%s", pseudo );
    fflush(stdin);
    fflush(stdout);
    send(sock,pseudo,strlen(pseudo),0);

    pthread_t pthread;

    pthread_create(&pthread,NULL,&listen_all_time,NULL);


    // boucle pour envoyer des messages au serveur
    while (1)
    {
        char *message = (char *) malloc(1024);

        scanf("%s",message);
        send(sock,message, strlen(message),0);
        fflush(stdin);
        fflush(stdout);

        free(message);
    }


    return 0;
}


/**
 * @brief Fonction qui écoute à l'infini sur le socket du serveur.
 * @param argv
 * @return
 */
void *listen_all_time(void *argv){
    while (1)
    {
        char buffer[1024];
        int n = 0;

        if ((n = recv(sock, buffer, sizeof buffer - 1, 0)) ==0)
        {
            perror("recv()");
            exit(errno);
        }
        buffer[n] = '\0';

        printf("%s\n", buffer);
        fflush(stdout);
    }
}
