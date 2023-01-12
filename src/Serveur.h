//**********//
// Server.h //
//**********//

#ifndef SYSTEMES_ET_RESEAUX_PROJET_SERVEUR_H
#define SYSTEMES_ET_RESEAUX_PROJET_SERVEUR_H


#include<stdlib.h>
#include<stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */

#include "Jeu.h"
#include "Bot.h"


int PORT = 65534;

#define BUF_SIZE 1024


typedef struct
{
    short numero_joueur;
    short socket;
    short pret;
    Joueur *joueur;
    short bot_or_not;

} client;


/**
 * @brief Fonction qui crée un nouveau pointeur de client et le renvoie.
 * @return client
 */
client *init_joueur();


/**
 * @brief Fonction qui accepte ou non les clients qui se connectent au serveur.
 * @return void
 */
void *listen_joueurs();

/**
 * @brief Fonction qui envoie le message en paramêtre à tous les joueurs.
 * @param joueurs
 * @param nb_clients
 * @param message
 */
void send_all_joueurs(client **clients, int nb_clients, char *message);

/**
 * @brief Fonction qui écoute en continu le socket du joueur en paramètre, jusqu’à qu’il mette prêt.
 * @param argv
 * @return void
 */
void *joueur_pret(void *);

/**
 * @brief Fonction qui retourne 1 si tous les joueurs sont prêts et 0 sinon.
 * @return 1 ou 0
 */
int all_joueur_pret();

/**
 * @details Ecoute le joueur, pour récupérer la carte choisie par le joueur.
 * @param argv
 * @return void
 */
void *listen_choix_carte_joueur(void *);

/**
 * @details Ecoute le bot, pour récupérer la carte choisie par le bot.
 * @param argv
 * @return void
 */
void *listen_choix_carte_bot(void *);

/**
 * @details Fonction appelé quand un joueur quitte la partie.
 * @param c
 * @return void
 */
void client_quit(client *c);

/**
 * @details Ferme le socket de tous les clients et du serveur.
 */
void close_all_clients();

/**
 * @details Fonction qui écoute le client en paramètre et retourne les données envoyer par le client.
 * @param c
 * @param buffer
 * @return chaine de caractère.
 */
void recv_client_data(client *c, char *buffer);

/**
 * @details Quand un joueur joue une carte qui est trop petite.
 * @param c
 * @return ligne choisie par le client.
 */
int carte_trop_petite(client *c);

/**
 * @details boucle du jeu
 * @param jeu
 */
void jeu_play(Jeu *jeu);

/**
 * @details Met fin au serveur.
 */
void end_serveur();

/**
 * @details Fonction de gestion des signaux du programme
 * @param signal_recu
 */
void gestion_signaux_serveur(int signal_recu);

/**
 * @details Fonction qui test si le client en parametre un est bot ou pas.
 * @param nom
 * @param c
 * @return 1 si c’est un bot 0 sinon
 */
int is_bot(char *nom, client *c);

/**
 * @details Ajoute un bot au jeu.
 */
void ajout_bot();

/**
 * @details Fonction pour afficher le temps de jeu aux joueurs et dans le fichier log
 * @param duree
 */
void affiche_temps_jeu(double duree);

/**
 * @details Fonction utilisée par le serveur pour changer les réglages du jeu
 */
void change_limite_jeu();

/**
 * @details free les variables du serveur et du jeu.
 */
void free_serveur();

#endif //SYSTEMES_ET_RESEAUX_PROJET_SERVEUR_H
