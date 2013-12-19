#define SANS_CHOIX 0
#define QUITTER -1
#define TAILLE_CHOIX 128
#define MAX_FICHIERS 128

/* Gere le menu principal pour selectionner
   un dazibao
 */
int main(int argc, char ** argv);

/* Vérifie l'entete d'une liste de fichiers dazibao
   puis renvoie la liste des fichiers corrects dans 
   une structure les contenants avec leur nombre associé
*/
struct dazis_verifies * verifie_fichiers_dazibao(int argc, char ** argv);

/* Affiche le menu principal afin de selectionner le dazibao
   a modifier selon la structure verifiée sdv
 */
void affiche_menu_choix_fichier(struct dazis_verifies * sdv);

/* Permet de lire le choix fichier de l'utilisateur
   dans les menus et verifie sa cohérence.
   Elle renvoie le choix associé.
 */
int recupere_choix_menu(int nb_choix);





/* Gere le sous-menu afin de modifier le dazibao
   (affichage, ajout d'un TLV, suppression d'un TLV...)
*/
void menu_modif_dazibao(char * dazibao);

/*Affiche le sous-menu afin de modifier un dazibao
  selectionné
*/
void affiche_menu_modif_dazibao(char * dazibao);
