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
struct sockaddr_in serveur_addr = {0};
Jeu jeu;

FILE *fichier_log;
char date[40], pwd[256], nom_fichier[256];
int nb_client = 0, serveur_socket;
unsigned short nb_Vide = 0;

unsigned char nb_ia = 0;


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
    fprintf(fichier_log, "Socket lié avec succès sur le port [%i]\n\n", PORT);

    listen(serveur_socket, 5);

    ChangeLimiteJeu();

    //Lancement du thread qui gere les connections des joueurs.
    pthread_t thread;
    pthread_create(&thread, NULL, &listen_joueurs, NULL);


    while (all_joueur_pret() == 0) {}

    send_all_joueurs(clients, nb_client, BOLD_GREEN"\nTous les joueurs sont prêt la partie va commencer...\n"RESET);
    printf(BOLD_GREEN"La partie va commencer...\n"RESET);
    fprintf(fichier_log, "La partie commence.\n");

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
    char *buffer = (char *) malloc(1024 * sizeof(char));
    if (isOver == 0) {
        strcpy(message, "Envoyer 'pret' ou [y] pour vous mettre prêt\nEnvoyer [b] pour ajouter un bot.");
        send(c->socket, message, strlen(message), 0);
        while (1) {
            recv_client_data(c, buffer);

            if (strcmp(buffer, "pret") == 0 || strcmp(buffer, "y") == 0) {
                c->pret = 1;
                snprintf(message, 1024, BOLD_GREEN"Le joueur %s à mis prêt"RESET, c->pseudo);
                send_all_joueurs(clients, nb_client, message);
                break;
            }else{
                if(strcmp(buffer, "b") ==0){
                    if(nb_client < MAX_JOUEURS){
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

        for (int i = 0; i < nb_client; ++i)
            pthread_create(&threads[i], NULL, listen_choix_carte_joueur, (void *) clients[i]);
        for (int i = 0; i < nb_client; i++)
            pthread_join(threads[i], NULL);

        Joueur **joueurs = get_ordre_joueur_tour(jeu);


        //SI PARTIE N'EST PAS TERMINE
        if (isOver == 0) {

            for (int i = 0; i < nb_client; ++i) {

                send_all_joueurs(clients, nb_client, "----------------------------------\n");
                fprintf(fichier_log, "---------------------------------------------------------------\n");

                //Indiquer aux joueurs qui doit jouer
                char *quiJoue = malloc(64 * sizeof(char));
                snprintf(quiJoue, 64, "[INFO] - Le joueur ");
                snprintf(quiJoue + strlen(quiJoue), 64, BOLD_MAGENTA"%s"RESET, clients[i]->pseudo);
                snprintf(quiJoue + strlen(quiJoue), 64, " doit poser sa carte\n\n");
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

                    //Au Serveur
                    printf(BOLD_YELLOW"\n***FIN DE LA PARTIE***\n"RESET);
                    printf(BOLD_YELLOW"***NOMBRE DE TETES MAXIMAL ATTEINT***\n"RESET);
                    //Aux joueurs
                    send_all_joueurs(clients, nb_client, BOLD_YELLOW"\n***FIN DE LA PARTIE***\n"RESET);
                    send_all_joueurs(clients, nb_client, BOLD_YELLOW"***NOMBRE DE TETES MAXIMAL ATTEINT***\n"RESET);
                    //Dans fichier LOG
                    fprintf(fichier_log, BOLD_YELLOW"\n***FIN DE LA PARTIE***\n"RESET);
                    fprintf(fichier_log, "***NOMBRE DE TETES MAXIMAL ATTEINT***\n");

                    printf("%s", AfficheNbTeteJoueurs(*jeu));
                    PrintTableau(*jeu); // Dans le fichier log

                    jeu->joueur[i]->nb_defaite++;
                    isOver = 1; //Valeur qui nous fait sortir du WHILE
                    char *message = malloc(1024 * sizeof(char));
                    snprintf(message, 1024, "%s\n", Statistique(*jeu));
                    send_all_joueurs(clients, nb_client, message);
                    free(message);
                    gettimeofday(&end, 0); // Initialisation du temps quand jeu est terminée
                    long secondes = end.tv_sec - begin.tv_sec;
                    long microsecondes = end.tv_usec - begin.tv_usec;
                    double duree_partie = secondes + microsecondes * 1e-6; // Calcul du temps total en seconde
                    duree_total += duree_partie;
                    AfficheTempsJeu(duree_partie);
                    break;
                }

                send_all_joueurs(clients, nb_client, affiche_plateau(jeu));
            }


            //Cas où tous les joueurs n'ont plus de carte, on redonne des cartes et on change le tour
            if (nb_Vide == nb_Joueur) {
                isOver = 3;
                send_all_joueurs(clients, nb_client, BOLD_YELLOW"\n***MANCHE TERMINE***\n"RESET);
                send_all_joueurs(clients, nb_client, BOLD_YELLOW"\n***LA PARTIE CONTINUE***\n"RESET);
                send_all_joueurs(clients, nb_client, AfficheNbTeteJoueurs(*jeu));
                freeJeu(jeu);
                init_jeu(jeu);
                isOver = 0;
                nb_Vide = 0;
                tour++; // Incrementation du tour des joueurs
            }


            //Cas ou nb de tour max atteint
            if (tour >= nb_MancheMax) {
                //Au serveur
                printf(BOLD_YELLOW"\n***FIN DE LA PARTIE***\n"RESET);
                printf(BOLD_YELLOW"***NOMBRE DE TOURS MAXIMAL ATTEINT***\n"RESET);
                //Aux joueurs
                send_all_joueurs(clients, nb_client, BOLD_YELLOW"\n***FIN DE LA PARTIE***\n"RESET);
                send_all_joueurs(clients, nb_client, BOLD_YELLOW"***NOMBRE DE TOURS MAXIMAL ATTEINT***\n"RESET);
                //Dans fichier LOG
                fprintf(fichier_log, BOLD_YELLOW"\n***FIN DE LA PARTIE***\n"RESET);
                fprintf(fichier_log, "***NOMBRE DE TOURS MAXIMAL ATTEINT***\n");

                printf("%s", AfficheNbTeteJoueurs(*jeu));
                PrintTableau(*jeu); // Dans le fichier log
                isOver = 1; //Valeur qui nous fait sortir du WHILE
                char *message = malloc(1024 * sizeof(char));
                snprintf(message, 1024, "%s\n", Statistique(*jeu));
                send_all_joueurs(clients, nb_client, message);
                free(message);
                gettimeofday(&end, 0); // Initialisation du temps quand jeu est terminée
                long secondes = end.tv_sec - begin.tv_sec;
                long microsecondes = end.tv_usec - begin.tv_usec;
                double duree_partie = secondes + microsecondes * 1e-6; // Calcul du temps total en seconde
                duree_total += duree_partie;
                AfficheTempsJeu(duree_partie);
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
                ChangeLimiteJeu();
                send_all_joueurs(clients, nb_client, RecapRegle(*jeu));
                freeJeu(jeu);
                isOver = 0;
                nb_Partie++;
                init_jeu(jeu);
                send_all_joueurs(clients, nb_client, BOLD_GREEN"La partie va commencer...\n"RESET);
                printf(BOLD_GREEN"La partie va commencer...\n"RESET);
                send_all_joueurs(clients, nb_client, affiche_plateau(jeu));

            } else if (answer == 'n' || answer == 'N' || answer == 'x') {
                send_all_joueurs(clients, nb_client, BOLD_YELLOW"Le serveur arrête stop le jeu...\n"RESET);
                EndServeur();
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
        printf("%d\n", a);
        if(a){
            printf("bot ici\n");
            continue;
        }

        strcpy(c->pseudo, buffer);

        printf("Connection réalisé avec le joueur %s\n", c->pseudo);
        snprintf(message, 1024, "Vous avez rejoint le jeu, vous êtes le joueur n°%u\n", nb_client + 1);
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
    for (int i = 0; i < nb_client; ++i){
        if(clients[i]->bot_or_not == 0)
            send(clients[i]->socket, message, strlen(message), 0);
    }
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
    usleep(.1);
    return 0;
}

void *listen_choix_carte_joueur(void *argv) {

    client *c = (client *) argv;

    if (isOver == 0 && getNbCarteUtilisableDuJoueur(jeu, c->numero_joueur) > 0) {

        char *message = (char *) malloc(1024 * sizeof(char));
        char *buffer = (char *) malloc(1024 * sizeof(char));

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
            recv_client_data(c,buffer);

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
        }

        //Si joueur n'a pas de carte, on incremente le nombre de joueur n'ayant pas de carte
        if (getNbCarteUtilisableDuJoueur(jeu, c->numero_joueur) == 0) {
            nb_Vide++;
        }

        free(message);
        free(buffer);
    }

}

void init_jeu(Jeu *jeu) {

    jeu->plateau = cree_plateau(); // Création du plateau de carte 4*6

    //Creation des 104 cartes avec numéro de tête random
    for (int i = 0; i < 104; i++) jeu->liste_carte[i] = create_carte(i + 1);

    //SI c'est la premiere partie, on initialise le nombre de défaites à 0.
    if (nb_Partie == 0) for (int i = 0; i < nb_Joueur; i++) jeu->joueur[i]->nb_defaite = 0;

    //Nombre de têtes à 0 vu que le jeu commence et si jeu pas terminé on ne remet pas les têtes à 0.
    if (isOver == 0) {
        for (int i = 0; i < nb_Joueur; i++) jeu->joueur[i]->nb_penalite = 0;
    }

    //On initialise le plateau à 0.
    Carte carte_0 = {0, 0, 0, 0};
    for (int i = 0; i < 4; i++) for (int j = 0; j < 6; j++) jeu->plateau[i][j] = carte_0;

    //Carte de la premiere colonne du plateau distribué
    creation_premiere_colonne_plateau(jeu);
    distribution_carte_joueurs(jeu);
//    affiche_plateau(jeu);
}

void client_quit(client *c) {
    char *mess = (char *) malloc(128 * sizeof(char));
    snprintf(mess, 128, BOLD_YELLOW"\nLe client %s a quitté la partie\n"RESET, c->pseudo);
    fprintf(fichier_log, "\nLe client %s a quitté la partie\n", c->pseudo);
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

void recv_client_data(client *c,char *buffer) {
    if (recv(c->socket, buffer, sizeof(buffer) - 1, 0) == 0) client_quit(c);
    printf("%s\n", buffer);
}

int carte_trop_petite(client *c) {

    char *message = malloc(256 * sizeof(char));
    char *buffer = (char *) malloc(1024 * sizeof(char));

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

void ChangeLimiteJeu() {
    send_all_joueurs(clients, nb_client, BOLD_YELLOW"En attente du serveur...\n"RESET);
    printf(BOLD_HIGH_WHITE"\nVoulez-vous changer les règles du jeu ? [y] / [n]\n"RESET);
    printf(BOLD_HIGH_WHITE"Actuellement le nombre de têtes maximal est de %d et la limite de tours maximal est de %d\n"RESET,
           nb_TeteMax, nb_MancheMax);
    printf(">");
    char answer;
    scanf(" %c", &answer);
    if (answer == 'y' || answer == 'Y' || answer == '\n') {
        printf(BOLD_HIGH_WHITE"Définissez le nombre de têtes maximal\n>"RESET);
        nb_TeteMax = AskNombreUser(0, 10000);
        printf(BOLD_HIGH_WHITE"Définissez le nombre de tours maximal\n>"RESET);
        nb_MancheMax = AskNombreUser(0, 10000);
        fprintf(fichier_log, "Changement du nombre de manches maximal : %d\n", nb_MancheMax);
        fprintf(fichier_log, "Changement du nombre de têtes maximal : %d\n\n", nb_TeteMax);
    }
}

void EndServeur() {
    printf(BOLD_YELLOW"LA PARTIE A ÉTÉ INTERROMPU\n"RESET);
    printf(BOLD_HIGH_WHITE"LE RÉSUMÉ DE LA PARTIE EST DISPONIBLE DANS LE DOSSIER DES LOGS\n"RESET);
    close_all_clients();
    fprintf(fichier_log, "\n***FIN***\n");
    fclose(fichier_log);
    fflush(stdout);
    fflush(stdin);
    printf(RESET);
    exit(EXIT_SUCCESS);
}

void ajout_bot(){

    int x = fork();


    char * nom_programme = "bot" ;
    char port[1024];
    snprintf(port, 1024, "%d", PORT);
    char * args [ ] = { nom_programme , port , "127.0.0.1" , NULL } ;

    if (x == 0)
        execv(nom_programme, args);
}

int is_bot(char *nom, client *c){

    printf("%s - %llu - %llu",nom , atoll(nom), bot_type);
    if(atoll(nom)  == bot_type){
        c->pret = 1;
        c->bot_or_not = 1;
        snprintf(c->pseudo,64,"bot n°%d", nb_ia);
        nb_bot++;
        nb_ia++
        clients[nb_client] =c;
        send_all_joueurs(clients, nb_client,"un bot à été ajouté.");
        nb_client++;
        nb_Joueur++;
        return 1;
    }

    return 0;
}
