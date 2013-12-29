#include "dazis-verifies.c"
#include <sys/types.h>          
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include "service-notif.h"


//SERVICE
int main ( int argc , char ** argv ) {
  /*
  struct sockaddr_un addr ;
  int srv_fd ;
  ssize_t bytes ;
  */
  char buf [ BUF_SIZE ];
  pid_t pid;
  int fd_tube[2];
  int lus, ecrits, i;
  struc stat fichiers[argc]
 
  if(1 < argc){
    printf("pas assez d'args\n");
    return 0;
  }

  //On crée un canal de com
  if (pipe(fd_tube) < 0){
    perror("pipe");
    return errno;
  }

  //On sépare les processus
  pid = fork();
  if(pid < 0){
    perror("fork");
    return errno;
  }



  //FILS 
  //Il envoie les donnés qu'il reçoit au user
  if(pid == 0){
    
    //close(fd[1]);
    lus = read(fd_tube[0], buf, BUF_SIZE);
    if(lus > 0){
      ecrits = write(STDOUT_FILENO, buf, BUF_SIZE);
      if(ecrits < 0){
	perror("write");
	return errno;
      }
    }else{
      
    }

      //ENvoi des modifs
      /*
if ( ( srv_fd = socket ( AF_UNIX , SOCK_STREAM, 0 ) ) < 0)
      perror ( " socket e rro r ");

    memset(&addr , 0 , sizeof ( struct sockaddr_un ) ) ;
    addr.sun_family = AF_UNIX;  
    strncpy(addr.sun_path, SOCKET_RDV, sizeof(addr.sun_path) - 1);  
    if( connect ( srv_fd , ( struct sockaddr * ) &addr , sizeof(struct sockaddr_un ) ) < 0)
      perror ( " connect e rro r " ) ;
    while ( ( bytes = read ( STDIN_FILENO , buf , BUF_SIZE ) ) > 0)
      if ( write ( srv_fd , buf , bytes ) != bytes )
	perror ( " wri te e rro r " ) ;
    if ( bytes < 0)
      perror ( " read error " ) ;


    */
    
    //PERE
  }else{
    //close(fd[0]);
    while(1){
      for(i = 1; i < argc; i++){
	argv[i]
      }
      sleep(5);
    }
  }



  
  return 0;
}
