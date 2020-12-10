#include "header.h"

char clientfifo[CLIENT_FIFO_NAME_LEN];


int main(int argc, char *argv[]){

        int serverFd, clientFd;
        struct request req;
        struct response resp;

        if (argc != 1 ){
            printf("invalid argc\n");
            exit(1);
        }
                

        /* Create our FIFO (before sending request, to avoid a race) */

        umask(0);                   /* So we get the permissions we want */
    snprintf(clientfifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) getpid());
        if (mkfifo(clientFifo, 0666) == -1  && errno != EEXIST){
           perror("mkfifo");
           exit(1);
        }



        /* Construct request message, open server FIFO, and send request */

    req.pid = getpid();
    req.seqLen = strlen(argv[1]);
    strcpy(argv[1], req.filename);

    serverFd = Openfd(SERVER_FIFO, O_WRONLY | O_NONBLOCK, "");
    DisableNONBLOCK(serverFd);
   
           

        /* Open our FIFO, read and display response */
    int clientFd = Openfd(clientfifo, O_RDONLY | O_NONBLOCK, "file receiverFd open");
    DisableNONBLOCK(clientFd);

     write(serverFd, &req, sizeof(struct request));

    char buf[PIPE_BUF] = "";
    int indicator = 0;
    int reallength = PIPE_BUF;

    while(reallength == PIPE_BUF && !buf[0] ) { 
        int i = 0;
        while(i < 5)
        {
            ioctl(clientFd, FIONREAD, &indicator);
            if (indicator){
                break;
            }
            i++;
            sleep(1);
        }

        if (i == WAITSECONDS){
            printf("server failed or going too slow\n");
            exit(1);
        }
        reallength = read(clientFd, buf,  PIPE_BUF);
        write(STDOUT_FILENO, buf + 1 , reallength - 1); 

    }

    Closefd(clientFd, "rewriteFd close");
    Closefd(serverFd, "contactFd close");
          
}
    
