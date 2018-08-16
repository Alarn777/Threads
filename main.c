#include <stdio.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#define MSGSIZE 512
#define BUFFSIZE 256
int run_p(char *arguments,char* pros_num) {
    char temp[BUFFSIZE];
    char *cptr = arguments;
    int argument_count = 1;
    for (int count = 0; cptr[count] != '\n' && cptr[count] != '\0';count++) {
        if(cptr[count] == ' ')
            argument_count++;
    }
    argument_count++;
    char ** args = (char**) malloc(sizeof(char*)*argument_count);
    int i = 0,j = 0;
    for (;j < argument_count-1;j++,i++)
    {
        int k = 0;
        for (; cptr[i] != ' ' && cptr[i] != '\0'; ++i,k++) {
            temp[k] = cptr[i];
        }
        temp[k] = 0;
        args[j] = (char*) malloc(sizeof(char)*i+1);
        strcpy(args[j],temp);
    }
    args[argument_count-1] = NULL;
    int fd, my_pipe[2];                                                                 //pipe implementation
    if (pipe(my_pipe) < 0) {
        printf("Pipe creation error!\n");
        exit(1);
    }
    char filename[BUFFSIZE / 2] = {'o', 'u', 't'};
    strcat(filename, pros_num);
    strcat(filename, ".txt\0");
    pid_t childProsess = -1;

    if ((childProsess =fork()) == 0) {
        close(my_pipe[0]);
        dup2(my_pipe[1], STDOUT_FILENO);
        dup2(my_pipe[1], STDERR_FILENO);                    //if needed
        close(my_pipe[1]);
        execvp(args[0], args);
        for (int k = 0; args[k] != NULL ; ++k) {
            free(args[k]);
        }
        free(args);
        exit(errno);

    }
    else {
        char pipe_buffer[MSGSIZE];
        close(my_pipe[1]);
        if ((fd = open(filename, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR)) ==
            -1) { /*open the file */
            perror("File open error");
            return 1;    //exit
        }
        ssize_t numbers;
        while ((numbers = read(my_pipe[0], pipe_buffer, sizeof(pipe_buffer))) != 0) {
            write(fd, pipe_buffer, (size_t) (numbers));
        }
        close(my_pipe[0]);
        fflush(stdout);
        int status;
        waitpid(childProsess,&status,0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
        for (int k = 0; args[k] != NULL ; ++k) {
            free(args[k]);
        }
        free(args);
        return 0;
    }
    return 0;
}

int main (int argc, const char* argv[]) {
    int file, prosessNum = 0;
    char buffer_thread[10][512] = {};
    if((file = open(argv[1],O_RDONLY))==-1){ /*open the file */
        perror("file open error\n");
        return 1;    //exit
    }
    char buffer[MSGSIZE*2] = {};
    read(file,buffer, sizeof(buffer));

    for (int i = 0; i < sizeof(buffer) && buffer[i] != '\0'; ++i) {
        int k = 0;
        for (;buffer[i] != '\n' && buffer[i] != '\0';i++,k++)
            buffer_thread[prosessNum][k] = buffer[i];

        char number[4];
        sprintf(number, "%d", prosessNum);
        strcat(number, "\0");
        int return_value = run_p(buffer_thread[prosessNum],number);
        printf("%s :%d\n",buffer_thread[prosessNum], return_value);
        prosessNum++;
    }
    return 0;
}

