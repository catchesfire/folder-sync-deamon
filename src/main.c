#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <syslog.h>
#include <fcntl.h>
#include <linux/fs.h>
#include "list.h"
#include "sync.h"
#include "copy.h"

int initParams(int argc, char** argv, char** source, char** destination, int* time, size_t* size, int* isRecursive);


int initParams(int argc, char** argv, char** source, char** destination, int* time, size_t* size, int* isRecursive)
{
    int opt;

    DIR *dir;

    *source = argv[1];
    *destination = argv[2];

    if((dir = opendir(*source)) == NULL)
    {
        if(errno == ENOTDIR){
            fprintf(stderr, "%s is not a directory\n", *source);
            exit(EXIT_FAILURE);
        }

        fprintf(stderr, "%s is not a valid directory\n", *source);
        exit(EXIT_FAILURE);
    }

    closedir(dir);

    if(( dir = opendir(*destination)) == NULL)
    {
        if(errno == ENOTDIR){
            fprintf(stderr, "%s is not a directory\n", *destination);
            exit(EXIT_FAILURE);
        }

        fprintf(stderr, "%s is not a valid directory\n", *destination);
        exit(EXIT_FAILURE);
    }

    closedir(dir);

    if(argc < 3) //at least 3 arguments are necessary, e.g. ./sync source destination 
    {
        fprintf(stderr, "Usage: %s source destination [-t secs] [-s size] [-R]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if(strcmp(*source, *destination) == 0) // source and destination have to be different directories
    {
        fprintf(stderr, "Source and destination paths are the same\n");
        exit(EXIT_FAILURE);
    }

    while ((opt = getopt(argc, argv, "t:s:R")) != -1)
    {
        switch(opt)
        {
            case 't':
                *time = atoi(optarg);
                break;
            case 's':
                *size = atoi(optarg);
                break;
            case 'R':
                *isRecursive = 1;
                break;
            default: /* '?' */
                fprintf(stderr, "Usage: %s source destination [-t secs] [-s size] [-R]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    return optind;
}

int main(int argc, char** argv)
{
    pid_t pid;
    int i;
    char* sourceDirPath;
    char* destinationDirPath;
    int time = 300;
    size_t sizeTh = 1073741824;
    int isRecursive = 0;

    if(initParams(argc, argv, &sourceDirPath, &destinationDirPath, &time, &sizeTh, &isRecursive) >= argc)
    {
       fprintf(stderr, "Expected argument after options\n");
       exit(EXIT_FAILURE);
    }

    pid = fork();
    if(pid == -1)
    {
        return -1;
    }
    else if(pid != 0)
    {
        exit(EXIT_SUCCESS);
    }

    if(setsid() == -1)
    {
        return -1;
    }

    for(i = 0; i <  sysconf(_SC_OPEN_MAX); i++)
    {
        close(i);
    }

    open("/dev/null", O_RDWR);
    dup(0);
    dup(0);

    openlog("test", LOG_PID, LOG_USER);
    
    while(1)
    {
        syslog(LOG_INFO, "Deamon has been started automatically.");

        if(syncFiles(sourceDirPath, destinationDirPath, sizeTh, isRecursive) == -1)
        {
            syslog(LOG_ERR, "Synchronization failed.");
            return 0;
        }

        sleep(time);
    }

    closelog();
    return 0;
}   
