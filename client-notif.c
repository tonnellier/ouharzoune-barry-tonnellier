#include "dazis-verifies.c"
#include <sys/types.h>          
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include "service-notif.h"


//SERVICE
int main ( int argc , char ** argv ) {
  
  struct sockaddr_un addr ;
  int srv_fd ;
  //ssize_t bytes ;
  
  char buf [ BUF_SIZE ];
  pid_t pid;
  int fd_tube[2];
  int lus, i, rc;
  //int ecrits;
  struct stat fichiers[argc];
  struct stat tmp;
 
  if(argc < 2){
    printf("pas assez d'args\n");
    return 0;
  }
  printf("%s\n", SOCKET_RDV);
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
    
    //creation de socket
    if ( ( srv_fd = socket ( AF_UNIX , SOCK_STREAM, 0 ) ) < 0){
      perror ( " socket e rro r ");
    }
    memset(&addr , 0 , sizeof ( struct sockaddr_un ) ) ;
    addr.sun_family = AF_UNIX;  

    if(strncpy(addr.sun_path, SOCKET_RDV, sizeof(addr.sun_path) - 1) < 0){
      printf("erreur strncpy()\n");
      return -1;
    }  

   
    //connection aux users
      if( connect(srv_fd,(struct sockaddr *)&addr, 
		  sizeof(struct sockaddr_un ) ) < 0){
	perror ( " connect e rro r " ) ;
      }
      printf("connexion au serveur\n");

    while(1){
    //close(fd[1]);
      lus = read(fd_tube[0], buf, BUF_SIZE);
      if(lus < 0){
	perror("read");
	return errno;
      }
      if(0 < lus){
      
	printf("%s\n", buf);
	
	if ( write ( srv_fd , buf , lus) < 0){
	  perror ( " write error " ) ;
	}
	if ( lus < 0){
	  perror ( " read error " ) ;
	}
       
      
      }

    }
    


    
    
    //PERE
  }else{
    //On recupere toutes les infos des fichiers
    for(i = 1; i < argc; i++){
      rc = stat(argv[i], &fichiers[i]);
	if(rc < 0){
	  perror("stat");
	  return errno;
	}

    }
    //On verifie les infos toutes les cinq secondes
    //close(fd[0]);
    
    while(1){
      buf[0]='C';
      for(i = 1; i < argc; i++){

	rc = stat(argv[i], &tmp);
	if(rc < 0){
	  perror("stat");
	  return errno;
	}
	if(fichiers[i].st_mtime != tmp.st_mtime){
	  //TODO envoyer les donnees
	  fichiers[i].st_mtime = tmp.st_mtime;
	  
	  
	  if(strncpy(buf+1, argv[i],BUF_SIZE)< 0){
	    printf("erreur strncpy\n");
	  } 
	  
	  rc = write(fd_tube[1], buf, strlen(argv[i])+14);
	  if(rc < 0){
	    perror("write");
	    return errno;
	  }
	}

      }
      sleep(1);

    }//fin while
    
  }



  
  return 0;
}
