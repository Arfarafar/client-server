#include "header.h"

char clientfifo[CLIENT_FIFO_NAME_LEN];


int main(int argc, char *argv[]){

    
        struct request req;
        

        if (argc != 2 ){
            printf("invalid argc\n");
            exit(1);
        }
                

        /*  */

        umask(0);                   /* Делаем фифошку клиента*/
    snprintf(clientfifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) getpid());
        if (mkfifo(clientfifo, 0666) == -1  && errno != EEXIST){
           perror("mkfifo");
           exit(1);
        }



        /* создаем запрос, открываем трубу сервера */

    req.pid = getpid();
    strcpy(req.filename, argv[1]);

    int serverFd = Openfd(SERVER_FIFO, O_WRONLY | O_NONBLOCK, "");
    DisableNONBLOCK(serverFd);
   
           

        /* открываем нашу трубу, если открывать без нонблока то она будет ждать пока откроется второй конец, нам это не нужно */
    int clientFd = Openfd(clientfifo, O_RDONLY | O_NONBLOCK, "file receiverFd open");
    DisableNONBLOCK(clientFd);  /* выключаем нонблок и переходим в блокирующий режим */

     /* после того как все сделали пише серверу запрос */
     write(serverFd, &req, sizeof(struct request));

    char buf[PIPE_BUF] = "";
    int indicator = 0;
    int reallength = PIPE_BUF;

     /* внутренний цикл нужен для того чтобы ждать ответа сервера только определенное время */

    while(reallength == PIPE_BUF && !buf[0] ) {   /* первый байт каждого сообщения это 0 если оно не последнее и 1 если последнее */
        int i = 0;
        while(i < 5)  /*  ждем до 5 секунд*/
        {
            ioctl(clientFd, FIONREAD, &indicator);  /* проверить если ли инфа в трубе */
            if (indicator){
                break;
            }
            i++;
            sleep(1);
        }

        if (i == 5){     /* если 5 секунд прошли то ошибка */
            printf("server failed or going too slow\n");
            exit(1);
        }
        reallength = read(clientFd, buf,  PIPE_BUF); 
        write(STDOUT_FILENO, buf + 1 , reallength - 1); 

    }

    Closefd(clientFd, "rewriteFd close");
    Closefd(serverFd, "contactFd close");
          
}
    
