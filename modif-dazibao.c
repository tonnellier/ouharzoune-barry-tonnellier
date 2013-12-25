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
#define ERROR_WRITE_DAZI -6
#define ERROR_SEEK_DAZI -5
#define ERROR_WRITE_TEXT -3
#define ERROR_UNKNOW -4
#define EOF_DAZI -1
#define BUF_LEN_TEXT 64
#define BUF_LEN_CPY 1024

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
    printf("-------------Text--------------\n");
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
    printf("------------Fin-Text------------\n");
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
/*
int recupere_date(int fd){
  //unsigned int date;
  return 0;
}
*/



/* AFFICHE_TLV */

int affiche_tlv(int fd, int tailledonnees, int num){
  int i = 0, rc, length, rc3;
  unsigned char typetlv;

  while(i < tailledonnees){
    
    printf("tailledonnees=%d\n", tailledonnees);

    //On lit le type du TLV puis on avance
    //d'un octet
    rc = read(fd, &typetlv, TYPE_SIZE);
    if(rc < 0){
      perror("read:affiche_tlv()");
      return ERROR_READ_DAZI;
    }
    i+=TYPE_SIZE;
    
    //Si on est dans le cas d'un Pad1
    //Le champ length et autres n'exitent pas
    //Donc on sautes ces etapes
    if (typetlv == TYPE_PAD1) {
      printf("TLV%d type: Pad1, length: 0\n", num);
      num++;
      continue;
    }

    //on lit le champ length puis on
    //avance de 3 octets
    length = recupere_length(fd);
    if(length < 0){
      printf("read:recupere_length()\n");
      return ERROR_READ_DAZI;
    }
    i+= LENGTH_SIZE;
    
    
    if(rc == TYPE_SIZE){
      
      
      switch(typetlv){
		
      case 1: {
	printf("TLV%d type: PadN, length: %d\n", num, length);
      
      }	break;
	
      case 2: {
	printf("TLV%d type: Text, length: %d\n", num, length);
	
	//Affiche le texte et laisse le curseur 
	//du fd initialement donné
	/*  
	    rc = affiche_texte(fd, length);
	    if(rc < 0) {
	    printf("Erreur:affiche_tlv():TLV Text\n");
	    return rc;
	    }
	*/

      }	break;

      case 3: {
	printf("TLV%d type: PNG, length: %d\n", num, length);
      }	break;
	
      case 4: {
	printf("TLV%d type: JPEG, length: %d\n", num, length);
      }	break;
	


      case 5: {
	printf("TLV%d type: Compound, length: %d\n", num, length);
	
	printf("----------Compound-------------\n");
	num = affiche_tlv(fd, length, num+1);
	printf("----------Fin-Compound------------\n");
	
	if(num < 0){
	  printf("Erreur:affiche_tlv():TLV Compound\n");	 
	  return num;
	}
	
	//on a avancé avec affiche_tlv(),
	//on augmente logiquement i
	i+=length;
	continue;
	
      }	break;



      case 6: {
	printf("TLV%d type: Dated, length: %d\n", num, length);

	//On recupere le champ Date pour éviter la suite de TLV
	rc3 = lseek(fd, DATE_SIZE, SEEK_CUR);
	if(rc3 < 0){
	  perror("lseek:affiche_tlv()");
	  return ERROR_SEEK_DAZI;
	}
	
	
	printf("-----------Dated---------------\n");
	num = affiche_tlv(fd, length-DATE_SIZE, num+1);
	printf("----------Fin-Dated--------------\n");
	
	  
	if(num < 0){
	  printf("Erreur:affiche_tlv():TLV Dated\n");
	  return num;
	}
	  
	//on a avancé avec affiche_tlv,
	//on augmente logiquement i 
	i+=length;
	continue;
	
      }	break;


	
      default: {
	printf("TLV%d inconnu: type:%d length:%d\n", num, typetlv, length);
      }
	
      }//fin switch




      rc = lseek(fd, length, SEEK_CUR);
      if(rc < 0){
	perror("lseek:affiche_tlv()");
	return ERROR_SEEK_DAZI;
      }

      i+=length;
      
      num++;

    }else{
      printf("La taille du type ne correspond pas !\n");
      return -1;
    }
    

  }

  return num;
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
  printf("---------Dazibao------------\n\n");
  printf("taillesdonnees=%d\n", tailledonnees);
  rc = affiche_tlv(fd, tailledonnees-HEADER_SIZE, 1);
  printf("\n----------Fin-Dazibao------------\n");
  if(rc < 0){
    printf("Erreur d'affichage des TLVs\n");
  }

  close(fd);
  return;
}


int supprime_tlv_aux(int fd, int tailledonnees, int num){
  int rc, length, cur = 1, i=0;
  unsigned char typetlv;

  if (num < cur){
    printf("numero de TLV incorrect num=%d\n", num);
    return -1;
  }

  //On passe tous les TLVs qui précèdent celui a supprimer
  while(i < tailledonnees){
    
    //Cas du bon TLV a supprimer
    if(cur == num){
      
      rc = read(fd, &typetlv, TYPE_SIZE);
      if(rc < 0){
	perror("read:supprime_tlv_aux()");
	return ERROR_READ_DAZI;
      }
      //Cas Pad1 (on laisse tel quel)
      if(typetlv == 0) return 0;

      //Sinon on transforme le TLV en PadN.
      rc = lseek(fd, -TYPE_SIZE,SEEK_CUR);
      if(rc < 0){
	perror("lseek");
	return ERROR_SEEK_DAZI;
      }
      printf("offset debut du TLV a supprimer=%x\n", rc);
      
      

      typetlv = 1;
      //On le remplace par un PaDN
      rc = write(fd, &typetlv, TYPE_SIZE);
      if(rc < 0){
	perror("write:supprime_tlv_aux()");
	return ERROR_WRITE_DAZI;
      }
     
      printf("Le TLV a ete supprimé !\n");
      
      return 0;
    }

    //On lit le type du TLV
    rc = read(fd, &typetlv, TYPE_SIZE);
    if(rc < 0){
      perror("read:supprime_tlv_aux()");
      return rc;
    }
    i += TYPE_SIZE;

    printf("typetlv=%d\n", typetlv);

     //Test du Pad1 qui est sur un seul octet
    if (typetlv == TYPE_PAD1) {
      cur ++;
      continue;
    }
    //on lit le champ length puis on
    //avance de 3 octets
    length = recupere_length(fd);
    if(length < 0){
      printf("Erreur:supprime_tlv_aux():recupere_length()");
      return length;
    }
    i+= LENGTH_SIZE;
    //------------------------

    printf("length=%d\n", length);
    
    //Cas du compound
    if(typetlv == 5){
      
      cur++;
      continue;
    }
    

    //Cas du dated
    if(typetlv == 6){
      rc = lseek(fd, DATE_SIZE, SEEK_CUR);
      if(rc < 0){
	perror("lseek:supprime_tlv_aux()");
	return ERROR_SEEK_DAZI;
      }
      i+= DATE_SIZE;

      //On ne passe pas les donnees
      //dans le dated, on refait un tour de 
      //boucle pour pouvoir les lire
      
      cur++;
      continue;
      
    }
    


    //On passe les donnees du TLV en question
    rc = lseek(fd, length, SEEK_CUR);
    if(rc < 0){
      perror("lseek:supprime_tlv_aux()");
      return ERROR_SEEK_DAZI;
    }
    //ATTENTION au cas 5 et 6
    i+=length;
    
    printf("nouvel offset a la fin du TLV=%d\n\n", rc);
    
    cur++;

  }//fin while
  
  
  
  
  
  printf("Le TLV numero=%d n'a pas ete modifie,", num);
  printf("il n'existe peut etre pas\n");
  return -1;
}


int supprime_tlv(char * dazibao, int num){
  int fd, rc, tailledonnees;
  
  //On ouvre le fichier
  fd = open(dazibao, O_RDWR);
  if(fd < 0){
    perror("open:supprime_tlv()");
    return errno;
  }
  tailledonnees = lseek(fd, 0, SEEK_END);
  if(tailledonnees < 0){
    perror("lseek:affiche_tlv()");
    return ERROR_SEEK_DAZI;
  }
  tailledonnees -= HEADER_SIZE;

  //On passe l'entete du dazibao
  rc = lseek(fd, HEADER_SIZE, SEEK_SET);
  if(rc < 0){
    perror("lseek:supprime_tlv()");
    return errno;
  }
  
  //On appelle la fonction qui va
  //transformer le TLV en PadN
  rc = supprime_tlv_aux(fd, tailledonnees, num);
  if(rc < 0){
    printf("Erreur:supprime_tlv()\n");
    return rc;
  }

  close(fd);
  return 0;
}

unsigned char * int_to_char4(unsigned int entier){
  unsigned char * char4 = (unsigned char *)malloc(sizeof(unsigned char) *4);
  int i;
  
  if(char4 == NULL){
    printf("malloc:int_to_char4()\n");
    return NULL;
  }

  for(i = 3; 0 <= i; i--){
    
    char4[i] = (unsigned char) entier;
    entier = entier >> 8;
  }
  return char4;
}

int ajoute_tlv(char * dazibao, unsigned char typetlv, int length, 
	       char * fichierdonnees){

  int fd,  ecrits,  cpt_donnees = 0;
  int fd2, lus;
  int rc;
  unsigned char * len3;
  char buf[BUF_LEN_CPY];
  
  //On ouvre le fichier afin d'ajouter des donnees a la fin de celui-ci
  fd = open(dazibao, O_WRONLY | O_APPEND);
  if(fd < 0){
    perror("open:ajoute_tlv()");
    return ERROR_READ_DAZI;
  }

  switch(typetlv){
  



    //Pad1
  case 0: {
    //On ecrit le type du TLV
    ecrits = write(fd, &typetlv, TYPE_SIZE);
    if(ecrits < 0){
      perror("write:ajoute_tlv()");
      return ERROR_WRITE_DAZI;
    }
  }break;





    //PadN
  case 1: {
    //On ecrit le type du TLV
    ecrits = write(fd, &typetlv, TYPE_SIZE);
    if(ecrits < 0){
      perror("write:ajoute_tlv()");
      return ERROR_WRITE_DAZI;
    }

    //On écrit la taille du PadN

    //On met l'entier length dans un buffer (len3)
    len3 = int_to_char4(length);
    if(len3 == NULL){
      return -1;
    }
    //On avance d'un octets pour avoir le buffer len3 sur 3 octets
    //au lieu de 4
    len3++;
    //On ecrit la taille
    ecrits = write(fd, len3, LENGTH_SIZE);
    if(ecrits < 0){
      perror("write:ajoute_tlv()");
      return errno;
    } 

    cpt_donnees = length;
    //On ecrit des donnees au hasard afin de remplir le PadN
    //Mais on le fait aussi pour tout autre TLV
    //Car si l'on arrive pas a lire le fichier source de donnees
    //On aura rempli le fichier avec des donnees de taille length
    //Et on laissera le TLV en PadN
    do{
      if(cpt_donnees < BUF_LEN_CPY)
	ecrits = write(fd, buf, cpt_donnees);
      else
	ecrits = write(fd, buf, BUF_LEN_CPY);
      if(ecrits < 0){
	perror("write:ajoute_tlv()");
	return errno;
      }
      cpt_donnees -= ecrits;

    }while(0 < cpt_donnees);
    printf("Les donnees du PadN sont ecrites !\n");
    
  }break;





    //autres TLVs
  default: {
    
    //On ouvre le second fichier source d'abord
    //Pour verifier que fichier existe
    //et pour y recuperer sa taille
    fd2 = open(fichierdonnees, O_RDONLY);
    if(fd2 < 0){
      perror("open:ajoute_tlv()");
      return errno;
    }

    
    //On recupere sa taille
    length = lseek(fd2, 0, SEEK_END);
    if(length < 0){
      perror("lseek:ajoute_tlv");
      return errno;
    }
      
    rc = lseek(fd2, 0, SEEK_SET);
    if(rc < 0){
      perror("lseek:ajoute_tlv");
      return errno;
    }
    

    //On commence l'ecriture

    //On ecrit le type du TLV
    ecrits = write(fd, &typetlv, TYPE_SIZE);
    if(ecrits < 0){
      perror("write:ajoute_tlv()");
      return ERROR_WRITE_DAZI;
    }

    //On met l'entier length dans un buffer (len3)
    len3 = int_to_char4(length);
    if(len3 == NULL){
      return -1;
    }
    //On avance d'un octets pour avoir le buffer len3 sur 3 octets
    //au lieu de 4
    len3++;

    //On ecrit la taille
    ecrits = write(fd, len3, LENGTH_SIZE);
    if(ecrits < 0){
      perror("write:ajoute_tlv()");
      return errno;
    } 
    
    //Copie de fichierdonnees dans le TLV
    do{
      lus = read(fd2, buf, BUF_LEN_CPY);
      if(lus < 0){
	perror("read:ajoute_tlv()");
	return errno;
      }

      ecrits = write(fd, buf, lus);
      if(ecrits < 0){
	perror("write:ajoute_tlv()");
	return errno;
      }
    }while(0 < lus);

  }

  }//fin switch

  return 0;
}

int compacte(char * dazibao){
  /*
  int fd_read, fd_write, lus, ecrits, length, lus2, rc, cpt_donnees;
  char typetlv;
  char buf[BUF_LEN_CPY];
  unsigned char * len3;

  fd_read = open(dazibao, O_RDONLY);
  if(fd_read < 0){
    perror("read:compacte()");
    return ERROR_READ_DAZI;
  }

  fd_write = open(dazibao, O_WRONLY | O_NONBLOCK);
  if(fd_write < 0){
    perror("read:compacte()");
    return ERROR_READ_DAZI;
  }

  do{
    //lire le type du tlv
    lus = read(fd_read, &typetlv, TYPE_SIZE);
    if(lus < 0){
      perror("read:compacte()");
      return ERROR_READ_DAZI;
    }

    //On ignore les donnees du Pad1
    if(typetlv == 0) continue;
    
    length = recupere_length(fd_read);
    if(length < 0){
      printf("Erreur:compacte():length\n");
      return length;
    }
    
    //On ignore les donnees du PadN
    if(typetlv == 1){    

      rc = lseek(fd_read, length, SEEK_CUR);
      if(rc < 0){
	perror("lseek:compacte()");
	return errno;
      }
      continue;
    }


    //Pour tout autre TLV, on effectue la copie

    //On copie le type du TLV
    ecrits = write(fd_write, &typetlv, TYPE_SIZE);
    if(ecrits < 0){
      perror("write:compacte()");
      return ERROR_WRITE_DAZI;
    }
    
    //On met l'entier length dans un buffer (len3)
    len3 = int_to_char4(length);
    if(len3 == NULL){
      return -1;
    }
    //On avance d'un octets pour avoir le buffer len3 sur 3 octets
    //au lieu de 4
    len3++;
    //On ecrit la taille
    ecrits = write(fd_write, len3, LENGTH_SIZE);
    if(ecrits < 0){
      perror("write:ajoute_tlv()");
      return errno;
    } 

    cpt_donnees = length;

    //On copie les donnees du TLV
    do{
      
      if(BUF_LEN_CPY < cpt_donnees)
	lus2 = read(fd_read, buf, BUF_LEN_CPY);
      else
	lus2 = read(fd_read, buf, cpt_donnees);
      if(lus2 < 0){
	perror("read:compacte()");
	return ERROR_READ_DAZI;
      }
      cpt_donnees -= lus2;

      ecrits = write(fd_write, buf, lus2);
      if(ecrits < 0){
	perror("write:compacte()");
	return ERROR_WRITE_DAZI;
      }
      
      }while(0 < cpt_donnees);

  }while(0 < lus);

  close(fd_read);
  close(fd_write);
}
  */
  return 0;
}
