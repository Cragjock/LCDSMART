#include <stdio.h>
#include <stdlib.h>
//#include <math.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>



char* pitime();
int buf_pitime(char* myrec);
int no_net_pitime(char* myrec);
