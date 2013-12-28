#include "dazis-verifies.c"
#include <sys/types.h>          
#include <sys/socket.h>
#include <sys/un.h>

#define DOMAIN_SIZE 32
#define MAX_CONNEX 128

int main(int argc, char ** argv){

  struct dazis_verifies * dazis;
  struct sockaddr_un saddr;
  int fds;
  int lien;
  int ecoute;
  char nom_domaine[DOMAIN_SIZE] = ".dazibao-notification-socket";
  int connex[MAX_CONNEX];
  int nb_connex = 0;
  socklen_t slen = sizeof(struct sockaddr_un * );

  if(argc < 2){
    printf("usage: ./service-notif <dazibao1> <dazibao2> ...\n");
    return -1;
  }
  //On cree la structure qui permet de stocker 
  //les noms de dazibaos valides
  dazis = verifie_fichiers_dazibao(argc, argv);
  if(dazis == NULL){
    printf("Erreur:verifie_fichiers_dazibao()\n");
    return -1;
  }
  
  //On cree la socket de service
  fds = socket(AF_UNIX, SOCK_STREAM, 0);
  if(fds < 0){
    perror("socket");
    return errno;
  }

  //On configure l'addresse de la socket
  memset(&saddr, 0, sizeof(saddr));
  saddr.sun_family = AF_UNIX;
  strncpy(saddr.sun_path, nom_domaine, DOMAIN_SIZE);

  //On cree un lien avec la socket et l'adresse
  lien = bind(fds,(const struct sockaddr*)&saddr,sizeof(struct sockaddr_un));
  if(lien < 0){
    perror("bind");
    return errno;
  }
  
  //On se prepare a prendre en charge des connexions
  ecoute = listen(fds, MAX_CONNEX);
  if(ecoute < 0){
    perror("listen");
    return errno;
  }
  
  memset(connex, 0, MAX_CONNEX);
  
  //On attend la connexion d'un client
  connex[nb_connex] = accept(fds, (struct sockaddr *)&saddr, 
			     (socklen_t *) &slen);
  nb_connex++;


  return 0;
}
