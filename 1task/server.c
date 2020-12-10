
#include "header.h"

int main(int argc, char *argv[])
{
    int serverFd, clientFd;
    char clientfifo[CLIENT_FIFO_NAME_LEN];
    struct request req;
   

        /* Create well-known FIFO, and open it for reading */

    umask(0);                           /* So we get the permissions we want */
    if (mkfifo(SERVER_FIFO, 0666) == -1 && errno != EEXIST){
           perror("mkfifo");
           exit(1);
        }

    serverFd = Openfd(SERVER_FIFO, O_RDWR, "");

    signal(SIGPIPE, SIG_IGN);


    for (;;) {                          /* Read requests and send responses */
        if (read(serverFd, &req, sizeof(struct request)) != sizeof(struct request)) {
            fprintf(stderr, "Error reading request; discarding\n");
            continue;                  
        }

        /* Open client FIFO (previously created by client) */

        snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) req.pid);
        int clientFd = open(clientfifo, O_WRONLY | O_NONBLOCK);
        if (clientFd == -1) {           /* Open failed, give up on client */
            perror("CLIENT");
            continue;
        }
        DisableNONBLOCK(clientFd);

        FILE* flin = fopen(req.filename, "rb");
        if (flin == -1) {           /* Open failed, give up on client */
            perror("CLIENT");
            continue;
        }

        char buf[PIPE_BUF] = "";
         int reallength = 0;
            /* Send response and close FIFO */

        while((reallength = fread(buf + 1, SIZEOFCHAR,  PIPE_BUF - 1, flin)) == PIPE_BUF - 1 ){
        
            buf[0] = 0;
            if(write(clientFd, buf, PIPE_BUF) == -1){
                perror("CLIENT");
                 Closefd(clientFd,   "clientFd close"); 
                break;
            }

        }
        buf[0] = 1;
        write(clientFd, buf, reallength + 1);

        
        Closefd(clientFd,   "clientFd close");           /* Update our sequence number */
    }
}