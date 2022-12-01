//
// Created by rahman on 21/11/22.
//

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
    unsigned short Numero, Tete, isPicked, isUsed;
} Carte;

typedef struct Joueur {
    unsigned short nb_penalite, nb_defaite;
    Carte *carte[10];
    Carte *carte_choisie;
} Joueur;


typedef struct Jeu {
    Carte **plateau;
    Joueur *joueur[MAX_JOUEURS];
    Carte *liste_carte[104];
} Jeu;

unsigned int isOver = 0, tour = 1, nb_Joueur = 0, nb_Partie = 0, nb_TeteMax = 66, nb_MancheMax = 999;
unsigned short nextPartie = 0;
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
void freeJeu(Jeu *jeu);


/**
 * @details Fonction qui permet d'afficher les règles du jeu à l'écran quand le jeu démarre
 * @param jeu
 * @return char*
 */
char *RecapRegle(Jeu jeu);


/**
 * @details Fonction pour afficher les statistiques à l'écran
 * @param jeu
 * @return char*
 */
char *Statistique(Jeu jeu);

/**
 * @details Fonction qui fait la moyenne des têtes
 * @param jeu
 * @return int
 */
int MoyenneDesTetes(Jeu jeu);

/**
 * @details Fonction qui affiche quel joueur a le plus et qui a le moins de défaite
 * @param jeu
 * @return char*
 */
char *MinEtMaxDefaite(Jeu jeu);

/**
 * @details Fonction pour afficher le tableau dans un fichier log
 * @param jeu
 */
void PrintTableau(Jeu jeu);

/**
 * @details Fonction qui force la fin du jeu si l'utilisateur appuie sur 'x'
 */
void ForceFinDuJeu();

//POUR CARTE

/**
 * @details Fonction qui permet de créer une carte avec en paramètre son numéro
 * @param i
 * @return Carte
 */
Carte *create_carte(unsigned short i);

/**
 * @details Fonction qui retourne une carte de la liste non utilisé et distribué.
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
 * @details Fonction qui distribue les cartes du joueur en parametre.
 * @param jeu
 * @param j
 */
void distribution_carte_joueur(Jeu *jeu, Joueur *j);

/**
 * @details Fonction qui donne la position de la ligne où sera la dernière carte
 * @param jeu
 * @param numeroCarte
 * @return unsigned short
 */
unsigned short getLastCarteDeLaLigne(Jeu jeu, int numeroCarte);

/**
 * @details Fonction qui donne le rang i de là où doit être posé la carte
 * @param jeu
 * @param colonne
 * @return unsigned short
 */
unsigned short getLastPositionDeLaLigne(Jeu jeu, int colonne);

/**
 * @details Fonction qui affiche le nombre de têtes des joueurs
 * @param jeu
 * @return char*
 */
char *AfficheNbTeteJoueurs(Jeu jeu);

/**
 * @details Fonction qui affiche le nombre de cartes que l'utilisateur peut encore poser
 * @param jeu
 * @param idJoueur
 * @return unsigned short
 */
unsigned short getNbCarteUtilisableDuJoueur(Jeu jeu, short idJoueur);

/**
 * @details Fonction qui affiche le nombre de cartes restantes dans la liste de carte
 * @param jeu
 * @return unsigned short
 */
unsigned short getCarteRestante(Jeu jeu);

/**
 * @details Fonction qui retourne la carte la plus petite du plateau
 * @param jeu
 * @return unsigned short
 */
unsigned short getCartePlusPetiteDuPlateau(Jeu jeu);

/**
 * @brief Retourne sous forme de chaine de charactere les cartes du joueur en parametre.
 * @param joueur
 * @return La liste des cartes du Joueur joueur.
 */
char *affiche_cartes_joueur(Joueur *joueur);


/**
 * @details Retourne le numero de la dernière carte de la ligne mise en parametre.
 * @param jeu
 * @param ligne
 * @return unsigned short
 */
unsigned char get_dernier_carte_ligne(Jeu *jeu, int ligne);


/**
 * @details Retourne la position de la derniere carte de la ligne mise en parametre.
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
 * @return 1 succes, -1 pas possible, 0 derniere case d'une ligne.
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
 * @details Supprime toutes les cartes de la ligne en parametre, ajoute la carte a la premiere
 * colonne de la ligne en parametre, et ajoute les pénalités de la ligne au joueur en parametre.
 * @param jeu
 * @param ligne
 * @param j
 */
void place_carte_si_trop_petite_ou_derniere_ligne(Jeu *jeu, int ligne, Joueur *j);

void freeJeu(Jeu *jeu)
{
    if(jeu->plateau == NULL)
    {
        for (int i = 0; i < 6; i++) {
            free(jeu->plateau[i]);
        }
        free(jeu->plateau);
    }

}

Carte **cree_plateau() {
    Carte **plateau = (Carte **) malloc(6 * sizeof(Carte *));
    for (int i = 0; i < 6; i++) plateau[i] = (Carte *) malloc(6 * sizeof(Carte));
    return plateau;
}

void creation_premiere_colonne_plateau(Jeu *jeu) {

    Carte *cartes[4];

    for (int i = 0; i < 4; ++i)
        cartes[i] = get_carte_liste(jeu);


    for (int j = 1; j <= 4; j++) {
        for (int i = 0; i < 3; i++) {
            if (cartes[i]->Numero > cartes[i + 1]->Numero) {
                Carte *c = cartes[i];
                cartes[i] = cartes[i + 1];
                cartes[i + 1] = c;
            }
        }
    }

    for (int i = 0; i < 4; i++)
        jeu->plateau[i][0] = *cartes[i];


}

Joueur **get_ordre_joueur_tour(Jeu *jeu) {
    Joueur **joueurs = (Joueur **) malloc(nb_Joueur * sizeof(Joueur *));

    for (int i = 0; i < nb_Joueur; ++i)
        joueurs[i] = jeu->joueur[i];

    for (int j = 1; j <= nb_Joueur; j++) {
        for (int i = 0; i < nb_Joueur - 1; i++) {
            if (joueurs[i]->carte_choisie->Numero > joueurs[i + 1]->carte_choisie->Numero) {
                Joueur *jo = joueurs[i];
                joueurs[i] = joueurs[i + 1];
                joueurs[i + 1] = jo;
            }
        }
    }

    return joueurs;
}

char *affiche_plateau(Jeu *jeu) {
    char *res = (char *) malloc(1024 * sizeof(char));

    snprintf(res, 1024, BOLD_HIGH_WHITE"\t\t\t\t\t\tPLATEAU:\n"RESET);

    for (int i = 0; i < 4; i++) {
        snprintf(res + strlen(res), 1024, "Ligne %u\t", i + 1);
        for (int j = 0; j < 6; j++)
            snprintf(res + strlen(res), 1024, BOLD_HIGH_WHITE"\t[%03d-%d]\t"RESET, jeu->plateau[i][j].Numero,
                     jeu->plateau[i][j].Tete);
        snprintf(res + strlen(res), 1024, "\n");
    }
    return res;
}

unsigned char get_pos_carte_derniere_ligne(Jeu *jeu, int ligne) {
    for (int i = 0; i < 6; ++i) {
        if (jeu->plateau[ligne][i].Numero == 0) {
            return i - 1;
        }
    }
    return 0;
}

char ajoute_carte_au_plateau(Jeu *jeu, Carte *carte) {
    char pos = get_pos_carte_mini(jeu, carte->Numero);
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
        if (jeu->plateau[ligne][i].Numero == 0)
            break;
        penalite += jeu->plateau[ligne][i].Tete;
    }
    for (int i = 0; i < 6; ++i) {
        if (jeu->plateau[ligne][i].Numero == 0)
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



//unsigned short getCartePlusPetiteDuPlateau(Jeu jeu) {
//    unsigned short min = 104; //Min théorique dans la liste de carte
//    for (int i = 0; i < 4; i++) {
//        for (int j = 0; j < 6; j++) {
//            if (jeu.plateau[i][j].Numero == 0)
//                continue; //Saute la carte quand elle vaut 0 car ce n'est pas une carte
//            else if (jeu.plateau[i][j].Numero < min) min = jeu.plateau[i][j].Numero;
//        }
//    }
//    return min; // Retourne le numero de carte le plus petit du plateau
//}

unsigned char get_dernier_carte_ligne(Jeu *jeu, int ligne) {
    for (int i = 0; i < 6; ++i) {
        if (jeu->plateau[ligne][i].Numero == 0) {
            return jeu->plateau[ligne][i - 1].Numero;
        }
    }
    return 0;
}

void PrintTableau(Jeu jeu) {
    fprintf(fichier_log, "\t\t\t\t\t\t\t\tPLATEAU:\n");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 6; j++)
            fprintf(fichier_log, "\t[%03d-%d]\t", jeu.plateau[i][j].Numero, jeu.plateau[i][j].Tete);
        fprintf(fichier_log, "\n");
    }
    fprintf(fichier_log, "\n");
}

Carte *create_carte(unsigned short i) {
    Carte *c = (Carte *) malloc(sizeof(Carte));
    c->Numero = i;
    c->isPicked = 0;
    c->isUsed = 0;
    c->Tete = (rand() % 7) + 1;
    return c;
}

Carte *get_carte_liste(Jeu *jeu) {
    Carte *tmp[104];
    int cpt = 0;
    //Parcours de la liste pour trouver les cartes pas encore prise
    for (int i = 0; i < 104; ++i) {
        if (jeu->liste_carte[i]->isPicked == 0) {
            tmp[cpt] = jeu->liste_carte[i];
            cpt++;
        }
    }


    Carte *c = tmp[rand() % cpt];
    c->isPicked = 1;
    return c;
}

char *affiche_cartes_joueur(Joueur *joueur) {
    char *res = malloc(1024 * sizeof(char));

    for (int i = 0; i < 10; i++) {
        if (joueur->carte[i]->isUsed == 0)
            snprintf(res + strlen(res), 1024, "Carte %02d > Numéro[%03d] Tete[%d]\n", i + 1, joueur->carte[i]->Numero,
                     joueur->carte[i]->Tete);
    }
    return res;
}

unsigned short getCarteRestante(Jeu jeu) {
    unsigned short cpt = 0;
    for (int i = 0; i < 104; i++) if (jeu.liste_carte[i]->isPicked == 0) cpt++;
    return cpt;
}

void distribution_carte_joueurs(Jeu *jeu) {
    for (int i = 0; i < nb_Joueur; i++)
        distribution_carte_joueur(jeu, jeu->joueur[i]);
}
//
//unsigned short getLastCarteDeLaLigne(Jeu jeu, int numeroCarte) {
//    int tabLastNumber[4];
//    for (int i = 0; i < 4; i++) {
//        for (int j = 0; j < 6; j++) {
//            //Recupère dernier nombre des lignes du plateau
//            if (jeu.plateau[i][j].Numero != 0) {
//                tabLastNumber[i] = jeu.plateau[i][j].Numero;
//            } else break;
//        }
//    }
//    int min = 104; // La valeur maximale d'une carte est par défaut le minimum
//    int position = 0;
//    //Calcul la différence d'écart avec ces nombres
//    for (int i = 0; i < 4; i++) {
//        tabLastNumber[i] = abs(tabLastNumber[i] - numeroCarte);
//    }
//    //On cherche le minimum du tableau qui sera là où on pose la carte
//    for (int i = 0; i < 4; i++) {
//        if (tabLastNumber[i] < min) {
//            min = tabLastNumber[i];
//            position = i;
//        }
//    }
//    return position; //Retourne la position de la colonne au rang n-1
//}

//unsigned short getLastPositionDeLaLigne(Jeu jeu, int colonne) {
//    unsigned short lastPosition = 0;
//    for (short i = 0; i < 6; i++) {
//        if (jeu.plateau[colonne][i].Numero == 0) {
//            lastPosition = i;
//            break;
//        }
//    }
//    return lastPosition;
//}


char *Statistique(Jeu jeu) {

    char *tmp = malloc(1024 * sizeof(char));

    //DANS TERMINAL DES JOUEURS
    snprintf(tmp + strlen(tmp), 1024, BOLD_CYAN"\n\t[STATISTIQUES]\n"RESET);
    snprintf(tmp + strlen(tmp), 1024, "\nNombre de parties joué : %d\n", nb_Partie);
    snprintf(tmp + strlen(tmp), 1024, "Nombre de tours effectué : %d\n", tour);
    snprintf(tmp + strlen(tmp), 1024, "Moyenne des têtes obtenues : %d\n", MoyenneDesTetes(jeu));
    snprintf(tmp + strlen(tmp), 1024, "%s", AfficheNbTeteJoueurs(jeu));


    //DANS FICHIER
    fprintf(fichier_log, "\n\t[STATISTIQUES]\n");
    fprintf(fichier_log, "\nNombre de parties joué : %d\n", nb_Partie);
    fprintf(fichier_log, "Nombre de tours effectué : %d\n", tour);
    fprintf(fichier_log, "Moyenne des têtes obtenues : %d\n", MoyenneDesTetes(jeu));
    fprintf(fichier_log, "%s\n", AfficheNbTeteJoueurs(jeu));
    snprintf(tmp + strlen(tmp), 1024, "%s", MinEtMaxDefaite(jeu));


    char *res = malloc(strlen(tmp) * sizeof(char));
    strcpy(res, tmp);
    free(tmp);
    return res;
}

char *RecapRegle(Jeu jeu) {

    char *tmp = malloc(255 * sizeof(char));

    snprintf(tmp, 1024, BOLD_HIGH_WHITE"\t\nINFO DU JEU:\n"RESET);
    snprintf(tmp + strlen(tmp), 1024, "Le nombre de têtes maximal est de ");
    snprintf(tmp + strlen(tmp), 1024, BOLD_MAGENTA"%d"RESET, nb_TeteMax);
    snprintf(tmp + strlen(tmp), 1024, "\nLe nombre de tours maximal est de ");
    snprintf(tmp + strlen(tmp), 1024, BOLD_MAGENTA"%d\n"RESET, nb_MancheMax);
    snprintf(tmp + strlen(tmp), 1024, "Nombre de joueurs: ");
    snprintf(tmp + strlen(tmp), 1024, BOLD_MAGENTA"%d\n"RESET, nb_Joueur);

    char *res = malloc(strlen(tmp) * sizeof(char));
    strcpy(res, tmp);
    free(tmp);

    return res;
}

char *AfficheNbTeteJoueurs(Jeu jeu) {
    char *tmp = malloc(60 * nb_Joueur * sizeof(char));
    for (short i = 0; i < nb_Joueur; i++) {
        snprintf(tmp + strlen(tmp), 1024, "Le joueur %d possède %d têtes \n", i,
                 jeu.joueur[i]->nb_penalite);
        fprintf(fichier_log, "ROUND [%d] > Le joueur %d possède %d têtes\n", tour, i, jeu.joueur[i]->nb_penalite);
    }
    fprintf(fichier_log, "\n");

    char *res = malloc(strlen(tmp) * sizeof(char));
    strcpy(res, tmp);
    free(tmp);
    return res;
}

void distribution_carte_joueur(Jeu *jeu, Joueur *j) {
    for (int i = 0; i < 10; i++)
        j->carte[i] = get_carte_liste(jeu);
}

unsigned short getNbCarteUtilisableDuJoueur(Jeu jeu, short idJoueur) {
    unsigned short cpt = 0;
    for (int i = 0; i < 10; i++) if (jeu.joueur[idJoueur]->carte[i]->isUsed == 0) cpt++;
    return cpt;
}

char *MinEtMaxDefaite(Jeu jeu) {
    unsigned short min = jeu.joueur[0]->nb_defaite, max = jeu.joueur[0]->nb_defaite;
    unsigned short imin = 0, imax = 0;

    for (int i = 0; i < nb_Joueur; i++) {
        if (jeu.joueur[i]->nb_defaite < min) {
            min = jeu.joueur[i]->nb_defaite;
            imin = i;
        }
        if (jeu.joueur[i]->nb_defaite > max) {
            max = jeu.joueur[i]->nb_defaite;
            imax = i;
        }
    }

    char *tmp = malloc(200 * sizeof(char));

    //DANS TERMINAL
    snprintf(tmp, 1024, "Le joueur ayant obtenue le moins de défaite est le joueur %d avec %d défaites\n", imin, min);
    snprintf(tmp + strlen(tmp), 1024, "Le joueur ayant obtenue le plus de défaite est le joueur %d avec %d défaites\n",
             imax,
             max);

    //DANS FICHIER
    fprintf(fichier_log, "Le joueur ayant obtenu le moins de défaite est le joueur %d avec %d défaites\n", imin,
            min);
    fprintf(fichier_log, "Le joueur ayant obtenu le plus de défaite est le joueur %d avec %d défaites\n", imax,
            max);

    printf("Le joueur ayant obtenue le moins de défaite est le joueur %d avec %d défaites\n", imin, min);
    printf("Le joueur ayant obtenu le plus de défaite est le joueur %d avec %d défaites\n", imax, max);

    char *res = malloc(strlen(tmp) * sizeof(char));
    strcpy(res, tmp);
    free(tmp);
    return res;
}

int MoyenneDesTetes(Jeu jeu) {
    int somme = 0;
    for (int i = 0; i < nb_Joueur; i++) somme += jeu.joueur[i]->nb_penalite;
    return somme / nb_Joueur;
}

int AskNombreUser(int min, int max) {
    char str[16] = "";
    scanf("%s", str);
    if (strlen(str) > 16) str[0] = '\0'; //Gestion cas ou nombre est trop grand pour pas crash le programme

    //SI utilisateur veut arreter le jeu en appuyant sur x
    if (strcmp(str, "x") == 0 || strcmp(str, "X") == 0) {
        printf(BOLD_RED"ARRET DU JEU...\n"RESET);
        ForceFinDuJeu();
    }
    int nombre = atoi(str);//Conversion de la chaine de caractère en nombre entier pour savoir le nombre entré
    while (nombre < min || nombre > max) {
        printf(BOLD_RED"La valeur doit être comprise entre [%d-%d], vous avez entré : %d\n>"RESET, min, max, nombre);
        scanf("%s", str);
        if (strlen(str) > 16) str[0] = '\0';
        if (strcmp(str, "x") == 0 || strcmp(str, "X") == 0) {
            printf(BOLD_RED"ARRET DU JEU...\n"RESET);
            ForceFinDuJeu();
        }
        nombre = atoi(str);
        fflush(stdin);
        fflush(stdout);
    }

    fflush(stdin);
    fflush(stdout);
    return nombre;
}

void ForceFinDuJeu() {
    printf(RESET);
    isOver = 1;
    nextPartie = 1;
    fprintf(fichier_log, "\n***LE JEU A ETE ARRETE***\n");
    fclose(fichier_log);
    exit(EXIT_SUCCESS);
}


/**
 * @details Fonction pour afficher le temps de jeu aux joueurs et dans le fichier log
 * @param duree
 */
void AfficheTempsJeu(double duree);


/**
 * @details Fonction utilisée par le serveur pour changer les réglages du jeu
 */
void ChangeLimiteJeu();



#endif //SYSTEMES_ET_RESEAUX_PROJET_JEU_H
