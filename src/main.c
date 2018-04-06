#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "list.h"
#include "sync.h"
#include "copy.h"

int initParams(int argc, char** argv, char** source, char** destination, int* time, size_t* size, int* isRecursive);


int initParams(int argc, char** argv, char* source, char* destination, int* time, size_t* size, int* isRecursive)
{
    int opt;

    *source = argv[1];
    *destination = argv[2];

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
    if(syncFiles(argv[1], argv[2], sizeTH, 1) == -1)
    {
        printf("sync wrong\n");
        return 0;
    }


    return 0;
}   
