#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "norme.h"
#include "modif-dazibao.h"

int verifie_entete(char * dazibao){
  char head[HEADER_SIZE];
  int fd, rc;

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

/* A FINIR */

int lire_entete_TLV(int fd){
  int rc, length;
  char headTLV[TYPE_SIZE+LENGTH_SIZE];

  rc = read(fd, headTLV, TYPE_SIZE);
  if(rc < 0){
    perror("read");
    return errno;
  }else{
    if(rc == TYPE_SIZE){
      if(headTLV[0] == TYPE_PAD1){	
	return rc;
      }
      if(headTLV[0] == TYPE_PADN){
	//lseek()
	return rc;
      }
      if(headTLV[0] == TYPE_TEXT){
	
	return rc;
      }
      if(headTLV[0] == TYPE_PNG){
	return rc;
      }
      if(headTLV[0] == TYPE_JPEG){
	return rc;
      }
      if(headTLV[0] == TYPE_COMPOUND){
	return rc;
      }
      if(headTLV[0] == TYPE_DATED){
	return rc;
      }
    }else{
      printf("\nfin de la lecture !\n");
    }
    
  }
  return rc;
}



int affiche_dazibao(char * dazibao){

  char head[HEADER_SIZE];
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
    rc = lire_entete_TLV(fd);
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

