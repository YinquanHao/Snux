#ifndef _DIRENT_H
#define _DIRENT_H

#define NAME_MAX 255
#define FILE 1
#define DIRECTORY 2
#define RD_ONLY 1
#define WR_ONLY 2
#define RD_WR  3
#include <sys/defs.h>
typedef struct file_t file_t;
typedef struct dirent dirent;
typedef struct fd fd;

struct file_t {
    uint64_t first;
    uint64_t last;
    uint64_t current;
    file_t* child[20];
    char name[20];
    int type;
    uint64_t inode_no;
};

//reverse the attribute order to fool the mem
struct dirent {
 	char d_name[NAME_MAX+1];
 	uint64_t inode_no;
};

typedef struct DIR DIR;

struct DIR {
    file_t* node;
    uint64_t current;
    dirent current_dirent;
	int fd;
	char buf[30];
};


struct fd {
    uint64_t current;
    uint64_t permission;
    uint64_t inode_no;
    file_t* node;
};

//Done
DIR *opendir(const char *name);

//Done
struct dirent *readdir(DIR *dirp);

//Done
int closedir(DIR *dirp);

//the root file
file_t* root;

#endif