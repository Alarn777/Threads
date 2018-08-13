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

void *run_p(void *arguments){
    int i,default_stdout,default_stderr;
//    int out = open("out.txt", O_RDWR | O_APPEND);
    char temp[256],temp2[256],temp3[256];
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
    int fd;
//    default_stdout = STDOUT_FILENO;
//    default_stderr = STDERR_FILENO;
    //    temp3,'.','t','x','t'
    char filename[128] = {'o','u','t'};
    strcat(filename, temp3);
    strcat(filename, ".txt");
    strcat(filename, "\0");
    int saved_stdout = dup(STDOUT_FILENO);
    if((fd = open(filename, O_RDWR | O_CREAT |O_APPEND , S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR))==-1){ /*open the file */
        perror("File open error");
        return arguments;    //exit
    }
    char *const args[] = {temp,temp2 , NULL };
    int childId = -1;
    if ((childId =fork()) == 0) {
        dup2(fd, STDOUT_FILENO);
        close(fd);
        execvp(args[0], args);
        dup2(saved_stdout, STDOUT_FILENO);

//        dup2(default_stdout, 1);
//        close(default_stdout);
    }
    else {
        wait(&childId);
//        dup2(fd, STDOUT_FILENO);
//        dup2(fd, STDERR_FILENO);
//        close(fd);
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
//            temp_buff[k] = buffer[i];
            buffer_thread[prosessNum][k] = buffer[i];
        k++;

        char number[4];
        sprintf(number, " %d", prosessNum);
        strcat(buffer_thread[prosessNum], number);
        strcat(buffer_thread[prosessNum], "\0");
        prosessNum++;
        //do shit here
//        char buffer_thread[10][512] = {};
//        strcpy(buffer_threads.buffer0,temp_buff);
//        strcpy(temp_buff,buffer[prosessNum]);
        if (pthread_create(&my_thread[threadNum], NULL, run_p, buffer_thread[prosessNum-1]))
            return 1;
        else

            threadNum++;

    }
    for (int i = 0; i < 100 && NULL != my_thread[i]; i++)
        pthread_join(my_thread[i], NULL);

    return 0;
}

