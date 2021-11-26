#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>


#define DELIMITATORS " \t\r\n\a"

int command_exit(char **args);


char *builtin[] = {
  "exit"
};


int (*builtin_func[]) (char **) = {
  &command_exit
};


int qty_builtins() {
  return sizeof(builtin) / sizeof(char *);
}


int command_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "Expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}


int command_exit(char **args) {
    return 0;
}


char *command_pwd(char **args) {
    int buffer_size = 64;
    char *buffer;
    buffer = malloc(buffer_size * sizeof(char*));
    
    if (getcwd(buffer, buffer_size) == NULL) {
        buffer = realloc(buffer, buffer_size * sizeof(char*));
        getcwd(buffer,buffer_size * 2);
    }
    
    return buffer;
}


char *read_command_line() {
    char *line = NULL;
    size_t buffer_size = 0;
    
    if (getline(&line, &buffer_size, stdin) == -1){
        if (feof(stdin)) {
            fprintf(stderr, "There was an allocation errr\n");
            exit(EXIT_SUCCESS); 
        } else  {
            fprintf(stderr, "There was an allocation errr\n");
            exit(EXIT_FAILURE);
        }
    }
    
    return line;
}


char **split_command_input(char *command) {
    int buffer_size = 64, position = 0;
    char **arguments = malloc(buffer_size * sizeof(char*));
    char *argument;
    
    if (!arguments) {
        fprintf(stderr, "There was an allocation error\n");
        exit(EXIT_FAILURE);
    }
    
    argument = strtok(command, DELIMITATORS);
    
    while (argument != NULL) {
        arguments[position] = argument;
        position++;
        
        if (position >= buffer_size) {
            buffer_size += buffer_size;
            arguments = realloc(arguments, buffer_size * sizeof(char*));
            
            if (!arguments) {
                fprintf(stderr, "There was an allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        argument = strtok(NULL, DELIMITATORS);
    }
    
    arguments[position] = NULL;
    return arguments;
}


int launch_command(char **args)
{
  pid_t pid;

  pid = fork();
  if (pid == 0) {
    
    if (execvp(args[0], args) == -1) {
        printf("CE_COP: command not found %s\n", args[0]);
    }
    
    exit(EXIT_FAILURE);
    
  } else if (pid < 0) {
    fprintf(stderr, "There was an unexpected error!\n");
  }

  return 1;
}


int execute_command(char **args) {
    int i; 
    
    if (args[0] == NULL) {
        return 1;
    }
    
    for (i = 0; i < qty_builtins(); i++) {
        if (strcmp(args[0], builtin[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }
    
    return launch_command(args);
}


void loop(void) {
    char *command_line;
    char **args;
    int status;
    
    do {
        printf("sh> ");
        command_line = read_command_line();
        args = split_command_input(command_line);
        status = execute_command(args);
        
        free(command_line);
        free(args);
    } while(status);
}


int main() {
    loop();
    return EXIT_SUCCESS;
}
