#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "modif-dazibao.h"

int ajout(int n, char buf[], char type, char * dazibao){
  //  open...
  int fic, rc, ecrits;
  fic =open(dazibao, O_RDONLY);
  if(fic <0){
    perror("open:ajout()");
    return ERROR_READ_DAZI;
  }
  rc = lseek(fic, 0, SEEK_END);
  if(rc <0){
    perror("lseek:ajout()");
    return ERROR_SEEK_DAZI;
  }
  ecrits = write(fic, &type, 1);
  if(ecrits < 0){

  }
    //recupere taille (type int) et la mettre dans un buffer de
    //octects
  return 0;
 }


int main(int argc, char ** argv){

  return 0;
}
