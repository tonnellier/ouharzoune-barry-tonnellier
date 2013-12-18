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
#define ERROR_READ_DAZI -2
#define ERROR_SEEK_DAZI -5
#define ERROR_WRITE_TEXT -3
#define ERROR_UNKNOW -4
#define EOF_DAZI -1
#define BUF_LEN_TEXT 64

/* TEST OK */
int verifie_entete(char * dazibao){
  unsigned char head[HEADER_SIZE];
  int fd, rc;
  /* TODO
    ajouter la pose d'un verrou flock    
   */

  fd = open(dazibao, O_RDONLY);
  if(fd < 0){
    perror("open:verifie_entete()");
    close(fd);
    return fd;
  }

  /* Lecture de l'entete */
  rc =  read(fd, head, HEADER_SIZE);
  if(rc < 0){
    perror("read:verifie_entete()");
    close(fd);
    return rc;
  }else{


    if(rc != HEADER_SIZE){
      printf("La taille de l'entete n'a pu etre lue\n");
      return HEADER_ERROR;
    }else{
      if(head[0] != MAGIC){
	printf("Le numero magic n'est pas correct:MAGIC=%d\n",head[0]);
	return HEADER_ERROR;
      }
      if(head[1] != VERSION){
	printf("Le numero de version n'est pas correct:");
	printf("VERSION=%d\n", head[1]);
	return HEADER_ERROR;
      }
    }
    
  }

  close(fd);
  return 0;
}

/* TEST OK
   TODO
   Il reste encore a bien decouper les mots du texte
 */
int affiche_texte(int fd, int length){
    unsigned char buf[BUF_LEN_TEXT];
    int lus, ecrits, reste = length, rc;
    while(1){

	if(reste < BUF_LEN_TEXT) 
	    lus = read(fd, buf, reste);
	else
	    lus = read(fd, buf, BUF_LEN_TEXT);

	if (lus < 0){
	    perror("read:affiche_texte(int,int)");
	    return ERROR_READ_DAZI;
	}
	else{
	    if(lus == 0){
		break;
	    }
	    else{
	      ecrits = write(STDOUT_FILENO, buf, lus);
	      printf("\n");
	      if(ecrits < 0){
		perror("write:affiche_texte(int,int)");
		return ERROR_WRITE_TEXT;
	      }
	      
	    }
	}

	reste -= lus;
    }
    //On replace le curseur comme avant la lecture
    rc = lseek(fd, -length, SEEK_CUR);
    if(rc < 0){
      perror("lseek:affiche_texte(int,int)");
      return ERROR_SEEK_DAZI;
    }
    return 0;
}

// TODO
int recupere_length(int fd){
  int rc, length = 0;
  unsigned char buflen[LENGTH_SIZE];
  // On avance ! ******************************
  rc = read(fd, buflen, LENGTH_SIZE);
  if(rc < 0){
    perror("read:recupere_length");
    return ERROR_READ_DAZI;
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
      return ERROR_READ_DAZI;
    }
  }
  return length;
}

/* TODO

 */
int recupere_date(int fd){
  //unsigned int date;
  return 0;
}

/* TODO */
int affiche_tlv(int fd){
  int rc;
  unsigned char typetlv;
  int length = -1;
  // On est a ?
  rc = lseek(fd, 0, SEEK_CUR);
  if(rc < 0){
      perror("lseek:affiche_tlv");
      return ERROR_SEEK_DAZI;
  }
  // On avance ! ********************
  rc = read(fd, &typetlv, TYPE_SIZE);

  if(rc < 0){
    perror("read:affiche_tlv");
    return ERROR_READ_DAZI;
  }
      
  if(rc == TYPE_SIZE){

    if (typetlv == TYPE_PAD1) {
      printf("TLV type: Pad1\n");
      return TYPE_PAD1;
    }
    // On avance ! ******************************
    length = recupere_length(fd);
    


    
    switch(typetlv){      
    case 1: {
      printf("TLV type: PadN, length: %d\n", length);
      
    }
      break;
      
    case 2: {
      printf("TLV type: Text, length: %d\n", length);
      rc = affiche_texte(fd, length);
      if(rc < 0) 
	return rc;
    }
      break;
      
      
      
    case 3: {
      printf("TLV type: PNG, length: %d\n", length);
    }
      break;

    case 4: {
      printf("TLV type: JPEG, length: %d\n", length);
    }
      break;
      

    case 5: {
      printf("TLV type: Compound, length: %d\n", length);
      /* TODO
	 Prevoir d'une façon recursive de prevoir 
	 la profondeur d'un TLV
	 
       */
      return typetlv;
    }
      break;
      
      	
    case 6: {
      printf("TLV type: Dated, length: %d\n", length);
      
      //pour sauter le champ date
      rc = lseek(fd, DATE_SIZE, SEEK_CUR);
      if(rc < 0){
	perror("lseek:affiche_tlv:dated");
	return ERROR_SEEK_DAZI;
      }
      /*on rappelle la fonction dans le TLV Dated
      TODO prevoir recursivement pour lire tous les
      TLV d'un meme compound
      */
      affiche_tlv(fd);
      printf("\n");
      return typetlv;
    }
      break;
      
      // On saute automatiquement les donnees du TLV
      
    default: {
      printf("TLV inconnu: type:%d length:%d\n", typetlv, length);
    }
    }
    //fin switch




    // On avance ! ***************************
    rc = lseek(fd, length, SEEK_CUR);
    if(rc < 0){
      perror("lseek:affiche_tlv");
      return ERROR_SEEK_DAZI;
    }
    return typetlv;

  }
  else{
    
    if(rc == 0){
      return EOF_DAZI;
    }
  } 
   
  return ERROR_UNKNOW;
}


/* TODO */
void affiche_dazibao(char * dazibao){

  int fd, rc;
  
  /* TODO
     Prise en charge du verrou 
  */


  /* Ouverture du fichier  */
  fd = open(dazibao, O_RDONLY);
  if(fd < 0){
    perror("open:affiche_dazibao(char *)\n");
    return;
  }  

  //On passe l'entete du fichier
  rc = lseek(fd, HEADER_SIZE, SEEK_SET);
  if(rc < 0){
    perror("lseek:affiche_dazibao");
    return;
  }


  /* Lecture et affichage du fichier */
  do{

    rc = affiche_tlv(fd);
    if(rc < EOF_DAZI){
      printf("Erreur d'affichage:%d\n", rc);
      close(fd);
      return;
    }

  }while(EOF_DAZI < rc);
  
  close(fd);
}

