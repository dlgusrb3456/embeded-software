#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

#include "ch3.h"

int main(int argc, char *argv[]){
	int dev;
	int i;
	dev = open("/dev/ch3_mod_201912698_dev",O_RDWR);
	for(i=1;i<argc;i++){
		
		struct msg_st user_str = {
			0,"\0"
		};
		strcpy(user_str.str,argv[i]);

		user_str.len = strlen(user_str.str);
		ioctl(dev,CH3_IOCTL_WRITE,&user_str);
		printf("Writer: %s\n",user_str.str);
		
	}
	close(dev);
}
