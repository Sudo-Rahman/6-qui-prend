//
// Created by rahman on 21/11/22.
//
#include "Jeu.h"


Jeu initJeu(Jeu jeu) {
    printf("ici");

    jeu.plateau = creePlateau(); // Création du plateau de carte 4*6

    //Creation des 104 cartes avec numéro de tête random
    for (int i = 0; i < 104; i++) jeu.listeCarte[i] = CreateCarte(i+1);

    //SI c'est la premiere partie, on initialise le nombre de défaites à 0.
    if (nb_Partie == 0) for (int i = 0; i < nb_Joueur; i++) jeu.joueur[i].NB_DEFAITE = 0;

    //Nombre de têtes à 0 vu que le jeu commence
    for (int i = 0; i < nb_Joueur; i++) jeu.joueur[i].NB_TETE = 0;

    //On initialise le plateau à 0.
    Carte carte_0 = {0, 0, 0, 0};
    for (int i = 0; i < 4; i++) for (int j = 0; j < 6; j++) jeu.plateau[i][j] = carte_0;

    //Carte de la premiere ligne du plateau distribué
    for (int i = 0; i < 4; i++) {
        int rd = getCarteDeLaListe(&jeu);
        jeu.listeCarte[rd].isPicked = 1;
        jeu.plateau[i][0] = jeu.listeCarte[rd];
    }
    DistributionCarteAuxJoueurs(&jeu);
    return jeu;
}


Carte **creePlateau() {
    Carte **plateau = (Carte **) malloc(6 * sizeof(Carte *));
    for (int i = 0; i < 6; i++) plateau[i] = (Carte *) malloc(6 * sizeof(Carte));
    return plateau;
}

char *affichePlateau(Jeu jeu) {
    char *tmp = malloc(600 * sizeof(char));

    sprintf(tmp, BOLD_HIGH_WHITE"\t\t\t\t\t\t\t\tPLATEAU:\n"RESET);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 6; j++)
            sprintf(tmp + strlen(tmp), BOLD_HIGH_WHITE"\t[%d-%d]\t"RESET, jeu.plateau[i][j].Numero,
                jeu.plateau[i][j].Tete);
        sprintf(tmp + strlen(tmp), "\n");

    }
    sprintf(tmp + strlen(tmp), "\n");

    char *res = malloc(strlen(tmp) * sizeof(char));
    strcpy(res, tmp);
    free(tmp);
    return res;
}

Carte CreateCarte(unsigned short i) {
    Carte c = {i, (rand() % 7) + 1, 0, 0};
    return c;
}
int getCarteDeLaListe(Jeu *jeu) {
    int tmp[105];
    int cpt = 0;
    //Parcours de la liste pour trouver les cartes pas encore prise
    for (int i = 1; i < 105; i++) {
        if (jeu->listeCarte[i].isPicked == 0) {
            tmp[cpt] = jeu->listeCarte[i].Numero;
            cpt++;
        }
    }

    //Si le nombre de cartes restantes est supérieur à 0, on va tirer une carte aléatoire de notre tableau, sinon on ne retourne pas de valeur de carte.
    if (cpt > 0) {
        int tirage[cpt];
        for (int i = 0; i < cpt; i++) tirage[i] = tmp[i];
        return tirage[rand() % cpt];
    } else {
        printf(BOLD_RED"PLUS DE CARTE DANS LE PAQUET\n"RESET);
        return 999;
    }
}

char *AfficheCarteJoueur(Jeu jeu, int idJoueur) {
    char *tmp = malloc(350 * sizeof(char));

    for (int i = 0; i < 10; i++) {
        if (jeu.joueur[idJoueur].carte[i].isUsed == 0)
//            printf("Carte %d > Numéro[%d] Tete[%d]\n", i, jeu.joueur[idJoueur].carte[i].Numero, jeu.joueur[idJoueur].carte[i].Tete);
            sprintf(tmp + strlen(tmp), "Carte %d > Numéro[%d] Tete[%d]\n", i, jeu.joueur[idJoueur].carte[i].Numero,
                    jeu.joueur[idJoueur].carte[i].Tete);
    }
    char *res = malloc(strlen(tmp) * sizeof(char));
    strcpy(res, tmp);
    free(tmp);
    return res;
}

unsigned short getCarteRestante(Jeu jeu) {
    unsigned short cpt = 0;
    for (int i = 1; i < 105; i++) if (jeu.listeCarte[i].isPicked == 0) cpt++;
    return cpt;
}

void DistributionCarteAuxJoueurs(Jeu *jeu) {
    for (int i = 0; i < nb_Joueur; i++) {
        for (int j = 0; j < 10; j++) {
            unsigned short rd = getCarteDeLaListe(jeu);
            jeu->listeCarte[rd].isPicked = 1;
            jeu->joueur[i].carte[j] = jeu->listeCarte[rd];
        }
    }
}

unsigned short getLastCarteDeLaLigne(Jeu jeu, int numeroCarte) {
    int tabLastNumber[4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 6; j++) {
            //Recupère dernier nombre des lignes du plateau
            if (jeu.plateau[i][j].Numero != 0) {
                tabLastNumber[i] = jeu.plateau[i][j].Numero;
            } else break;
        }
    }
    int min = 104; // La valeur maximale d'une carte est par défaut le minimum
    int position = 0;
    //Calcul la différence d'écart avec ces nombres
    for (int i = 0; i < 4; i++) {
        tabLastNumber[i] = abs(tabLastNumber[i] - numeroCarte);
    }
    //On cherche le minimum du tableau qui sera là où on pose la carte
    for (int i = 0; i < 4; i++) {
        if (tabLastNumber[i] < min) {
            min = tabLastNumber[i];
            position = i;
        }
    }
    return position; //Retourne la position de la colonne au rang n-1
}

unsigned short getLastPositionDeLaLigne(Jeu jeu, int colonne) {
    unsigned short lastPosition = 0;
    for (short i = 0; i < 6; i++) {
        if (jeu.plateau[colonne][i].Numero == 0) {
            lastPosition = i;
            break;
        }
    }
    return lastPosition;
}

int AskNombreUser(int min, int max) {
    char str[16] = "";
    scanf("%s", str);
    if (strlen(str) > 16) str[0] = '\0'; //Gestion cas ou nombre est trop grand pour pas crash le programme

    //SI utilisateur veut arreter le jeu en appuyant sur x
    if (strcmp(str, "x") == 0 || strcmp(str, "X") == 0) {
        printf(BOLD_RED"ARRET DU JEU...\n"RESET);
        ForceFinDuJeu();
        return 0;
    }
    int nombre = atoi(str);//Conversion de la chaine de caractère en nombre entier pour savoir le nombre entré
    while (nombre < min || nombre > max) {
        printf(BOLD_RED"La valeur doit être comprise entre [%d-%d], vous avez entré : %d\n>"RESET, min, max, nombre);
        scanf("%s", str);
        if (strlen(str) > 16) str[0] = '\0';
        if (strcmp(str, "x") == 0 || strcmp(str, "X") == 0) {
            printf(BOLD_RED"ARRET DU JEU...\n"RESET);
            ForceFinDuJeu();
            return 0;
        }
        nombre = atoi(str);
        fflush(stdin);
        fflush(stdout);
    }

    fflush(stdin);
    fflush(stdout);
    return nombre;
}

char *Statistique(Jeu jeu) {

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

char *RecapRegle(Jeu jeu) {

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

char *AfficheNbTeteJoueurs(Jeu jeu) {

    char *tmp = malloc(45 * nb_Joueur * sizeof(char));

    for (short i = 0; i < nb_Joueur; i++) {
        sprintf(tmp + strlen(tmp), BOLD_CYAN"Le joueur %d possède %d têtes \n"RESET, i, jeu.joueur[i].NB_TETE);
        fprintf(fichier_log, "ROUND [%d] > Le joueur %d possède %d têtes\n", tour, i, jeu.joueur[i].NB_TETE);
    }
    fprintf(fichier_log, "\n");

    char *res = malloc(strlen(tmp) * sizeof(char));
    strcpy(res, tmp);
    free(tmp);
    return res;
}

void DistributionCarteAuJoueur(Jeu *jeu, short idJoueur) {
    for (int i = 0; i < 10; i++) {
        unsigned short rd = getCarteDeLaListe(jeu);
        jeu->listeCarte[rd].isPicked = 1;
        jeu->joueur[idJoueur].carte[i] = jeu->listeCarte[rd];
    }
}

unsigned short getNbCarteUtilisableDuJoueur(Jeu jeu, short idJoueur) {
    unsigned short cpt = 0;
    for (int i = 0; i < 10; i++) if (jeu.joueur[idJoueur].carte[i].isUsed == 0) cpt++;
    return cpt;
}

char *MinEtMaxDefaite(Jeu jeu) {
    unsigned short min = jeu.joueur[0].NB_DEFAITE, max = jeu.joueur[0].NB_DEFAITE;
    unsigned short imin = 0, imax = 0;

    for (int i = 0; i < nb_Joueur; i++) {
        if (jeu.joueur[i].NB_DEFAITE < min) {
            min = jeu.joueur[i].NB_DEFAITE;
            imin = i;
        }
        if (jeu.joueur[i].NB_DEFAITE > max) {
            max = jeu.joueur[i].NB_DEFAITE;
            imax = i;
        }
    }

    char *tmp = malloc(200 * sizeof(char));

    //DANS TERMINAL
//    printf("Le joueur ayant obtenue le moins de défaite est le joueur %d avec %d défaites\n", imin, min);
//    printf("Le joueur ayant obtenue le plus de défaite est le joueur %d avec %d défaites\n", imax, max);
    sprintf(tmp, "Le joueur ayant obtenue le moins de défaite est le joueur %d avec %d défaites\n", imin, min);
    sprintf(tmp + strlen(tmp), "Le joueur ayant obtenue le plus de défaite est le joueur %d avec %d défaites\n", imax,
            max);

    //DANS FICHIER
    fprintf(fichier_log, "Le joueur ayant obtenu le moins de défaite est le joueur %d avec %d défaites\n", imin, min);
    fprintf(fichier_log, "Le joueur ayant obtenu le plus de défaite est le joueur %d avec %d défaites\n", imax, max);

    char *res = malloc(strlen(tmp) * sizeof(char));
    strcpy(res, tmp);
    free(tmp);
    return res;
}

int MoyenneDesTetes(Jeu jeu) {
    int somme = 0;
    for (int i = 0; i < nb_Joueur; i++) somme += jeu.joueur[i].NB_TETE;
    return somme / nb_Joueur;
}



void ForceFinDuJeu() {
    printf(RESET);
    isOver = 1;
    nextPartie = 1;
    fprintf(fichier_log, "\n***LE JEU A ETE ARRETE***\n");
    fclose(fichier_log);
    exit(0);
}
