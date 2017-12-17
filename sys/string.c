#include <sys/string.h>
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

char* strtok(char* parse_str,const char* delims){

	static char *token = NULL;
	char *str_ptr = NULL;
	int index = 0;
	int str_len = strlen(delims);
 
	if(parse_str && !token)
        	token = parse_str;
    if(!parse_str && !token)
		return NULL;
	str_ptr = token;
	while(1) {
        	for(index = 0; index < str_len; index ++) {
            		if(*str_ptr == delims[index]) {
                		str_ptr ++;
                		break;
            		}
        	}
 
		if(index == str_len) {
               		token = str_ptr;
               		break;
        	}
    	}
	if(*token == '\0') {
		token = NULL;
		return token;
	}

	while(*token != '\0') {
		for(index = 0; index < str_len; index ++) {
			if(*token == delims[index]) {
				*token = '\0';
                		break;
            		}
        	}
 
        	token ++;
        	
		if (index < str_len)
            		break;
    	}
    	return str_ptr;
}


char *strcat(char *dest, const char *src){
	char *res=dest;
	while(*dest){
		dest++;
	}
	while((*dest++=*src++)!='\0');
	return res;
}

