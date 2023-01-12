# Systemes-et-Reseaux-Projet
Projet de systèmes et réseaux du semestre 1 de L3 informatique.
# 6 qui prend

Jeu codé en C utilisant les sockets TCP pour la communication réseau.

## Prérequis :

- Compilateur C (ex: GCC)
- Bibliothèque de sockets (UNIX)

## Installation :

- Téléchargez les fichiers du jeu sur votre ordinateur.
- Ouvrez un terminal dans le répertoire où les fichiers se trouvent.
- Utilisez la commande suivante pour compiler les differents programmes :

### Pour Unix :
```
gcc Serveur.c -lpthread -o Serveur
```
```
gcc Client.c -lpthread -o Client
```
```
gcc Bot.c -o bot
```
## Utilisation : 

Lancez le Serveur en premier en utilisant la commande suivante :

- Le port est facultatif (par défaut 65534).
```
./Serveur [port]
```

<br>

- L'adresse ip du serveur est facultative (par défaut localhost), de même pour le port (par défaut 65534).
```
./Client [adresse IP] [port] 
```

## Remarques

- Assurez-vous que le serveur est en cours d'exécution et accessible avant de lancer le jeu.
- Le jeu peut être joué en réseau en spécifiant l'adresse IP et le port du serveur distant lors de l'exécution du client.
