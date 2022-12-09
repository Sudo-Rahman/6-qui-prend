//
// Created by Rahman  Yilmaz on 01/12/2022.
//

#include "Bot.h"
#include "Color.h"
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
    printf("%d - %s\n", PORT, hostname);
    fflush(stdout);


    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror(BOLD_RED"erreur bot"RESET);
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
        perror(BOLD_RED"Connection impossible"RESET);
        exit(errno);
    }

    char nom[1024];

    snprintf(nom, 1024, "%llu", bot_type);

    send(sock, nom, strlen(nom), 0);


    while (1)
    {
        fflush(stdout);

        char buffer[2048];
        int n = 0;


        if ((n = recv(sock, buffer, sizeof buffer -1, 0)) == 0)
        {
            perror(BOLD_RED"Erreur reception données"RESET);
            exit(errno);
        }
        buffer[n] = '\0';
//        printf("%s\n", buffer);
        char chiffre;
        do
        {
            chiffre = buffer[rand()%n];
//            printf("%c\n", chiffre);
        } while (strcmp(&chiffre, "") ==0);

        send(sock, &chiffre, 1, 0);

    }

    exit(EXIT_SUCCESS);
}