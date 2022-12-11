/* 
Starter Reference : https://people.cs.rutgers.edu/~pxk/416/notes/c-tutorials/pipe.html

This program emulates a simple  pipe-operation in the shell.

#### Example 1 ####
the command `ls | tr a-z A-Z` can be executed by doing :
-> gcc unix_pipe_simulator.c && ./a.out cmd_1 "ls" cmd_2 "tr a-z A-Z"


#### Example 2 ####
To add, let's pipe our two custom programs using this unix_pipe_simulator.c program.
-> gcc unix_pipe_simulator.c && ./a.out cmd_1 "./prod" cmd_2 "./con"

### Explanation ### 
In the file-descriptor table of process that runs after executing the first command,  "stdout"
at location 1 will be replaced by a output popped from a `Queue`. 

Similarly, for process that runs after executing second command, "stdin" at location 0 if it's file-descriptor
table will be replaced by input pushed to the same `Queue`.  




NOTES : 
- Output of command typed after `cmd_1` flag will be redirected as input to command typed after `cmd_2` flag
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


typedef struct {
	char *command_1[10];
	char *command_2[10];
} command_list;


void runpipe(int pfd[], command_list* command);

void disp_command(char *command_tokens[]){
    for (int i = 0; i < 10; ++i){
		if (command_tokens[i] == NULL)
			break;
		printf(" AT index : %d || %s\n", i, command_tokens[i]);
	}
}

void tokenize_and_fill(char *command_tokens[], char *command_str){
	// Tokenize the command-string and fill it into the array.
	char *p = strtok (command_str, " ");
	int i = 0;
	while (p != NULL)
    {	
		// This strdup is absolutely Essential.
		//  
        (command_tokens)[i++] = strdup(p);
        p = strtok (NULL, " ");        
    }
	command_tokens[i] = 0;
}


command_list* arg_parser(int num_args, char *args[]){

	char command_1[10]; 
	char command_2[10];
	
	printf("Num of Command-line Arguments %d\n", num_args);

	int cmd_1_match;
	int cmd_2_match;

	for (int i = 0; i < num_args; i++){
		printf("Argument %d : %s \n", i, args[i]);

		if (cmd_1_match) {
			strcpy(command_1, args[i]);
			cmd_1_match = 0;
		}
		if (cmd_2_match) {
			strcpy(command_2, args[i]);
			// command2 = args[i];
			cmd_2_match = 0;
		}
		if (strcmp(args[i], "cmd_1") == 0) {
			puts("Found the first Command");
			cmd_1_match = 1;
		}
		if (strcmp(args[i], "cmd_2") == 0) {
			puts("Found the second Command");
			cmd_2_match = 1;
		}
		// printf("Stored Start Character  %c \n", *(args[i]) );
	}


	command_list *command = malloc(sizeof(command_list));

	tokenize_and_fill( command->command_2 , command_2);
	// disp_command( command->command_2 );

	tokenize_and_fill( command->command_1 , command_1);
	// disp_command( command->command_1 );

	return command;
}


int main(int argc, char *argv[])
{
	int pid, status;
	int fd[2];
	// Caller i.e main is now responsible to deallocate c. 
	command_list *c = arg_parser(argc, argv);


	puts("Insie Main");
	// disp_command(c->command_1);
	// disp_command(c->command_2);

	pipe(fd);

	switch (pid = fork()) {

	case 0: /* child */
		runpipe(fd, c);
		exit(0);

	default: /* parent */
		while ((pid = wait(&status)) != -1)
			fprintf(stderr, "process %d exits with %d\n", pid, WEXITSTATUS(status));
		break;

	case -1:
		perror("fork");
		exit(1);
	}
	exit(0);
}

void runpipe(int pfd[], command_list* command)
{
	int pid;

	switch (pid = fork()) {

	case 0: 
        /* child process : 
        We execute the `tr` process. All process by default use stdin to read-input (from command line)  
        dup2 modifies the file-descriptor at index 0 of file table. And places fd[0] instead
        --------------
        | 0  | fd[1]  |
        ---------------
        | 1  | stdout |
        --------------
        | 2  | stderr |
        --------------
        | 4  | fd[0]  |
        --------------
        | 5  | fd[1]  |
        --------------
        Now `tr` process will read inputs from the in-memory Queue (i.e Pipe).
        */
		dup2(pfd[0], 0);
		close(pfd[1]);	/* the child does not need this end of the pipe */

        /* exec doesn't start a new process. We are in the same process with  */
		execvp((command->command_2)[0], command->command_2); 
		perror((command->command_2)[0]);

	default: /* parent */
		dup2(pfd[1], 1);
		/* the parent does not need this end of the pipe */
		close(pfd[0]);
		execvp((command->command_1)[0], command->command_1); 
		perror((command->command_1)[0]);

	case -1:
		perror("fork");
		exit(1);
	}
}
