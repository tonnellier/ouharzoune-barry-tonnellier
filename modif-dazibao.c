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




/* AFFICHE_TLV */

int affiche_tlv(int fd, int tailledonnees){
  int i = 0, rc, length;
  char typetlv;
  
  printf("--------------------------------\n");
  
  while(i < tailledonnees){
    
    //On lit le type du TLV puis on avance
    //d'un octet
    rc = read(fd, &typetlv, TYPE_SIZE);
    if(rc < 0){
      perror("read:affiche_tlv");
      return ERROR_READ_DAZI;
    }
    i+=TYPE_SIZE;

    //Si on est dans le cas d'un Pad1
    //Le champ length et autres n'exitent pas
    //Donc on sautes ces etapes
    if (typetlv == TYPE_PAD1) {
	printf("TLV type: Pad1\n");
	continue;
    }

    //on lit le champ length puis on
    //avance de 3 octets
    length = recupere_length(fd);
    if(length < 0){
      perror("read:recupere_length()");
      return ERROR_READ_DAZI;
    }
    i+= LENGTH_SIZE;

    //specialement pour le dated on saute le champ Date
    //On lit le champ date de 4 octets
    //puis on avance
    if(typetlv == 6){
      rc = lseek(fd, DATE_SIZE, SEEK_CUR);
      if(rc < 0){
	perror("lseek:affiche_tlv");
	return ERROR_SEEK_DAZI;
      }
      i+=DATE_SIZE;
    }    

    //----------------------TO CHECK-----------------------
    if(rc == TYPE_SIZE){      

      //recupere la taille d'un TLV et avance
      //jusqu'a la donnée de celui-ci
     
      
      switch(typetlv){
	
      case 1: {
	printf("TLV type: PadN, length: %d\n", length);
      
      }	break;
	
      case 2: {
	printf("TLV type: Text, length: %d\n", length);
	
	//Affiche le texte et laisse le curseur 
	//du fd initialement donné
	  rc = affiche_texte(fd, length);
	  if(rc < 0) {
	    printf("Erreur:affiche_tlv():TLV Text\n");
	    return rc;
	  }

      }	break;
      
      case 3: {
	printf("TLV type: PNG, length: %d\n", length);
      }	break;

      case 4: {
	printf("TLV type: JPEG, length: %d\n", length);
      }	break;
      
	
      case 5: {
	printf("TLV type: Compound, length: %d\n", length);
	
	rc = affiche_tlv(fd, length);
	if(rc < 0){
	  printf("Erreur:affiche_tlv():TLV Compound\n");
	  return rc;
	}
	continue;

      }	break;
      
	
      case 6: {
	printf("TLV type: Dated, length: %d\n", length);

	rc = affiche_tlv(fd, length);
	if(rc < 0){
	  printf("Erreur:affiche_tlv():TLV Dated\n");
	  return rc;
	}
	continue;

      }	break;
	
	// On saute automatiquement les donnees du TLV
	
      default: {
	printf("TLV inconnu: type:%d length:%d\n", typetlv, length);
      }
	
      }
      //fin switch
      
     rc = lseek(fd, length, SEEK_CUR);
     if(rc < 0){
       perror("lseek:affiche_tlv");
       return ERROR_SEEK_DAZI;
     }
     
     i+=length;
     
    }
     // ---------------END TO CHECK----------------------------------
    
  }
  printf("--------------------------------\n");
  return 0;
}





/* TODO */
void affiche_dazibao(char * dazibao){

  int fd, rc, tailledonnees;
  
  /* TODO
     Prise en charge du verrou 
  */


  /* Ouverture du fichier  */
  fd = open(dazibao, O_RDONLY);
  if(fd < 0){
    perror("open:affiche_dazibao(char *)\n");
    return;
  }  
  tailledonnees = lseek(fd, 0,SEEK_END);
  if(tailledonnees < 0){
    perror("lseek:affiche_dazibao()");
    return;
  }
  
  rc = lseek(fd, HEADER_SIZE, SEEK_SET);
  if(rc < 0){
    perror("lsseek:affiche_dazibao()");
    return;
  }
  //On recupere le nombre d'octets dans le fichier en excluant l'entete
  //tailledonnees = tailledonnees - rc;
  
  rc = affiche_tlv(fd, tailledonnees-HEADER_SIZE);
  if(rc < 0){
    printf("Erreur d'affichage des TLVs\n");
  }

  close(fd);
  return;
}

