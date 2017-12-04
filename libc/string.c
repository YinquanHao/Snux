#include <stdio.h>
#include <sys/defs.h>

char *strcpy(char *dest, const char *source){
/*for(int i=0;i<strlen(dest);i++){
	*dest='\0';
}*/
	int i=0;
	while(source[i]!='\0'){
		dest[i]=source[i];
		i++;
	}
	dest[i]='\0';
	return dest;
}

size_t strlen(char *str){
	size_t len;
	for(len=0;str[len];len++);
	return len;
}

int strcmp(const char *str1,const char *str2){
	while(*str1==*str2){
		if(*str1=='\0'){
			return 0;
		}
		str1++;
		str2++;		
	}
	return *str1-*str2;
}

char *strstr(const char *haystack, const char *needle){
	char *p1;
	p1=(char*)haystack;	
	if(!*needle){return p1;}
	while(*p1){
		char *begin=(char*)p1;
		char *p2=(char*)needle;
		while(*p2&&*p1&&*p1==*p2){
			p1++;
			p2++;
		}
		if(!*p2){
			return begin;
		}
		p1=begin+1;
	}
	return NULL;
}


char* strtok(char *str, const char* delim) {
    static char* _buffer;
    if(str != NULL) _buffer = str;
    if(_buffer[0] == '\0') return NULL;
 
    char *ret = _buffer, *b;
    const char *d;
 
    for(b = _buffer; *b !='\0'; b++) {
        for(d = delim; *d != '\0'; d++) {
            if(*b == *d) {
                *b = '\0';
                _buffer = b+1;
 
                // skip the beginning delimiters
                if(b == ret) { 
                    ret++; 
                    continue; 
                }
                return ret;
            }
        }
    }
 
    return ret;
}


char *strcat(char *dest, const char *src){
	char *res=dest;
	while(*dest){
		dest++;
	}
	while((*dest++=*src++)!='\0');
	return res;
}

