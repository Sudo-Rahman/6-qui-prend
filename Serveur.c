//**********//
// Server.c //
//**********//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Serveur.h"

// ********* //
// VARIABLES //
// ********* //
client *clients[MAX_JOUEURS];

int nb_client = 0;
int serveur_socket;

struct sockaddr_in serveur_addr = {0};

FILE *fichier_log;
char date[40];
char pwd[256];
char nom_fichier[256];


// ******** //
//   MAIN   //
// ******** //
int main(int argc, char **argv) {
    srand(time(NULL));

    //Recuperation du dossier ou l'utilisateur travail pour vérification
    if (getcwd(pwd, sizeof(pwd)) == NULL){
        printf(BOLD_RED"IMPOSSIBLE DE TROUVER L'EMPLACEMENT DU DOSSIER ACTUEL\n"RESET);
        exit(EXIT_FAILURE);
    }

    //Verification si dossier des LOGS existe + création si existe pas
    if (access(strcat(pwd, "/LOG"), F_OK) != 0) {
        unsigned short check = mkdir("LOG", 0771);
        if (!check) printf("SUCCÈS CREATION DU DOSSIER LOG\n");
        else {
            printf(BOLD_RED"IMPOSSIBLE DE CREER LE DOSSIER POUR LES LOGS\n"RESET);
            exit(EXIT_FAILURE);
        }
    }

    //Creation du nom du fichier LOG avec la date du jour + heure pour savoir de quand date la partie
    time_t heure_local = time(NULL);
    struct tm *tm = localtime(&heure_local);
    strftime(date, 128, "%Y-%m-%d_%H:%M:%S_FICHIER_LOG.txt", tm);
    strcpy(nom_fichier, pwd);
    strcat(nom_fichier, "/");
    strcat(nom_fichier, date);


    //Creation du fichier dans le dossier LOG
    fichier_log = fopen(nom_fichier, "w");
    fprintf(fichier_log, "[%s] CREE\n\n", date);
    fprintf(fichier_log, "***DEBUT DU JEU***\n");

    //GESTION DES SIGNAUX pour fermer le programme correctement
    signal(SIGINT, GestionSignaux);
    signal(SIGTERM, GestionSignaux);

    if (argc == 2) {
        PORT = atoi(argv[1]);
    }

    serveur_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (serveur_socket == -1) {
        perror(BOLD_RED"Erreur création socket"RESET);
        exit(errno);
    }
    printf(BOLD_YELLOW"Création du socket...\n"RESET);


    serveur_addr.sin_family = AF_INET;
    serveur_addr.sin_addr.s_addr = INADDR_ANY;
    serveur_addr.sin_port = htons(PORT);

    if (bind(serveur_socket, (struct sockaddr *) &serveur_addr, sizeof(struct sockaddr)) == -1) {
        perror(BOLD_RED"Erreur de la liaison des sockets"RESET);
        exit(errno);
    }

    printf(BOLD_GREEN"Socket lié avec succès sur le port [%i]\n"RESET, PORT);
    fprintf(fichier_log, "Socket lié avec succès sur le port [%i]\n", PORT);

    listen(serveur_socket, 5);

    pthread_t thread;

    // lancement du thread qui gere les connections des joueurs.
    pthread_create(&thread, NULL, &listen_joueurs, NULL);


    while (all_joueur_pret() == 0) {}

    send_all_joueurs(clients, nb_client, BOLD_GREEN"\nTous les joueurs sont pret la partie va commencer."RESET);
    printf(BOLD_GREEN"La partie va commencer...\n"RESET);
    fprintf(fichier_log, "La partie commence.\n");


    Jeu jeu;
    for (int i = 0; i < nb_client; i++) jeu.joueur[i] = clients[i]->joueur;


    initJeu(&jeu);

    send_all_joueurs(clients, nb_client, RecapRegle(jeu));
    send_all_joueurs(clients, nb_client, affiche_plateau(&jeu));

    jeu_play(&jeu);

    return EXIT_SUCCESS;
}



// ********* //
// FONCTIONS //
// ********* //


void *joueur_pret(void *argv) {
    client *c = (client *) argv;
    char *message = (char *) malloc(1024 * sizeof(char));

    strcpy(message, "Envoyer 'pret' ou appuyer sur [y] pour vous mettre prêt.");

    send(c->socket, message, strlen(message), 0);

    while (1) {
        char *buffer = recv_client_data(c);

        if (strcmp(buffer, "pret") == 0 || strcmp(buffer, "y") == 0) {
            c->pret = 1;
            sprintf(message, BOLD_GREEN"Le joueur %s à mis prêt."RESET, c->pseudo);
            send_all_joueurs(clients, nb_client, message);
            break;
        }
        free(buffer);
    }
    free(message);

    return 0;
}

void jeu_play(Jeu *jeu) {
    while (1) {
        pthread_t threads[nb_client];

        for (int i = 0; i < nb_client; ++i)
            pthread_create(&threads[i], NULL, listen_choix_carte_joueur, (void *) clients[i]);
        for (int i = 0; i < nb_client; i++)
            pthread_join(threads[i], NULL);

        Joueur **joueurs = get_ordre_joueur_tour(jeu);
        for (int i = 0; i < nb_client; ++i) {
            int retour = ajoute_carte_au_plateau(jeu, joueurs[i]->carte_choisie);
            if (retour == 0) {
                printf("joueur : %s ", clients[i]->pseudo);
            } else {
                if (retour == -1) {
                    client *c;
                    for (int j = 0; j < nb_client; ++j) {
                        if (clients[j]->joueur == joueurs[i])
                            c = clients[j];

                    }
                    int ligne = carte_trop_petite(c);
                    place_carte_mini(jeu, ligne, c->joueur);
                }
            }
        }
        send_all_joueurs(clients, nb_client, "----------------------------------\n");
        fprintf(fichier_log,"---------------------------------------------------------------\n");
        send_all_joueurs(clients, nb_client, affiche_plateau(jeu));
        free(joueurs);
    }
}

void *listen_joueurs() {
    while (1) {
        printf("En attente de connexion de client...\n");

        struct sockaddr_in client_addr = {0};

        int taille = (int) sizeof(serveur_addr);

        int client_socket = accept(serveur_socket, (struct sockaddr *) &serveur_addr, (socklen_t *) &taille);

        if (all_joueur_pret()) {
            char mes[1024];
            if (nb_client == MAX_JOUEURS) strcpy(mes, BOLD_YELLOW"Le nombre de joueurs maximum a été atteint."RESET);
            else strcpy(mes, BOLD_YELLOW"La partie à deja commencé."RESET);
            send(client_socket, mes, strlen(mes), 0);
            close(client_socket);
            continue;
        }

        client *c = init_joueur();
        c->socket = client_socket;

        char *message = (char *) calloc(1024, sizeof(char));
        char *buffer = recv_client_data(c);

        strcpy(c->pseudo, buffer);

        printf("Connection réalisé avec le joueur %s\n", c->pseudo);
        sprintf(message, "Vous avez rejoint le serveur, vous etes le joueur n°%u.", nb_client + 1);
        send(client_socket, message, strlen(message), 0);

        sprintf(message, "Le joueur : %s vient de se connecter.", c->pseudo);
        fprintf(fichier_log, "Le joueur : %s vient de se connecter.\n", c->pseudo);

        send_all_joueurs(clients, nb_client, message);

        clients[nb_client] = c;

        nb_client++;
        nb_Joueur++;

        if (nb_client == MAX_JOUEURS) {
            send_all_joueurs(clients, nb_client,
                             BOLD_YELLOW"Nombre max de joueur atteint la partie va commencer."RESET);
            break;
        }


        free(buffer);
        free(message);

        pthread_t pthread;

        pthread_create(&pthread, NULL, &joueur_pret, (void *) c);
    }

}

void send_all_joueurs(client **clients, int nb_client, char *message) {
    for (int i = 0; i < nb_client; ++i)
        send(clients[i]->socket, message, strlen(message), 0);
}

client *init_joueur() {
    client *c = (client *) malloc(sizeof(client));
    c->pseudo = (char *) calloc(128, sizeof(char));
    c->pret = 0;
    c->bot_or_not = 0;
    c->joueur = (Joueur *) malloc(sizeof(Joueur));
    return c;
}

int all_joueur_pret() {
    if (nb_client == 0)
        return 0;
    int compteur = 0;
    for (int i = 0; i < nb_client; i++) {
        if (clients[i]->pret == 1)
            compteur++;
    }
    if (compteur == nb_client) {
        return 1;
        if (compteur >= MIN_JOUEURS)
            return 1;
    }
    return 0;
}

void *listen_choix_carte_joueur(void *argv) {
    client *c = (client *) argv;

    char *message = (char *) malloc(1024 * sizeof(char));


    sprintf(message + strlen(message), "\n%s choisissez une carte de votre jeu :\n", c->pseudo);
    sprintf(message + strlen(message), "Vous possédez %d têtes:\n", c->joueur[c->numero_joueur].nb_penalite);


    send(c->socket, message, strlen(message), 0);

    strcpy(message, affiche_cartes_joueur(c->joueur));
    send(c->socket, message, strlen(message), 0);

    while (1) {
        char *buffer = recv_client_data(c);

        int nb = atoi(buffer);
        if (nb == 0 || (nb > 10 || nb < 1)) {
            strcpy(message, BOLD_YELLOW"Erreur : la carte indiquée n’existe pas."RESET);
            send(c->socket, message, strlen(message), 0);
            continue;
        } else {
            if (c->joueur->carte[nb - 1]->isUsed == 1) {
                strcpy(message, BOLD_YELLOW"Cette carte a deja été utilisée, choisissez en une autre."RESET);
                send(c->socket, message, strlen(message), 0);
            } else {
                c->joueur->carte[nb - 1]->isUsed = 1;
                c->joueur->carte_choisie = c->joueur->carte[nb - 1];

                printf("Le joueur %s pose sa carte %d > [%d - %d]\n", c->pseudo, nb - 1,
                       c->joueur->carte[nb - 1]->Numero, c->joueur->carte[nb - 1]->Tete);
                fprintf(fichier_log, "Le joueur %s pose sa carte %d > [%d - %d] - Possède %d têtes\n", c->pseudo, nb - 1,
                        c->joueur->carte[nb - 1]->Numero, c->joueur->carte[nb - 1]->Tete, c->joueur[c->numero_joueur].nb_penalite);
                break;
            }
        }
        free(buffer);

    }
    free(message);

}

void client_quit(client *c) {
    char *mess = (char *) malloc(1024 * sizeof(char));
    sprintf(mess, BOLD_YELLOW"Le client %s a quitté la partie\nLA PARTIE A ÉTÉ INTERROMPU"RESET, c->pseudo);
    printf("%s\n", mess);
    fprintf(fichier_log, "Le client : %s a quitté la partie\nLA PARTIE A ÉTÉ INTERROMPU", c->pseudo);
    send_all_joueurs(clients, nb_client, mess);
    close_all_clients();
    fprintf(fichier_log, "\n***FIN***\n");
    fclose(fichier_log);
    free(mess);
    fflush(stdout);
    fflush(stdin);
    exit(EXIT_FAILURE);
}

void close_all_clients() {
    for (int i = 0; i < nb_client; ++i) {
        close(clients[i]->socket);
    }
    close(serveur_socket);
}

char *recv_client_data(client *c) {
    char *buffer = (char *) calloc(1024, sizeof(char *));

    if (recv(c->socket, buffer, sizeof(buffer) - 1, 0) == 0) {
        client_quit(c);
    }
    return buffer;
}

int carte_trop_petite(client *c) {

    char *message = BOLD_MAGENTA"Vous avez posé la carte la plus petite du plateau\n"
                    "Quelle rangée voulez vous prendre [1-4] ?\n"RESET;


    send(c->socket, message, strlen(message), 0);
    int nb = 10;
    while (nb < 1 || nb > 4) {
        char *buff = recv_client_data(c);
        nb = atoi(buff);

        if (nb < 1 || nb > 4) {
            strcpy(message, BOLD_YELLOW"Erreur, vous devez entrer une ligne entre 1 et 4\n"RESET);
            send(c->socket, message, strlen(message), 0);
        }
        free(buff);
    }
    printf("Le joueur %s choisit de prendre la ligne %d complète\n", c->pseudo, nb - 1);
    fprintf(fichier_log, "Le joueur %s choisit de prendre la ligne %d complète\n", c->pseudo, nb - 1);
    return nb - 1;
}

void EndServeur()
{
    printf(BOLD_YELLOW"LA PARTIE A ÉTÉ INTERROMPU"RESET);
    close_all_clients();
    fprintf(fichier_log, "\n***FIN***\n");
    fclose(fichier_log);
    fflush(stdout);
    fflush(stdin);
    exit(EXIT_FAILURE);
    close_all_clients();
    printf(RESET);
    fflush(stdout);
    fflush(stdin);
    exit(0);
}
