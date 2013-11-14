#define HEADER_ERROR -53
#define ERROR_READ_TLV -2
#define EOF_DAZI -1

/* Verifie l'entete d'un fichier 
   cense etre un dazibao
 */
int verifie_entete(char * dazibao);

/* Affiche le contenu d'un dazibao   
*/
int affiche_dazibao(char * dazibao);

