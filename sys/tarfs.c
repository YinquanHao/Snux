#include <sys/elf.h>
#include <sys/elf64.h>
#include <sys/defs.h>
#include <sys/tarfs.h>
#include <sys/virtualmem.h>
#include <sys/process.h>
#include <sys/string.h>
#include <dirent.h>

typedef struct file tarfs_file;
/*
tarfs_file* tarfs_open(char *filename, int flags) {
    if(filename==NULL) return NULL;
    if (flags == O_RDONLY) {
        posix_header_t *start = (posix_header_t*)&_binary_tarfs_start;
        while ((start < (posix_header_t*) &_binary_tarfs_end)&&(start->name[0]!='\0')){
        kprintf("start->name %s \n",start->name);
        if(strcmp(start->name,filename)!=0){
            int size = get_oct_size(start->size);
            if(size==-1){
                break;
            }
            if(size%512){
                size /= 512;
                size *= 512;
                size += 512;   
            }
            start = (posix_header_t *)((uint64_t)start + size + sizeof(posix_header_t));  
            cur = (uint64_t *)start; 
        }else{
            tarfs_file* file = 
            return (void *)start;
        }
     }












        uint64_t size;

        while (header_start < (struct posix_header_ustar*) &_binary_tarfs_end) {
            //printf("im in tarfs open 3\n");
            size = get_size_oct(header_start->size);

            //printf("size= %d\n", size);

            //if(!tarfs_namencmp(header_start->name, name, sizeof(header_start->name)))
            if (!strcmp(header_start->name, name)) //here may be a bug in future,using strcmp
                    {
                //printf("im in tarfs open 4\n");
                //dprintf("size= %d\n", size);
                tarfs_file* file = (tarfs_file*) kmalloc(FILE);
                file->file_header = header_start;
                file->size = size;

                file->start = (void*) header_start + 512;

                return file;
            }
            //printf("im in tarfs open 5\n");
            //header_start=tarfs_next_header(header_start, size);
            header_start = (struct posix_header_ustar*) ((void*) header_start
                    + ((size + 511) / 512 + 1) * 512);

        }

        //printf("im in tarfs open 6\n");

        dprintf("ERROR: tarfs open file failed\n");
    }
    dprintf("we only support O_RDONLY flags\n");
    return NULL;

}
*/



struct file_t* create_node(char *name,file_t *parent_node,uint64_t type,uint64_t first,uint64_t last,uint64_t inode_no) {    
    //allocate a new page for kernel
    file_t *new_node = (file_t *)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
    //set the type
    new_node->type = type;
    new_node->first = first;
    new_node->last =last; 
    new_node->current = first;
    new_node->child[0] = new_node;
    new_node->child[1] = parent_node;
    strcpy(new_node->name,name);
    new_node->inode_no = inode_no;
    return new_node;

}


/* intiliaze tarfs */
void init_tarfs(){
    //create first node -> '/'
    file_t *node;
    //malloc a page for the root
    root = (file_t *)get_vir_from_phy(kmalloc(KERNAL_MEM,1));
    root->type = DIRECTORY;
    root->first = 0;
    root->last = 2;
    root->current = 0;
    root->child[0] = root;
    root->child[1] = root;
    root->inode_no = 0;
    /*first node is "/" */
    strcpy(root->name,"/"); 



    /*second node is rootfs */ 
    node = create_node("rootfs",root,DIRECTORY,0,2,0);
    root->last += 1;
    root->child[2]=node;


    posix_header_t *start = (posix_header_t*)&_binary_tarfs_start;
    uint64_t *cur = (uint64_t *)start;

    while ((start < (posix_header_t*) &_binary_tarfs_end)&&(start->name[0]!='\0')){
        //the file is a directory
        if (strcmp(start->typeflag,"5") == 0){
        }
        //the file is a file
        else{

        }


        int size = get_oct_size(start->size);
        if(size==-1){
                break;
        }
        if(size%512){
            size /= 512;
            size *= 512;
            size += 512;   
        }
        start = (posix_header_t *)((uint64_t)start + size + sizeof(posix_header_t));  
        cur = (uint64_t *)start;

    }
}