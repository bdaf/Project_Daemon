#include <syslog.h>
#include <ctype.h>
#include <fcntl.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

void checkNumberOfArguments(int argc);
int checkIfPathIsCorrect(char* argv);
void preparingDaemon();
void deleting(char* sourcePath, char* targetPath, int ifRecursion);
void reviewing(char* sourcePath, char* targetPath, int ifRecursion, int dependenceOfFileSize);

void copyFile(char* sourcePath, char* targetPath, int dependenceOfFileSize);
void copyRead(char* sourcePath, char* targetPath);

void makePath(char* path, char* fileName, char* result);
void writeToLog(char* msg);
time_t getTime(char* path);
void setTime(char* path, time_t timeToSet); 


int main(int argc, char **argv) {
	int i;
	/* Error Handling */
	checkNumberOfArguments(argc);
	/* Initialing variables */
	char *sourcePath = NULL;			/* Flag a */
	char *targetPath = NULL;			/* Flag b */
	int timeDeamon = 360;				/* Flag t */
	int ifRecursion = 0;				/* Flag R */
	int dependenceOfFileSize = 1024;	/* Flag s */
	/* Reseting opterr */
	opterr = 0;
	/* Validation, checking if arguments are properly typed */
	int choice;
	while ((choice = getopt (argc, argv, "a:b:t:s:R")) != -1)
	switch (choice)
	{
	case 'a':
		sourcePath = optarg;
		break;
	case 'b':
		targetPath = optarg;
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
		ifRecursion = 1;
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
	/* End of validation */
	
	/* Daemon Itself */
	preparingDaemon();
	while (1) {
		int _pid = fork();
		/* Error handling */
		if (_pid < 0) 
			exit(EXIT_FAILURE);
		/* Parent process */
		if (_pid > 0) {
			deleting(sourcePath, targetPath, ifRecursion);
			reviewing(sourcePath, targetPath, ifRecursion, dependenceOfFileSize);
			



		}
		/* Child process */
		sleep(timeDeamon); /* wait that many seconds as is set after '-t' argument */
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
void makePath(char* path, char* fileName, char* result){
	strcpy(result,path);
	strcat(result,"/");
	strcat(result,fileName);
}


void deleting(char* sourcePath, char* targetPath, int ifRecursion){
	struct dirent* file = NULL;
	DIR* targetFolder = opendir(targetPath);
	DIR* sourceFolder = opendir(sourcePath);
	/* Deleting in while */
	char path[511];
	while(file = readdir(targetFolder)){
		if(file->d_type == DT_REG){ /* If this is a regular file or empty dir. */
			makePath(sourcePath, file->d_name, path);
			if(open(path, O_RDONLY)<0){
				makePath(targetPath, file->d_name, path);		
				remove(path);
			}
		} /* If this is a directory but not ('.' or '..') */
		else if(file->d_type == DT_DIR && !(strcmp( file->d_name, "." )==0 || strcmp( file->d_name, ".." )==0) && ifRecursion == 1){
			char sPath[255];
			makePath(sourcePath, file->d_name, sPath);	
			if(open(sPath, O_RDONLY)<0){
				makePath(targetPath, file->d_name, path);		
				deleting(sPath, path, ifRecursion);
				remove(path);
			}
		}
	}
}

void reviewing(char* sourcePath, char* targetPath, int ifRecursion, int dependenceOfFileSize){
	struct dirent* file = NULL;
	DIR* targetFolder = opendir(targetPath); /* Opening directories */
	DIR* sourceFolder = opendir(sourcePath);
	char sPath[511];
	char tPath[511];
	while(file = readdir(sourceFolder)){ /* Reading directory - going through the files one by one */
		makePath(sourcePath, file->d_name, sPath);
        makePath(targetPath, file->d_name, tPath);
		int descOfFile = open(tPath, O_RDONLY);	
		if(descOfFile<0){ /* We check if file from source is in destination target path, if not, we create it. */
			if(file->d_type == DT_REG){ /* If this is a regular file or empty dir. */
				copyFile(sPath, tPath, dependenceOfFileSize); /* Copying / Creating file */	
			} /* If this is a directory but not ('.' or '..') */
			else if(file->d_type == DT_DIR && !(strcmp( file->d_name, "." )==0 || strcmp( file->d_name, ".." )==0) && ifRecursion == 1){
                mkdir(tPath, 0755);
			    reviewing(sPath, tPath, ifRecursion, dependenceOfFileSize);	
			}
		}/* We check if file from directory source is in destination target path, if so, we check modification date either */
		else if(descOfFile>=0 && getTime(sPath)!=getTime(tPath)){ 
            writeToLog("SiemaSiemaSiemaSiemaSiemaSiemaSiemaSiemaSiemaSiemaSiemaSiemaSiemaSiemaSiemaSiemaSiemaSiemaSiemaSiemaSiemaSiemaSiemaSiema");
			if(file->d_type == DT_REG){
				remove(tPath);
				copyFile(sPath, tPath, dependenceOfFileSize);
				setTime(tPath, getTime(sPath));
			}
			  
		}
	}
}

void copyFile(char* sourcePath, char* targetPath, int dependenceOfFileSize){
	copyRead(sourcePath, targetPath);
}

void copyRead(char* sourcePath, char* targetPath){
	int bufor[16];
	int sourceFile = open(sourcePath,O_RDONLY);
	int targetFile = open(targetPath, O_CREAT | O_WRONLY | O_TRUNC , 0644);
	/* Error handling */
	if(sourceFile == -1 || targetFile == -1){
		char msg[511];
		strcpy(msg,"Error occured during opening file: ");
		strcat(msg,sourcePath);
		strcat(msg," or ");
		strcat(msg,targetPath);
		writeToLog(msg);
		exit(EXIT_FAILURE);
	}

	int readSource;
	int writeTarget;
	while ((readSource = read(sourceFile, bufor, sizeof(bufor))) > 0){
		writeTarget = write(targetFile, bufor, (ssize_t)readSource);
		if (writeTarget != readSource){
			char msg[511];
			strcpy(msg,"Error writing one of target files: ");
	        strcat(msg,targetPath);
			writeToLog(msg);
			exit(EXIT_FAILURE);
		}
	}
	close(writeTarget);
	close(readSource);
}

void writeToLog(char* msg){
	openlog("File synchronization Daemon", LOG_PID, LOG_USER);
    syslog(LOG_INFO, msg);
	closelog();
}

time_t getTime(char* path) { /* Gets modyfication time from file / directory */
    struct stat file;
    if(stat(path, &file) == -1){
		char msg[511];
		strcpy(msg,"Error during downloading modyfication time from file: ");
		strcat(msg,path);
		writeToLog(msg);
        exit(EXIT_FAILURE);
		}
    return file.st_mtime;
}

void setTime(char* path, time_t timeToSet) { /* Sets modyfication time in file / directory */
    struct stat file;
    if(stat(path, &file) == -1){
		char msg[511];
		strcpy(msg,"Error during downloading modyfication time from file: ");
		strcat(msg,path);
		writeToLog(msg);
        exit(EXIT_FAILURE);
    }
    file.st_mtime = timeToSet;
}


