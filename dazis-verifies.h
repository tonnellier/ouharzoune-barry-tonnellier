/* Vérifie l'entete d'une liste de fichiers dazibao
   puis renvoie la liste des fichiers corrects dans 
   une structure les contenants avec leur nombre associé
*/
struct dazis_verifies * verifie_fichiers_dazibao(int argc, char ** argv);

/* Affiche le menu principal afin de selectionner le dazibao
   a modifier selon la structure verifiée sdv
 */
void affiche_menu_choix_fichier(struct dazis_verifies * sdv);
