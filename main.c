#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "main.h"
#include "norme.h"
#include "modif-dazibao.c"



int recupere_choix_menu(int nb_choix){
  char choix_string[TAILLE_CHOIX];
  int choix_fichier = SANS_CHOIX;
  int rc = 0;

  rc = scanf("%s", choix_string);
  if(!rc){
    perror("scanf\n");
    exit(EXIT_FAILURE);
  }else{      
    rc = strcmp(choix_string,"q");
    if(!rc){
      choix_fichier = QUITTER;
    }else{
      /* atoi renvoie 0 en cas d'erreur
	 comme choix ne dois pas valoir 0
	 pour ne pas quitter on change celle-ci
      */
      choix_fichier = atoi(choix_string);
      // choix non disponible
      if(choix_fichier <= 0 || nb_choix < choix_fichier){ 
	choix_fichier = SANS_CHOIX;
	printf("Erreur de choix !\n");
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
  /*
  printf("2 - ajouter un TLV\n");
  printf("3 - supprimer un TLV\n");
  printf("4 - compacter le dazibao\n");
  */
  printf("q - quitter pour le menu précédent\n");
  printf("******************************************\n");
}
/* TODO 5
   Deuxieme gros menu a finir
 */
void menu_modif_dazibao(char * dazibao){  
  int choix, rc;

  while(1){
    affiche_menu_modif_dazibao(dazibao);
    choix = recupere_choix_menu(1);
    
    if(choix == QUITTER) return;
    switch(choix){

    case 1:
	rc = affiche_dazibao(dazibao);
	if(rc == HEADER_ERROR){
	  printf("L'entete fichier n'est pas correct !");
	  return;
	}
	break;
    }
    
    /* TODO 2
       recuperer le choix
    */
  

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



/* TODO 1
 */
char * lire_string(){
  printf("Tapez le repertoire du fichier à trouver");
  return NULL;
} 




void affiche_menu_choix_fichier(int argc, char ** argv){
  int i = 1;
  printf("\n\n************** choix fichier *************\n");
  for(i = 1; i < argc; i++){
    printf("%d - modifier le dazibao \"%s\" ?\n", i, argv[i]);
  }
  if(argc == 1)
    printf("%d - modifier un dazibao\n", argc);
  else
    printf("%d - modifier un autre dazibao\n", argc);
  printf("q - quitter\n");
  printf("******************************************\n");
}



int main(int argc, char ** argv){  
  int choix_fichier = SANS_CHOIX;

  do{
    affiche_menu_choix_fichier(argc,argv);

    choix_fichier = recupere_choix_menu(argc);

    if(choix_fichier == QUITTER) return 0;
    if(choix_fichier == SANS_CHOIX) continue;
    if(choix_fichier < argc){
	menu_modif_dazibao(argv[choix_fichier]);
    }else{
      /* TODO 1
	 faire une fonction qui récupere 
	 un nom de fichier (un char *)
       */
      printf("Recherche fichier non prise en charge !\n");
    }    

  }while(1);

  return 0;
}
