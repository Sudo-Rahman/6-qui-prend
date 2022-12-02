//
// Created by Rahman  Yilmaz on 01/12/2022.
//

#include "Bot.h"

#include <string.h>

int sock;
int pid;


int main(int argc, char * argv[]){

    if (argc == 3) {
        PORT = atoi(argv[1]);
        strcpy(argv[2], hostname);
    }

    pid = getpid();

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("ereur bot");
        exit(errno);
    }

    struct sockaddr_in sin = {0}; /* initialise la structure avec des 0 */

    sin.sin_port = htons(PORT); /* on utilise htons pour le port */
    sin.sin_family = AF_INET;

    if (inet_pton(AF_INET, hostname, &sin.sin_addr) <= 0) {
        perror("\nAdresse invalide \n");
        exit(errno);
    }

    if (connect(sock, (struct sockaddr *) &sin, sizeof(struct sockaddr)) == -1) {
        perror("Connection impossible");
        exit(errno);
    }


    char *buff = (char *) malloc(1024 * sizeof (char ));
    while (1){
        recv(sock,buff,sizeof (buff) -1,0);
    }

    exit(EXIT_SUCCESS);
}