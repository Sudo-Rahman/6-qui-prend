//**********//
//   Jeu.h  //
//**********//

#ifndef SYSTEMES_ET_RESEAUX_PROJET_JEU_H
#define SYSTEMES_ET_RESEAUX_PROJET_JEU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include "Color.h"

#define MAX_JOUEURS 10
#define MIN_JOUEURS 2


typedef struct Carte {
    unsigned char numero, Tete, is_picked, is_used;
} Carte;

typedef struct Joueur {
    char pseudo[64];
    unsigned short nb_penalite, nb_defaite;
    Carte *carte[10];
    Carte *carte_choisie;
} Joueur;


typedef struct Jeu {
    Carte **plateau;
    Joueur *joueur[MAX_JOUEURS];
    Carte *liste_carte[104];
} Jeu;

unsigned int tour = 1, nb_partie = 0, nb_tete_max = 66, nb_manche_max = 999;
unsigned char isOver = 0, nb_joueur = 0, nb_bot = 0,nb_pret = 0;
double duree_total = 0;

FILE *fichier_log;
char date[40];
char nom_fichier[256];

/**
 * @details Fonction qui permet d'initialiser le jeu
 * @param jeu
 */
void init_jeu(Jeu *jeu);

/**
 * @details Fonction pour créer un plateau de carte
 * @return Un tableau de carte représentant le plateau
 */
Carte **cree_plateau();


/**
 * @details Fonction pour afficher le plateau ainsi que les cartes présentent dessus
 * @param jeu
 * @return char*
 */
char *affiche_plateau(Jeu *jeu);

/**
 * @details Fonction pour libérer de la mémoire la matrice de carte
 * @param jeu
 */
void free_jeu(Jeu *jeu);


/**
 * @details Fonction qui permet d'afficher les règles du jeu à l'écran quand le jeu démarre
 * @param jeu
 * @return char*
 */
char *recap_regle();


/**
 * @details Fonction pour afficher les statistiques à l'écran
 * @param jeu
 * @return char*
 */
char *statistique(Jeu *jeu);

/**
 * @details Fonction qui fait la moyenne des têtes
 * @param jeu
 * @return int
 */
int moyenne_des_tetes(Jeu *jeu);

/**
 * @details Fonction qui affiche quel joueur a le plus et qui a le moins de défaite
 * @param jeu
 * @return char*
 */
char *min_max_defaite(Jeu *jeu);

/**
 * @details Fonction pour afficher le tableau dans un fichier log
 * @param jeu
 */
void print_tableau(Jeu *jeu);

/**
 * @details Fonction qui force la fin du jeu si l'utilisateur appuie sur 'x'
 */
void force_fin_jeu();

//POUR CARTE

/**
 * @details Fonction qui permet de créer une carte avec en paramètre son numéro
 * @param i
 * @return Carte
 */
Carte *create_carte(unsigned short i);

/**
 * @details Fonction qui retourne une carte de la liste non utilisée et distribuée.
 * @param jeu
 * @return carte
 */
Carte *get_carte_liste(Jeu *jeu);


/**
 * @details Fonction qui distribue les cartes aux joueurs.
 * @param jeu
 */
void distribution_carte_joueurs(Jeu *jeu);

/**
 * @details Fonction qui distribue les cartes du joueur en paramètre.
 * @param jeu
 * @param j
 */
void distribution_carte_joueur(Jeu *jeu, Joueur *j);



/**
 * @details Fonction qui affiche le nombre de cartes que l'utilisateur peut encore poser
 * @param jeu
 * @param idJoueur
 * @return unsigned short
 */
unsigned short get_nb_carte_utilisable_joueur(Jeu *jeu, short idJoueur);



/**
 * @brief Retourne sous forme de chaine de charactère les cartes du joueur en paramètre.
 * @param joueur
 * @return La liste des cartes du Joueur joueur.
 */
char *affiche_cartes_joueur(Joueur *joueur);


/**
 * @details Retourne le numéro de la dernière carte de la ligne mise en paramètre.
 * @param jeu
 * @param ligne
 * @return unsigned short
 */
unsigned char get_dernier_carte_ligne(Jeu *jeu, int ligne);


/**
 * @details Retourne la position de la dernière carte de la ligne mise en paramètre.
 * @param jeu
 * @param ligne
 * @return unsigned char
 */
unsigned char get_pos_carte_derniere_ligne(Jeu *jeu, int ligne);

/**
 * @details Retourne la position de la carte qui est le plus proche du numéro en paramètre
 * @param jeu
 * @param numero
 * @return unsigned char
 */
char get_pos_carte_mini(Jeu *jeu, int numero);


/**
 * @details Ajoute la carte en paramètre au plateau de jeu, si c'est possible l'ajoute et retourne 1, sinon retourne -1
 * si la carte est trop petite, 0 si la carte est ajoutée a la dernière colonne d'une ligne
 * @param jeu
 * @param carte
 * @return 1 succès, -1 pas possible, 0 dernière case d'une ligne.
 */
char ajoute_carte_au_plateau(Jeu *jeu, Carte *carte);


/**
 * @details Remplie la premiere colonne du plateau par des cartes au hasard.
 * @param jeu
 */
void creation_premiere_colonne_plateau(Jeu *jeu);

/**
 * @details Renvoie un tableau de joueurs classé par leur carte jouer en ordre croissant.
 * @param jeu
 * @return Joueur
 */
Joueur **get_ordre_joueur_tour(Jeu *jeu);

/**
 * @details Supprime toutes les cartes de la ligne en paramètre, ajoute la carte à la premiere
 * colonne de la ligne en paramètre et ajoute les pénalités de la ligne au joueur en paramètre.
 * @param jeu
 * @param ligne
 * @param j
 */
void place_carte_si_trop_petite_ou_derniere_ligne(Jeu *jeu, int ligne, Joueur *j);

/**
 * @details Fonction qui affiche le nombre de têtes des joueurs
 * @param jeu
 * @return char*
 */
void affiche_nb_tete_joueurs(Jeu *jeu,char * buffer);




void init_jeu(Jeu *jeu) {

    jeu->plateau = cree_plateau(); // Création du plateau de carte 4*6

    //Creation des 104 cartes avec numéro de tête random
    for (int i = 0; i < 104; i++) jeu->liste_carte[i] = create_carte(i + 1);

    //SI c'est la premiere partie, on initialise le nombre de défaites à 0.
    if (nb_partie == 0) for (int i = 0; i < nb_joueur; i++) jeu->joueur[i]->nb_defaite = 0;

    //Nombre de têtes à 0 vu que le jeu commence et si jeu pas terminé on ne remet pas les têtes à 0.
    if (isOver == 0) {
        for (int i = 0; i < nb_joueur; i++) jeu->joueur[i]->nb_penalite = 0;
    }

    //On initialise le plateau à 0.
    Carte carte_0 = {0, 0, 0, 0};
    for (int i = 0; i < 4; i++) for (int j = 0; j < 6; j++) jeu->plateau[i][j] = carte_0;

    //Carte de la premiere colonne du plateau distribué
    creation_premiere_colonne_plateau(jeu);
    distribution_carte_joueurs(jeu);
//    affiche_plateau(jeu);
}

void free_jeu(Jeu *jeu) {
    for (int i = 0; i < 6; i++) free(jeu->plateau[i]);
    free(jeu->plateau);
    for (int i = 0; i < 104; i++) free(jeu->liste_carte[i]);
}

Carte **cree_plateau() {
    Carte **plateau = (Carte **) malloc(6 * sizeof(Carte *));
    for (int i = 0; i < 6; i++) plateau[i] = (Carte *) malloc(6 * sizeof(Carte));
    return plateau;
}

void creation_premiere_colonne_plateau(Jeu *jeu) {
    Carte *cartes[4];

    for (int i = 0; i < 4; ++i) cartes[i] = get_carte_liste(jeu);

    for (int j = 1; j <= 4; j++) {
        for (int i = 0; i < 3; i++) {
            if (cartes[i]->numero > cartes[i + 1]->numero) {
                Carte *c = cartes[i];
                cartes[i] = cartes[i + 1];
                cartes[i + 1] = c;
            }
        }
    }

    for (int i = 0; i < 4; i++) jeu->plateau[i][0] = *cartes[i];

}

Joueur **get_ordre_joueur_tour(Jeu *jeu) {
    Joueur **joueurs = (Joueur **) malloc(nb_joueur * sizeof(Joueur *));

    for (int i = 0; i < nb_joueur; ++i) joueurs[i] = jeu->joueur[i];

    for (int j = 1; j <= nb_joueur; j++) {
        for (int i = 0; i < nb_joueur - 1; i++) {
            if (joueurs[i]->carte_choisie->numero > joueurs[i + 1]->carte_choisie->numero) {
                Joueur *jo = joueurs[i];
                joueurs[i] = joueurs[i + 1];
                joueurs[i + 1] = jo;
            }
        }
    }

    return joueurs;
}

char *affiche_plateau(Jeu *jeu) {
    char *res = (char *) calloc(1024, sizeof(char));

    snprintf(res, 1024, BOLD_HIGH_WHITE"\t\t\t\t\t\tPLATEAU:\n"RESET);

    for (int i = 0; i < 4; i++) {
        snprintf(res + strlen(res), 1024, "Ligne %u\t", i + 1);
        for (int j = 0; j < 6; j++)
            snprintf(res + strlen(res), 1024, BOLD_HIGH_WHITE"\t[%03d-%d]\t"RESET, jeu->plateau[i][j].numero,
                     jeu->plateau[i][j].Tete);
        snprintf(res + strlen(res), 1024, "\n");
    }
    return res;
}

unsigned char get_pos_carte_derniere_ligne(Jeu *jeu, int ligne) {
    for (int i = 0; i < 6; ++i) {
        if (jeu->plateau[ligne][i].numero == 0) {
            return i - 1;
        }
    }
    return 0;
}

char ajoute_carte_au_plateau(Jeu *jeu, Carte *carte) {
    char pos = get_pos_carte_mini(jeu, carte->numero);
    if (pos == -1)
        return -1;
    if (pos % 6 == 4) {
        return 0;
    } else {
        jeu->plateau[pos / 6][(pos % 6) + 1] = *carte;
        return 1;
    }
}

void place_carte_si_trop_petite_ou_derniere_ligne(Jeu *jeu, int ligne, Joueur *j) {
    int penalite = 0;
    for (int i = 0; i < 6; ++i) {
        if (jeu->plateau[ligne][i].numero == 0)
            break;
        penalite += jeu->plateau[ligne][i].Tete;
    }
    for (int i = 0; i < 6; ++i) {
        if (jeu->plateau[ligne][i].numero == 0)
            break;
        Carte c = {0, 0, 0, 0};
        jeu->plateau[ligne][i] = c;
    }
    jeu->plateau[ligne][0] = *j->carte_choisie;
    j->nb_penalite += penalite;
}

char get_pos_carte_mini(Jeu *jeu, int numero) {
    short diff = numero - get_dernier_carte_ligne(jeu, 0);
    char pos = -1;
    if (diff > 0)
        pos = get_pos_carte_derniere_ligne(jeu, 0);
    for (int i = 1; i < 4; ++i) {
        if ((numero - get_dernier_carte_ligne(jeu, i)) >= 0) {
            if ((numero - get_dernier_carte_ligne(jeu, i)) < diff) {
                diff = numero - get_dernier_carte_ligne(jeu, i);
                pos = (i * 6) + get_pos_carte_derniere_ligne(jeu, i);
            }
        }
    }
    return pos;
}

unsigned char get_dernier_carte_ligne(Jeu *jeu, int ligne) {
    for (int i = 0; i < 6; ++i) {
        if (jeu->plateau[ligne][i].numero == 0) {
            return jeu->plateau[ligne][i - 1].numero;
        }
    }
    return 0;
}

void print_tableau(Jeu *jeu) {
    fprintf(fichier_log, "\t\t\t\t\t\t\t\tPLATEAU:\n");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 6; j++)
            fprintf(fichier_log, "\t[%03d-%d]\t", jeu->plateau[i][j].numero, jeu->plateau[i][j].Tete);
        fprintf(fichier_log, "\n");
    }
    fprintf(fichier_log, "\n");
}

Carte *create_carte(unsigned short i) {
    Carte *c = (Carte *) malloc(sizeof(Carte));
    c->numero = i;
    c->is_picked = 0;
    c->is_used = 0;
    c->Tete = (rand() % 7) + 1;
    return c;
}

Carte *get_carte_liste(Jeu *jeu) {
    Carte *tmp[104];
    unsigned char cpt = 0;
    //Parcours de la liste pour trouver les cartes pas encore prise
    for (int i = 0; i < 104; ++i) {
        if (jeu->liste_carte[i]->is_picked == 0) {
            tmp[cpt] = jeu->liste_carte[i];
            cpt++;
        }
    }

    Carte *c = tmp[rand() % cpt];
    c->is_picked = 1;
    return c;
}

char *affiche_cartes_joueur(Joueur *joueur) {
    char *res = malloc(1024 * sizeof(char));

    for (int i = 0; i < 10; i++) {
        if (joueur->carte[i]->is_used == 0)
            snprintf(res + strlen(res), 1024, "Carte %02d > Numéro[%03d] Tete[%d]\n", i + 1, joueur->carte[i]->numero,
                     joueur->carte[i]->Tete);
    }
    return res;
}


void distribution_carte_joueurs(Jeu *jeu) {
    for (int i = 0; i < nb_joueur; i++)
        distribution_carte_joueur(jeu, jeu->joueur[i]);
}

char *recap_regle() {

    char *tmp = malloc(255 * sizeof(char));

    snprintf(tmp, 1024, BOLD_HIGH_WHITE"\t\nINFO DU JEU:\n"RESET);
    snprintf(tmp + strlen(tmp), 1024, "Le nombre de têtes maximal est de ");
    snprintf(tmp + strlen(tmp), 1024, BOLD_MAGENTA"%d"RESET, nb_tete_max);
    snprintf(tmp + strlen(tmp), 1024, "\nLe nombre de tours maximal est de ");
    snprintf(tmp + strlen(tmp), 1024, BOLD_MAGENTA"%d\n"RESET, nb_manche_max);
    snprintf(tmp + strlen(tmp), 1024, "Nombre de joueurs total bot confondu : ");
    snprintf(tmp + strlen(tmp), 1024, BOLD_MAGENTA"%d\n"RESET, nb_joueur);
    snprintf(tmp + strlen(tmp), 1024, "Nombre de bots: ");
    snprintf(tmp + strlen(tmp), 1024, BOLD_MAGENTA"%d\n"RESET, nb_bot);

    char *res = malloc(strlen(tmp) * sizeof(char));
    strcpy(res, tmp);
    free(tmp);

    return res;
}


void distribution_carte_joueur(Jeu *jeu, Joueur *j) {
    for (int i = 0; i < 10; i++)
        j->carte[i] = get_carte_liste(jeu);
}

unsigned short get_nb_carte_utilisable_joueur(Jeu *jeu, short idJoueur) {
    unsigned short cpt = 0;
    for (int i = 0; i < 10; i++) if (jeu->joueur[idJoueur]->carte[i]->is_used == 0) cpt++;
    return cpt;
}

char *min_max_defaite(Jeu *jeu) {
    unsigned short min = jeu->joueur[0]->nb_defaite, max = jeu->joueur[0]->nb_defaite;
    unsigned short imin = 0, imax = 0;

    for (int i = 0; i < nb_joueur; i++) {
        if (jeu->joueur[i]->nb_defaite < min) {
            min = jeu->joueur[i]->nb_defaite;
            imin = i;
        }
        if (jeu->joueur[i]->nb_defaite > max) {
            max = jeu->joueur[i]->nb_defaite;
            imax = i;
        }
    }

    char *tmp = malloc(200 * sizeof(char));

    //DANS TERMINAL
    snprintf(tmp + strlen(tmp), 1024, "Le joueur ayant obtenue le moins de défaite est le joueur %d avec %d défaites\n",
             imin, min);
    snprintf(tmp + strlen(tmp), 1024, "Le joueur ayant obtenue le plus de défaite est le joueur %d avec %d défaites\n",
             imax,
             max);

    char *res = malloc(strlen(tmp) * sizeof(char));
    strcpy(res, tmp);
    free(tmp);
    return res;
}

void affiche_nb_tete_joueurs(Jeu *jeu,char * buffer)
{
    char tmp[1024];
    for (short i = 0; i < nb_joueur; i++)
        snprintf(tmp + strlen(tmp), 512, "Le joueur %s possède %d têtes \n", jeu->joueur[i]->pseudo,
                 jeu->joueur[i]->nb_penalite);

    fprintf(fichier_log, "\n");

    strcpy(buffer, tmp);
}


int moyenne_des_tetes(Jeu *jeu) {
    int somme = 0;
    for (int i = 0; i < nb_joueur; i++) somme += jeu->joueur[i]->nb_penalite;
    return somme / nb_joueur;
}

int AskNombreUser(int min, int max) {
    char str[16] = "";
    scanf("%s", str);
    if (strlen(str) > 16) str[0] = '\0'; //Gestion cas ou nombre est trop grand pour pas crash le programme

    //SI utilisateur veut arreter le jeu en appuyant sur x
    if (strcmp(str, "x") == 0 || strcmp(str, "X") == 0) {
        printf(BOLD_RED"ARRET DU JEU...\n"RESET);
        force_fin_jeu();
    }
    int nombre = atoi(str);//Conversion de la chaine de caractère en nombre entier pour savoir le nombre entré
    while (nombre < min || nombre > max) {
        printf(BOLD_RED"La valeur doit être comprise entre [%d-%d], vous avez entré : %d\n>"RESET, min, max, nombre);
        scanf("%s", str);
        if (strlen(str) > 16) str[0] = '\0';
        if (strcmp(str, "x") == 0 || strcmp(str, "X") == 0) {
            printf(BOLD_RED"ARRET DU JEU...\n"RESET);
            force_fin_jeu();
        }
        nombre = atoi(str);
        fflush(stdin);
        fflush(stdout);
    }

    fflush(stdin);
    fflush(stdout);
    return nombre;
}

void force_fin_jeu() {
    printf(RESET);
    isOver = 1;
    fprintf(fichier_log, "\n***LE JEU A ETE ARRÊTÉ***\n");
    fclose(fichier_log);
    exit(EXIT_SUCCESS);
}

char *statistique(Jeu *jeu)
{

    char *tmp = malloc(1024 * sizeof(char));

    //DANS TERMINAL DES JOUEURS
    snprintf(tmp + strlen(tmp), 1024, BOLD_CYAN"\n\t[STATISTIQUES]\n"RESET);
    snprintf(tmp + strlen(tmp), 1024, "\nNombre de parties joué : %d\n", nb_partie + 1);
    snprintf(tmp + strlen(tmp), 1024, "Nombre de tours effectué : %d\n", tour);

    snprintf(tmp + strlen(tmp), 1024, "Moyenne des têtes obtenues : %d\n", moyenne_des_tetes(jeu));

    char tete[1024];
    affiche_nb_tete_joueurs(jeu,tete);
    snprintf(tmp + strlen(tmp), 1024, "%s", tete);

    char *def = min_max_defaite(jeu);
    snprintf(tmp + strlen(tmp), 1024, "%s", def); //Toujours en fin de stats

    char *res = malloc(strlen(tmp) * sizeof(char));
    strcpy(res, tmp);

    free(tete);
    free(def);
    free(tmp);
    return res;
}



#endif //SYSTEMES_ET_RESEAUX_PROJET_JEU_H
