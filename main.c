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
	/* Tu się zaczyna wklejony kod */
	char *sourcePath = NULL;			/* Flag a */
	char *targetPath = NULL;			/* Flag b */
	int timeDeamon = 360;				/* Flag t */
	int recursion = 0;					/* Flag R */
	int dependenceOfFileSize = 1024;	/* Flag s */

	opterr = 0;

	int choice;
	while ((choice = getopt (argc, argv, "a:b:t:s:R")) != -1)
	switch (choice)
	{
	case 'a':
		targetPath = optarg;
		break;
	case 'b':
		sourcePath = optarg;
		break;
	case 't':
		timeDeamon = atoi(optarg);
		if(timeDeamon == 0){
			write(1,"Invalid value of argument -t 🤔️\n",37);
			exit (EXIT_FAILURE); 
		}
		break;
	case 's':
		dependenceOfFileSize = atoi(optarg);
		if (dependenceOfFileSize == 0)
		{
			write(1,"Invalid value of argument -s 🤔️\n",37);
			exit (EXIT_FAILURE); 
		}
		
		break;
	case 'R':
		recursion = 1;
		break;
	case '?':
		if (optopt == 'a' || optopt == 'b'|| optopt == 't'|| optopt == 's')
			printf ("Option -%c requires an argument.\n", optopt);
		else if (isprint (optopt))
			printf ("Unknown option `-%c'.\n", optopt);
		else
			printf ("Unknown option character `\\x%x'.\n",optopt);
		return EXIT_FAILURE;
		default:
			abort ();
		}

	if(checkIfPathIsCorrect(targetPath) != 0 || checkIfPathIsCorrect(sourcePath) != 0 ) 
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
		sleep(timeDeamon); /* wait 20 seconds */
    }

		printf("\nEverything worked! 🤩️\n"); 
		printf("\nClick anything to back to console..\n"); 
		getchar(); 
		return 0; 
}

/* Checking whether number of arguments is properly */ 
void checkNumberOfArguments(int argc){
	if(argc < 5){
		write(1,"Eroor, too few arguments!\n",26);
		exit (EXIT_FAILURE); 
	}/*
	if(argc > 8){
		write(1,"Eroor, too many arguments!\n",27);
		exit (EXIT_FAILURE); 
	}*/
}

/* Returns '1' if error has been found, otherwise '0' */ 
int checkIfPathIsCorrect(char* argv){
	if(argv == NULL){ // do poprawy sprawdzic czy można w ejdnym ifie
		write(1,"You need to use -a and -b to set paths 🤔️\n",47);
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

