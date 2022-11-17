//
// Created by rahman on 16/11/22.
//

#include "Client.h"

int main(int argc, char **argv)
{


    int sock = socket(AF_INET, SOCK_STREAM, 0);
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


    while (1)
    {
        char buffer[1024];
        int n = 0;

        if ((n = recv(sock, buffer, sizeof buffer - 1, 0)) < 0)
        {
            perror("recv()");
            exit(errno);
        }

        buffer[n] = '\0';
        printf("%s", buffer);
    }


    return 0;
}
