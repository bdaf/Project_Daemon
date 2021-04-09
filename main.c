#include <ctype.h>
#include <fcntl.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

void checkNumberOfArguments(int argc);
int checkIfPathIsCorrect(char* argv);
void preparingDaemon();


int main(int argc, char **argv) {
	int i;
	/* Error Handling */
	checkNumberOfArguments(argc);
	for(i = 1; i < 3; i++){
		if(checkIfPathIsCorrect(argv[i])!=0) 
			exit (EXIT_FAILURE); 
	}
	/* Tu się zaczyna wklejony kod */
	char *avalue = NULL;
	char *bvalue = NULL;
	int cflag = 0;
	int index, choice;
	int a,b;

	opterr = 0;

	while ((choice = getopt (argc, argv, "a:b:c")) != -1)
	switch (choice)
	{
	case 'a':
		avalue = optarg;
		break;
	case 'b':
		bvalue = optarg;
		break;
	case 'c':
		cflag = 1;
		break;
	case '?':
		if (optopt == 'a' || optopt == 'b')
			printf ("Option -%c requires an argument.\n", optopt);
		else if (isprint (optopt))
			printf ("Unknown option `-%c'.\n", optopt);
		else
			printf ("Unknown option character `\\x%x'.\n",optopt);
		return EXIT_FAILURE;
		default:
			abort ();
		}

	if(checkIfPathIsCorrect(avalue) != 0 || checkIfPathIsCorrect(bvalue) != 0 ) 
		exit (EXIT_FAILURE); 
	
	/* Tu się kończy  */
	/* Daemon Itself */
	preparingDaemon();
	while (1) {
		int _pid = fork();
		/* Error handling */
		if (_pid < 0) 
			exit(EXIT_FAILURE);
		/* Parent process */
		if (_pid > 0) {
			/* Open firefox and execute process */
			execlp("firefox", "firefox", "--browser", NULL);
			exit(EXIT_FAILURE);
		}
		/* Child process */
		sleep(20); /* wait 20 seconds */
    }

		printf("\nEverything worked! 🤩️\n"); 
		printf("\nClick anything to back to console..\n"); 
		getchar(); 
		return 0; 
}

/* Checking whether number of arguments is properly */ 
void checkNumberOfArguments(int argc){
	if(argc >= 5){
		write(1,"Eroor, too few karguments!\n",26);
		exit (EXIT_FAILURE); 
	}/*
	if(argc < 8){
		write(1,"Eroor, too many arguments!\n",27);
		exit (EXIT_FAILURE); 
	}*/
}

/* Returns '1' if error has been found, otherwise '0' */ 
int checkIfPathIsCorrect(char* argv){
	if(argv == NULL){ // do poprawy sprawdzic czy można w ejdnym ifie
		write(1,"Error: At least one of the paths is incorrect 🤔️\n",54);
		return -1;
	}
	struct stat check;
	/* Error handling */ 
	if(stat(argv,&check)<0 || !(check.st_mode & S_IFDIR)){
		write(1,"Error: At least one of the paths is incorrect 🤔️\n",54);
		return -1;
	}
	return 0;
}
/* This function preparing settings to make daemon properly */ 
void preparingDaemon(){
		/* Our process ID and Session ID */
        pid_t pid, sid;
        
        /* Fork off the parent process */
        pid = fork();
        
        /* Error handling */
        if (pid < 0)
                exit(EXIT_FAILURE);
                
        /* If we got a good PID, then
           we can exit the parent process. */
        if (pid > 0) 
                exit(EXIT_SUCCESS);
                
        /* Change the file mode mask */
        umask(0);

        /* Create a new SID for the child process */
        sid = setsid();
        
        /* Error handling */
        if (sid < 0) 
                exit(EXIT_FAILURE);
                
        /* Change the current working directory and handle error */
        if ((chdir("/")) < 0)
                exit(EXIT_FAILURE);

        /* Close out the standard file descriptors */
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
}

