/* entete */
#define MAGIC 53
#define VERSION 0
#define MBZ 0
#define HEADER_SIZE 4

/* taille TLV */
#define TYPE_SIZE 1
#define LENGTH_SIZE 3
#define DATE_SIZE 4

/* types TLV reserves */
#define TYPE_PAD1 0
#define TYPE_PADN 1
#define TYPE_TEXT 2
#define TYPE_PNG 3
#define TYPE_JPEG 4
#define TYPE_COMPOUND 5
#define TYPE_DATED 6

/* types TLV etudiantes*/


/* Erreurs */
#define HEADER_ERROR -53
#define ERROR_READ_DAZI -2
#define ERROR_WRITE_DAZI -6
#define ERROR_SEEK_DAZI -5
#define ERROR_WRITE_TEXT -3
#define ERROR_LOCK_FILE -7
#define ERROR_UNKNOW -4

/* Autres constantes */
#define EOF_DAZI -1
#define BUF_LEN_TEXT 64
#define BUF_LEN_CPY 1024
