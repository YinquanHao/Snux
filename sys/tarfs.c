#include <sys/elf.h>
#include <sys/elf64.h>
#include <sys/defs.h>
#include <sys/tarfs.h>
#include <sys/virtualmem.h>
#include <sys/process.h>
#include <sys/string.h>
#include <dirent.h>

typedef struct file tarfs_file;
struct file_t* create_node(char *name,file_t *parent_node,uint64_t type,uint64_t first,uint64_t last,uint64_t inode_no) {    
    //allocate a new page in kernel
    file_t *new_node = (file_t *)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
    //set the type
    new_node->type = type;

    new_node->first = first;
    new_node->last =last; 
    new_node->current = first;
    new_node->child[0] = new_node;
    new_node->child[1] = parent_node;
    //copy the name of file node
    strcpy(new_node->name,name);
    //set the inode number
    new_node->inode_no = inode_no;
    return new_node;

}


/* initialize tarf file system */
void init_tarfs(){
    //malloc a page for the root
    root = (file_t *)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
    //root file is '/'
    strcpy(root->name,"/");
    //type of root file 
    root->type = DIRECTORY;

    root->first = 0;
    root->last = 2;
    root->current = 0;
    root->child[0] = root;
    root->child[1] = root;
    //set root's inode number to 0
    root->inode_no = 0;

    //create second node which is /rootfs
    file_t *node = create_node("rootfs",root,DIRECTORY,0,2,0);
    //increment the file number under root
    root->last += 1;
    //assign the third node under root as node
    root->child[2]=node;

    //get the tarf header
    posix_header_t *start = (posix_header_t*)&_binary_tarfs_start;
    //TODO yinquanhao can we clear this?
    uint64_t *cur = (uint64_t *)start;

    //walk through the tarfs
    while ((start < (posix_header_t*) &_binary_tarfs_end)&&(start->name[0]!='\0')){
        //get the file size of current tarf
        uint64_t filesz = get_oct_size(start->size);
        //if the file is a directory
        if (strcmp(start->typeflag,"5") == 0){

            parse_tarfs(start->name,DIRECTORY,0,2);
        }
        //the file is a file
        else{

            parse_tarfs(start->name,FILE,(uint64_t)(start+1),(uint64_t)((void*)start + filesz + sizeof(posix_header_t)));
        
        }
        //reach the end of the tarfs
        if(filesz==-1){
                break;
        }
        //round the tarf
        if(filesz%512){
            filesz /= 512;
            filesz *= 512;
            filesz += 512;   
        }
        //get next header
        start = (posix_header_t *)((uint64_t)start + filesz + sizeof(posix_header_t));  
        //TODO yinquanhao can we clear this?
        cur = (uint64_t *)start;
    }
}


void parse_tarfs(char* path, int type, uint64_t first, uint64_t last){
    file_t* tmp;
    file_t* current;
    file_t* tmp1;
    char* name;
    char* dir;
    int i=0;
    //allocate a space for dir's name
    dir = (char*)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
    //copy the path to dir name
    strcpy(dir,path);
    //set current to the rootfs
    current = root->child[2];
    //tokenize the dir name
    name = strtok(dir,"/");

    if(name == NULL){
        return NULL;
    }

    while(name!=NULL){
        //set tmp as rootfs node
        tmp = current;
        //walk through the directy skip the first two, self and parent 
        for(i=2;i<(current->last);i++){
            //find the next level
            if(strcmp(name,current->child[i]->name)==0){
                //set the current as the next level we find
                current = (file_t*) current->child[i];
                break;
            }
        }
        //if we reach the end of the files in the level
        if(i==tmp->last){
            //create a new node as tmp1
            tmp1 = create_node(name,current,type,first,last,0);
            //update current level's last file as the new node
            current->child[current->last] = tmp1;
            //increment the counter last by 1
            current->last+=1;
        }
        name = strtok(NULL,"/");
    }

}