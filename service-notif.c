#include "dazis-verifies.c"
#include <sys/types.h>          
#include <sys/socket.h>
#include <sys/un.h>

#include "service-notif.h"

//client en attente d'info
int main(){
//struct dazis_verifies * sdv;
  struct sockaddr_un saddr;
  int sfd;
  int cfd;
  int lien;  
  int ecoute;

  char buf[BUF_SIZE];
  int lus;
  //int connex[MAX_CONNEX];

  //int nb_connex = 0;
  

  /*
  if(argc < 2){
    printf("usage: ./service-notif <dazibao1> <dazibao2> ...\n");
    return -1;
  }
  
  //On cree la structure qui permet de stocker 
  //les noms de dazibaos valides
  sdv = verifie_fichiers_dazibao(argc, argv);
  if(sdv == NULL){
    printf("Erreur:verifie_fichiers_dazibao()\n");
    return -1;
  }
  */
  //On cree la socket de service
  sfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if(sfd < 0){
    perror("socket");
    return errno;
  }

  //On configure l'addresse de la socket
  memset(&saddr, 0, sizeof(struct sockaddr_un));
  saddr.sun_family = AF_UNIX;
  strncpy(saddr.sun_path, SOCKET_RDV, sizeof(saddr.sun_path)-1);

  printf("sun_path=%s\n", saddr.sun_path);
  
  //On cree un lien avec la socket et l'adresse
  lien = bind(sfd,(struct sockaddr *) &saddr,sizeof(struct sockaddr_un));
  if(lien < 0){
    perror("bind");
    return errno;
  }
  
  //TO CHECK - TO CHECK - TO CHECK - TO CHECK - TO CHECK
  //---------------------------------------------------
  //On se prepare a prendre en charge des connexions
  ecoute = listen(sfd, MAX_CONNEX);
  if(ecoute < 0){
    perror("listen");
    return errno;
  }
  
  

  //accept(sfd, (struct sockaddr *)&saddr, (socklen_t *) &slen);
  
  while(1){
    if(( cfd = accept (sfd, NULL ,NULL )) < 0){
      perror ("accept error");
    }
    while( ( lus = read ( cfd , buf , BUF_SIZE ) ) > 0){
      if ( write ( STDOUT_FILENO , buf , lus ) != lus ){
	perror(" wri te e rro r " ) ;
	return errno;
      }
      printf("\n");
    }
    printf("fin de lecture\n");
    if(lus < 0){
      perror ( " read e rro r " ) ;
      return errno;
    }
    if( close(cfd) < 0){
      perror ( " close e rro r " ) ;
      return errno;
    }
  }

  /*
  memset(connex, 0, MAX_CONNEX);
  
  //On attend la connexion d'un client
  connex[nb_connex] = 
  nb_connex++;
  */

  return 0;

}
