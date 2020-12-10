
#include "header.h"

int main(int argc, char *argv[])
{
    int serverFd, clientFd;
    char clientfifo[CLIENT_FIFO_NAME_LEN];
    struct request req;
   

        /* открываем трубу сервера (если клиент откроет ее раньше нас то умрет и это логично)*/

    umask(0);                           
    if (mkfifo(SERVER_FIFO, 0666) == -1 && errno != EEXIST){
           perror("mkfifo");
           exit(1);
        }

    serverFd = Openfd(SERVER_FIFO, O_RDWR, "");

    signal(SIGPIPE, SIG_IGN);  /* если словим сигпайп от записи в трубу без кнца на чтение(потому что клиент умер)
                                сервер не умрет и это важно */



    for (;;) {                          /* цикл обработки запросов, на любую ошибку просто идем дальше*/
        if (read(serverFd, &req, sizeof(struct request)) != sizeof(struct request)) {
            fprintf(stderr, "Error reading request\n");
            continue;                  
        }
        
        /* после того как считали структуру, открываем клиентское фифо и файл, который нам передали */

        snprintf(clientfifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) req.pid);
        int clientFd = open(clientfifo, O_WRONLY | O_NONBLOCK);
        if (clientFd == -1) {           
            perror("CLIENT");
            continue;
        }
        DisableNONBLOCK(clientFd);

        FILE* flin = fopen(req.filename, "rb");
        if (flin == NULL) {           
            perror("CLIENT");
            continue;
        }

        char buf[PIPE_BUF] = "";
         int reallength = 0;


            /* первый байт - индикатор сообщения, пишем пока reallength работает как надо */


        while((reallength = fread(buf + 1, SIZEOFCHAR,  PIPE_BUF - 1, flin)) == PIPE_BUF - 1 ){
            
            
            buf[0] = 0;
            if(write(clientFd, buf, PIPE_BUF) == -1){
                perror("CLIENT");
                 close(clientFd); 
                break;
            }
            

        }
        buf[0] = 1;
        write(clientFd, buf, reallength + 1);

        
        close(clientFd);           /* закрываем клиентскую трубу, идем к следующему клиенту */
    }
}