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
      perror("Error forking into child process");
      exit(0);
    }
    //Child process
    else if (pid == 0) {
      //the function only returns if an error has occurred. the return value is -1
      //Run the args
      //Array pointers termintated by NULL
      if (execvp(args[0], args) < 0) {
        perror("Error in executing \n");
        exit(0);
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
    //save stdin
    int saved = dup(0);
    //Fork a child process
    int pid = fork();
    //On failure -1 is returned in the parent, no child process is created
    if (pid < 0) {
      perror("Error forking into child process");
      exit(1);
    }
    //Child process
    else if (pid == 0) {
      //Read only open
      readFile = open(args[i+1], O_RDONLY);
      //put readfile into stdin
      dup2(readFile, 0);
      if (execvp(args[0], args) < 0 ) {
        perror("execvp");
        return shellLoop();
      }
      //put stdin back in
      dup2(saved,1);
      close(readFile);
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
      perror("Error forking into child process");
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
      //execute
      execvp(args[0], args);
      //close
      close(newOutput);
    //parent process wait
    }else{
      pid = wait(&stat);
    }
    return shellLoop();
  }

  // special case >>
  //same as outputRedirection
  //except append to the end. if no file then make new file
  int outputRedirection2(char **args, int i){
    int stat;
    int newOutput;

    int pid = fork();
    if (pid < 0) {
      perror("Error forking into child process");
      exit(1);
    }
    else if (pid == 0) {
      newOutput = open(args[i+1],O_APPEND | O_CREAT | O_WRONLY );
      if (dup2(newOutput, 1) < 0) {
        perror("dup2 error");
        exit(1);
      }
      close(newOutput);
      args[i] = NULL;
      if (execvp(args[0], args) < 0 ) {
        perror("execvp");
        return shellLoop();
      }
    }else{
      pid = wait(&stat);
    }

    return shellLoop();
  }

  int semicolonRedirection(char **args, int i){
    //fork
    int pid = fork();
    int stat;
    if ((pid < 0)) {
      perror("Error forking into child process");
      exit(0);
    }
    //child process
    else if (pid == 0) {
      //wile there are semiclon counters
      //Gonna go backwards.
      for (int i = sizeof(args); args[i] != NULL; i--) {
        int count = 0;
        if (strcmp(args[i], ";") == 0) {
          //Fork a child process
          int pidSecond = fork();
          //On failure -1 is returned in the parent, no child process is created
          if ((pidSecond < 0)) {
            perror("Error forking into child process");
            exit(0);
          }
          //Child process
          else if (pidSecond == 0) {
            //the function only returns if an error has occurred. the return value is -1
            //Run the args
            //Array pointers termintated by NULL
            if (execvp(args[i - count], args) < 0) {
              perror("Error in executing \n");
              exit(0);
            }
          }
          //parent process wait until child is done
          else {
            pid = wait( &stat);

          }
        }
        count += 1;
      }
    }
    return shellLoop();
  }

  //cd Commands
  int cdCommand(char **args, int i){

    //if cd has nothing go to cwd
    if ((args[i+1]) == NULL) {
      if (chdir(cwd) != 0) {
        perror("chdir() failed cwd");
        return shellLoop();
      }
    }

    //go to directory
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
          //Did not finish
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
    printf("Command ('exit' to quit): ");
    while (1) {
      args = get_args();
      checkCommands(args);
    }
    return (0);
  }

  int main() {
    int stat;
    char **args;
    //if first time get cwd
    if (cwdFlag == false) {
      cwdFlag = true;
      if(getcwd(cwd, sizeof(cwd)) == NULL){
        perror("get cwd error");
        return shellLoop();
      }
    }
    //Get args continue until exit is called
    while (1) {
      printf("Command ('exit' to quit): ");
      args = get_args();
      checkCommands(args);
    }
  }
