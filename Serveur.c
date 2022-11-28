//**********//
// Server.c //
//**********//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Serveur.h"
#include <signal.h>


// ********* //
// VARIABLES //
// ********* //
client *clients[MAX_JOUEURS];

int nb_client = 0;
int serveur_socket;
struct timeval begin, end; // Pour calculer la durée d'une partie
struct sockaddr_in serveur_addr = {0};
Jeu jeu;

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
    if (getcwd(pwd, sizeof(pwd)) == NULL) {
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

    printf(BOLD_WHITE"BIENVENUE SUR LE JEU "RESET);
    printf(BOLD_GREEN"6"RESET);
    printf(BOLD_CYAN" QUI"RESET);
    printf(BOLD_MAGENTA" PREND!\n"RESET);


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
    signal(SIGINT, GestionSignauxServeur);
    signal(SIGTERM, GestionSignauxServeur);

    if (argc == 2) {
        PORT = atoi(argv[1]);
    }

    serveur_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (serveur_socket == -1) {
        perror(BOLD_RED"Erreur création socket"RESET);
        EndServeur();
        exit(errno);
    }
    printf(BOLD_YELLOW"Création du socket...\n"RESET);


    serveur_addr.sin_family = AF_INET;
    serveur_addr.sin_addr.s_addr = INADDR_ANY;
    serveur_addr.sin_port = htons(PORT);

    if (bind(serveur_socket, (struct sockaddr *) &serveur_addr, sizeof(struct sockaddr)) == -1) {
        perror(BOLD_RED"Erreur de la liaison des sockets"RESET);
        EndServeur();
        exit(errno);
    }

    printf(BOLD_GREEN"Socket lié avec succès sur le port [%i]\n"RESET, PORT);
    fprintf(fichier_log, "Socket lié avec succès sur le port [%i]\n", PORT);

    listen(serveur_socket, 5);


    printf(BOLD_HIGH_WHITE"\nVoulez-vous changer les règles du jeu ? [y] / [n]\n>"RESET);
    char answer;
    scanf(" %c", &answer);
    if (answer == 'y' || answer == 'Y' || answer == '\n') {
        printf(BOLD_HIGH_WHITE"Définissez le nombre de têtes maximal\n>"RESET);
        nb_TeteMax = AskNombreUser(0, 10000);
        printf(BOLD_HIGH_WHITE"Définissez le nombre de tours maximal\n>"RESET);
        nb_MancheMax = AskNombreUser(0, 10000);
    } else {
        nb_TeteMax = 66;
        nb_MancheMax = 999;
    }

    //Lancement du thread qui gere les connections des joueurs.
    pthread_t thread;
    pthread_create(&thread, NULL, &listen_joueurs, NULL);

    while (all_joueur_pret() == 0) {}

    send_all_joueurs(clients, nb_client, BOLD_GREEN"\nTous les joueurs sont prêt la partie va commencer...\n"RESET);
    printf(BOLD_GREEN"La partie va commencer...\n"RESET);
    fprintf(fichier_log, "La partie commence.\n");

    fprintf(fichier_log, "Nombre de manches maximal : %d\n", nb_MancheMax);
    fprintf(fichier_log, "Nombre de têtes maximal : %d\n\n", nb_TeteMax);

    for (int i = 0; i < nb_client; i++) jeu.joueur[i] = clients[i]->joueur;


    init_jeu(&jeu);

    send_all_joueurs(clients, nb_client, RecapRegle(jeu));
    send_all_joueurs(clients, nb_client, affiche_plateau(&jeu));

    gettimeofday(&begin, 0); //Initialisation du temps quand le jeu commence

    jeu_play(&jeu);

    EndServeur();
    return EXIT_SUCCESS;
}



// ********* //
// FONCTIONS //
// ********* //


void *joueur_pret(void *argv) {
    client *c = (client *) argv;
    char *message = (char *) malloc(1024 * sizeof(char));

    if (isOver == 0) {
        strcpy(message, "Envoyer 'pret' ou appuyer sur [y] pour vous mettre prêt\n");
        send(c->socket, message, strlen(message), 0);
        while (1) {
            char *buffer = recv_client_data(c);

            if (strcmp(buffer, "pret") == 0 || strcmp(buffer, "y") == 0) {
                c->pret = 1;
                snprintf(message, 1024, BOLD_GREEN"Le joueur %s à mis prêt"RESET, c->pseudo);
                send_all_joueurs(clients, nb_client, message);
                break;
            }
            free(buffer);
        }
        free(message);

    } else if (isOver == 3) {

        while (1) {
            char *buffer = recv_client_data(c);
            if (strcmp(buffer, "y") == 0 || strcmp(buffer, "Y") == 0) {
                c->pret = 1;
                snprintf(message, 1024, BOLD_GREEN"Le joueur %s à mis prêt"RESET, c->pseudo);
                send_all_joueurs(clients, nb_client, message);
                break;
            } else if (strcmp(buffer, "n") == 0 || strcmp(buffer, "N") == 0) {
                snprintf(message, 1024, BOLD_YELLOW"Le joueur %s à quitté"RESET, c->pseudo);
                send_all_joueurs(clients, nb_client, message);
                EndServeur();
            } else {
                printf(BOLD_RED"RECU [%s] taille %d\n"RESET, buffer, strlen(buffer));
            }

            free(buffer);
        }
        free(message);
    }


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


        //SI PARTIE N'EST PAS TERMINE
        if (isOver == 0) {

            for (int i = 0; i < nb_client; ++i) {

                //Indiquer aux joueurs qui doit jouer
                char *quiJoue = malloc(64 * sizeof(char));
                snprintf(quiJoue, 64, "[INFO] - C'est au tour de ");
                snprintf(quiJoue + strlen(quiJoue), 64, BOLD_MAGENTA"%s"RESET, clients[i]->pseudo);
                snprintf(quiJoue + strlen(quiJoue), 64, " de jouer\n\n");
                send_all_joueurs(clients, nb_client, quiJoue);
                free(quiJoue);


                int retour = ajoute_carte_au_plateau(jeu, joueurs[i]->carte_choisie);
                if (retour == 0 || retour == -1) {
                    client *c;
                    for (int j = 0; j < nb_client; ++j) {
                        if (clients[j]->joueur == joueurs[i])
                            c = clients[j];

                    }
                    int ligne = get_pos_carte_mini(jeu, joueurs[i]->carte_choisie->Numero) / 6;
                    if (retour == -1)
                        ligne = carte_trop_petite(c);
                    place_carte_si_trop_petite_ou_derniere_ligne(jeu, ligne, c->joueur);
                    printf("joueur : %s tete : %i\n", clients[i]->pseudo, c->joueur->nb_penalite);
                    fflush(stdout);
                }

                if (jeu->joueur[i]->nb_penalite >= nb_TeteMax) {
                    printf(BOLD_YELLOW"***FIN DE LA PARTIE***\n"RESET);
                    send_all_joueurs(clients, nb_client, BOLD_YELLOW"***FIN DE LA PARTIE***\n"RESET);
                    send_all_joueurs(clients, nb_client, BOLD_YELLOW"***NOMBRE DE TETES MAXIMAL ATTEINT***\n"RESET);
                    printf(BOLD_YELLOW"***NOMBRE DE TETES MAXIMAL ATTEINT***\n"RESET);
                    fprintf(fichier_log, "***NOMBRE DE TETES MAXIMAL ATTEINT***\n");
                    printf("%s", AfficheNbTeteJoueurs(*jeu));
                    PrintTableau(*jeu); // Dans le fichier log

                    jeu->joueur[i]->nb_defaite++;
                    nb_Partie++;
                    isOver = 1; //Valeur qui nous fait sortir du WHILE
                    char *message = malloc(1024 * sizeof(char));
                    snprintf(message, 1024, "%s\n", Statistique(*jeu));
                    send_all_joueurs(clients, nb_client, message);
                    gettimeofday(&end, 0); // Initialisation du temps quand jeu est terminée
                    long secondes = end.tv_sec - begin.tv_sec;
                    long microsecondes = end.tv_usec - begin.tv_usec;
                    double duree_partie = secondes + microsecondes * 1e-6; // Calcul du temps total en seconde
                    duree_total += duree_partie;
                    AfficheTempsJeu(duree_partie);
                    free(message);
                    break;
                }
                send_all_joueurs(clients, nb_client, "----------------------------------\n");
                fprintf(fichier_log, "---------------------------------------------------------------\n");
                send_all_joueurs(clients, nb_client, affiche_plateau(jeu));
            }
            tour++; // Incrementation du tour des joueurs

            //Cas ou nb de tour max atteint
            if (tour >= nb_MancheMax) {
                printf(BOLD_YELLOW"***FIN DE LA PARTIE***\n"RESET);
                send_all_joueurs(clients, nb_client, BOLD_YELLOW"***FIN DE LA PARTIE***\n"RESET);
                send_all_joueurs(clients, nb_client, BOLD_YELLOW"***NOMBRE DE TOURS MAXIMAL ATTEINT***\n"RESET);
                printf(BOLD_YELLOW"***NOMBRE DE TOURS MAXIMAL ATTEINT***\n"RESET);
                fprintf(fichier_log, "***NOMBRE DE TOURS MAXIMAL ATTEINT***\n");
                printf("%s", AfficheNbTeteJoueurs(*jeu));
                PrintTableau(*jeu); // Dans le fichier log
                char *message = malloc(1024 * sizeof(char));
                snprintf(message, 1024, "\n%s\n", Statistique(*jeu));
                send_all_joueurs(clients, nb_client, message);
                gettimeofday(&end, 0); // Initialisation du temps quand jeu est terminée
                long secondes = end.tv_sec - begin.tv_sec;
                long microsecondes = end.tv_usec - begin.tv_usec;
                double duree_partie = secondes + microsecondes * 1e-6; // Calcul du temps total en seconde
                duree_total += duree_partie;
                AfficheTempsJeu(duree_partie);
                nb_Partie++;
                isOver = 1; //Valeur qui nous fait sortir du WHILE
                free(message);
                break;
            }

            free(joueurs);

        } // FIN IF JEU TOURNE


            //Cas ou partie est finie
        else {
            ProcedureFinPartie(isOver);
        }

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
        snprintf(message, 1024, "Vous avez rejoint le serveur, vous êtes le joueur n°%u\n", nb_client + 1);
        send(client_socket, message, strlen(message), 0);

        snprintf(message, 1024, "Le joueur : %s vient de se connecter.", c->pseudo);
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

    if (isOver == 0) {
        client *c = (client *) argv;
        char *message = (char *) malloc(1024 * sizeof(char));

        //AFFICHAGE DES INFOS DU JOUEUR
        snprintf(message + strlen(message), 1024, BOLD_CYAN"\n\t*** ROUND [%d] ***\n"RESET, tour);
        snprintf(message + strlen(message), 1024, BOLD_CYAN"\n\t*** MANCHE [%d] ***\n\n"RESET,
                 abs(getNbCarteUtilisableDuJoueur(jeu, (unsigned short) c->numero_joueur) - 10));
        snprintf(message + strlen(message), 1024, BOLD_BLUE"Joueur %s, il vous reste %d cartes:\n"RESET, c->pseudo,
                 getNbCarteUtilisableDuJoueur(jeu, (unsigned short) c->numero_joueur));
        snprintf(message + strlen(message), 1024, "Vous possedez %d tetes\n", c->joueur[c->numero_joueur].nb_penalite);
        send(c->socket, message, strlen(message), 0);
        strcpy(message, affiche_cartes_joueur(c->joueur));
        send(c->socket, message, strlen(message), 0);

        //BOUCLE DE SAISIE DE LA CARTE
        while (1) {
            char *buffer = recv_client_data(c);

            int nb = atoi(buffer);
            if (nb == 0 || (nb > 10 || nb < 1)) {
                strcpy(message, BOLD_YELLOW"Erreur : la carte indiquée n’existe pas\n"RESET);
                send(c->socket, message, strlen(message), 0);
                continue;
            } else {
                if (c->joueur->carte[nb - 1]->isUsed == 1) {
                    strcpy(message, BOLD_YELLOW"Cette carte a deja été utilisée, choisissez en une autre\n"RESET);
                    send(c->socket, message, strlen(message), 0);
                } else {
                    c->joueur->carte[nb - 1]->isUsed = 1;
                    c->joueur->carte_choisie = c->joueur->carte[nb - 1];

                    printf("Le joueur %s pose sa carte %d > [%d - %d]\n", c->pseudo, nb - 1,
                           c->joueur->carte[nb - 1]->Numero, c->joueur->carte[nb - 1]->Tete);
                    fprintf(fichier_log, "Le joueur %s pose sa carte %d > [%d - %d] - Possède %d têtes\n", c->pseudo,
                            nb - 1, c->joueur->carte[nb - 1]->Numero, c->joueur->carte[nb - 1]->Tete,
                            c->joueur[c->numero_joueur].nb_penalite);
                    break;
                }
            }
            free(buffer);

        }

        //Si joueur n'a pas de carte, on lui en redonnne 10
        if (getNbCarteUtilisableDuJoueur(jeu, c->numero_joueur) == 0) {
            distribution_carte_joueur(&jeu, jeu.joueur[c->numero_joueur]);
            snprintf(message, 1024, "Le joueur %s reçoit 10 cartes\n", c->pseudo);
            send(c->socket, message, strlen(message), 0);
            printf("Le joueur %s reçoit 10 cartes", c->pseudo);
        }

        free(message);
    }

}

void client_quit(client *c) {
    char *mess = (char *) malloc(1024 * sizeof(char));
    snprintf(mess, 1024, BOLD_YELLOW"Le client %s a quitté la partie\n"RESET, c->pseudo);
    printf("%s\n", mess);
    send_all_joueurs(clients, nb_client, mess);
    free(mess);
    EndServeur();
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

    char *message = BOLD_CYAN"Vous avez posé la carte la plus petite du plateau\n"
                    "Quelle rangée voulez vous prendre [1-4] ?\n"RESET;

    send(c->socket, message, strlen(message), 0);
    char *buff = recv_client_data(c);
    int nb = atoi(buff);

    if (nb < 1 || nb > 4) {
        strcpy(message, BOLD_YELLOW"Erreur, vous devez entrer une ligne entre 1 et 4\n"RESET);
        send(c->socket, message, strlen(message), 0);
    }

    while (nb < 1 || nb > 4) {
        buff = recv_client_data(c);
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

void GestionSignauxServeur(int signal_recu) {
    switch (signal_recu) {

        //SIGNAL CTRL + C
        case SIGINT:
            fprintf(fichier_log, "SIGNAL %d REÇU\n", signal_recu);
            EndServeur();
            break;

            //SIGNAL POUR ARRETER PROGRAMME
        case SIGTERM:
            printf(BOLD_YELLOW"\nSIGNAL SIGTERM RECU\n"RESET);
            fprintf(fichier_log, "SIGNAL %d REÇU\n", signal_recu);
            EndServeur();
            break;

        default:
            printf(BOLD_YELLOW"\nSIGNAL RECU > %d\n"RESET, signal_recu);
            fprintf(fichier_log, "SIGNAL %d REÇU\n", signal_recu);
            break;
    }
}

void AfficheTempsJeu(double duree) {
    char *message = malloc(512 * sizeof(char));
    printf("Durée de la partie [%d] > %.5f secondes\n", nb_Partie, duree);
    printf("Temps de jeu total :  %.5f secondes\n", duree_total);
    fprintf(fichier_log, "Durée de la partie [%d] > %.5f secondes\n", nb_Partie, duree);
    fprintf(fichier_log, "Temps de jeu total :  %.5f secondes\n", duree_total);
    snprintf(message, 1024, "Durée de la partie [%d] > %.5f secondes\n", nb_Partie, duree);
    snprintf(message + strlen(message), 1024, "Temps de jeu total :  %.5f secondes\n", duree_total);
    send_all_joueurs(clients, nb_client, message);
    free(message);
}

void ProcedureFinPartie(int etat) {

    switch (etat) {

        case 1:
            //On remet les clients en mode pas prêt
            for (int i = 0; i < nb_client; i++) {
                printf("%s pas prêt\n", clients[i]->pseudo);
                clients[i]->pret = 0;
            }

//            char *message1 = malloc(64 * sizeof(char));
//            snprintf(message1, 64, BOLD_MAGENTA"Voulez-vous refaire une partie ? [y] / [n]\n"RESET);
//            send_all_joueurs(clients, nb_client, message1);
//            free(message1);
            isOver = 2; // Attente que les joueurs se mettent prêt à nouveau
            break;

        case 2:
//            printf("Création Threads ecoute...\n");
//            pthread_t pthread;
//            pthread_create(&pthread, NULL, &joueur_pret, (void *) clients[nb_client]);
            isOver = 3;
            break;

        case 3:
//            printf("En attente des joueurs...\n");
//            send_all_joueurs(clients, nb_client, "En attente des joueurs...\n");
//            while (all_joueur_pret() == 0) {
//
//            }
//            char *message3 = malloc(128 * sizeof(char));
//            snprintf(message3, 128, BOLD_GREEN"Tous les joueurs sont prêts, la partie %d va commencer...\n"RESET,
//                     nb_Partie);
//            send_all_joueurs(clients, nb_client, message3);
            send_all_joueurs(clients, nb_client,
                             BOLD_GREEN"Tous les joueurs sont prêts, la partie va commencer...\n"RESET);
//            free(message3);
            init_jeu(&jeu);
            resetJeu();
            break;
    }


}

void EndServeur() {
    printf(BOLD_YELLOW"LA PARTIE A ÉTÉ INTERROMPU\n"RESET);
    printf(BOLD_WHITE"LE RÉSUMÉ DE LA PARTIE EST DISPONIBLE DANS LE DOSSIER DES LOGS\n"RESET);
    close_all_clients();
    fprintf(fichier_log, "\n***FIN***\n");
    fclose(fichier_log);
    fflush(stdout);
    fflush(stdin);
    printf(RESET);
    exit(EXIT_SUCCESS);
}
