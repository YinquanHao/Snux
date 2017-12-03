#ifndef _DIRENT_H
#define _DIRENT_H

#define NAME_MAX 255
#define FILE 1
#define DIRECTORY 2
typedef struct file_t file_t;

struct file_t {
        uint64_t first;
        uint64_t last;
        uint64_t current;
        file_t* child[20];
        char name[20];
        int type;
        uint64_t inode_no;
};

struct dirent {
 char d_name[NAME_MAX+1];
};

typedef struct DIR DIR;

DIR *opendir(const char *name);
struct dirent *readdir(DIR *dirp);
int closedir(DIR *dirp);

//the root file
file_t* root;

#endif