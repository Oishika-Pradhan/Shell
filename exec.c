#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <makefile.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pipe.h>
#include <execute.h>
void shell_initialization();

void shell_loop();

int main(int argc,char *argv[])
{
  
  shell_loop();
  return 0;
}

void shell_initialization()
{
  //bgflag=0;
  shell=STDIN_FILENO;
  if(isatty(STDIN_FILENO))
  {
    while(tcgetpgrp(STDIN_FILENO)!=(shell_pgid=getpgrp()))
      kill( shell_pgid, SIGTTIN);
    signal (SIGINT, SIG_IGN);
        signal (SIGQUIT, SIG_IGN);
        signal (SIGTSTP, SIG_IGN);
        signal (SIGTTIN, SIG_IGN);
        signal (SIGTTOU, SIG_IGN);
        signal (SIGCHLD, SIG_IGN);
        if(setpgid(shell_pgid,shell_pgid)<0)
        {
          perror("Can't put a shell as a member of it's own process group");
          exit(1);
        }
        tcsetpgrp(STDIN_FILENO,shell_pgid);
  }
}

void insert(char *process,pid_t pid)
{
  //printf("%s\n",process);
  bg *new=(bg*)malloc(sizeof(bg));
  strcpy(new->name,process);
  new->pid=new->pgid=pid;
  new->next=NULL;
  if(background==NULL)
    background=new;
  else
  {
    bg *temp=background;
    while(temp->next!=NULL)
      temp=temp->next;
    temp->next=new; 
  }
}

void delete(pid_t pid)
{
  if(background!=NULL)
  {
    bg *temp=background;
    if(background->pid==pid)
    {
      background=background->next;
      free(temp);
    } 
    else
    {
      bg *temp2;
      while(temp!=NULL && temp->pid!=pid)
      {
        temp2=temp;
        temp=temp->next;
      }
      if(temp!=NULL)
      {
        temp2->next=temp->next;
        free(temp);
      }
    
    }
  }
}

void sigintHandler(int sig_num)
{
    signal(SIGINT, sigintHandler);
    printf("\n");
    fflush(stdout);
}

void redir_out(char *token,char *command)
{
        int status=0,pid,r;
        char **k,*filename;
        filename = strtok(token,TOK_DELIM);
        FILE *test=fopen(filename, "w");
        int fd=fileno(test); 
        k=lineparser(command);
        pid=fork(); 
        if (pid == 0) 
        { 
          dup2(fd,1);
          r=exec(k); 
          exit(0);
        }
        else
          while(wait(&status)!=pid);

}
void redir_in(char *token,char *command)
{
        int status=0,pid,r;
        char **k,*filename;
        filename = strtok(token,TOK_DELIM);
        FILE *test=fopen(filename, "r");
        int fd=fileno(test); 
        k=lineparser(command);
        pid=fork(); 
        if (pid == 0) 
        { 
          dup2(fd,0);
          r=exec(k); 
          exit(0);
        }
        else
          while(wait(&status)!=pid);

}
void redir_app(char *token,char *command)
{
        int status=0,pid,r;
        char **k,*filename;
        filename = strtok(token,TOK_DELIM);
        FILE *test=fopen(filename, "a");
        int fd=fileno(test); 
        k=lineparser(command);
        pid=fork(); 
        if (pid == 0) 
        { 
          dup2(fd,1);
          r=exec(k); 
          exit(0);
        }
        else
          while(wait(&status)!=pid);

}

void redir_ior(char *token,char *command,char *token2)
{
        int status=0,pid,r;
        char **k,*filename,*file2;
        filename = strtok(token,TOK_DELIM);
        file2 = strtok(token2,TOK_DELIM);
        FILE *test=fopen(filename, "r");
        FILE *test1=fopen(file2, "w");
        int fd=fileno(test); 
        int fd1=fileno(test1);
        k=lineparser(command);
        pid=fork(); 
        if (pid == 0) 
        { 
          dup2(fd,0);
          dup2(fd1,1);
          r=exec(k);
          exit(0);
        }
        else
          while(wait(&status)!=pid);
}
void redir(int flag,char **l)
{
  if(flag==0)
      {
        //printf("Hi\n");
        redir_out(l[1],l[0]);

      }
      else if(flag==1)
      {
        redir_in(l[1],l[0]);
      }
      else if(flag==2)
       {
        //printf("hi\n");
        redir_app(l[1],l[0]);
       }
      else if(flag==3)
      {
        redir_ior(l[1],l[0],l[2]);
      }
}

int pipeCommand(char **args) 
{
  int status=0,numOfPipes=-1,flag=-1,len;
  pid_t pid, wpid;
  char **k,**l;
  int i, j, total,r;
  for(i=0;args[i]!=NULL;i++)
   {
    //printf("%s\n",args[i]);
    numOfPipes++;
      }
  int pipes[2*numOfPipes];
  for(i=0;i<(2*numOfPipes);i+=2) 
  {
    pipe(pipes + i);
  }
  for(i=-2;i<(2*numOfPipes);i+=2) 
  {
    //printf("%s\n",args[(i+2)/2]);
    if ((pid = fork()) == 0) 
    {
      if((i+3)<(2*numOfPipes))
        dup2(pipes[i+3], 1);
      if(i!=-2)
        dup2(pipes[i], 0);
      for(j=0;j<(2*numOfPipes);j++)
        close(pipes[j]);
      r = i/2 + 1;
      len = check1(args[(i+2)/2]);
      if(len==2)
      {
         flag=check(args[(i+2)/2]);
         if(flag!=2)
          flag=3;
      }
      else
        flag=check(args[(i+2)/2]);
      
      l=redirparser(args[(i+2)/2]);
      if(l[1]!='\0')
      { 
       redir(flag,l);
       r=1;
      }
      else
      {
        k=lineparser(args[(i+2)/2]);
        r=exec(k);
      }
      exit(EXIT_FAILURE);
    }
  }
  i = 0;
  while(i<(2*numOfPipes)){
    close(pipes[i]);
    i++;
  }
  while ((wpid = wait(&status)) > 0);
  return 0;
}

void shell_loop()
{
  register struct passwd *pw;
  register uid_t uid;
	char hostname[1024],cwd[1024],*a;
	int res,r,c,i,flag,len,length;
  char *inp_com,**k,**l,*inp,**p;
  uid = geteuid();
  pw = getpwuid(uid);
  int status = 0;
  hostname[1023] = '\0';
  signal(SIGINT, sigintHandler);
  gethostname(hostname, 1023);
  do
  {
    //fflush(stdin);
    if (pw)
    {
        printf(ANSI_COLOR_GREEN  "%s@%s:"  ANSI_COLOR_BLUE,pw->pw_name, hostname);
        pwd();
        printf("$" ANSI_COLOR_RESET);
    }
    //fflush(stdout);
    inp_com=readline();
    len=check1(inp_com);
    if(len==2)
    {
       flag=check(inp_com);
       if(flag!=2)
        flag=3;
    }
    else
      flag=check(inp_com);
    if(inp_com[0]=='\0')
      continue; 
    p=pipeparser(inp_com);
    if(p[1]=='\0')
    { 
      l=redirparser(inp_com);
      if(l[1]=='\0')
      {
        k=lineparser(inp_com);
        r=exec(k);
      }
      else
      {
        redir(flag,l);
        r=1;
      }
    }
    else
    {
      r=pipeCommand(p);
      r=1;
    }
  }while(r);
}
