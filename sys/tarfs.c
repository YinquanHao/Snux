#include <sys/elf.h>
#include <sys/elf64.h>
#include <sys/defs.h>
#include <sys/tarfs.h>
#include <sys/virtualmem.h>
#include <sys/process.h>

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