#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "norme.h"
#include "modif-dazibao.h"

#define HEADER_ERROR -53
#define ERROR_READ_TLV -2
#define EOF_DAZI -1

/* TEST OK */
int verifie_entete(char * dazibao){
  char head[HEADER_SIZE];
  int fd, rc;
  /* TODO
    ajouter la pose d'un verrou flock    
   */

  fd = open(dazibao, O_RDONLY);
  if(fd < 0){
    perror("open:verifie_entete");
    return errno;
  }

  /* Lecture de l'entete */
  rc =  read(fd, head, HEADER_SIZE);
  if(rc < 0){
    perror("read:verifie_entete");
    return errno;
  }else{
    if(rc != HEADER_SIZE || head[0] != MAGIC || head[1] != VERSION)
      return HEADER_ERROR;
    else
      printf("L'entete de \"%s\" est correct !\n", dazibao);
  }

  close(fd);
  return 0;
}


int lire_length(int fd){
  int rc, length = 0;
  int i;
  char buflen[1024] = {0};
  rc = read(fd, buflen, 1024);
  if(rc < 0){
    perror("read:lire_length");
    return ERROR_READ_TLV;
  }
  else{    
    if(rc == 1024){

      for(i=0; i < rc;i++){
	if (i%16==0) printf("\n");
	printf("%d ",buflen[i]);
	
      }
      length = buflen[0];
      length = length << 8;
      length += buflen[1];
      length = length << 8;
      length += buflen[2];
    }
    else{
      printf("read:lire_length(tous les octets n'ont pas etes lus)\n");
      return ERROR_READ_TLV;
    }
  }
  return length;
}

/* A FINIR */

int lire_tlv(int fd){
  int rc;
  char typetlv;
  
  rc = read(fd, &typetlv, TYPE_SIZE);

  if(rc < 0){
    perror("read");
    return ERROR_READ_TLV;
  }

  else{
    if(rc == TYPE_SIZE){
      switch(typetlv){
      case 0: printf("TLV type: Pad1\n"); 
	break;
      case 1: printf("TLV type: PadN\n"); 
	break;
      case 2: break;
      case 3: break;
      case 4: break;
      case 5: break;
      case 6: break;
      default: printf("TLV inconnu: type:%d\n", typetlv);
      }
      return typetlv;
    }
  }

  printf("\nlire_tlv(int):fin lecture\n");

  return EOF_DAZI;
}



int affiche_dazibao(char * dazibao){

  int fd, rc;
  
  /* TODO
     Prise en charge du verrou 
  */


  /* Ouverture du fichier  */
  fd = open(dazibao, O_RDONLY);
  if(fd < 0){
    perror("open");
    return errno;
  }  

  /* Lecture et affichage du fichier */
  while(1){

    rc = lire_tlv(fd);
    if(rc < 0){
      printf("Erreur d'affichage !\n");
    }else{
      if(rc == 0) {
	printf("Fin d'affichage !\n");
	break;
      }
    }

  }
  
  close(fd);
  return 0;
}

