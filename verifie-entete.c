#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "norme.h"



/* TEST OK */
int verifie_entete(char * dazibao){
  unsigned char head[HEADER_SIZE];
  int fd, rc, verrou;
  
  /* TODO
    ajouter la pose d'un verrou flock
  */

  fd = open(dazibao, O_RDONLY);
  if(fd < 0){
    perror("open:verifie_entete()");
    close(fd);
    return fd;
  }
  
  /* VERROUILLAGE */
  verrou = flock(fd, LOCK_SH);
  if(verrou < 0){
    perror("flock:verifie_entete()");
    return ERROR_LOCK_FILE;
  }

  /* Lecture de l'entete */
  rc =  read(fd, head, HEADER_SIZE);
  if(rc < 0){
    perror("read:verifie_entete()");
    close(fd);
    return rc;
  }else{


    if(rc != HEADER_SIZE){
      printf("La taille de l'entete n'a pu etre lue\n");
      return HEADER_ERROR;
    }else{
      if(head[0] != MAGIC){
	printf("Le numero magic n'est pas correct:MAGIC=%d\n",head[0]);
	return HEADER_ERROR;
      }
      if(head[1] != VERSION){
	printf("Le numero de version n'est pas correct:");
	printf("VERSION=%d\n", head[1]);
	return HEADER_ERROR;
      }
    }
    
  }

  /* DEVERROUILLAGE */
  verrou = flock(fd, LOCK_SH);
  if(verrou < 0){
    perror("flock:verifie_entete()");
    return ERROR_LOCK_FILE;
  }

  close(fd);
  return 0;
}
