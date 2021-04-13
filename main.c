#define _GNU_SOURCE
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
#include <utime.h>
#include <signal.h>


void checkNumberOfArguments(int argc);
int checkIfPathIsCorrect(char* argv);
void preparingDaemon();
void deleting(char* sourcePath, char* targetPath, int ifRecursion);
void reviewing(char* sourcePath, char* targetPath, int ifRecursion, int dependenceOfFileSize);
void deletingAndReviewing(char* sourcePath, char* targetPath, int ifRecursion, int dependenceOfFileSize);

void copyFile(char* sourcePath, char* targetPath, int dependenceOfFileSize);
void copyRead(char* sourcePath, char* targetPath);
void copyHeavyFile(char* sourcePath, char* targetPath);

void makePath(char* path, char* fileName, char* result);
void writeToLog(char* msg);
time_t getTime(char* path);
off_t getSize(char* path);
void setTime(char* targetPath, time_t timeOfMod);
void handler(int signum);

char* sourcePath = NULL;
char* targetPath = NULL;
int timeDeamon, ifRecursion, dependenceOfFileSize;

int main(int argc, char **argv) {
	int i;
	/* Error Handling */
	checkNumberOfArguments(argc);
	/* Initialing variables */
	sourcePath = NULL;					/* Flag a */
	targetPath = NULL;					/* Flag b */
	timeDeamon = 360;					/* Flag t */
	ifRecursion = 0;					/* Flag R */
	dependenceOfFileSize = 1024;		/* Flag s */
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
		if (optopt == 'a' || optopt == 'b'|| optopt == 't'|| optopt == 's'){
			write(1,"Option -",8);
			write(1,(const char)optopt,1);
			write(1," requires an argument.\n",23);			
		}
		else if (isprint (optopt)){
			write(1,"Unknown option `-",17);
			write(1,(const char)optopt,1);
			write(1,"'.\n", 3);	
		}
		else{
			write(1,"Unknown option character `\\x",29);
			write(1,"%x", 3);
			write(1,"'.\n",3);
		}
		return EXIT_FAILURE;
		default:
			abort();
	}

	if(checkIfPathIsCorrect(targetPath) != 0 || checkIfPathIsCorrect(sourcePath) != 0 ) 
		exit (EXIT_FAILURE); 
	/* End of validation */

	/* Daemon */
	preparingDaemon();
	while (1) {
		/* Making a break signal named "SIGUSR1" */
		if(signal(SIGUSR1, handler)==SIG_ERR)
    		exit(EXIT_FAILURE);
		deleting(sourcePath, targetPath, ifRecursion);
		reviewing(sourcePath, targetPath, ifRecursion, dependenceOfFileSize);
		sleep(timeDeamon); /* Wait that many seconds as is set after '-t' argument */
    }
}

void deletingAndReviewing(char* sourcePath, char* targetPath, int ifRecursion, int dependenceOfFileSize){

}

void handler(int signum){
	writeToLog("Ctrl+'/' used");
	deleting(sourcePath, targetPath, ifRecursion);
	reviewing(sourcePath, targetPath, ifRecursion, dependenceOfFileSize);
}

/* Checking whether number of arguments is properly */ 
void checkNumberOfArguments(int argc){
	if(argc < 5){
		write(1,"Eroor, too few arguments!\n",26);
		exit (EXIT_FAILURE); 
	}
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
        if (pid < 0){
			openlog("File synchronization Daemon", LOG_PID, LOG_USER);
    		syslog(LOG_ERR,"Error during making a fork");
			closelog();
            exit(EXIT_FAILURE); 
		}

        /* If we got a good PID, then
           we can exit the parent process. */
        if (pid > 0) 
                exit(EXIT_SUCCESS);  

        /* Change the file mode mask */
        umask(0);

        /* Create a new SID for the child process */
        sid = setsid();

        /* Error handling */
        if (sid < 0) {
			openlog("File synchronization Daemon", LOG_PID, LOG_USER);
    		syslog(LOG_ERR,"Error during making a setsid()");
			closelog();
            exit(EXIT_FAILURE);
		}

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
				setTime(tPath,getTime(sPath)); /* Set time for file in 'tPath' from file in sPath */
			} /* If this is a directory but not ('.' or '..') */
			else if(file->d_type == DT_DIR && !(strcmp( file->d_name, "." )==0 || strcmp( file->d_name, ".." )==0) && ifRecursion == 1){
                mkdir(tPath, 0755);
			    reviewing(sPath, tPath, ifRecursion, dependenceOfFileSize);	
			}
		}/* We check if file from directory source is in destination target path, if so, we check modification date either */
		else if(descOfFile>=0 && getTime(sPath)!=getTime(tPath)){ 
			if(file->d_type == DT_REG){
				remove(tPath);
				copyFile(sPath, tPath, dependenceOfFileSize);
				setTime(tPath,getTime(sPath));
			}
			else if(file->d_type == DT_DIR && !(strcmp( file->d_name, "." )==0 || strcmp( file->d_name, ".." )==0) && ifRecursion == 1){
				reviewing(sPath, tPath, ifRecursion, dependenceOfFileSize);	
			}
			  
		}
	}
}
/* Checking if file is in border of dependenceOfFileSize. Way of updating files depends on it. */
void copyFile(char* sourcePath, char* targetPath, int dependenceOfFileSize){
	if(getSize(sourcePath) > dependenceOfFileSize)
		copyHeavyFile(sourcePath,targetPath);
	else
		copyRead(sourcePath, targetPath);
}
// TEŻ DOBRZE BY BYŁO NAPISAĆ CO TO ROBI JAK KOPIUJE
void copyRead(char* sourcePath, char* targetPath){
	int bufor[16];
	int sourceFile = open(sourcePath,O_RDONLY);
	int targetFile = open(targetPath, O_CREAT | O_WRONLY | O_TRUNC , 0644);
	/* Error handling */
	if(sourceFile == -1 || targetFile == -1){
		openlog("File synchronization Daemon", LOG_PID, LOG_USER);
    	syslog(LOG_ERR,"Error occured during opening file: %s or %s",sourcePath, targetPath);
		closelog();
		exit(EXIT_FAILURE);
	}

	int readSource;
	int writeTarget; // O CO TU CHODZI TRZEBA OPISAĆ
	while ((readSource = read(sourceFile, bufor, sizeof(bufor))) > 0){
		writeTarget = write(targetFile, bufor, (ssize_t)readSource);
		if (writeTarget != readSource){
			openlog("File synchronization Daemon", LOG_PID, LOG_USER);
    		syslog(LOG_ERR,"Error writing one of target files: %s", targetPath);
			closelog();
			exit(EXIT_FAILURE);
		}
	}
	close(writeTarget);
	close(readSource);
}

void writeToLog(char* msg){
	openlog("File synchronization Daemon", LOG_PID, LOG_USER);
    syslog(LOG_NOTICE,msg);
	closelog();
}

off_t getSize(char* path)
{
    struct stat size;
    if(stat(path, &size)==0)
        return size.st_size;
	openlog("File synchronization Daemon", LOG_PID, LOG_USER);
    syslog(LOG_ERR,"Error during geting size of file: %s", path);
	closelog();
    exit(EXIT_FAILURE);
}

time_t getTime(char* path) { /* Gets modyfication time from file / directory */
    struct stat file;
    if(stat(path, &file) == -1){
		openlog("File synchronization Daemon", LOG_PID, LOG_USER);
    	syslog(LOG_ERR,"Error during downloading modyfication time from file: %s",path);
		closelog();
        exit(EXIT_FAILURE);
	}
    return file.st_mtime;
}

void copyHeavyFile(char* sourcePath, char* targetPath){
	int fd_in, fd_out;
           struct stat stat;
           off64_t len, ret;

           fd_in = open(sourcePath, O_RDONLY);
           if (fd_in == -1) {
			  	openlog("File synchronization Daemon", LOG_PID, LOG_USER);
    			syslog(LOG_ERR,"Error occured during opening source file: %s",sourcePath);
				closelog();
              	exit(EXIT_FAILURE);
           }

           if (fstat(fd_in, &stat) == -1) {
               	openlog("File synchronization Daemon", LOG_PID, LOG_USER);
    			syslog(LOG_ERR,"Error occured during using fstat");
				closelog();
               	exit(EXIT_FAILURE);
           }

           len = stat.st_size;

           fd_out = open(targetPath, O_CREAT | O_WRONLY | O_TRUNC, 0644);
           if (fd_out == -1) {
               	openlog("File synchronization Daemon", LOG_PID, LOG_USER);
    			syslog(LOG_ERR,"Error occured during opening target file: %s",targetPath);
				closelog();
               	exit(EXIT_FAILURE);
           }

           do {
               ret = copy_file_range(fd_in, NULL, fd_out, NULL, len, 0);
               if (ret == -1) {
                    openlog("File synchronization Daemon", LOG_PID, LOG_USER);
    				syslog(LOG_ERR,"Error occured during using copy_file_range from: %s to: %s",sourcePath, targetPath);
					closelog();
                   	exit(EXIT_FAILURE);
               }

               len -= ret;
           } while (len > 0 && ret > 0);

           close(fd_in);
           close(fd_out);
		   //writeToLog(targetPath);
}

void setTime(char* targetPath, time_t timeOfMod){
	struct utimbuf time;
	time.actime = timeOfMod;
	time.modtime = timeOfMod;
	if(utime(targetPath, &time)<0){
		openlog("File synchronization Daemon", LOG_PID, LOG_USER);
    	syslog(LOG_ERR,"Error occured during setting time for file: %s",targetPath);
		closelog();
		exit(EXIT_FAILURE);
	}
}