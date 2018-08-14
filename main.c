#include <stdio.h>
#include <sys/errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#define MSGSIZE 512
#define BUFFSIZE 256
void *run_p(void *arguments){
    int i,default_stdout,default_stderr;
    char temp[BUFFSIZE],temp2[BUFFSIZE],temp3[BUFFSIZE];
    char *cptr = (char *)(arguments);
    for (i = 0; cptr[i] != ' ' && cptr[i] != '\0' ; ++i) {
        temp[i] = cptr[i];
    }
    i++;
    int j;
    for (j =0; cptr[i] != ' ' && cptr[i] != '\0' ; ++i,j++) {
        temp2[j] = cptr[i];
    }
    i++;
    for (j =0; cptr[i] != ' ' && cptr[i] != '\0' ; ++i,j++) {
        temp3[j] = cptr[i];
    }
    int fd,my_pipe[2];                                                                 //pipe implementation
    if (pipe(my_pipe) < 0) {
        printf("Pipe creation error!\n");
        exit(1);
    }
    char filename[BUFFSIZE/2] = {'o','u','t'};
    strcat(filename, temp3);
    strcat(filename, ".txt");
    strcat(filename, "\0");
    int saved_stdout = dup(STDOUT_FILENO);
    char *const args[] = {temp,temp2 , NULL };
    int childProsess = -1;

    if ((childProsess =fork()) == 0) {
        close(my_pipe[0]);
        dup2(my_pipe[1], STDOUT_FILENO);
        close(my_pipe[1]);
        execvp(args[0], args);
        exit(1);
    }
    else {
        char pipe_buffer[MSGSIZE];
        close(my_pipe[1]);
        if((fd = open(filename, O_RDWR | O_CREAT |O_APPEND , S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR))== -1){ /*open the file */
            perror("File open error");
            return arguments;    //exit
        }
        ssize_t numbers;
        while ((numbers = read(my_pipe[0], pipe_buffer, sizeof(pipe_buffer))) != 0) {
            write(fd, pipe_buffer, (size_t)(numbers));
        }
        wait(&childProsess);
        return NULL;
    }
    return NULL;
}

int main (int argc, const char* argv[]) {
    int file,fd,threadNum = 0;
    char buffer_thread[10][512] = {};
    pthread_t my_thread[100];
    if((file = open(argv[1],O_RDONLY))==-1){ /*open the file */
        perror("file open error\n");
        return 1;    //exit
    }
    char buffer[1024] = {};
    read(file,buffer, sizeof(buffer));
    int prosessNum = 0;
    for (int i = 0; i < sizeof(buffer) && buffer[i] != '\0'; ++i) {
        int k = 0;
        char temp_buff[512] = {};
        for (;buffer[i] != '\n' && buffer[i] != '\0';i++,k++)
            buffer_thread[prosessNum][k] = buffer[i];
        k++;

        char number[4];
        sprintf(number, " %d", prosessNum);
        strcat(buffer_thread[prosessNum], number);
        strcat(buffer_thread[prosessNum], "\0");
        prosessNum++;
        if (pthread_create(&my_thread[threadNum], NULL, run_p, buffer_thread[prosessNum-1]))
            return 1;
        else

            threadNum++;

    }
    for (int i = 0; i < 100 && NULL != my_thread[i]; i++)
        pthread_join(my_thread[i], NULL);

    return 0;
}

