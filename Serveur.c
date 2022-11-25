#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <time.h>


#include "Serveur.h"


client *clients[MAX_JOUEURS];

int nb_client = 0;


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
    srand(time(NULL));

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


    while (all_joueur_pret() == 0) {}

    send_all_joueurs(clients, nb_client, "Tous les joueurs sont pret la partie va commencer.");
    printf("La partie commence.\n");


    Jeu jeu;
    for (int i = 0; i < nb_client; i++)
    {
        jeu.joueur[i] = clients[i]->joueur;
    }
    initJeu(&jeu);


    send_all_joueurs(clients, nb_client, RecapRegle(jeu));
    send_all_joueurs(clients, nb_client, affiche_plateau(&jeu));


    jeu_play(&jeu);


    return EXIT_SUCCESS;
}


/**
 * @brief Fonction qui ecoute en continu le socket du joueur en parametre, jusqu’à qu’il mette pret.
 * @param argv
 * @return void
 */
void *joueur_pret(void *argv)
{
    client *c = (client *) argv;
    char *message = (char *) malloc(1024 * sizeof(char));

    strcpy(message, "Envoyer 'pret' pour vous mettre pret.");

    send(c->socket, message, strlen(message), 0);

    while (1)
    {
        char *buffer = recv_client_data(c);
        if (strcmp(buffer, "pret") == 0)
        {
            c->pret = 1;
            sprintf(message, "Le joueur : %s à mis pret.", c->pseudo);
            send_all_joueurs(clients, nb_client, message);
            break;
        }
        free(buffer);
    }
    free(message);

    return 0;
}

void jeu_play(Jeu *jeu)
{
    while (1)
    {
        pthread_t threads[nb_client];

        for (int i = 0; i < nb_client; ++i)
            pthread_create(&threads[i], NULL, listen_choix_carte_joueur, (void *) clients[i]);
        for (int i = 0; i < nb_client; i++)
            pthread_join(threads[i], NULL);

        Joueur **joueurs = get_ordre_joueur_tour(jeu);
        for (int i = 0; i < nb_client; ++i)
        {
            int retour = ajoute_carte_au_plateau(jeu, joueurs[i]->carte_choisie);
            if (retour == 0 || retour == -1)
            {
                client *c;
                for (int j = 0; j < nb_client; ++j)
                {
                    if (clients[j]->joueur == joueurs[i])
                        c = clients[j];

                }
                int ligne = get_pos_carte_mini(jeu,joueurs[i]->carte_choisie->Numero)/6;
                if (retour == -1)
                    ligne = carte_trop_petite(c);
                place_carte_si_trop_petite_ou_derniere_ligne(jeu, ligne, c->joueur);
                printf("joueur : %s tete : %i", clients[i]->pseudo, c->joueur->nb_penalite);
                fflush(stdout);
            }
        }
        free(joueurs);
        send_all_joueurs(clients, nb_client, affiche_plateau(jeu));
    }
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

        int client_socket = accept(serveur_socket, (struct sockaddr *) &serveur_addr, (socklen_t * ) & taille);

        if (all_joueur_pret())
        {
            char mes[1024];

            if (nb_client == MAX_JOUEURS)
                strcpy(mes, "Le nombre de joueurs maximum a été atteint.");
            else
                strcpy(mes, "La partie à deja commencé.");

            send(client_socket, mes, strlen(mes), 0);


            close(client_socket);
            continue;
        }

        client *c = init_joueur();
        c->socket = client_socket;

        char *message = (char *) calloc(1024, sizeof(char));
        char *buffer = recv_client_data(c);

        strcpy(c->pseudo, buffer);

        printf("Connection réaliser avec le joueur %s\n", c->pseudo);
        sprintf(message, "Vous avez rejoint le serveur, vous etes le joueur n°%u.", nb_client + 1);
        send(client_socket, message, strlen(message), 0);

        sprintf(message, "Le joueur : %s vient de se connecter.", c->pseudo);
        send_all_joueurs(clients, nb_client, message);

        clients[nb_client] = c;


        nb_client++;
        nb_Joueur++;

        if (nb_client == MAX_JOUEURS)
        {
            send_all_joueurs(clients, nb_client, "nombre max de joueur atteint la partie va commencer.");
            break;
        }


        free(buffer);
        free(message);

        pthread_t pthread;

        pthread_create(&pthread, NULL, &joueur_pret, (void *) c);
    }

}

/**
 * @brief Fonction qui envoie le message en parametre a tous les joueurs.
 * @param joueurs
 * @param nb_joueur
 * @param message
 */
void send_all_joueurs(client **clients, int nb_client, char *message)
{
    for (int i = 0; i < nb_client; ++i)
        send(clients[i]->socket, message, strlen(message), 0);
}

/**
 * @brief Fonction qui creé un nouveau pointeur de client et le renvoie.
 * @return client
 */
client *init_joueur()
{
    client *c = (client *) malloc(sizeof(client));
    c->pseudo = (char *) calloc(128, sizeof(char));
    c->pret = 0;
    c->bot_or_not = 0;
    c->joueur = (Joueur *) malloc(sizeof(Joueur));
    return c;
}

/**
 * @brief Fonction qui retourne 1 si tous les joueurs sont pret et 0 sinon.
 * @return 1 ou 0
 */
int all_joueur_pret()
{
    if (nb_client == 0)
        return 0;
    int compteur = 0;
    for (int i = 0; i < nb_client; i++)
    {
        if (clients[i]->pret == 1)
            compteur++;
    }
    if (compteur == nb_client)
    {
        return 1;
        if (compteur >= MIN_JOUEURS)
            return 1;
    }
    return 0;
}


/**
 * @details Ecoute le joueur, pour récuperer la carte choisie par le joueur.
 * @param argv
 * @return void
 */
void *listen_choix_carte_joueur(void *argv)
{
    client *c = (client *) argv;

    char *message = (char *) malloc(1024 * sizeof(char));

    strcpy(message, "Choisissez une carte parmi celles que vous avez.\n");

    send(c->socket, message, strlen(message), 0);

    strcpy(message, affiche_cartes_joueur(c->joueur));
    send(c->socket, message, strlen(message), 0);

    while (1)
    {
        char *buffer = recv_client_data(c);

        int nb = atoi(buffer);
        if (nb == 0 || (nb > 10 || nb < 1))
        {
            strcpy(message, "Erreur la carte indiquée n’existe pas.");
            send(c->socket, message, strlen(message), 0);
            continue;
        } else
        {
            if (c->joueur->carte[nb - 1]->isUsed == 1)
            {
                strcpy(message, "Cette carte a deja été utilisé choisissez en une autre.");
                send(c->socket, message, strlen(message), 0);
            } else
            {
                c->joueur->carte[nb - 1]->isUsed = 1;
                c->joueur->carte_choisie = c->joueur->carte[nb - 1];
                break;
            }
        }
        free(buffer);

    }
    free(message);

}


/**
 * @details Ecoute du client mis en parametre si celui qui le serveur la partie s'arrete et tous les clients sont déconnecter du serveur.
 * @param argv
 * @return void
 */
void client_quit(client *c)
{
    char *mess = (char *) malloc(1024 * sizeof(char));

    sprintf(mess, "Le client : %s à quitter la partie, donc la partie s'arrete.", c->pseudo);
    printf("%s\n", mess);
    send_all_joueurs(clients, nb_client, mess);
    close_all_clients();
    free(mess);
    exit(-1);

}

/**
 * @details Ferme le socket de tous les clients et du serveur.
 */
void close_all_clients()
{
    for (int i = 0; i < nb_client; ++i)
    {
        close(clients[i]->socket);
    }
    close(serveur_socket);
}


/**
 * @details Fonction qui écoute le client en parametre et retourne les données envoyer par le client.
 * @param c
 * @return chaine de caractere.
 */
char *recv_client_data(client *c)
{
    char *buffer = (char *) calloc(1024, sizeof(char *));

    if (recv(c->socket, buffer, sizeof(buffer) - 1, 0) == 0)
    {
        client_quit(c);
    }
    return buffer;
}

/**
 * @details Quand un joueur joue une carte qui est trop petite.
 * @param c
 * @return Ligne choisie par le client.
 */
int carte_trop_petite(client *c)
{

    char *message = "Votre carte est trop petite, vous devez choisir la ligne"
                    " ou la mettre\nsuite a cela vous aurez la somme des tetes des cartes de la ligne en pénalité.";

    send(c->socket, message, strlen(message), 0);
    int nb = 10;
    while (nb < 1 || nb > 4)
    {
        char *buff = recv_client_data(c);
        nb = atoi(buff);

        if (nb < 1 || nb > 4)
        {
            strcpy(message, "Ligne incorrecte, refaite la saisie.");
            send(c->socket, message, strlen(message), 0);
        }
        free(buff);
    }
    return nb - 1;
}