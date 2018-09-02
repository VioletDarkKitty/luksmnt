#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <string.h>

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Missing argument filename or too many arguments\n");
		return 1;
	}
	
	unsigned int len = strlen(argv[1]) * sizeof(char) + 1;
	char *filename = (char*)malloc(len);
	memcpy(filename, argv[1], len);
	
	char *mount = (char*)malloc(len);
	memcpy(mount, argv[1], len);
	mount = basename(mount);
	
	for(unsigned int i = strlen(mount); i > 0; i--) {
		if (mount[i] == '.') {
			mount[i] = '\0';
			break;
		}
	}
	
	char* password = NULL;
	
	// zenity provides nice commandline gtk windows without importing loads of extra stuff
	FILE *fp = popen("zenity --entry --text='Encryption Password' --hide-text", "r");
	if (fp == NULL) {
		printf("Failed to execute");
		return 1;
	}
	
	#define BUFLEN 128
	char buf[BUFLEN];
	int readlen = 0;
	while(fgets(buf, BUFLEN, fp) != NULL) {
		readlen += BUFLEN;
		password = realloc(password, sizeof(char) * readlen);
		
		char *pos;
		if ((pos = strchr(buf, '\n')) != NULL) {
			*pos = '\0';
		}
		
		memcpy(password + (readlen - BUFLEN), buf, BUFLEN);
	}
	
	/// TODO: inspect status of pclose for success
	pclose(fp);
	
	char* cmdprebase = "echo -n '%s' | ";
	char* cmdpre = (char*)malloc(strlen(cmdprebase) * sizeof(char)
									+ strlen(password) * sizeof(char)
									+ 1);
	sprintf(cmdpre, cmdprebase, password);
	char* cmdbase = "sudo cryptsetup open -d - ";
	char* command = (char*)malloc(strlen(cmdbase) * sizeof(char) 
								+ strlen(cmdpre) * sizeof(char)
								+ len);
	sprintf(command, "%s %s '%s' '%s'", cmdpre, cmdbase, filename, mount);
	
	/// TODO: replace with cryptsetup function call
	/// TODO: use popen to check for failure status
	system(command);
	
	return 0;
}
