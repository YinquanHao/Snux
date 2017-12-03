#ifndef _TERMINAL_H
#define _TERMINAL_H
#include <sys/defs.h>
typedef struct terminal
{
	uint64_t write;
	uint64_t read;
	//uint64_t tx;//terminal x
	//uint64_t ty;//terminal y
	//char *tbuffer;
	
}terminal;
void terminal_init();
void clear_terminal();
void terminal_input(char input_state,char input_char);
uint64_t terminal_read(char *buf, int count);
uint64_t terminal_write(int fd, char *buf, int count);
void t_scroll();
#endif