#define HEADER_ERROR -53
#define ERROR_READ_TLV -2
#define EOF_DAZI -1

/* Verifie l'entete d'un fichier 
   cense etre un dazibao et renvoie le
   resultat de la verification.
   La fonction renvoie 0 si tout va bien, 
   sinon un entier negatif.
 */
int verifie_entete(char * dazibao); // TEST OK



/* Affiche le texte d'un TLV. Le descripteur 
   de fichier fd est deja positionné correctement
   au debut du texte.
   Length est la taille des données à lire.
   Renvoie 0 si tout va bien, sinon un 
   entier negatif.
 */
int affiche_texte(int fd, int length); //TEST OK - REMAIN



/* Lit la taille d'un TLV à partir d'un
   descripteur de fichier fd positionne
   au bon endroit puis renvoie 
   le champs length de celui-ci.
   Renvoie la taille length si tout va bien, 
   sinon un entier negatif.
*/
int recupere_length(int fd); // TEST OK


/* Lit la date d'un TLV de type dated.
   Renvoie un entier positif pour la date
   sinon un entier negatif en cas d'erreur
 */
int recupere_date(int fd);

/* Affiche un TLV à partir d'un descripteur de 
   fichier positionné au début de celui-ci.
   Renvoie le type du TLV lu.
   Renvoie un entier negatif en cas de
   fin du fichier ou d'erreur.
 */
int affiche_tlv(int fd); //TEST EN COURS



/* Affiche le contenu d'un dazibao
   Renvoie 0 si tout va bien, sinon
   un entier negatif.
*/
void affiche_dazibao(char * dazibao);

