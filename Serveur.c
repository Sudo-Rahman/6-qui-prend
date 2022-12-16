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


// ********* //
// VARIABLES //
// ********* //
client *clients[MAX_JOUEURS];
struct timeval begin, end; // Pour calculer la durée d'une partie
struct sockaddr_in serveur_addr = {0}; // Socket
Jeu jeu;//Le jeu
FILE *fichier_log; //Le fichier log
char date[40], pwd[256], nom_fichier[256];
int nb_client = 0, serveur_socket;
unsigned char nb_ia = 0, nb_Vide = 0;


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

        if (!mkdir("LOG", 0771)) printf("SUCCÈS CREATION DU DOSSIER LOG\n");
        else {
            printf(BOLD_RED"IMPOSSIBLE DE CREER LE DOSSIER POUR LES LOGS\n"RESET);
            exit(EXIT_FAILURE);
        }
    }

    printf(BOLD_HIGH_WHITE"BIENVENUE SUR LE JEU "RESET);
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
    signal(SIGINT, gestion_signaux_serveur);
    signal(SIGTERM, gestion_signaux_serveur);
    signal(SIGABRT, gestion_signaux_serveur);


    if (argc == 2) {
        PORT = atoi(argv[1]);
    }

    serveur_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (serveur_socket == -1) {
        perror(BOLD_RED"Erreur création socket"RESET);
        end_serveur();
        exit(errno);
    }
    printf(BOLD_YELLOW"Création du socket...\n"RESET);


    serveur_addr.sin_family = AF_INET;
    serveur_addr.sin_addr.s_addr = INADDR_ANY;
    serveur_addr.sin_port = htons(PORT);

    if (bind(serveur_socket, (struct sockaddr *) &serveur_addr, sizeof(struct sockaddr)) == -1) {
        perror(BOLD_RED"Erreur de la liaison des sockets"RESET);
        end_serveur();
        exit(errno);
    }

    printf(BOLD_GREEN"Socket lié avec succès sur le port [%i]\n"RESET, PORT);
    fprintf(fichier_log, "Socket lié avec succès sur le port [%i]\n\n", PORT);

    listen(serveur_socket, 5);

    change_limite_jeu();

    //Lancement du thread qui gere les connections des joueurs.
    pthread_t thread;
    pthread_create(&thread, NULL, &listen_joueurs, NULL);


    //Tant que conditions pour lancer le jeu sont pas bonnes, on attends
    while (all_joueur_pret() != 1) {
        usleep((useconds_t) .1);
    }


    send_all_joueurs(clients, nb_client, BOLD_GREEN"\nTous les joueurs sont prêt la partie va commencer...\n"RESET);
    printf(BOLD_GREEN"La partie va commencer...\n"RESET);
    fprintf(fichier_log, "La partie commence.\n");

    for (int i = 0; i < nb_client; i++) jeu.joueur[i] = clients[i]->joueur;

    //Initialisation du jeu
    init_jeu(&jeu);

    send_all_joueurs(clients, nb_client, recap_regle());
    send_all_joueurs(clients, nb_client, affiche_plateau(&jeu));

    gettimeofday(&begin, 0); //Initialisation du temps quand le jeu commence

    jeu_play(&jeu);

    end_serveur();
    return EXIT_SUCCESS;
}



// ********* //
// FONCTIONS //
// ********* //

void *joueur_pret(void *argv) {
    client *c = (client *) argv;
    char *message = (char *) malloc(1024 * sizeof(char));
    char *buffer = (char *) malloc(1024 * sizeof(char));
    if (isOver == 0) {
        strcpy(message, "Envoyer 'pret' ou [y] pour vous mettre prêt\nEnvoyer [b] pour ajouter un bot.");
        send(c->socket, message, strlen(message), 0);
        while (1) {
            recv_client_data(c, buffer);

            if (strcmp(buffer, "pret") == 0 || strcmp(buffer, "y") == 0) {
                c->pret = 1;
                nb_pret++;
                snprintf(message, 1024, BOLD_GREEN"Le joueur %s à mis prêt [%d/%d]\n"RESET, c->pseudo, nb_pret,
                         nb_Joueur);
                send_all_joueurs(clients, nb_client, message);

                //Nombre de joueur pas encore prêt
                if (nb_pret != nb_Joueur) send_all_joueurs(clients, nb_client, "En attente des autres joueurs...\n");
                    //Si il manque encore des joueurs pour lancer la partie
                else if (nb_Joueur < MIN_JOUEURS) {
                    snprintf(message, 1024, BOLD_YELLOW"Il manque encore %d joueur pour commencer la partie\n"RESET,
                             MIN_JOUEURS - nb_Joueur);
                    send_all_joueurs(clients, nb_client, message);
                }
                while (nb_pret < MIN_JOUEURS) {
                    strcpy(message, "Envoyer [b] pour ajouter un bot.");
                    send(c->socket, message, strlen(message), 0);
                    recv_client_data(c, buffer);
                    if (strcmp(buffer, "b") == 0) {
                        if (nb_client < MAX_JOUEURS) {
                            ajout_bot();
                        }
                    }
                }
                break;
            } else {
                if (strcmp(buffer, "b") == 0) {
                    if (nb_client < MAX_JOUEURS) {
                        ajout_bot();
                    }
                }
            }
        }
        free(message);
        free(buffer);

    }

    return 0;
}

void jeu_play(Jeu *jeu) {

    while (1) {
        pthread_t threads[nb_client];

        for (int i = 0; i < nb_client; ++i) {
            if (clients[i]->bot_or_not == 0)
                pthread_create(&threads[i], NULL, listen_choix_carte_joueur, (void *) clients[i]);
            else
                pthread_create(&threads[i], NULL, listen_choix_carte_bot, (void *) clients[i]);

        }
        for (int i = 0; i < nb_client; i++)
            pthread_join(threads[i], NULL);

        Joueur **joueurs = get_ordre_joueur_tour(jeu);


        //SI PARTIE N'EST PAS TERMINE
        if (isOver == 0) {

            for (int i = 0; i < nb_client; ++i) {

                send_all_joueurs(clients, nb_client, "----------------------------------\n");
                fprintf(fichier_log, "---------------------------------------------------------------\n");


                int retour = ajoute_carte_au_plateau(jeu, joueurs[i]->carte_choisie);
                if (retour == 0 || retour == -1) {
                    client *c;
                    for (int j = 0; j < nb_client; ++j) {
                        if (clients[j]->joueur == joueurs[i]) c = clients[j];
                    }
                    int ligne = get_pos_carte_mini(jeu, joueurs[i]->carte_choisie->numero) / 6;
                    if (retour == -1) ligne = carte_trop_petite(c);
                    place_carte_si_trop_petite_ou_derniere_ligne(jeu, ligne, c->joueur);
                    printf("joueur : %s tete : %i\n", clients[i]->pseudo, c->joueur->nb_penalite);
                    fflush(stdout);
                }

                if (jeu->joueur[i]->nb_penalite >= nb_tete_max && !isOver) {

                    //Au Serveur
                    printf(BOLD_YELLOW"\n***FIN DE LA PARTIE***\n"RESET);
                    printf(BOLD_YELLOW"***NOMBRE DE TETES MAXIMAL ATTEINT***\n"RESET);
                    //Aux joueurs
                    send_all_joueurs(clients, nb_client, BOLD_YELLOW"\n***FIN DE LA PARTIE***\n"RESET);
                    send_all_joueurs(clients, nb_client, BOLD_YELLOW"***NOMBRE DE TETES MAXIMAL ATTEINT***\n"RESET);
                    //Dans fichier LOG
                    fprintf(fichier_log, "\n***FIN DE LA PARTIE***\n");
                    fprintf(fichier_log, "***NOMBRE DE TETES MAXIMAL ATTEINT***\n");

                    print_tableau(*jeu); // Dans le fichier log

                    jeu->joueur[i]->nb_defaite++;
                    isOver = 1; //Valeur qui nous fait sortir du WHILE

                    //Envoie des stats aux joueurs
                    char *message = malloc(1024 * sizeof(char));
                    snprintf(message, 1024, "%s\n", statistique(*jeu));
                    send_all_joueurs(clients, nb_client, message);
                    fprintf(fichier_log, "%s\n", message); //DANS FICHIER
                    printf("%s\n", message);//DANS TERMINAL
                    free(message);

                    gettimeofday(&end, 0); // Initialisation du temps quand jeu est terminée
                    long secondes = end.tv_sec - begin.tv_sec;
                    long microsecondes = end.tv_usec - begin.tv_usec;
                    double duree_partie = secondes + microsecondes * 1e-6; // Calcul du temps total en seconde
                    duree_total += duree_partie;
                    affiche_temps_jeu(duree_partie);
                    break;
                }

                send_all_joueurs(clients, nb_client, affiche_plateau(jeu));
            }


            //Cas où tous les joueurs n'ont plus de carte, on redonne des cartes et on change le tour
            if (nb_Vide == nb_Joueur && !isOver) {
                isOver = 3;
                send_all_joueurs(clients, nb_client, BOLD_YELLOW"\n***MANCHE TERMINE***\n"RESET);
                send_all_joueurs(clients, nb_client, BOLD_YELLOW"\n***LA PARTIE CONTINUE***\n"RESET);
                send_all_joueurs(clients, nb_client, affiche_nb_tete_joueurs(*jeu));
                free_jeu(jeu);
                init_jeu(jeu);
                isOver = 0;
                nb_Vide = 0;
                tour++; // Incrementation du tour des joueurs
            }


            //Cas ou nb de tour max atteint
            if (tour >= nb_manche_max && !isOver) {
                //Au serveur
                printf(BOLD_YELLOW"\n***FIN DE LA PARTIE***\n"RESET);
                printf(BOLD_YELLOW"***NOMBRE DE TOURS MAXIMAL ATTEINT***\n"RESET);
                //Aux joueurs
                send_all_joueurs(clients, nb_client, BOLD_YELLOW"\n***FIN DE LA PARTIE***\n"RESET);
                send_all_joueurs(clients, nb_client, BOLD_YELLOW"***NOMBRE DE TOURS MAXIMAL ATTEINT***\n"RESET);
                //Dans fichier LOG
                fprintf(fichier_log, "\n***FIN DE LA PARTIE***\n");
                fprintf(fichier_log, "***NOMBRE DE TOURS MAXIMAL ATTEINT***\n");

                print_tableau(*jeu); // Dans le fichier log
                isOver = 1; //Valeur qui nous fait sortir du WHILE

                //Envoie des stats aux joueurs
                char *message = malloc(1024 * sizeof(char));
                snprintf(message, 1024, "%s\n", statistique(*jeu));
                send_all_joueurs(clients, nb_client, message);
                fprintf(fichier_log, "%s\n", message); //DANS FICHIER
                printf("%s\n", message);//DANS TERMINAL
                free(message);

                gettimeofday(&end, 0); // Initialisation du temps quand jeu est terminée
                long secondes = end.tv_sec - begin.tv_sec;
                long microsecondes = end.tv_usec - begin.tv_usec;
                double duree_partie = secondes + microsecondes * 1e-6; // Calcul du temps total en seconde
                duree_total += duree_partie;
                affiche_temps_jeu(duree_partie);
            }
            free(joueurs);

        } // FIN IF JEU TOURNE

            //Cas ou partie est finie
        else {
            send_all_joueurs(clients, nb_client, BOLD_YELLOW"\nEn attente du serveur...\n"RESET);
            printf(BOLD_HIGH_WHITE"\nRelancer une partie ? [y] / [n]\n>"RESET);
            char answer;
            scanf(" %c", &answer);
            if (answer == 'y' || answer == 'Y' || answer == '\n') {
                change_limite_jeu();
                send_all_joueurs(clients, nb_client, recap_regle());
                free_jeu(jeu);
                isOver = 0;
                nb_partie++;
                init_jeu(jeu);
                send_all_joueurs(clients, nb_client, BOLD_GREEN"La partie va commencer...\n"RESET);
                printf(BOLD_GREEN"La partie va commencer...\n"RESET);
                send_all_joueurs(clients, nb_client, affiche_plateau(jeu));
                gettimeofday(&begin, 0); //Initialisation du temps quand le jeu commence

            } else if (answer == 'n' || answer == 'N' || answer == 'x') {
                send_all_joueurs(clients, nb_client, BOLD_YELLOW"Le serveur arrête stop le jeu...\n"RESET);
                end_serveur();
            }

        }

    }
}

void *listen_joueurs() {
    while (1) {
        printf("\nEn attente de connexion de client...\n");

        //Initialisation du socket
        struct sockaddr_in client_addr = {0};
        int taille = (int) sizeof(serveur_addr);
        int client_socket = accept(serveur_socket, (struct sockaddr *) &serveur_addr, (socklen_t *) &taille);

        //Attente que les joueurs soient prêt
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
        char *buffer = (char *) malloc(1024 * sizeof(char));

        recv_client_data(c, buffer);

        int a = is_bot(buffer, c);
        if (a) {
            printf("Un bot s'est connecté au serveur\n");
            continue;
        }

        strcpy(c->pseudo, buffer);

        printf("Connexion réalisé avec le joueur %s\n", c->pseudo);
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
    for (int i = 0; i < nb_client; ++i) {
        if (clients[i]->bot_or_not == 0)
            send(clients[i]->socket, message, strlen(message), 0);
    }
}

client *init_joueur() {
    client *c = (client *) malloc(sizeof(client));
    c->pseudo = (char *) calloc(128, sizeof(char));
    c->pret = 0;
    c->bot_or_not = 0;
    c->joueur = (Joueur *) malloc(sizeof(Joueur));
    c->numero_joueur = nb_client;
    return c;
}

int all_joueur_pret() {
    //Si aucun client
    if (nb_client == 0) return 0;

    unsigned char compteur = 0;
    for (int i = 0; i < nb_client; i++) if (clients[i]->pret == 1) compteur++;

    //Si tous les joueurs sont prets et que le nombre est bon le jeu commence
    if (compteur == nb_client && compteur >= MIN_JOUEURS) return 1;

    usleep((useconds_t) .1);
    return 0;
}

void *listen_choix_carte_joueur(void *argv) {

    client *c = (client *) argv;

    if (isOver == 0 && get_nb_carte_utilisable_joueur(jeu, c->numero_joueur) > 0) {

        char *message = (char *) malloc(1024 * sizeof(char));
        char *buffer = (char *) malloc(1024 * sizeof(char));

        //AFFICHAGE DES INFOS DU JOUEUR
        snprintf(message + strlen(message), 1024, BOLD_CYAN"\n\t*** ROUND [%d] ***\n"RESET, tour);
        snprintf(message + strlen(message), 1024, BOLD_CYAN"\n\t*** MANCHE [%d] ***\n\n"RESET,
                 abs(get_nb_carte_utilisable_joueur(jeu, (unsigned short) c->numero_joueur) - 10));
        snprintf(message + strlen(message), 1024, BOLD_BLUE"Joueur %s, il vous reste %d cartes:\n"RESET,
                 c->pseudo,
                 get_nb_carte_utilisable_joueur(jeu, (unsigned short) c->numero_joueur));
        snprintf(message + strlen(message), 1024, "Vous possedez %d tetes\n",
                 c->joueur[c->numero_joueur].nb_penalite);
        send(c->socket, message, strlen(message), 0);
        strcpy(message, affiche_cartes_joueur(c->joueur));
        send(c->socket, message, strlen(message), 0);

        //BOUCLE DE SAISIE DE LA CARTE
        while (1) {
            recv_client_data(c, buffer);

            int nb = atoi(buffer);
            if (nb == 0 || (nb > 10 || nb < 1)) {
                strcpy(message, BOLD_YELLOW"Erreur : la carte indiquée n’existe pas\n"RESET);
                send(c->socket, message, strlen(message), 0);
                continue;
            } else {
                if (c->joueur->carte[nb - 1]->is_used == 1) {
                    strcpy(message, BOLD_YELLOW"Cette carte a deja été utilisée, choisissez en une autre\n"RESET);
                    send(c->socket, message, strlen(message), 0);
                } else {
                    c->joueur->carte[nb - 1]->is_used = 1;
                    c->joueur->carte_choisie = c->joueur->carte[nb - 1];

                    printf("Le joueur %s pose sa carte %d > [%d - %d]\n", c->pseudo, nb - 1,
                           c->joueur->carte[nb - 1]->numero, c->joueur->carte[nb - 1]->Tete);
                    fprintf(fichier_log, "Le joueur %s pose sa carte %d > [%d - %d] - Possède %d têtes\n", c->pseudo,
                            nb - 1, c->joueur->carte[nb - 1]->numero, c->joueur->carte[nb - 1]->Tete,
                            c->joueur[c->numero_joueur].nb_penalite);
                    break;
                }
            }
        }

        //Si joueur n'a pas de carte, on incremente le nombre de joueur n'ayant pas de carte
        if (get_nb_carte_utilisable_joueur(jeu, c->numero_joueur) == 0) {
            nb_Vide++;
        }

        free(message);
        free(buffer);
    }
}

void *listen_choix_carte_bot(void *argv) {
    client *c = (client *) argv;

    if (isOver == 0 && get_nb_carte_utilisable_joueur(jeu, c->numero_joueur) > 0) {
        char *buffer = (char *) malloc(1024 * sizeof(char));

        while (1) {
            char message[1024];
            unsigned char compteur = 0;

            for (int i = 0; i < 10; ++i) {
                if (c->joueur->carte[i]->is_used == 0) {
                    message[compteur] = "0123456789"[i];
//                    printf("%s - %c\n", message, "0123456789"[i]);
                    fflush(stdout);
                    compteur++;
                }
            }
            message[compteur] = '\0';


            printf("%s - %lu - %u\n", message, strlen(message), compteur);
            fflush(stdout);


            send(c->socket, message, compteur, 0);
            fflush(stdout);


            recv_client_data(c, buffer);
//            printf("carte : %s\n", buffer);
            fflush(stdout);
//            sleep(5);

            int nb = atoi(buffer);
            if (nb > 10 || nb < 0) {
                continue;
            } else {
                if (c->joueur->carte[nb]->is_used == 1) {
                } else {
                    c->joueur->carte[nb]->is_used = 1;
                    c->joueur->carte_choisie = c->joueur->carte[nb];

                    if (c->bot_or_not) {
                        printf("Le bot %s pose sa carte %d > [%d - %d]\n", c->pseudo, nb,
                               c->joueur->carte[nb]->numero, c->joueur->carte[nb]->Tete);
                    } else {
                        printf("Le joueur %s pose sa carte %d > [%d - %d]\n", c->pseudo, nb,
                               c->joueur->carte[nb]->numero, c->joueur->carte[nb]->Tete);
                    }

                    fprintf(fichier_log, "Le joueur %s pose sa carte %d > [%d - %d] - Possède %d têtes\n", c->pseudo,
                            nb, c->joueur->carte[nb]->numero, c->joueur->carte[nb]->Tete,
                            c->joueur[c->numero_joueur].nb_penalite);
                    break;
                }
            }
        }

        //Si joueur n'a pas de carte, on incremente le nombre de joueur n'ayant pas de carte
        if (get_nb_carte_utilisable_joueur(jeu, c->numero_joueur) == 0) {
            nb_Vide++;
        }

        free(buffer);
    }
}

void client_quit(client *c) {
    char *mess = (char *) malloc((128 * sizeof(char)) + strlen(c->pseudo));
    snprintf(mess, 128 + strlen(c->pseudo), BOLD_YELLOW"\nLe client %s a quitté la partie\n"RESET, c->pseudo);
    fprintf(fichier_log, "\nLe client %s a quitté la partie\n", c->pseudo);
    printf("%s\n", mess);
    send_all_joueurs(clients, nb_client, mess);
    free(mess);
    end_serveur();
}

void close_all_clients() {
    for (int i = 0; i < nb_client; ++i) {
        close(clients[i]->socket);
    }
    close(serveur_socket);
}

void recv_client_data(client *c, char *buffer) {
    int n;
    memset(buffer, 0, sizeof(char) * strlen(buffer));
    if ((n = recv(c->socket, buffer, sizeof(buffer) - 1, 0)) == 0) client_quit(c);
    buffer[n] = '\0';
}

int carte_trop_petite(client *c) {

    char *message = malloc(256 * sizeof(char));
    char *buffer = (char *) malloc(1024 * sizeof(char));

    if (c->bot_or_not == 1) {
        char mes[1024];
        send(c->socket, "1234\0", 5, 0);
        char buffer[1024];
        recv_client_data(c, buffer);
        return atoi(buffer) - 1;
    }

    strcpy(message,
           BOLD_CYAN"Vous avez posé la carte la plus petite du plateau\nQuelle rangée voulez vous prendre [1-4] ?\n"RESET);
    send(c->socket, message, strlen(message), 0);

    int nb;

    while (1) {
        recv_client_data(c, buffer);
        nb = atoi(buffer);

        if (nb < 1 || nb > 4) {
            strcpy(message, BOLD_YELLOW"Erreur, vous devez entrer une ligne entre 1 et 4\n"RESET);
            send(c->socket, message, strlen(message), 0);
        } else break;
    }

    free(message);
    free(buffer);

    printf("Le joueur %s choisit de prendre la ligne %d complète\n", c->pseudo, nb - 1);
    fprintf(fichier_log, "Le joueur %s choisit de prendre la ligne %d complète\n", c->pseudo, nb - 1);
    return nb - 1;
}

void gestion_signaux_serveur(int signal_recu) {
    switch (signal_recu) {
        //SIGNAL CTRL + C
        case SIGINT:
            fprintf(fichier_log, "SIGNAL %d REÇU\n", signal_recu);
            end_serveur();
            break;
            //SIGNAL POUR ARRETER PROGRAMME
        case SIGTERM:
            printf(BOLD_YELLOW"\nSIGNAL SIGTERM RECU\n"RESET);
            fprintf(fichier_log, "SIGNAL %d REÇU\n", signal_recu);
            end_serveur();
            break;

            //SIGNAL QUAND PROGRAMME CRASH
        case SIGABRT:
            printf(BOLD_YELLOW"\nSIGNAL SIGABRT RECU\n"RESET);
            fprintf(fichier_log, "SIGNAL %d REÇU\n", signal_recu);
            end_serveur();
            break;

        default:
            printf(BOLD_YELLOW"\nSIGNAL RECU > %d\n"RESET, signal_recu);
            fprintf(fichier_log, "SIGNAL %d REÇU\n", signal_recu);
            break;
    }
}

void affiche_temps_jeu(double duree) {
    char *message = malloc(1024 * sizeof(char));
    printf("Durée de la partie [%d] > %.3f secondes\n", nb_partie + 1, duree);
    printf("Temps de jeu total :  %.3f secondes\n", duree_total);
    fprintf(fichier_log, "Durée de la partie [%d] > %.3f secondes\n", nb_partie + 1, duree);
    fprintf(fichier_log, "Temps de jeu total :  %.3f secondes\n", duree_total);
    snprintf(message, 1024, "Durée de la partie [%d] > %.3f secondes\n", nb_partie + 1, duree);
    snprintf(message + strlen(message), 1024, "Temps de jeu total :  %.3f secondes\n", duree_total);
    send_all_joueurs(clients, nb_client, message);
    free(message);
}

void change_limite_jeu() {
    send_all_joueurs(clients, nb_client, BOLD_YELLOW"En attente du serveur...\n"RESET);
    printf(BOLD_HIGH_WHITE"\nVoulez-vous changer les règles du jeu ? [y] / [n]\n"RESET);
    printf(BOLD_HIGH_WHITE"Actuellement le nombre de têtes maximal est de %d et la limite de tours maximal est de %d\n"RESET,
           nb_tete_max, nb_manche_max);
    printf(">");
    char answer;
    scanf(" %c", &answer);
    if (answer == 'y' || answer == 'Y' || answer == '\n') {
        printf(BOLD_HIGH_WHITE"Définissez le nombre de têtes maximal\n>"RESET);
        nb_tete_max = AskNombreUser(0, 10000);
        printf(BOLD_HIGH_WHITE"Définissez le nombre de tours maximal\n>"RESET);
        nb_manche_max = AskNombreUser(0, 2000000000);
        fprintf(fichier_log, "Changement du nombre de manches maximal : %d\n", nb_manche_max);
        fprintf(fichier_log, "Changement du nombre de têtes maximal : %d\n\n", nb_tete_max);
    }
}

void end_serveur() {
    printf(BOLD_YELLOW"\nLA PARTIE A ÉTÉ INTERROMPU\n"RESET);
    printf(BOLD_HIGH_WHITE"LE RÉSUMÉ DE LA PARTIE EST DISPONIBLE DANS LE DOSSIER DES LOGS\n"RESET);
    close_all_clients();
    fprintf(fichier_log, "\n***FIN***\n");
    fclose(fichier_log);
    fflush(stdout);
    fflush(stdin);
    printf(RESET);
    exit(EXIT_SUCCESS);
}

void ajout_bot() {

    short x = fork();
    char *nom_programme = "bot";
    char port[1024];
    snprintf(port, 1024, "%d", PORT);
    char *args[] = {nom_programme, port, "127.0.0.1", NULL};

    if (x == 0) execv(nom_programme, args);
}

char *statistique(Jeu jeu) {

    char *tmp = malloc(1024 * sizeof(char));

    //DANS TERMINAL DES JOUEURS
    snprintf(tmp + strlen(tmp), 1024, BOLD_CYAN"\n\t[STATISTIQUES]\n"RESET);
    snprintf(tmp + strlen(tmp), 1024, "\nNombre de parties joué : %d\n", nb_partie + 1);
    snprintf(tmp + strlen(tmp), 1024, "Nombre de tours effectué : %d\n", tour);
    snprintf(tmp + strlen(tmp), 1024, "Moyenne des têtes obtenues : %d\n", moyenne_des_tetes(jeu));
    snprintf(tmp + strlen(tmp), 1024, "%s", affiche_nb_tete_joueurs(jeu));
    snprintf(tmp + strlen(tmp), 1024, "%s", min_max_defaite(jeu)); //Toujours en fin de stats

    char *res = malloc(strlen(tmp) * sizeof(char));
    strcpy(res, tmp);
    free(tmp);
    return res;
}

char *affiche_nb_tete_joueurs(Jeu jeu) {
    char *tmp = malloc(512 * nb_Joueur * sizeof(char));
    for (short i = 0; i < nb_Joueur; i++)
        snprintf(tmp + strlen(tmp), 512, "Le joueur %s possède %d têtes \n", clients[i]->pseudo,
                 jeu.joueur[i]->nb_penalite);

    fprintf(fichier_log, "\n");

    char *res = malloc(strlen(tmp) * sizeof(char));
    strcpy(res, tmp);
    free(tmp);
    return res;
}

int is_bot(char *nom, client *c) {

    if (atoll(nom) == bot_type) {
        c->pret = 1;
        nb_pret++;
        c->bot_or_not = 1;
        snprintf(c->pseudo, 64, "bot n°%d", nb_ia);
        nb_bot++;
        nb_ia++;
        clients[nb_client] = c;
        send_all_joueurs(clients, nb_client, BOLD_YELLOW"Un bot a été ajouté\n"RESET);
        fprintf(fichier_log, "Un bot a été ajouté\n");
        nb_client++;
        nb_Joueur++;
        return 1;
    }
    return 0;
}
