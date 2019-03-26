#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <makefile.h>

char **pipeparser(char *com)
{
  int tok_size=t_size;
  char **tokens=malloc(sizeof(char *)*tok_size);
  char *token;
  token = strtok(com,PIPE);
  int position=0;
  while (token != NULL)
  {
    tokens[position] = token;
	++position;
    if(position>=tok_size)
    {
      tok_size += t_size;
      tokens = realloc(tokens,tok_size*sizeof(char *));
    }
    token = strtok(NULL,PIPE);
  }
  return tokens;
}
