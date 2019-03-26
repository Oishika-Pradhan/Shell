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

void bgResponse(int signal)
 {
  pid_t pid;
  int status;
  pid_t processid;
  while((processid = waitpid(-1, &status,WNOHANG)) > 0)
  {
    
    if (processid){
    if(processid != -1)
    {
      bg *temp=background;
      while(temp!=NULL){
        while (temp -> pid != processid)
        temp=temp->next;   
      }
      if(WIFEXITED(status))
      {
        if(temp!=NULL)
        {
          fprintf(stderr,"%s with pid %d exited normally\n",temp->name,processid);
          delete(processid); 
        }
      }
    }}
  }
 }

void procSTOP(int signal) 
{
  
  printf("\nProcess %d has been stopped and sent to bg\n", PID);
  pid_t pid = PID;
  insert(ARR[0],PID); 
  int status=0;
  if(kill(pid, SIGSTOP) < 0)
    perror("Error in putting it the process to bg : ");

}

void exe(char **args)
  {
   PID = fork();
   bg *temp;
   int backg = 0, te = -1, status = 0, i;
   ARR=args;
   signal(SIGTSTP,procSTOP);
   for(i = 0; args[i]!=NULL;i++);
   if(i != 1)
   {
      if(strcmp(args[i-1], "&") == 0)
      {
          backg = 1;
          args[i-1] = NULL;
      }
    }
    if (PID == 0)
    {
         execvp(*args,args);
         signal(SIGTSTP,procSTOP);
         perror(*args);
         exit(1);
    }
    else
    {
        if (backg)
        {
             printf("starting background job %d\n", PID);
             insert(args[0],PID);
             signal(SIGCHLD,bgResponse);
        }
        else
          while(wait(&status)!=PID);
        if (status != 0)
            fprintf  (stderr, "error: %s exited with status code %d\n", args[0], status);
    }
}

int exec(char **tokens)
{
  if(strcmp(tokens[0],"cd") == 0)
  {
    cd(tokens);
    return 1;
  }
  else if(strcmp(tokens[0],"pwd") == 0)
  {
    pwd();
    printf("\n");
    return 1;
  }
  else if(strcmp(tokens[0],"echo") == 0)
  {
      echo(tokens);
      return 1;
  }
  else if(strcmp(tokens[0],"pinfo") == 0)
  {
    pinfo(tokens);
    return 1;
  }
  else if(strcmp(tokens[0],"quit") == 0)
  {
    exit(0);
    return 1;
  }
  else if(strcmp(tokens[0],"jobs") == 0)
  {
    jobs();
    return 1;
  }
  else if(strcmp(tokens[0],"killallbg") == 0)
  {
    killallbg();
    return 1;
  }
  else if(strcmp(tokens[0], "kjobs") == 0)
  {
    kjobs(tokens);
    return 1;
  }
  else if(strcmp(tokens[0], "fg") == 0)
  {
    fg(tokens);
    return 1;
  }
  else
  {
    exe(tokens);
    return 1;
  }
  return 0;
}