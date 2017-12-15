#include <stdio.h>
//#include <string.h>
#include <stdlib.h>
//#include  <unistd.h>
#define MAX_INPUT 1024
#define MAX_ARGUMENT 256
#define RUNASBG 1
#define RUNNONBG 0
#define O_RDONLY	     00

/*store the path variable*/
char PATH[MAX_INPUT];
/*store ps1 variable*/
char PS1[MAX_INPUT];

int read_command(char* buf);
//void getBoudleDir(char* buff);
void cmdStrip(char* cmdBuffer, char* modify);
//void setPath(char* path, char* pathval);
//void initPath(char* path, char* boundleDir);
int cmdParser(char* cmd, char* tokens[]);
int containBGSign(char* cmd);
int containSlash(char* filename);
void getEnv(char** environ,char* PATH);
//void setPS1(char* PS1, char* in);
//void renewPromort(char* promptBuff,char* prompt,char* brac);
//char* findBin(char* path, char* boundleDir, char* name);
//void exebin(char* tokens[], int asBG,char** environ);
void getAbsPath(char* cwdPath, char* fileName, char* absPath);
int isAbsPath(char* path);
void exeScript(char* fileName,char* cwd,int argc, char* argv[], char* envp[]);
void tokenize(char** tokens, char* cmd);
int tokenParse(char* token, int BG);
//int containPipe(char* filename);
//int splitPipe(char* cmd, char* pipecmds[]);
//void exebinPipe(char* cmds[], int asBG, int argc, char* boundleDir,char** envp);
//void exepipcmd(char* cmd, int in, int out, char* boundleDir,char** envp);


void cd(char *dir);

int main(int argc, char* argv[], char* envp[]) {
	//setPS1(PS1,"sbush");
	char* cmd = (char*) malloc(MAX_INPUT);
	//char* promptBuff = (char*) malloc(MAX_INPUT);
	char *cursor = (char*)malloc(MAX_INPUT);
//	while(1);
  //getcwd(promptBuff,MAX_INPUT);
  char* tokens[10]={0};
  char *args[5];
  char temp[3];
  	/*cwd buffer*/
  //char* boundleDir = (char*) malloc(MAX_INPUT);
  	/*get cwd and store in buffer*/
  //getcwd(boundleDir,MAX_INPUT);
 	//getBoudleDir(boundleDir);
  /* where to store cwd*/
  char* cwd = malloc(MAX_INPUT);
	int isFinished = 0;
	clear_screen();	
  int status=0;

printf("into the sbush\n");	
  while (!isFinished) {
		
    memset(cwd,0,1024);
		if(read_command(cursor)<=0){
			continue;
		}
    //printf("%s",prompt);
		cmdStrip(cursor, cmd);
		//printf("%s",cmd);
    //while(1);
    int cmdID = cmdParser(cmd,tokens);
/*    if(containPipe(cmdPipeCpy)){
      cmdID = 5;
    }*/
    
/*    if(cmdID==1){
      if(tokens[2]!=NULL){      
        setPath(PATH, tokens[2]);}
	    }*/
      if(cmdID==2){
            //fprintf(stderr,"cd1 path %s",tokens[1]);
        cd(tokens[1]);
        //getcwd(cwd,MAX_INPUT);
        //renewPromort(promptBuff,prompt,brac);
      }
      else if(cmdID==3){
   	    char* fileName = tokens[0];
        char* absPath = malloc(MAX_INPUT);
        if(isAbsPath(fileName)==0){
       //fprintf(stderr, "not a  absPath");
          getAbsPath(cwd,tokens[0],absPath);
       //fprintf(stderr, "the abs path %s\n", absPath);
          fileName = absPath;
        }
	      exeScript(fileName,cwd,argc,argv,envp);
     //exeScripta(fileName,boundleDir);
        free(absPath);
      }
      else if(cmdID==8){
        clear_screen();
      }
      else if(cmdID==9){
        getcwd(cwd,MAX_INPUT);
        printf("%s\n", cwd);
      }
      else if(cmdID==11){
/*        if(tokens[1]==NULL){
          printf("no input time\n");
        }
        else{
          int input =toInt(tokens[1]);
          sleep(input);
        }*/
        int p=fork();
        args[0]=tokens[1];
        args[1]=tokens[1];

        if(p){
            waitpid(2,&status);
        }
        else{
            execvpe("bin/sleep",args,envp);
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
            waitpid(2,&status);
          }
          else{
            execvpe("bin/cat",args,envp);
          }
      }
        //cat(tokens[1]);
      }
      else if(cmdID==13){
        int p=fork();
        if(p){
            waitpid(2,&status);
        }
        else{
            execvpe("bin/ls",argv,envp);
        }
        printf("\n");
      }
      else if(cmdID==14){
        args[0]=tokens[1];
        args[1]="0";
        args[2]="0";
        int p=fork();
        if(p){
            waitpid(2,&status);
        }
        else{
            execvpe("bin/kill",args,envp);
        }
      }
      else if(cmdID==15){
        //ps();
        int p=fork();
        if(p){
            waitpid(2,&status);
        }
        else{
            execvpe("bin/ps",argv,envp);
        }
      }
      else if(cmdID==16){
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
        printf("%s",args[0]);
        args[i]="0";
        int p=fork();
        if(p){
            waitpid(2,&status);
        }
        else{
            execvpe("bin/echo",args,envp);
        }
      }      
      else if(cmdID==10){
        printf("not found command\n");
/*        tokens[0] = findBin(PATH,boundleDir,tokens[0]);
        exebin(tokens,RUNNONBG,envp);*/
      }
      else{
        //printf("not found command\n");
      }
    //printf("%x",cmdID);

	}
}

int read_command(char* buf){
	memset(buf,0,MAX_INPUT);	
	int res=read_input(0,buf,63/*MAX_INPUT-1*/);
	if(res>0&&buf[0]!='\n'){
    //getcwd(promptBuff,MAX_INPUT);
    //printf("\n%s",prompt);
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
      //printf( "%s\n",cmdToken );
      if(strlen(cmdToken)==4){
        cmdToken = strtok(NULL, "=");
        strcpy(PATH,cmdToken);
        strcat(PATH,"\0");
       //return cmdToken;
     }
   }
 }
 //return NULL;
}

int cmdParser(char* cmd, char* tokens[]){
  //int BG = 0;
  int BG=containBGSign(cmd);
  int ct = 0;
  for (char* cmdToken = strtok(cmd, " "); cmdToken; cmdToken = strtok(NULL, " ")){
    if (ct >= 10){
        break;
      }
    tokens[ct++] = cmdToken;
	}
  for(;ct<10;ct++){
    tokens[ct] = NULL;
  }
/*  if((!strcmp(tokens[0],"set"))&&(!strcmp(tokens[1],"PATH"))){
    return 1;
  }*/
  if(!strcmp(tokens[0],"cd")){//change directory
	 return 2;
	/*if the first token is ./ return 3 */
	}
  else if(strstr(tokens[0],".sh")){
		return 3;
  }
/*  else if((!strcmp(tokens[0],"set"))&&(!strcmp(tokens[1],"PS1"))){
		return 4;
  }*/

  /*else if(containPipe(cmd)){
    return 5;

	}*/
  else if(containSlash(tokens[0])==1){
    return 7;
  }
  else if(!strcmp(tokens[0],"clear")){
    return 8;
  }
  else if(!strcmp(tokens[0],"pwd")){
    return 9;
  }
/*  else if(!strcmp(tokens[0],"echo")){
    return 10;
  }*/
  else if(!strcmp(tokens[0],"sleep")){
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
    //getppid();
    //printf("a");
      int p=fork();
        if(p){
            waitpid(2,0);
        }
        else{
            execvpe("bin/test1",0,0);
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
  //fprintf(stderr, "%s\n", absPath);
}

int isAbsPath(char* path){
  if(path==NULL) return -1;
  if(path[0]=='/') return 1;
  return 0;
}

void exeScript(char* fileName,char* cwd,int argc, char* argv[], char* envp[]){
 // fprintf(stderr, "enter exeScript");
  //fprintf(stderr, "file name %s\n",fileName);
  char *args[5];
  char temp[3];  
  int status=0;
  printf("\nsh");
  int ct = 0;
  int fd = open(fileName,O_RDONLY);
  char* buffer = malloc(MAX_INPUT);
  //fprintf(stderr, "fd %d\n", fd );
  int read_size = read(fd,buffer,1);
  //fprintf(stderr, "read size %d\n", read_size);
  //while(1);
  char* cmdBuff = malloc(MAX_INPUT);
  while(read_size>0){
    if(buffer[read_size-1] != '\n'){
      cmdBuff[ct++] = buffer[read_size-1]; 
    }else{
      cmdBuff[ct] = '\0';
      //fprintf(stderr, "%s\n", cmdBuff);
      if(strstr(cmdBuff,"#!")==NULL){
        //char* cmdClone = malloc(MAX_INPUT);
        //strcpy(cmdClone,cmdBuff);
        //char* dummy[10];
        //int cmdID  = cmdParser(cmdClone,dummy);
        //printf("cmd buff:%s",cmdBuff);
        
        //int BG = containBGSign(cmdBuff);
        char* tokens[10];
        tokenize(tokens,cmdBuff);
        //printf("%s\n",cmdBuff );
        int cmdID = tokenParse(tokens[0],cmdBuff/*BG*/);
        //printf(" tokens0 %d",tokens[0]);
        //printf( "cmd idskadjksandkjasndkjn %d\n", cmdID);
        if(!strcmp(tokens[0],"ls") && tokens[1]==NULL){
          //char* token = malloc(MAX_INPUT);
          //tokens[1] = getcwd(token,MAX_INPUT);
	        //tokens[1]=token;
          //listfiles(tokens[1],0);
          //while(1);
          int p=fork();
          if(p){
            waitpid(2,&status);
          }
          else{
            execvpe("bin/ls",argv,envp);
          }
        }

/*        if(cmdID == 10 || cmdID == -10){
          tokens[0] = findBin(PATH,boundleDir,tokens[0]);
          if(cmdID==10){
            exebin(tokens,RUNNONBG,envp);
          }else{
            exebin(tokens,RUNASBG,envp);
          }
        }*/else{
            //printf("\ncmd:%d\n", cmdID);
          if(cmdID==2){
            //printf("cd1 path %s",tokens[1]);
/*            chdir(tokens[1]);
            getcwd(cwd,MAX_INPUT);
            printf("%s\n",cwd, );*/
            //renewPromort(promptBuff,prompt,brac);
            cd(tokens[1]);
          }else if(cmdID==3){
            char* fileName = tokens[0];
            char* absPath = malloc(MAX_INPUT);
            if(isAbsPath(fileName)==0){
             //fprintf(stderr, "not a  absPath");
              getAbsPath(cwd,tokens[0],absPath);
              //printf( "the abs path %s\n", cwd);
              fileName = absPath;
            }
            exeScript(fileName,cwd,argc,argv,envp);
            free(absPath);
          }
          else if(cmdID==8){
            clear_screen();
          }
          else if(cmdID==9){
            getcwd(cwd,MAX_INPUT);
            printf("\n%s", cwd);
          }
          else if (cmdID==11){
            int p=fork();
            args[0]=tokens[1];
            args[1]="0";
            args[2]=NULL;
            if(p){
              waitpid(2,&status);
            }
            else{
              execvpe("bin/sleep",args,envp);
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
                waitpid(2,&status);
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
            args[0]=tokens[1];
            args[1]="0";
            args[2]="0";
            args[3]=NULL;            
            int p=fork();
            if(p){
              waitpid(2,&status);
            }
            else{
              execvpe("bin/kill",args,envp);
            }
          }
          else if(cmdID==15){
            
            int p=fork();
            if(p){
              waitpid(2,&status);
            }
            else{
              execvpe("bin/ps",argv,envp);
            }
          }
          else if(cmdID==16){         
            printf(" tokens111%s\n", tokens[1]);    
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
               waitpid(2,&status);
            }
            else{
                execvpe("bin/echo",args,envp);
            }
          }                                
          else{
            printf("no command");
          }
          /*else if(cmdID==4){
            if(tokens[2]!=NULL){
              setPS1(PS1,tokens[2]);
              renewPromort(promptBuff,prompt,brac);
            }
          }*/
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
    //closedir("rootfs");
    return;
  }
  int res=chdir(dir);
  if(res==-1){
    printf("dir not found\n");
  }
  //closedir(dir);
}

