#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pcap/socket.h>

#include "Serveur.h"



int main(int argc, char **argv)
{
    SOCKET serveur_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(serveur_socket == INVALID_SOCKET){
        printf("erreur création socket");
        exit(errno);
    }
    printf("création socket\n");

    SOCKADDR_IN serveur_addr;

    serveur_addr.sin_family = AF_INET;
    serveur_addr.sin_addr.s_addr = INADDR_ANY;
    serveur_addr.sin_port = htons(65535);

    if (bind(serveur_socket, (SOCKADDR *)&serveur_addr,sizeof(serveur_addr)) == -1){
        printf("erreur socket liaison");
        exit(errno);
    }

    printf("socket lié avec succès\n");

    listen(serveur_socket,5);

    while (1){
        printf("Attente client\n");

        SOCKADDR_IN client_addr;

        int taille = (int) sizeof(serveur_addr);

        SOCKET client = accept(serveur_socket, (SOCKADDR *)& serveur_addr, (socklen_t *)&taille);

        printf("connection réaliser avec un client\n");
        for (int i = 0; i < 50; ++i)
        {
            char  *mes = "ici";
            send(client, mes, strlen(mes),0);
        }
    }

    return EXIT_SUCCESS;
}