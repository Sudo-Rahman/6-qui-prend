//
// Created by Rahman  Yilmaz on 01/12/2022.
//

#include "Bot.h"

#include <string.h>
#include <time.h>

int sock;


int main(int argc, char *argv[])
{
    srand(time(NULL));

    if (argc == 3)
    {
        PORT = atoi(argv[1]);
        strcpy(argv[2], hostname);
    }


    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("ereur bot");
        exit(errno);
    }

    struct sockaddr_in sin = {0}; /* initialise la structure avec des 0 */

    sin.sin_port = htons(PORT); /* on utilise htons pour le port */
    sin.sin_family = AF_INET;

    if (inet_pton(AF_INET, hostname, &sin.sin_addr) <= 0)
    {
        perror("\nAdresse invalide \n");
        exit(errno);
    }

    if (connect(sock, (struct sockaddr *) &sin, sizeof(struct sockaddr)) == -1)
    {
        perror("Connection impossible");
        exit(errno);
    }

    char nom[1024];

    snprintf(nom, 1024, "%llu", bot_type);

    send(sock, nom, strlen(nom), 0);


    while (1)
    {
        char buffer[2048];
        char message[2048];
        int n = 0;

        if ((n = recv(sock, buffer, sizeof buffer - 1, 0)) == 0)
        {
            exit(errno);
        }
        buffer[n] = '\0';

        const long long int nb = atoll(buffer);

        memset(buffer, 0, sizeof(char) * strlen(buffer));


        if ((n = recv(sock, buffer, sizeof buffer - 1, 0)) == 0)
        {
            exit(errno);
        }
        buffer[n] = '\0';




        //todo probleme rand ne marche pas renvoie toujours la meme suite de nombre.
        if (nb == carte_type)
        {
            char *randd = &buffer[(rand() % strlen(buffer))];
            printf("%s\n", randd);
            send(sock, randd, strlen(randd), 0);
            continue;
        }
        if (nb == ligne_type)
        {
            int randd = (rand() % 4) + 1;
            snprintf(message, 1024, "%d", randd);
            send(sock, message, strlen(message), 0);
            continue;
        }
    }

    exit(EXIT_SUCCESS);
}