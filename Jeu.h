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


typedef struct Carte
{
    unsigned short Numero, Tete, isPicked, isUsed;
} Carte;

typedef struct Joueur
{
    unsigned short NB_TETE, NB_DEFAITE;
    Carte carte[10];
    Carte *carte_choisie;
} Joueur;


typedef struct Jeu
{
    Carte **plateau;
    Joueur *joueur[MAX_JOUEURS];
    Carte listeCarte[104];
} Jeu;

unsigned int isOver = 0, tour = 1, nb_Joueur = 0, nb_Partie = 0, nb_TeteMax = 10, nb_MancheMax = 900;
unsigned short nextPartie = 0;
struct timeval begin, end; // Pour calculer la durée d'une partie
double duree_total = 0;

FILE *fichier_log;
char date[40];
char nom_fichier[256];

/**
 * @details Fonction qui permet d'initialiser le jeu
 * @param jeu
 */
void initJeu(Jeu *jeu);

/**
 * @details Fonction pour créer un plateau de carte
 * @return Un tableau de carte représentant le plateau
 */
Carte **creePlateau();


/**
 * @details Fonction pour afficher le plateau ainsi que les cartes présentent dessus
 * @param jeu
 * @return char*
 */
char *affichePlateau(Jeu jeu);

/**
 * @details Fonction pour libérer de la mémoire la matrice de carte
 * @param jeu
 */
void freeJeu(Jeu jeu);




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
 * @details Fonction pour relancer le jeu une fois qu'il est terminé. Elle permet de réinitialiser le jeu en gardant la progression
 * @param jeu
 */
void resetJeu(Jeu *jeu);

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
Carte CreateCarte(unsigned short i);

/**
 * @details Fonction qui retourne le numéro d'une carte de la liste non utilisé
 * @param jeu
 * @return int
 */
int getCarteDeLaListe(Jeu *jeu);


/**
 * @details Fonction qui distribue 10 des cartes aux joueurs
 * @param jeu
 */
void DistributionCarteAuxJoueurs(Jeu *jeu);

/**
 * @details Fonction qui distribue 10 à un joueur
 * @param jeu
 * @param idJoueur
 */
void DistributionCarteAuJoueur(Jeu *jeu, short idJoueur);

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
char *affiche_joueur_cartes(Joueur *joueur);


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
unsigned char get_pos_carte_mini(Jeu *jeu, int numero);


/**
 * @details Ajoute la carte en paramètre au plateau de jeu, si c'est possible l'ajoute et retourne 1, sinon retourne -1
 * si la carte est trop petite, 0 si la carte est ajoutée a la dernière colonne d'une ligne
 * @param jeu
 * @param carte
 * @return 1 succes, -1 pas possible, 0 derniere case d'une ligne.
 */
char ajoute_carte_au_plateau(Jeu *jeu, Carte *carte);




void initJeu(Jeu *jeu)
{

    jeu->plateau = creePlateau(); // Création du plateau de carte 4*6

    //Creation des 104 cartes avec numéro de tête random
    for (int i = 0; i < 104; i++) jeu->listeCarte[i] = CreateCarte(i + 1);

    //SI c'est la premiere partie, on initialise le nombre de défaites à 0.
    if (nb_Partie == 0) for (int i = 0; i < nb_Joueur; i++) jeu->joueur[i]->NB_DEFAITE = 0;

    //Nombre de têtes à 0 vu que le jeu commence
    for (int i = 0; i < nb_Joueur; i++) jeu->joueur[i]->NB_TETE = 0;

    //On initialise le plateau à 0.
    Carte carte_0 = {0, 0, 0, 0};
    for (int i = 0; i < 4; i++) for (int j = 0; j < 6; j++) jeu->plateau[i][j] = carte_0;

    //Carte de la premiere ligne du plateau distribué
    for (int i = 0; i < 4; i++)
    {
        int rd = getCarteDeLaListe(jeu);
        jeu->listeCarte[rd].isPicked = 1;
        jeu->plateau[i][0] = jeu->listeCarte[rd];
    }
    DistributionCarteAuxJoueurs(jeu);
}


void resetJeu(Jeu *jeu)
{
    isOver = 0;
    tour = 1;
    initJeu(jeu);
}

void freeJeu(Jeu jeu)
{
    for (int i = 0; i < 6; i++) free(jeu.plateau[i]);
    free(jeu.plateau);
}

Carte **creePlateau()
{
    Carte **plateau = (Carte **) malloc(6 * sizeof(Carte *));
    for (int i = 0; i < 6; i++) plateau[i] = (Carte *) malloc(6 * sizeof(Carte));
    return plateau;
}

char *affichePlateau(Jeu jeu)
{
    char *res = (char *) malloc(1024 * sizeof(char));

    sprintf(res, BOLD_HIGH_WHITE"\t\t\t\t\t\t\t\tPLATEAU:\n"RESET);

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 6; j++)
            sprintf(res + strlen(res), BOLD_HIGH_WHITE"\t[%d-%d]\t"RESET, jeu.plateau[i][j].Numero,
                    jeu.plateau[i][j].Tete);
        sprintf(res + strlen(res), "\n");
    }
    return res;
}

unsigned char get_pos_carte_derniere_ligne(Jeu *jeu, int ligne)
{
    for (int i = 0; i < 6; ++i)
    {
        if (jeu->plateau[ligne][i].Numero == 0)
        {
            return i - 1;
        }
    }
    return 0;
}

char ajoute_carte_au_plateau(Jeu *jeu, Carte *carte)
{
    char pos = get_pos_carte_mini(jeu, carte->Numero);
    if (pos == 0)
        return -1;
    if (pos % 6 == 4)
    {
        return 0;
    } else
    {
        jeu->plateau[pos / 6][(pos % 6) + 1] = *carte;
        return 1;
    }
}

unsigned char get_pos_carte_mini(Jeu *jeu, int numero)
{
    unsigned short diff = numero - get_dernier_carte_ligne(jeu, 0);
    char pos = 0;
    if (diff > 0)
        pos = get_pos_carte_derniere_ligne(jeu, 0);
    for (int i = 1; i < 4; ++i)
    {
        if ((numero - get_dernier_carte_ligne(jeu, i)) >= 0)
        {
            if ((numero - get_dernier_carte_ligne(jeu, i)) < diff)
            {
                diff = numero - get_dernier_carte_ligne(jeu, i);
                pos = (i * 6) + get_pos_carte_derniere_ligne(jeu, i);
            }
        }
    }
    return pos;
}


unsigned short getCartePlusPetiteDuPlateau(Jeu jeu)
{
    unsigned short min = 104; //Min théorique dans la liste de carte
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            if (jeu.plateau[i][j].Numero == 0)
                continue; //Saute la carte quand elle vaut 0 car ce n'est pas une carte
            else if (jeu.plateau[i][j].Numero < min) min = jeu.plateau[i][j].Numero;
        }
    }
    return min; // Retourne le numero de carte le plus petit du plateau
}

unsigned char get_dernier_carte_ligne(Jeu *jeu, int ligne)
{
    for (int i = 0; i < 6; ++i)
    {
        if (jeu->plateau[ligne][i].Numero == 0)
        {
            return jeu->plateau[ligne][i - 1].Numero;
        }
    }
    return 0;
}

void PrintTableau(Jeu jeu)
{
    fprintf(fichier_log, "\t\t\t\t\t\t\t\tPLATEAU:\n");
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 6; j++)
            fprintf(fichier_log, "\t[%d-%d]\t", jeu.plateau[i][j].Numero, jeu.plateau[i][j].Tete);
        fprintf(fichier_log, "\n");
    }
    fprintf(fichier_log, "\n");
}

Carte CreateCarte(unsigned short i)
{
    Carte c = {i, (rand() % 7) + 1, 0, 0};
    return c;
}

int getCarteDeLaListe(Jeu *jeu)
{
    int tmp[104];
    int cpt = 0;
    //Parcours de la liste pour trouver les cartes pas encore prise
    for (int i = 0; i < 104; i++)
    {
        if (jeu->listeCarte[i].isPicked == 0)
        {
            tmp[cpt] = jeu->listeCarte[i].Numero;
            cpt++;
        }
    }

    //Si le nombre de cartes restantes est supérieur à 0, on va tirer une carte aléatoire de notre tableau, sinon on ne retourne pas de valeur de carte.
    if (cpt > 0)
    {
        int tirage[cpt];
        for (int i = 0; i < cpt; i++) tirage[i] = tmp[i];
        return tirage[rand() % cpt];
    } else
    {
        printf(BOLD_RED"PLUS DE CARTE DANS LE PAQUET\n"RESET);
        return 999;
    }
}

char *affiche_joueur_cartes(Joueur *joueur)
{
    char *tmp = malloc(1024 * sizeof(char));

    for (int i = 0; i < 10; i++)
    {
        if (joueur->carte[i].isUsed == 0)
            sprintf(tmp + strlen(tmp), "Carte %d > Numéro[%d] Tete[%d]\n", i + 1, joueur->carte[i].Numero,
                    joueur->carte[i].Tete);
    }
    char *res = malloc(strlen(tmp) * sizeof(char));
    strcpy(res, tmp);
    free(tmp);
    return res;
}

unsigned short getCarteRestante(Jeu jeu)
{
    unsigned short cpt = 0;
    for (int i = 0; i < 104; i++) if (jeu.listeCarte[i].isPicked == 0) cpt++;
    return cpt;
}

void DistributionCarteAuxJoueurs(Jeu *jeu)
{
    for (int i = 0; i < nb_Joueur; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            while (1)
            {
                unsigned short rd = getCarteDeLaListe(jeu);
                if(jeu->listeCarte[rd].isPicked != 1){
                    jeu->listeCarte[rd].isPicked = 1;
                    jeu->joueur[i]->carte[j] = jeu->listeCarte[rd];
                    break;
                }
            }
        }
    }
}

unsigned short getLastCarteDeLaLigne(Jeu jeu, int numeroCarte)
{
    int tabLastNumber[4];
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            //Recupère dernier nombre des lignes du plateau
            if (jeu.plateau[i][j].Numero != 0)
            {
                tabLastNumber[i] = jeu.plateau[i][j].Numero;
            } else break;
        }
    }
    int min = 104; // La valeur maximale d'une carte est par défaut le minimum
    int position = 0;
    //Calcul la différence d'écart avec ces nombres
    for (int i = 0; i < 4; i++)
    {
        tabLastNumber[i] = abs(tabLastNumber[i] - numeroCarte);
    }
    //On cherche le minimum du tableau qui sera là où on pose la carte
    for (int i = 0; i < 4; i++)
    {
        if (tabLastNumber[i] < min)
        {
            min = tabLastNumber[i];
            position = i;
        }
    }
    return position; //Retourne la position de la colonne au rang n-1
}

unsigned short getLastPositionDeLaLigne(Jeu jeu, int colonne)
{
    unsigned short lastPosition = 0;
    for (short i = 0; i < 6; i++)
    {
        if (jeu.plateau[colonne][i].Numero == 0)
        {
            lastPosition = i;
            break;
        }
    }
    return lastPosition;
}


char *Statistique(Jeu jeu)
{

    char *tmp = malloc(300 * sizeof(char));

    //DANS TERMINAL
    sprintf(tmp + strlen(tmp), BOLD_CYAN"\t[STATISTIQUES]\n"RESET);
    sprintf(tmp + strlen(tmp), "Nombre de parties joué : %d\n", nb_Partie);
    sprintf(tmp + strlen(tmp), "Moyenne des têtes obtenues : %d\n", MoyenneDesTetes(jeu));

    //DANS FICHIER
    fprintf(fichier_log, "\t[STATISTIQUES]\n");
    fprintf(fichier_log, "Nombre de parties joué : %d\n", nb_Partie);
    fprintf(fichier_log, "Moyenne des têtes obtenues : %d\n", MoyenneDesTetes(jeu));
    printf("%s", MinEtMaxDefaite(jeu));
    char *res = malloc(strlen(tmp) * sizeof(char));
    strcpy(res, tmp);
    free(tmp);
    return res;
}

char *RecapRegle(Jeu jeu)
{

    char *tmp = malloc(255 * sizeof(char));

    sprintf(tmp, BOLD_HIGH_WHITE"\t\nREGLE DU JEU:\n"RESET);
    sprintf(tmp + strlen(tmp), "Le nombre de têtes maximal est de ");
    sprintf(tmp + strlen(tmp), BOLD_MAGENTA"%d"RESET, nb_TeteMax);
    sprintf(tmp + strlen(tmp), "\nLe nombre de tours maximal est de ");
    sprintf(tmp + strlen(tmp), BOLD_MAGENTA"%d\n"RESET, nb_MancheMax);
    sprintf(tmp + strlen(tmp), "Nombre de joueurs: ");
    sprintf(tmp + strlen(tmp), BOLD_MAGENTA"%d\n"RESET, nb_Joueur);

    char *res = malloc(strlen(tmp) * sizeof(char));
    strcpy(res, tmp);
    free(tmp);

    return res;
}

char *AfficheNbTeteJoueurs(Jeu jeu)
{

    char *tmp = malloc(45 * nb_Joueur * sizeof(char));

    for (short i = 0; i < nb_Joueur; i++)
    {
        sprintf(tmp + strlen(tmp), BOLD_CYAN"Le joueur %d possède %d têtes \n"RESET, i, jeu.joueur[i]->NB_TETE);
        fprintf(fichier_log, "ROUND [%d] > Le joueur %d possède %d têtes\n", tour, i, jeu.joueur[i]->NB_TETE);
    }
    fprintf(fichier_log, "\n");

    char *res = malloc(strlen(tmp) * sizeof(char));
    strcpy(res, tmp);
    free(tmp);
    return res;
}

void DistributionCarteAuJoueur(Jeu *jeu, short idJoueur)
{
    for (int i = 0; i < 10; i++)
    {
        unsigned short rd = getCarteDeLaListe(jeu);
        jeu->listeCarte[rd].isPicked = 1;
        jeu->joueur[idJoueur]->carte[i] = jeu->listeCarte[rd];
    }
}

unsigned short getNbCarteUtilisableDuJoueur(Jeu jeu, short idJoueur)
{
    unsigned short cpt = 0;
    for (int i = 0; i < 10; i++) if (jeu.joueur[idJoueur]->carte[i].isUsed == 0) cpt++;
    return cpt;
}

char *MinEtMaxDefaite(Jeu jeu)
{
    unsigned short min = jeu.joueur[0]->NB_DEFAITE, max = jeu.joueur[0]->NB_DEFAITE;
    unsigned short imin = 0, imax = 0;

    for (int i = 0; i < nb_Joueur; i++)
    {
        if (jeu.joueur[i]->NB_DEFAITE < min)
        {
            min = jeu.joueur[i]->NB_DEFAITE;
            imin = i;
        }
        if (jeu.joueur[i]->NB_DEFAITE > max)
        {
            max = jeu.joueur[i]->NB_DEFAITE;
            imax = i;
        }
    }

    char *tmp = malloc(200 * sizeof(char));

    //DANS TERMINAL
//    printf("Le joueur ayant obtenue le moins de défaite est le joueur %d avec %d défaites\n", imin, min);
//    printf("Le joueur ayant obtenue le plus de défaite est le joueur %d avec %d défaites\n", imax, max);
    sprintf(tmp, "Le joueur ayant obtenue le moins de défaite est le joueur %d avec %d défaites\n", imin, min);
    sprintf(tmp + strlen(tmp), "Le joueur ayant obtenue le plus de défaite est le joueur %d avec %d défaites\n",
            imax,
            max);

    //DANS FICHIER
    fprintf(fichier_log, "Le joueur ayant obtenu le moins de défaite est le joueur %d avec %d défaites\n", imin,
            min);
    fprintf(fichier_log, "Le joueur ayant obtenu le plus de défaite est le joueur %d avec %d défaites\n", imax,
            max);

    char *res = malloc(strlen(tmp) * sizeof(char));
    strcpy(res, tmp);
    free(tmp);
    return res;
}

int MoyenneDesTetes(Jeu jeu)
{
    int somme = 0;
    for (int i = 0; i < nb_Joueur; i++) somme += jeu.joueur[i]->NB_TETE;
    return somme / nb_Joueur;
}

void ForceFinDuJeu()
{
    printf(RESET);
    isOver = 1;
    nextPartie = 1;
    fprintf(fichier_log, "\n***LE JEU A ETE ARRETE***\n");
    fclose(fichier_log);
    exit(0);
}

#endif //SYSTEMES_ET_RESEAUX_PROJET_JEU_H
