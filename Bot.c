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
        printf("ici\n");
        fflush(stdout);

        char *type =(char *) calloc(20, sizeof (char ));
        int n = 0;

        if ((n = recv(sock, type, sizeof type - 1, 0)) == 0)
        {
            perror(BOLD_RED"Erreur dans fonction listen_all_time()"RESET);
            exit(errno);
        }
        type[n] = '\n';


        const long long int nb = atoll(type);

        printf("%llu", nb);
        fflush(stdout);


        //todo probleme rand ne marche pas renvoie toujours la meme suite de nombre.
        if (nb == carte_type)
        {
            char cartes[10];
            if ((n = recv(sock, cartes, sizeof cartes - 1, 0)) == 0)
            {
                perror(BOLD_RED"Erreur dans fonction listen_all_time()"RESET);
                exit(errno);
            }
            cartes[n] = '\n';
            printf("%s\n", cartes);
            fflush(stdout);
            printf("Nombre entrée par le bot --> [%c] SIZE > %lu\n",cartes[rand()%strlen(cartes)],strlen(cartes));
            send(sock, &cartes[rand()%strlen(cartes)], 1, 0);
            continue;
        }
        if (nb == ligne_type)
        {
            char lignes[5];
            if ((n = recv(sock, cartes, sizeof cartes - 1, 0)) == 0)
            {
                perror(BOLD_RED"Erreur dans fonction listen_all_time()"RESET);
                exit(errno);
            }
            lign*[n] = '\n';
            printf("%s\n", lignes);
            send(sock, &"1234"[rand()%4], 1, 0);
            continue;
        }

        free(type);
    }

    exit(EXIT_SUCCESS);
}