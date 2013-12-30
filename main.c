#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <sys/un.h>

#include "service-notif.h"


#include "main.h"
#include "norme.h"
#include "modif-dazibao.c"
#include "dazis-verifies.c"


int lire_entier(){
  
  char * str;
  int rc;
  
  str = lire_string();
  if(str == NULL){
    return -1;
  }

  rc = atoi(str); 
 
  if(rc == 0){
    printf("chaine=\"%s\"\n", str);
  }
 
  return rc;
}


char * lire_string(){
  int rc;
  char * str = (char *)malloc(sizeof(char) * TAILLE_CHOIX);
 
  rc = scanf("%s", str);
  if(rc == 0){
    perror("scanf:lire_string()");
    return NULL;
  }
  return str;
}


int recupere_choix_menu(int nb_choix){
  char choix_string[TAILLE_CHOIX];
  int choix_fichier = SANS_CHOIX;
  int rc = 0;

  rc = scanf("%s", choix_string);
  if(!rc){
    perror("scanf\n");
    return SANS_CHOIX;
  }else{      
    rc = strcmp(choix_string,"q");
    if(!rc){
      return QUITTER;
    }else{
      /* atoi renvoie 0 en cas d'erreur
	 comme choix ne dois pas valoir 0
	 pour ne pas quitter on change celle-ci
      */
      choix_fichier = atoi(choix_string);
      // choix non disponible
      if(choix_fichier < 1 || nb_choix < choix_fichier){ 
	choix_fichier = SANS_CHOIX;
      }
    }
  }
  return choix_fichier;
}


void affiche_menu_modif_dazibao(char * dazibao){
  int i = 0;
  int dazibaolen = (int)strlen(dazibao);
  printf("\n\n");
  for(i = 0; i < 20-(dazibaolen/2); i++)
    printf("*");
  printf(" %s ", dazibao);
  for(i = 0; i < 20-(dazibaolen/2); i++)
    printf("*");
  printf("\n");
  printf("1 - afficher le dazibao\n");
  
  printf("2 - ajouter un TLV\n");
  printf("3 - supprimer un TLV\n");
  printf("4 - compacter le dazibao\n");
  
  printf("q - quitter pour le menu précédent\n");
  printf("******************************************\n");
}



void menu_modif_dazibao(char * dazibao){
  int choix, rc, num, length;
  unsigned char typetlv;
  char * fichierdonnees;
  //nombre de fonctionnalites a appliquer sur un dazibao
  int nbopt = 4;
  
  while(1){
    affiche_menu_modif_dazibao(dazibao);
    choix = recupere_choix_menu(nbopt);
    
    if(choix == QUITTER) return;

    switch(choix){


      //Affichage du dazibao 
    case 1: {
      
	affiche_dazibao(dazibao);
    }      break;


      //Ajout du TLV
    case 2: {

      //type TLV
      printf("Entrez le numero de type du TLV !\n");
      typetlv = (unsigned char)lire_entier();

      //Cas ou le TLV necessite un champ length
      if(0 < typetlv){
	//Pour le PadN on recupere juste sa length
	if(typetlv == 1){
	  //length PadN
	  printf("Entrez la taille length du PadN!\n");
	  length = lire_entier();
	  if(length < 0){
	    printf("Erreur:affiche_tlv():length negatif ou nul\n");
	    continue;
	  }
	  //Cas des autres TLV, on recupere
	  //les donnees a partir d'un fichier
	}else{
	  //nom de fichier de donnees
	  printf("Entrez le nom du fichier de donnees a mettre dans le TLV !\n");
	  fichierdonnees = lire_string();
	  if(fichierdonnees == NULL){
	    continue;
	  }
	}
      }
      printf("typetlv=%d, length=%d, fichierdonnees=%s\n", typetlv, 
	     length, fichierdonnees);
      
      //On ajoute le TLV
      rc = ajoute_tlv(dazibao, typetlv, length, fichierdonnees);
      if(rc < 0){
	printf("Erreur:ajoute_tlv()\n");
      }
      printf("L'ajout du TLV s'est bien passee\n");
      
    }break;
    

      //Supprimer un TLV dans le dazibao
    case 3: {

      printf("Entrer le numero du TLV a supprimer !\n");
      num = lire_entier();

      //On supprime le TLV numero num
      rc = supprime_tlv(dazibao, num);
      printf("supprime_tlv(%s,%d)=%d\n", dazibao, num, rc);
      if(rc < 0){
	printf("Erreur:menu_modif_dazibao()\n");
      }
      
    }break; 


      //Compaction du dazibao
    case 4: {

      rc = compacte(dazibao);
      if(rc < 0){
	printf("Erreur:compacte()\n");
      }

    }break;


    default: {
      printf("Mauvais choix\n");
    }

    }
    
  }

  return;
}


int main(int argc, char ** argv){
  int choix_fichier = SANS_CHOIX;
  struct dazis_verifies * sdv;

  //prétraitement des fichiers à vérifier
  sdv = verifie_fichiers_dazibao(argc, argv);
  if(sdv == NULL || sdv->nb_dazis == 0){
    printf("Il n'y a pas de dazibao valide\n");
    return 0;
  }
  
  
  while(1){

    affiche_menu_choix_fichier(sdv);
  
    choix_fichier = recupere_choix_menu(sdv->nb_dazis);
    
    if(choix_fichier == QUITTER) break;
    if(choix_fichier == SANS_CHOIX) {
      printf("Mauvais choix\n");
      continue;
    }
    if(choix_fichier <= sdv->nb_dazis){
      menu_modif_dazibao(sdv->dazis[choix_fichier-1]);
    }
    
  }
  
  return 0;
  
}
