#define HEADER_ERROR -53
#define ERROR_READ_DAZI -2
#define ERROR_SEEK_DAZI -5
#define ERROR_WRITE_TEXT -3
#define ERROR_UNKNOW -4
#define EOF_DAZI -1

/* Verifie l'entete d'un fichier 
   censé etre un dazibao et renvoie le
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
/*
int recupere_date(int fd);
*/

/* Affiche un TLV à partir d'un descripteur de
   fichier positionné au début de celui-ci.
   l'argument <offsetfin> permet de savoir
   jusqu'a quel endroit du fichier on va devoir
   lire les TLV d'une profondeur égale.
   En sachant qu'a chaque fois qu'on lit un TLV
   compound ou dated on change de profondeur.

   Renvoie 0 si l'affichage s'est bien passé.
   Renvoie un entier negatif en cas de
   fin du fichier ou d'erreur.

   <tailledonnees> permet de compter le nombre d'octets
   restant pour afficher un TLV compliqué
   <num> permet de numeroter les TLV de 1 à n
 */
int affiche_tlv(int fd, int tailledonnees, int num); //TEST EN COURS



/* Affiche le contenu d'un dazibao
   Renvoie 0 si tout va bien, sinon
   un entier negatif.
*/
void affiche_dazibao(char * dazibao);


/* Ouvre le fichier dazibao puis
   appelle supprime_tlv_aux()
   Renvoie 0 si tout va bien
 */
int supprime_tlv(char * dazibao, int num);

/* 
   Supprime un tlv a la position <num>
   Renvoie 0 si tout va bien
 */
int supprime_tlv_aux(int fd, int tailledonnees, int num);


/*
  Renvoie la representation d'un entier positif sous forme 
  de buffer a 4 caractere.
  En cas d'erreur du malloc, renvoie NULL
*/
unsigned char * int_to_char4(unsigned int entier);


/*
  Ajoute un TLV à la fin du fichier dazibao
  Renvoie 0 si tout va bien
  Sinon entier negatif
 */
int ajoute_tlv(char * dazibao, unsigned char typetlv, int length, char * fichierdonnees);
