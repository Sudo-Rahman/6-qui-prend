//
// Created by rahman on 16/11/22.
//

#include "Client.h"

int main(int argc , char **argv){


    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == INVALID_SOCKET)
    {
        perror("Impossible de créer le socket");
        exit(errno);
    }

    struct hostent *hostinfo = NULL;
    SOCKADDR_IN sin = { 0 }; /* initialise la structure avec des 0 */
    const char *hostname = "127.0.0.1";

    hostinfo = gethostbyname(hostname); /* on récupère les informations de l'hôte auquel on veut se connecter */
    if (hostinfo == NULL) /* l'hôte n'existe pas */
    {
        fprintf (stderr, "Unknown host %s.\n", hostname);
        exit(EXIT_FAILURE);
    }

    sin.sin_addr = *(IN_ADDR *) hostinfo->h_addr; /* l'adresse se trouve dans le champ h_addr de la structure hostinfo */
    sin.sin_port = htons(65535); /* on utilise htons pour le port */
    sin.sin_family = AF_INET;

    if(connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
    {
        perror("connect()");
        exit(errno);
    }

    printf("%ici\n",sock);

    while (1){
        char buffer[1024];
        int n = 0;

        if((n = recv(sock, buffer, sizeof buffer - 1, 0)) < 0)
        {
            perror("recv()");
            exit(errno);
        }

        buffer[n] = '\0';
        printf("%s",buffer);
    }


    return 0;
}
