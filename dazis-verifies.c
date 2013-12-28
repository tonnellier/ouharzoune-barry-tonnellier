#include <stdio.h>
#include <stdlib.h>

#include "verifie-entete.c"

#define MAX_FICHIERS 128

struct dazis_verifies {
  int nb_dazis;
  char ** dazis;
};

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

struct dazis_verifies * verifie_fichiers_dazibao(int argc, char ** argv){
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
