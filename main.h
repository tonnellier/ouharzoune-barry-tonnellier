#define SANS_CHOIX 0
#define QUITTER -1
#define TAILLE_CHOIX 128

/* Gere le menu principal pour selectionner
   un dazibao
 */
int main(int argc, char ** argv);

/* Affiche le menu principal afin de selectionner le dazibao
   a modifier selon les noms donnés en arguments du programme
 */
void affiche_menu_choix_fichier(int argc, char ** argv);

/* Permet de lire le choix fichier de l'utilisateur
   et verifie sa cohérence.
   Elle renvoie le choix.
 */
int recupere_choix_menu(int nb_choix);





/* Gere le sous-menu afin de modifier le dazibao
   (affichage, ajout d'un TLV, suppression d'un TLV...)
*/
void menu_modif_dazibao(char * dazibao);

/*Affiche le sous-menu afin de modifier un dazibao
  selectionne
*/
void affiche_menu_modif_dazibao(char * dazibao);



/* Lit une chaine de caractere entrée par l'utilisateur
   puis la renvoie.
 */
char * lire_string();
