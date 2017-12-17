#include <stdio.h>
//#include <string.h>
#include <stdlib.h>
#include  <unistd.h>
#define MAX_INPUT       1024
#define MAX_ARGUMENT     256
#define RUNASBG            1
#define RUNNONBG           0
#define O_RDONLY	        00

/*store the path variable*/
char PATH[MAX_INPUT];
/*store ps1 variable*/
char PS1[MAX_INPUT];

char CWD[MAX_INPUT];

int bgflag=0;

int read_command(char* buf);
void cmdStrip(char* cmdBuffer, char* modify);
int cmdParser(char* cmd, char* tokens[]);
int containBGSign(char* cmd);
int containSlash(char* filename);
void getEnv(char** environ,char* PATH);
void getAbsPath(char* cwdPath, char* fileName, char* absPath);
int isAbsPath(char* path);
void exeScript(char* fileName,char* cwd,int argc, char* argv[], char* envp[]);
void tokenize(char** tokens, char* cmd);
int tokenParse(char* token, int BG);
void cd(char *dir);

int main(int argc, char* argv[], char* envp[]) {
	char* cmd = (char*) malloc(MAX_INPUT);
	char *cursor = (char*)malloc(MAX_INPUT);
  char* tokens[10]={0};
  char *args[5];
  char temp[3];
  char* cwd = malloc(MAX_INPUT);
	int isFinished = 0;
	clear_screen();	
  int status=0;

printf("50 pt project\n"); 
printf("(1) cd: change work directory   ex: cd /rootfs\n");
printf("(2) pwd: print current working directory  ex: pwd\n");
printf("(3) clear: clear the print  ex: clear \n");
printf("(4) script.sh: execute the script. Need absolute path   ex: /rootfs/script.sh\n");
printf("(5) echo var: print the vars in terminal  ex: echo abc def\n");
printf("(6) ls: list in current directory ex: ls\n");
printf("(7) cat var1: print content of the file, need absolute path  ex: cat /rootfs/aaa.txt\n  ");
printf("(8) sleep var1 : sleep the given time  ex sleep 2\n ");
printf("(9) ps : print the process status  ex: ps\n");
printf("(10) kill var1: kill the process with given id ex: kill 4\n");
printf("(11) &: run in background ex &test  \n");
printf("(12) test command can test all functions in inculde *.h ex:test \n");
printf("(13) BUG!!!! too many times of fork may cause crash \n");
  while (!isFinished) {
    memset(cwd,0,MAX_INPUT);
    getcwd(cwd,MAX_INPUT);
		if(read_command(cursor)<=0){
			continue;
		}
		cmdStrip(cursor, cmd);
    int cmdID = cmdParser(cmd,tokens);
      if(cmdID==2){
        if(isAbsPath(tokens[1])==1){
          cd(tokens[1]);
        }else{
          char* absPath = malloc(MAX_INPUT);
          getAbsPath(cwd,tokens[1],absPath);
          cd(absPath);
          free(absPath);
        }
        getcwd(cwd,MAX_INPUT);
        printf("%s\n", cwd);
      }
      else if(cmdID==3){
   	    char* fileName = tokens[0];
	      printf("exescript");
        exeScript(fileName,cwd,argc,argv,envp);
      }
      else if(cmdID==8){
        clear_screen();
      }
      else if(cmdID==9){
        getcwd(cwd,MAX_INPUT);
        printf("%s\n", cwd);
      }
      else if(cmdID==11){
      if(tokens[1]){  
        int p=fork();
        args[0]=tokens[1];
        args[1]=tokens[1];
        if(p){
            if(bgflag==1){
                bgflag=0;
                background(p);
            }
            else{
              waitpid(p,&status);
            }
        }
        else{
            execvpe("bin/sleep",args,envp);
          }
        }
        else{
          printf("time needed");
        }
      }
      else if(cmdID==12){ 
        if(tokens[1]==NULL){
          printf("need filename\n");
        }
        else{
          int p=fork();
          args[0]=tokens[1];
          args[1]="0";
          if(p){
            if(bgflag==1){
                bgflag=0;
                background(p);
            }else{
              waitpid(p,&status);
            }
          }else{
            execvpe("bin/cat",args,envp);
          }
      }
      }
      else if(cmdID==13){
        getcwd(cwd,100);
        cd(cwd);
        if(tokens[1]){
          args[0]=cwd;
          args[1]=tokens[1];
          args[2]="0";
        }
        else{
          args[0]=cwd;
          args[1]="0";
          args[2]="0";  
        }
        int p=fork();
        if(p){
            if(bgflag==1){
              bgflag=0;
              background(p);
            }
            else{
              waitpid(p,&status);
            }
        }
        else{
            execvpe("bin/ls",args,envp);
        }
        printf("\n");
      }
      else if(cmdID==14){
        if(tokens[1]){
        args[0]=tokens[1];
        args[1]="0";
        args[2]="0";
        int p=fork();
        if(p){
            if(bgflag==1){
                bgflag=0;
                background(p);
            }
            else{
              waitpid(2,&status);
            }
        }
        else{
            execvpe("bin/kill",args,envp);
        }
      }
      else{
          printf("pid needed");
        }
      }
      else if(cmdID==15){
        //ps();
        int p=fork();
        if(p){
            if(bgflag==1){
                bgflag=0;
                background(p);
            }
            else{
              waitpid(p,&status);
            }
        }
        else{
            execvpe("bin/ps",argv,envp);
        }
      }
      else if(cmdID==16){
        if(tokens[1]==NULL){
          continue;
        }
        int i=1;
        while(tokens[i]){
          args[i]=tokens[i];
          i++;
        }
        temp[0]=i+'0';
        temp[1]='\0';
        args[0]=temp;
        args[i]="0";
        int p=fork();
        if(p){
            if(bgflag==1){
                bgflag=0;
                background(p);
            }
            else{
              waitpid(p,&status);
            }
        }
        else{
            execvpe("bin/echo",args,envp);
        }
      }      
      else if(cmdID==10){
        printf("not found command\n");
      }
      else{
        //printf("not found command\n");
      }
	}
}

int read_command(char* buf){
	memset(buf,0,MAX_INPUT);	
	int res=read_input(0,buf,63);
	if(res>0&&buf[0]!='\n'){
		buf[res-1]='\0';
	}
	return res;	
}

void cmdStrip(char* cmdBuffer, char* modify){
  memset(modify, '\0', MAX_INPUT);
  int counter = 0;
  while(*cmdBuffer && (*cmdBuffer==' ')){
    cmdBuffer++;
  }
  while(*cmdBuffer!='\0'){
    while(*cmdBuffer == ' '&& *(cmdBuffer+1)==' '){
      cmdBuffer++;
    }
    modify[counter]= *cmdBuffer;
    counter++;
    cmdBuffer++;
  }
  modify[counter]=' ';
}


void getEnv(char** environ,char* PATH){
 for(int i=0;;i++){
    if(environ[i]==NULL) break;
    char* environcp = malloc(MAX_INPUT);
    strcpy(environcp,environ[i]);
    if(strstr(environcp,"PATH")!=NULL){
      char* cmdToken = strtok(environcp,"=");
      if(strlen(cmdToken)==4){
        cmdToken = strtok(NULL, "=");
        strcpy(PATH,cmdToken);
        strcat(PATH,"\0");
     }
   }
 }
}

int cmdParser(char* cmd, char* tokens[]){
  int BG=containBGSign(cmd);
  int ct = 0;
  if(!strcmp(cmd," ")){
    return 10;
  }
  for (char* cmdToken = strtok(cmd, " "); cmdToken; cmdToken = strtok(NULL, " ")){
    if (ct >= 10){
        break;
      }
    tokens[ct++] = cmdToken;
	}
  for(;ct<10;ct++){
    tokens[ct] = NULL;
  }
  if(BG==1){
    bgflag=1;
  }
  if(!strcmp(tokens[0],"cd")){
	 return 2;
	}else if(strstr(tokens[0],".sh")){
		return 3;
  }else if(containSlash(tokens[0])==1){
    return 7;
  }else if(!strcmp(tokens[0],"clear")){
    return 8;
  }else if(!strcmp(tokens[0],"pwd")){
    return 9;
  }else if(!strcmp(tokens[0],"sleep")){
    return 11;
  }
  else if(!strcmp(tokens[0],"cat")){
    return 12;
  }
  else if(!strcmp(tokens[0],"ls")){
    return 13;
  }
  else if(!strcmp(tokens[0],"kill")){
    return 14;
  }
  else if(!strcmp(tokens[0],"ps")){
    return 15;
  }
  else if(!strcmp(tokens[0],"echo")){
    //printf("ehotest\n");
    return 16;
  }
  else if(!strcmp(tokens[0],"test")){
    getppid();
    //printf("a");
     int p=fork();
        if(p){
            waitpid(2,0);
        }
        else{
            execvpe("bin/testbin",0,0);
        }

    return 10;
  }    
  else{
	//fprintf(stderr, "chech tokens1  %s\n", tokens[1]);
   if(!strcmp(tokens[0],"ls")&& tokens[1]==NULL){
     printf("cwd");
     char* token = malloc(MAX_INPUT);
     tokens[1] = getcwd(token,MAX_INPUT);
	   tokens[1]=token;
     printf("ccccc %s\n", tokens[1]);
   }
  if(BG==1){
      printf("BG\n");
      return -10;
    }
    return 10;
  }
  while(1);
	return -1;
}



int containBGSign(char* cmd){
  char* ptr = cmd;
  size_t i=0;
  int result =0;
  for(i=0;i<strlen(cmd);i++){
    char current  = *ptr;
    if(current =='&'){
	*ptr='\0';      
	result =1;
      break;
    }
    ptr++;
  }
  return result;
}

int containSlash(char* filename){
  char* ptr = filename;
  size_t i=0;
  int result =0;
  for(i=0;i<strlen(filename);i++){
    char current  = *ptr;
    if(current =='/'){
      result =1;
      break;
    }
    ptr++;
  }
  return result;
}

void getAbsPath(char* cwdPath, char* fileName, char* absPath){
  strcat(absPath,cwdPath);
  strcat(absPath,"/");
  strcat(absPath,fileName);
}

int isAbsPath(char* path){
  if(path==NULL) return -1;
  if(path[0]=='/') return 1;
  return 0;
}

void exeScript(char* fileName,char* cwd,int argc, char* argv[], char* envp[]){
  char *args[5];
  char temp[3];  
  int status=0;
  printf("\nsh");
  int ct = 0;
  int fd = open(fileName,O_RDONLY);
  char* buffer = malloc(MAX_INPUT);
  int read_size = read(fd,buffer,1);
  char* cmdBuff = malloc(MAX_INPUT);
  while(read_size>0){
    if(buffer[read_size-1] != '\n'){
      cmdBuff[ct++] = buffer[read_size-1]; 
    }else{
      cmdBuff[ct] = '\0';
      if(strstr(cmdBuff,"#!")==NULL){
        int BG = containBGSign(cmdBuff);
        if(BG==1){
          bgflag=1;
        }
        char* tokens[10];
        tokenize(tokens,cmdBuff);
        int cmdID = tokenParse(tokens[0],cmdBuff);
        if(!strcmp(tokens[0],"ls")){
        getcwd(cwd,100);
        cd(cwd);
        if(tokens[1]){
          args[0]=cwd;
          args[1]=tokens[1];
          args[2]="0";
          args[3]=NULL;
        }
        else{
          args[0]=cwd;
          args[1]="0";
          args[2]="0";
          args[3]=NULL;  
        }
          int p=fork();
          if(p){
              if(bgflag==1){
                bgflag=0;
                background(p);
              }
              else{
                waitpid(p,&status);
              }
          }
          else{
            execvpe("bin/ls",args,envp);
          }
        }else{
            //printf("\ncmd:%d\n", cmdID);
          if(cmdID==2){
            if(isAbsPath(tokens[1])==1){
            cd(tokens[1]);
            }else{
              char* absPath = malloc(MAX_INPUT);
              getAbsPath(cwd,tokens[1],absPath);
              cd(absPath);
              free(absPath);
            }
            getcwd(cwd,MAX_INPUT);
            printf("%s\n", cwd);
          }else if(cmdID==3){
            char* fileName = tokens[0];
            exeScript(fileName,cwd,argc,argv,envp);
          }
          else if(cmdID==8){
            clear_screen();
          }
          else if(cmdID==9){
            getcwd(cwd,MAX_INPUT);
            printf("\n%s", cwd);
          }
          else if (cmdID==11){
            if(tokens[1]){
            int p=fork();
            args[0]=tokens[1];
            args[1]="0";
            args[2]=NULL;
            if(p){
              //waitpid(2,&status);
              if(bgflag==1){
                bgflag=0;
                background(p);
              }
              else{
                waitpid(p,&status);
              }
            }
            else{
              execvpe("bin/sleep",args,envp);
            }
          }
          else{
            printf("time needed");
          }
          }
          else if(cmdID==12){ 
            if(tokens[1]==NULL){
              printf("need filename\n");
            }
            else{
              int p=fork();
              args[0]=tokens[1];
              args[1]="0";            
              args[2]=NULL;
              if(p){
                if(bgflag==1){
                  bgflag=0;
                  background(p);
                }
                else{
                  waitpid(p,&status);
                }
              }
              else{
                execvpe("bin/cat",args,envp);
              }
            }
        //cat(tokens[1]);
          }
/*          else if(cmdID==13){

          }*/
          else if(cmdID==14){
            if(tokens[1]){
            args[0]=tokens[1];
            args[1]="0";
            args[2]="0";
            args[3]=NULL;            
            int p=fork();
            if(p){
              if(bgflag==1){
                bgflag=0;
                background(p);
              }
              else{
                waitpid(p,&status);
              }
            }
            else{
              execvpe("bin/kill",args,envp);
            }
          }
          else{
            printf("pid needed");
          }
          }
          else if(cmdID==15){
            
            int p=fork();
            if(p){
              if(bgflag==1){
                bgflag=0;
                background(p);
              }
              else{
                waitpid(p,&status);
              }
            }
            else{
              execvpe("bin/ps",argv,envp);
            }
          }
          else if(cmdID==16){         
            //printf(" tokens111%s\n", tokens[1]);    
            if(tokens[1]==NULL){//nothing to echo
              continue;
            }
            int i=1;
            while(tokens[i]){   
              args[i]=tokens[i];
              i++;
            }
            temp[0]=i+'0';
            temp[1]='\0';
            args[0]=temp;
            //printf("  %s",args[0]);
            
            args[i]="0";
            args[i+1]=NULL;     
            int p=fork();
            if(p){
              if(bgflag==1){
                bgflag=0;
                background(p);
              }
              else{
                waitpid(p,&status);
              }
            }
            else{
                execvpe("bin/echo",args,envp);
            }
          }                                
          else{
            printf("no command");
          }
        }
        //free(cmdClone);
      }
      ct=0;
    }
    buffer[read_size] = '\0';
    read_size = read(fd,buffer,1);
  }
  close(fd);
}

/*tokenize a bash cmd*/
void tokenize(char** tokens, char* cmd){
char* cmd1=malloc(MAX_INPUT);
cmdStrip(cmd,cmd1);
	//printf("cmd1: %s\n",cmd1);
  //fprintf(stderr, "exepipcmd %s \n", cmd);
  int ct = 0;
  for (char* cmdToken = strtok(cmd1, " "); cmdToken; cmdToken = strtok(NULL, " ")){
      if (ct >= 10){
          break;
      }
      tokens[ct++] = cmdToken;
      //printf("tokenized cmd %s\n", tokens[ct-1]);
  }
//printf("tokenize count:%d",ct);
  for(;ct<10;ct++){
    tokens[ct] = NULL;
  }

}

int tokenParse(char* token, int BG){
  if(!strcmp(token,"set")){
    return 1;
  }else if(!strcmp(token,"cd")){
    return 2;
  /*if the first token is ./ return 3 */
  }else if(strstr(token,".sh")!=NULL){
    return 3;
  }else if(!strcmp(token,"set")){
    return 4;
  }else if(containSlash(token)==1){
    return 7;
  }
  else if(!strcmp(token,"clear")){
    return 8;
  }
  else if(!strcmp(token,"pwd")){
    return 9;
  }
  else if(!strcmp(token,"sleep")){
    return 11;
  }
  else if(!strcmp(token,"cat")){
    return 12;
  }
  else if(!strcmp(token,"ls")){
    return 13;
  }
  else if(!strcmp(token,"kill")){
    return 14;
  }
  else if(!strcmp(token,"ps")){
    return 15;
  }
  else if(!strcmp(token,"echo")){
    return 16;
  }
  else{
    if(BG==1){
      return -10;
    }
    return 10;
  }
}



void cd(char* dir){
  if(dir==NULL){
    chdir("rootfs");
    return;
  }
  int res=chdir(dir);
  if(res==-1){
    printf("dir not found\n");
  }
}


int isDot(char* name){
  if(name == NULL){
    return -1;
  }else if(name[0]=='.'){
    if(name[1]=='.'){
      return 1;
    }else{
      return 2;
    }
  }
}

