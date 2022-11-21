#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#include "Serveur.h"


joueur *joueurs[8];


int serveur_socket;

struct sockaddr_in serveur_addr = {0};


/**
 * @brief Main.
 * @param argc
 * @param argv
 * @return int
 */
int main(int argc, char **argv)
{
    if (argc == 2)
    {
        PORT = atoi(argv[1]);
    }

    serveur_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (serveur_socket == -1)
    {
        perror("erreur création socket");
        exit(errno);
    }
    printf("création socket\n");


    serveur_addr.sin_family = AF_INET;
    serveur_addr.sin_addr.s_addr = INADDR_ANY;
    serveur_addr.sin_port = htons(PORT);

    if (bind(serveur_socket, (struct sockaddr *) &serveur_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("erreur socket liaison");
        exit(errno);
    }

    printf("socket lié avec succès au port %i\n", PORT);

    listen(serveur_socket, 5);

    pthread_t thread;

    // lancement du thread qui gere les connections des joueurs.
    pthread_create(&thread, NULL, &listen_joueurs, NULL);


    while (all_joueur_pret() == 0)
    {}

    send_all_joueurs(joueurs, nb_Joueur, "Tous les joueurs sont pret la partie va commencer.");
    printf("La partie commence.\n");

    printf("ici\n");

    Jeu jeu;
    printf("ici\n");

    initJeu(jeu);
    printf("ici\n");


    send_all_joueurs(joueurs, nb_Joueur,affichePlateau(jeu));
    printf("ici\n");


    while (1)
    {

    }

    return EXIT_SUCCESS;
}


/**
 * @brief Fonction qui ecoute en continu le socket du joueur en parametre, jusqu’à qu’il mette pret.
 * @param argv
 * @return void
 */
void *joueur_pret(void *argv)
{
    joueur *j = (joueur *) argv;
    char *buffer = (char *) calloc(128, sizeof(char *));
    char message[1024];

    strcpy(message, "Envoyer 'pret' pour vous mettre pret.");

    send(j->socket, message, strlen(message), 0);

    while (1)
    {
        recv(j->socket, buffer, sizeof(buffer) - 1, 0);
        if (strcmp(buffer, "pret") == 0)
        {
            j->pret = 1;
            strcpy(message, "Le joueur ");
            strcat(message, j->pseudo);
            strcat(message, " à mis pret");
            send_all_joueurs(joueurs, nb_Joueur, message);
            break;
        }
    }
    free(buffer);

    return 0;
}


/**
 * @brief Fonction qui accepte ou non les clients qui se connecte au serveur.
 * @return void
 */
void *listen_joueurs()
{
    while (1)
    {
        printf("Attente client\n");

        struct sockaddr_in client_addr = {0};

        int taille = (int) sizeof(serveur_addr);

        int client = accept(serveur_socket, (struct sockaddr *) &serveur_addr, (socklen_t * ) & taille);

        if(all_joueur_pret()){
            char mes[1024];

            if(nb_Joueur == MAX_JOUEURS)
                strcpy(mes,"Le nombre de joueurs maximum a été atteint.");
            else
                strcpy(mes,"La partie à deja commencé.");

            send(client, mes, strlen(mes), 0);


            close(client);
            continue;
        }

        joueur *j = init_joueur();
        j->socket = client;

        char *buffer = (char *) calloc(1024, sizeof(char *));
        char *message = (char *) calloc(1024, sizeof(char *));

        if (recv(client, buffer, sizeof(buffer) - 1, 0) < 0)
        {
            perror("bug reception");
            exit(-1);
        }
        strcpy(j->pseudo, buffer);

        printf("Connection réaliser avec le joueur %s\n", j->pseudo);
        strcpy(message, "Vous avez rejoint le serveur, vous etes le joueur n°");
        char nb[8];
        sprintf(nb, "%i", nb_Joueur + 1);
        strcat(message, nb);
        strcat(message, ".");
        send(client, message, strlen(message), 0);

        strcpy(message, "Le joueur ");
        strcat(message, j->pseudo);
        strcat(message, " vient de se connecter");

        send_all_joueurs(joueurs, nb_Joueur, message);

        joueurs[nb_Joueur] = j;


        nb_Joueur++;

        if (nb_Joueur == MAX_JOUEURS)
        {
            send_all_joueurs(joueurs, nb_Joueur, "nombre max de joueur atteint la partie va commencer.");
            break;
        }


        free(buffer);
        free(message);

        pthread_t pthread;

        pthread_create(&pthread, NULL, &joueur_pret, (void *) j);
    }

}

/**
 * @brief Fonction qui envoie le message en parametre a tous les joueurs.
 * @param joueurs
 * @param nb_joueur
 * @param message
 */
void send_all_joueurs(joueur **joueurs, int nb_joueur, char *message)
{
    for (int i = 0; i < nb_joueur; ++i)
        send(joueurs[i]->socket, message, strlen(message), 0);
}

/**
 * @brief Fonction qui creé un nouveau pointeur de joueur et le renvoie.
 * @return joueur
 */
joueur *init_joueur()
{
    joueur *j = (joueur *) malloc(1);
    j->pseudo = (char *) calloc(128, sizeof(char *));
    j->pret = 0;

    return j;
}

/**
 * @brief Fonction qui retourne 1 si tous les joueurs sont pret et 0 sinon.
 * @return 1 ou 0
 */
int all_joueur_pret()
{
    if (nb_Joueur == 0)
        return 0;
    int compteur = 0;
    for (int i = 0; i < nb_Joueur; i++)
    {
        if (joueurs[i]->pret == 1)
            compteur++;
    }
    if (compteur == nb_Joueur)
        return 1;
    return 0;
}
