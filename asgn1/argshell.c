#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

extern char ** get_args();

int main() {
  int stat;
  char **args;

  //Get args continue until exit is called
  while (1) {
    printf("Command ('exit' to quit): ");
    args = get_args();
    checkCommands(args);
  }
}

//Exit command terminates the shell
//shell commands exiting the shell
void exitCommand() {
  exit(0);
}

int shellLoop(){
  int stat = 1;
  char ** args;
  while (1) {
    args = get_args();
    checkCommands(args);
  }
  return (0);
}

//Runs commands
int executeCommand(char **args) {
  int stat;
  //Fork a child process
  int pid = fork();
  //On failure -1 is returned in the parent, no child process is created
  if ((pid < 0)) {
    printf("Error forking into child process");
    exit(1);
  }
  //Child process
  else if (pid == 0) {
    //the function only returns if an error has occurred. the return value is -1
    //Run the args
    //Array pointers termintated by NULL
    if (execvp(args[0], args) < 0) {
      perror("Error in executing \n");
      exit(1);
    }
  }
  //parent process wait until child is done
  else {
    pid = wait( &stat)
  }
  return (0);
}

// Special case <
int inputRedirection(char **args, int i){
  int readFile;
  int stat;
  //Fork a child process
  int pid = fork();
  //On failure -1 is returned in the parent, no child process is created
  if (pid < 0) {
    printf("Error forking into child process");
    exit(1);
  }
  //Child process
  else if (pid == 0) {
    //Read only open
    readFile = open(args[i+1], O_RDONLY);

    //on cucess system returns the new file descriptor. on error -1 is returned
    //dup2(in, 0) ==  replace standard input with input part of pipe
    //Copy readFile into slot 0
    if(dup2(readFile, 0) < 0){
      perror("dup2 error");
      exit(1);
    }
    //close unused files
    close(readFile);

    //Replace < with NULL so it knows when to stop
    arg[i] = NULL;
    //the function only returns if an error has occurred. the return value is -1
    //Run the args
    //Array pointers termintated by NULL
    execvp(args[0], args);
  }
  //parent process wait until child is done
  else{
    pid = wait(&stat);
  }
  return shellLoop();
}

// special case >
int outputRedirection(char **args, int i){
  int stat;
  int pid;
  int newOutput;

  //Fork a child process
  int pid = fork();
  //On failure -1 is returned in the parent, no child process is created
  if (pid < 0) {
    printf("Error forking into child process");
    exit(1);
  }

  //Child process
  else if (pid == 0) {
    //If the file does not exist, create it. If the O_CREAT option is used, then you must include the third parameter.
    //Combined with the O_CREAT option, it ensures that the caller must create the file. If the file already exists, the call will fail.
    //	Open the file so that it can be read from and written to.
    newOutput = open(args[i+1],O_CREAT|O_EXCL| O_RDWR)
    //dup into output
    if (dup2(newOutput, 1) < 0) {
      perror("dup2 error");
      exit(1);
    }
    //close
    close(newOutput);
    //NULL to know when it ends
    arg[i] = NULL;
    //execute
    execvp(args[0], args);

  //parent process wait
  }else{
    pid = wait(&stat);
  }
  return shellLoop();
}

// special case >>
//same as outputRedirection
//except append to the end. if no file then dont make new file
int outputRedirection2(char **args, int i){
  int stat;
  int pid;
  int newOutput;

  if (pid < 0) {
    printf("Error forking into child process");
    exit(1);
  }
  else if (pid == 0) {
    newOutput = open(args[i+1],O_APPEND|O_WRONLY)
    if (dup2(newOutput, 1) < 0) {
      perror("dup2 error");
      exit(1);
    }
    close(newOutput);
    arg[i] = NULL;
    execvp(args[0], args);
  }else{
    pid = wait(&stat);
  }

  return shellLoop();
}

// special character |
int outputPiped(char **args, int i){

  int pipeArray[2];
  int pid;
  int pid2;
  int stat;

  //Create 2d array to
  char **argsMatrix = [2][sizeof(args)]

  //Populate 2d Array
  for (int q = 0; q < 2; q++) {
    for (int w = 0; w < sizeof(args); w++) {
      argsMatrix[q][w] = args[w];
    }
  }

  args[i] = NULL;

  //Populate
  for (int k = 0; args[k] != NULL; k++) {
    argsMatrix[0][k] = args[k];
  }

  // populate with new
  for (int j = i+1; args[j] != NULL; j++) {
    argsMatrix[1][j] = args[j];
  }

  //pipeArray[0] = read pipeArray[1] = writing
  pipe(pipeArray);

  //Fork child process
  pid = fork();

  if (pid < 0) {
    printf("Error forking into child process");
    exit(1);
  }

  //child
  else if(pid == 0){
    dup2(pipefd[0], 0);
    close(pipefd[1]);
    close(pipefd[0]);
    execvp(args[i+1], argsMatrix[1][]);
    return shellLoop();
    }
  //parent
  else{
    //Fork second child process
    pid2 = fork();
    // parent's second child
    if (pid2 == 0){
      //copy into write
      dup2(pipefd[1], 1);
      close(pipefd[0]);
      close(pipefd[1]);
      execvp(args[0], argsMatrix[0][]);
      return shellLoop();
      }
    }
    //Parent returns
    return shellLoop();
}

int semicolonRedirection(char **args, int i){
  int semicolonCounter = 0;

  //count semicolons
  for (int i = 0; args[i] != NULL; i++) {
    if (strcmp(args[i], ";") == 0) {
      semicolonCounter+=1;
    }
  }

  //create 2d array
  char **argsMatrix = [2][sizeof(args)]

  //populate 2d array
  for (int q = 0; q < 2; q++) {
    for (int w = 0; w < sizeof(args); w++) {
      argsMatrix[q][w] = args[w];
    }
  }

  //first ; is null
  args[i] = NULL;

  //populate
  for (int k = 0; args[k] != NULL; k++) {
    argsMatrix[0][k] = args[k];
  }

//populate
  for (int j = i+1; args[j] != NULL; j++) {
    argsMatrix[1][j] = args[j];
  }



  int pid_child;
  int status_child;
  int pid_par;

  //fork
  pid_child = fork();

  //wile there are semiclon counters
  for (int p = 0; p < semicolonCounter; p++) {
    //child process
    if (pip_child == 0) {
      if (execvp(argsMatrix[0][0], argsMatrix[0][]) < 0) {
        printf("Error");
      }
      else if (execvp(argsMatrix[1][0], argsMatrix[1][]) < 0) {
        printf("Error");
      }
    }
    else{
      //while
      while(pid_par != pid_child){
        pid_par = wait(&status_child);
      }
      return status_child
    }
  }
  return shellLoop();
}

int cdCommand(char **args, int i){
  int pid;
  int stat;
  int dir;
  char *path;
  char *buffer;
  long size;

  //CD = NULL
  args[1] = NULL;

  if(pid < 0){
    perror("Error forking into child process");
  }
  //Child
  else if(pid == 0){
    //returns the maximum number of characters in a complete pathname. This number does not include a terminating NULL for pathnames stored as strings.
    size = pathconf(".", _PC_PATH_MAX);
    if((buffer = (char *)malloc((size_t)size)) !=NULL){
      //If successful, getcwd() returns a pointer to the buffer.
      path = getcwd(buffer, (size_t)size);
    }
    //Move to the path
    if ((dir = chdir(path)) == 0){
      return(dir);
    }
    else{
      perror("cd fail");
    }
    else{
      pid = wait(&stat);
    }
  }
  return shellLoop();
}

//Check through each arg first before executing
void checkCommands(char ** args) {
  if (args[0] == NULL) {
    printf ("No arguments on line!\n");
  }
  else if (strcmp(args[0], "exit") == 0) {
      printf ("Exiting...\n");
      break;
  }
  else if (strcmp(args[0], "cd") == 0) {
    cdCommand(args);
  } else {
    for (i = 1; args[i] != NULL; i++) {
      printf("Argument %d: %s\n", i, args[i]);
      if ((strcmp(args[i], "|") == 0)) {
        outputPiped(args, i);
      } else if (strcmp(args[i], "<") == 0) {
        inputRedirection(args, i);
      } else if (strcmp(args[i], ">") == 0) {
        outputRedirection(args, i);
      } else if (strcmp(args[i], ">>") == 0) {
        outputRedirection2(args, i);
      } else if (strcmp(args[i], ";") == 0) {
        semicolonRedirection(args, i);
      }
    }
  }
  executeCommand(args);
}
