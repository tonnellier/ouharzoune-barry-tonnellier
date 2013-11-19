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
#define ERROR_SEEK_TLV -5
#define ERROR_WRITE_TEXT -3
#define ERROR_UNKNOW -4
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

/* TEST OK
   TODO
   Il reste encore a bien decouper les mots du texte
 */
int affiche_texte(int fd, int length){
    char buf[BUF_LEN];
    int lus, ecrits, reste = length, rc;
    printf("\n");
    while(1){

	if(reste < BUF_LEN) 
	    lus = read(fd, buf, reste);
	else
	    lus = read(fd, buf, BUF_LEN);

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
	      ecrits = write(STDOUT_FILENO, buf, lus);
	      if(ecrits < 0){
		perror("write:affiche_texte(int,int)");
		return ERROR_WRITE_TEXT;
	      }
	      printf("\n");
	    }
	}

	reste -= lus;
    }
    //On replace le curseur comme avant la lecture
    rc = lseek(fd, -length, SEEK_CUR);
    if(rc < 0){
      perror("lseek:affiche_texte(int,int)");
      return ERROR_SEEK_TLV;
    }
    return 0;
}

// TEST OK
int recupere_length(int fd){
  int rc, length = 0;
  char buflen[LENGTH_SIZE];
  rc = read(fd, buflen, LENGTH_SIZE);
  if(rc < 0){
    perror("read:recupere_length");
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
      printf("read:recupere_length(tous les octets n'ont pas etes lus)\n");
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
    perror("read:affiche_tlv");
    return ERROR_READ_TLV;
  }
      
  if(rc == TYPE_SIZE){

    if (typetlv == TYPE_PAD1) {
      printf("TLV type: Pad1\n");
      return TYPE_PAD1;
    }

    length = recupere_length(fd);
    printf("length:affiche_tlv:%d", length);
    switch(typetlv){

      
    case 1: {
      printf("TLV type: PadN length: %d\n", length);
    }
      break;

    case 2: {
      printf("TLV type: Text length: %d\n", length);
      rc = affiche_texte(fd, length);
      if(rc < 0) 
	return rc;
    }
      break;
      /*
	case 3: break;
	case 4: break;
	case 5: break;
	case 6: break;
      */
      // On saute automatiquement les donnees du TLV
      
    default: {
      printf("TLV inconnu: type:%d length:%d\n", typetlv, length);
    }
      
    }    
    rc = lseek(fd, length, SEEK_CUR);
    printf("lseek:affiche_tlv:%d", rc);
    if(rc < 0){
      perror("lseek:affiche_tlv");
      return ERROR_SEEK_TLV;
    }
    return typetlv;
  }
  else{
    
    if(rc == 0){
      printf("\nlire_tlv(int):fin lecture\n");
      return EOF_DAZI;
    }
  }
   
  return ERROR_UNKNOW;
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

