#include <stdio.h>
#include <time.h>
#include <string.h>
#include <wait.h>
//#include <unistd.h>

char* readLine();
char** tokenizeInput(char*);
int executeCommand(char**);

int main(int argc, char** argv) {
	char* input;
	printf("Welcome to Theo\'s Simple C Shell!\n");
	do {
		time_t t;
		time(&t);
		printf("\n%s_> ", ctime(&t));
		input = readLine();
		char** tokens = tokenizeInput(input);
		// printf("%d", sizeof(tokens)/sizeof(tokens[0]));
		if (strcmp(tokens[0], "cd") == 0) {
			chdir(tokens[1]);
		}
		else if (strcmp(tokens[0], "exit") == 0) {
			// Do nothing, the while loop will handle the exit
		}
		else {
			executeCommand(tokens);	
		}
	} while (strcmp("exit", input) != 0);
}

char* readLine() {
	char* input = NULL;
	ssize_t bufferSize = 0;
	int result = getline(&input, &bufferSize, stdin);
	if (result < 1) {
		printf("Error Reading Line");
	}
	return input;
}

char** tokenizeInput(char* inputLine) {
	const int BUFFER_INCREMENT = 64;
	int bufferSize = BUFFER_INCREMENT, bufferPosition = 0;
	char** tokens = malloc(bufferSize * sizeof (char*));
	char* token;

	if (!tokens) {
		fprintf(stderr, "Error Allocating Token Memory");
		exit(1);
	}

	token = strtok(inputLine, " ");
	while (token != NULL) {

		// printf("\nWriting token '%s' to buffer position '%d'", token, bufferPosition);
		for (int i = 0; i < strlen(token); i++) {
			if (token[i] == '\n') token[i] = 0;
		}
		tokens[bufferPosition] = token;
		bufferPosition++;


		if (bufferPosition >= bufferSize) {
			bufferSize += BUFFER_INCREMENT;
			tokens = realloc(tokens, bufferSize * sizeof (char*));
			if (!tokens) {
				fprintf(stderr, "Error Allocating Token Memory Loop");
				exit(1);
			}
		}

		token = strtok(NULL, " ");

	}
	tokens[bufferPosition] = NULL;
	// free(tokens);
	return tokens;
}

int executeCommand(char** commands) {
	pid_t pid, wpid;
	int status;

	pid = fork();
	if (pid == 0) {
		// Child
		int execResult = execvp(commands[0], commands);
		if (execResult) {
			printf("Error Executing execvp(%s)", commands[0]);
			exit(1);
		}
	} else if (pid > 0) {
		// Parent
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	} else {
		// Error
		perror("Error Forking");
	}
	return 1;
}