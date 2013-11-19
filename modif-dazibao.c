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
#define BUF_LEN 64

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

/* TODO
 */
int affiche_texte(int fd, int length){
    char buf[BUF_LEN];
    int lus;
    int ecrits;
    while(1){
      printf("length:%d\n", length);
	if(length < BUF_LEN) 
	    lus = read(fd, buf, length);
	else
	    lus = read(fd, buf, BUF_LEN);
	printf("lus:%d\n", lus);
	if (lus < 0){
	    perror("read:affiche_texte(int,int)");
	    return ERROR_READ_TLV;
	}
	else{
	    if(lus == 0){
		printf("\n");
		break;
	    }
	    else{
	      printf("%s\n", buf);
	      printf("\n");
	    }
	}
	length -= lus;
    }
    return 0;
}

// TEST OK
int recupere_length(int fd){
  int rc, length = 0;
  char buflen[LENGTH_SIZE];
  rc = read(fd, buflen, LENGTH_SIZE);
  if(rc < 0){
    perror("read:lire_length");
    return ERROR_READ_TLV;
  }
  else{
    if(rc == LENGTH_SIZE){
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

/* TODO */
int affiche_tlv(int fd){
  int rc;
  char typetlv;
  int length = -1;
  
  rc = read(fd, &typetlv, TYPE_SIZE);

  if(rc < 0){
    perror("read");
    return ERROR_READ_TLV;
  }
      
  if(rc == TYPE_SIZE){
    if (typetlv == TYPE_PAD1) {
      printf("TLV type: Pad1\n");
      return TYPE_PAD1;
    }
    switch(typetlv){
      length = recupere_length(fd);
    case 1: printf("TLV type: PadN length: %d\n", length);
      break;
    case 2: printf("TLV type: Text length: %d\n", length);
      if(affiche_texte(fd, length) < 0)
	return ERROR_READ_TLV;
      break;
      /*
	case 3: break;
	case 4: break;
	case 5: break;
	case 6: break;
      */
    default: printf("TLV inconnu: type:%d\n", typetlv);
    }    
    return typetlv;
  }
    
  if(rc == 0){
    printf("\nlire_tlv(int):fin lecture\n");
    return EOF_DAZI;
  }
   
  return ERROR_READ_TLV;
}


/* TODO */
int affiche_dazibao(char * dazibao){

  int fd, rc;
  
  /* TODO
     Prise en charge du verrou 
  */


  /* Ouverture du fichier  */
  fd = open(dazibao, O_RDONLY);
  if(fd < 0){
    perror("open:affiche_dazibao(char *)\n");
    return errno;
  }  

  /* Lecture et affichage du fichier */
  while(1){

    rc = affiche_tlv(fd);
    if(rc < 0){
      printf("Erreur d'affichage !\n");
      close(fd);
      return rc;
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

