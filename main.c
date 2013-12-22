#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "main.h"
#include "norme.h"
#include "modif-dazibao.c"

struct dazis_verifies {
  int nb_dazis;
  char ** dazis;
};

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


int lire_entier(){
  
  char str[TAILLE_CHOIX];
  int rc;
  
  
  rc = scanf("%s", str);
  
  if(rc <= 0){
    perror("scanf()");
    return errno;
  }
  
  rc = atoi(str); 
 
  if(rc == 0){
    printf("chaine=\"%s\" incorrecte\n", str);
  }
 
  return rc;
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
  
  //printf("2 - ajouter un TLV\n");
  printf("3 - supprimer un TLV\n");
  //printf("4 - compacter le dazibao\n");
  
  printf("q - quitter pour le menu précédent\n");
  printf("******************************************\n");
}
/* TODO 5
   Deuxieme gros menu a finir
 */
void menu_modif_dazibao(char * dazibao){
  int choix, rc,num;
  //nombre d'options de modification d'un dazibao
  int nbopt = 3;
  
  while(1){
    affiche_menu_modif_dazibao(dazibao);
    choix = recupere_choix_menu(nbopt);
    
    if(choix == QUITTER) return;

    switch(choix){

    case 1: {
	affiche_dazibao(dazibao);
    }
      break;

    case 2: {
      printf("AJOUT non implémenté\n");
      //------------------------AJOUT------
    }
      break;
    case 3: {
      printf("Entrer le numero du TLV a supprimer !\n");
      num = lire_entier();
      rc = supprime_tlv(dazibao, num);
      printf("supprime_tlv(%s, %d)=%d\n", dazibao,num, rc);
      if(rc < 0){
	printf("Erreur:menu_modif_dazibao()\n");
      }
      
    }
      break; 

    default: {
      printf("Mauvais choix\n");
    }

    }
    
    /* TODO 4
       a faire les fonctionnalités :
       1 - afficher le dazibao
       2 - ajouter un TLV
       3 - supprimer un TLV
       4 - compacter le dazibao
       q - quitter
    */
  }
  return;
}

void affiche_menu_choix_fichier(struct dazis_verifies * sdv){
  int i;
  printf("\n\n************** choix fichier *************\n");
  for(i = 1; i <= sdv->nb_dazis; i++){
    printf("%d - modifier le dazibao \"%s\" ?\n", i, sdv->dazis[i-1]);
  }
  printf("q - quitter\n");
  printf("******************************************\n");
  return;
}

struct dazis_verifies *  verifie_fichiers_dazibao(int argc, char ** argv){
  struct dazis_verifies * sdv = NULL;
  int i, rc, len;
  //initialisation de la structure pour stocker les dazibaos corrects
  sdv = (struct dazis_verifies *)malloc(sizeof(struct dazis_verifies));
  if(sdv == NULL){
    perror("malloc-1:verifie_fichiers_dazibao()");
    return NULL;
  }
  sdv->nb_dazis = 0;
  sdv->dazis = (char **)malloc(sizeof(char *)*MAX_FICHIERS);
  if(sdv->dazis == NULL){
    perror("malloc-2:verifie_fichiers_dazibao()");
    return NULL;
  }
  
  //parcours et vérification des dazibaos donnés dans argv
  for(i = 1; i < argc; i++){
    rc = verifie_entete(argv[i]);
    if(rc == 0){
      len = strlen(argv[i])+1;
      if(len < 0) return NULL;
      sdv->dazis[sdv->nb_dazis] = (char *)malloc(sizeof(char) * len);
      sdv->dazis[sdv->nb_dazis] = 
	strncpy(sdv->dazis[sdv->nb_dazis], argv[i], len);
      if(sdv->dazis[sdv->nb_dazis]==NULL) return NULL;
      sdv->nb_dazis++;
    }else{
      //TODO
      printf("le fichier \"%s\" n'est pas pris en charge\n\n", argv[i]);
    }
  }

  return sdv;
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

  do{
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
    
  }while(1);
  
  return 0;
}
