/*
  Recupere un entier a partir d'une chaine de caracteres
  lue en ligne de commande
 */
int lire_entier();


/*
  Permet de recuperer une chaine de caractere 
  entree en ligne de commande
 */
char * lire_string();


/* Permet de lire le choix fichier de l'utilisateur
   dans les menus et verifie sa cohérence.
   Elle renvoie le choix associé.
 */
int recupere_choix_menu(int nb_choix);


/*Affiche le sous-menu afin de modifier un dazibao
  selectionné
*/
void affiche_menu_modif_dazibao(char * dazibao);


/* Gere le sous-menu afin de modifier le dazibao
   (affichage, ajout d'un TLV, suppression d'un TLV...)
*/
void menu_modif_dazibao(char * dazibao);


/* Gere le menu principal pour selectionner
   un dazibao
 */
int main(int argc, char ** argv);
