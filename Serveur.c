#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "Serveur.h"


int main(int argc, char **argv)
{
    if(argc ==2){
        PORT = atoi(argv[1]);
    }

    int serveur_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(serveur_socket == -1){
        perror("erreur création socket");
        exit(errno);
    }
    printf("création socket\n");

    struct sockaddr_in serveur_addr = {0};

    serveur_addr.sin_family = AF_INET;
    serveur_addr.sin_addr.s_addr = INADDR_ANY;
    serveur_addr.sin_port = htons(PORT);

    if (bind(serveur_socket, (struct sockaddr *)&serveur_addr,sizeof(struct sockaddr)) == -1){
        perror("erreur socket liaison");
        exit(errno);
    }

    printf("socket lié avec succès au port %i\n",PORT);

    listen(serveur_socket,5);

    while (1){
        printf("Attente client\n");

        struct sockaddr_in client_addr = {0};

        int taille = (int) sizeof(serveur_addr);

        int client = accept(serveur_socket, (struct sockaddr *)& serveur_addr, (socklen_t *)&taille);

        printf("connection réaliser avec un client\n");
        for (int i = 0; i < 50; ++i)
        {
            char  *mes = "ici\n";
            send(client, mes, strlen(mes),0);
        }
    }

    return EXIT_SUCCESS;
}