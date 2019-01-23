  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <sys/types.h>
  #include <errno.h>
  #include <string.h>
  #include <fcntl.h>
  #include <sys/wait.h>
  #include <limits.h>
  #include <stdbool.h>

  int shellLoop();
  extern char ** get_args();
  char cwd[256];
  bool cwdFlag = false;

  //Exit command terminates the shell
  //shell commands exiting the shell
  void exitCommand() {
    exit(0);
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
      pid = wait( &stat);
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
      readFile = open(args[i+1], O_RDONLY | O_CREAT, 0755);

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
      args[i] = NULL;
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
      newOutput = open(args[i+1], O_WRONLY | O_CREAT, 0640);
      //dup into output
      if (dup2(newOutput, 1) < 0) {
        perror("dup2 error");
        exit(1);
      }
      //close
      close(newOutput);
      //NULL to know when it ends
      args[i] = NULL;
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
      newOutput = open(args[i+1],O_APPEND|O_WRONLY);
      if (dup2(newOutput, 1) < 0) {
        perror("dup2 error");
        exit(1);
      }
      close(newOutput);
      args[i] = NULL;
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

    char **args1 = malloc(sizeof(args));
    char **args2 = malloc(sizeof(args));

    memcpy(args1, args, sizeof(args));
    memcpy(args2, args, sizeof(args));

    args[i] = NULL;

    //creates new arrays to store sections of the command line input
    for(int j = 0; args[j] != NULL; j++) {
        args1[j] = args[j];
    }

    for(int z = i+1; args[z] != NULL; z++){
        args2[z] = args[z];
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
      dup2(pipeArray[0], 0);
      close(pipeArray[1]);
      close(pipeArray[0]);
      execvp(args[i+1], args2);
      return shellLoop();
      }
    //parent
    else{
      //Fork second child process
      pid2 = fork();
      // parent's second child
      if (pid2 == 0){
        //copy into write
        dup2(pipeArray[1], 1);
        close(pipeArray[0]);
        close(pipeArray[1]);
        execvp(args[0], args1);
        return shellLoop();
        }
      }
      //Parent returns
      return shellLoop();
  }

  int semicolonRedirection(char **args, int i){
    int semicolonCounter = 0;
    int status_child;
    int pid_par;

    //count semicolons
    for (int i = 0; args[i] != NULL; i++) {
      if (strcmp(args[i], ";") == 0) {
        semicolonCounter+=1;
      }
    }

    char **args1 = malloc(sizeof(args));
    char **args2 = malloc(sizeof(args));

    memcpy(args1, args, sizeof(args));
    memcpy(args2, args, sizeof(args));

    args[i] = NULL;

    //creates new arrays to store sections of the command line input
    for(int j = 0; args[j] != NULL; j++) {
        args1[j] = args[j];
    }

    for(int z = i+1; args[z] != NULL; z++){
        args2[z] = args[z];
    }

    int pid_child;
    //fork
    pid_child = fork();

    //wile there are semiclon counters
    for (int p = 0; p < semicolonCounter; p++) {
      //child process
      if (pid_child == 0) {
        if(execvp(args1[0], args1) < 0) {
          printf("Error");
        }
        if(execvp(args2[0], args2) < 0){
          printf("Error");
        }
      }
      else{
        //while
        while(pid_par != pid_child){
          pid_par = wait(&status_child);
        }
        return status_child;
      }
    }
    return shellLoop();
  }

  int cdCommand(char **args, int i){

    if ((args[i+1]) == NULL) {
      if (chdir(cwd) != 0) {
        perror("chdir() failed cwd");
        return shellLoop();
      }
    }

    if (chdir(args[i+1]) != 0) {
      perror("chdir() failed");
      return shellLoop();
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
        //break;
        exitCommand();
    }

    else if (strcmp(args[0], "cd") == 0) {
      cdCommand(args, 0);
    }

    else {
      for (int i = 1; args[i] != NULL; i++) {
        printf("Argument %d: %s\n", i, args[i]);
        if ((strcmp(args[i], "|") == 0)) {
          outputPiped(args, i);
        }
        else if (strcmp(args[i], "<") == 0) {
          inputRedirection(args, i);
        }
        else if (strcmp(args[i], ">") == 0) {
          outputRedirection(args, i);
        }
        else if (strcmp(args[i], ">>") == 0) {
          outputRedirection2(args, i);
        }
        else if (strcmp(args[i], ";") == 0) {
          semicolonRedirection(args, i);
        }
        else if (strcmp(args[0], "cd") == 0) {
          cdCommand(args, i);
        }
      }
    }
    executeCommand(args);
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

  int main() {
    int stat;
    char **args;

    if (cwdFlag == false) {
      cwdFlag = true;
      printf("cwdflag true");
      if(getcwd(cwd, sizeof(cwd)) == NULL){
        perror("get cwd error");
        return shellLoop();
      }else{
        perror("get cwd passed");
        printf("%s\n", cwd);
      }
    }
    //Get args continue until exit is called
    while (1) {
      printf("Command ('exit' to quit): ");
      args = get_args();
      checkCommands(args);
    }
  }
