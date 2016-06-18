

#include "pitime.h"
static void error(char*);
static int open_socket(char* host, char* port);
static int open_socket2(char* host, char* port);
static int say(int socket, char* s);



void error(char* msg)
 {
   fprintf(stderr, "%s: %s\n", msg, strerror(errno));
   exit(1);
 }

 int open_socket(char* host, char* port)
 {
   struct addrinfo *res;
   struct addrinfo hints;
   memset(&hints, 0, sizeof(hints));
   hints.ai_family = PF_UNSPEC;
   //hints.ai_family = PF_INET;
   //hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;

   if (getaddrinfo(host, port, &hints, &res) == -1)
     error("Can't resolve the address");

   int d_sock = socket(res->ai_family, res->ai_socktype,res->ai_protocol);

   if (d_sock == -1)
     error("Can't open socket");

   int c = connect(d_sock, res->ai_addr, res->ai_addrlen);

   freeaddrinfo(res);

   if (c == -1)
     error("Can't connect to socket");

   return d_sock;
 }

 int open_socket2(char* host, char* port)
 {
    int s = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in si;
   memset(&si, 0, sizeof(si));
   si.sin_family = PF_INET;
   si.sin_addr.s_addr = inet_addr("54.231.33.193");
   si.sin_port = htons(80);
   int sc=connect(s, (struct sockaddr*)&si, sizeof(si));
   if (sc == -1)
     error("Can't open socket");

   return s;
 }

 int say(int socket, char* s)
 {
   int result = send(socket, s, strlen(s), 0);
   if (result == -1)
     fprintf(stderr, "%s: %s\n", "Error talking to the server", strerror(errno));
   return result;
 }


char* pitime()
{
    printf("from pitime_1!\n");

    int d_sock;
    //d_sock = open_socket("time-a.timefreq.bldrdoc.gov", "13");
    //d_sock = open_socket("time-b.timefreq.bldrdoc.gov", "13");
    //d_sock = open_socket("time-a.nist.gov", "13");
    //d_sock = open_socket("time-b.nist.gov", "13");
    d_sock = open_socket("time-c.nist.gov", "13");

    char buf[255];

    //printf(buf);
    //say(d_sock, buf);

    say(d_sock, "Host: ntp2.usno.navy.mil\r\n\r\n");

    //char rec[256];  //char* rec = should have (char*)malloc(256*sizeof(char))
    char* rec = (char*)malloc(256*sizeof(char));
    int bytesRcvd = recv(d_sock, rec, 255, 0);
    //printf("\n=====BYTES is %i \n", bytesRcvd);
    while (bytesRcvd)
    {
        if (bytesRcvd == -1)
            error("Can't read from server");
        //printf("\n=====REC[] = 0 is next \n");
        rec[bytesRcvd] = '\0';
        printf("%s", rec);
        //printf("\n=====RECIEVING next is %i \n", bytesRcvd);
        bytesRcvd = recv(d_sock, rec, 255, 0);
        //bytesRcvd = recv(d_sock, rec, 255, MSG_DONTWAIT);
        //printf("\n=====BYTES is %i \n", bytesRcvd);
   }
    close(d_sock);
    return rec;
}

int buf_pitime(char* myrec)
{
    printf("from buf pitime!\n");

    int d_sock;
    //d_sock = open_socket("time-a.timefreq.bldrdoc.gov", "13");
    //d_sock = open_socket("time-b.timefreq.bldrdoc.gov", "13");
    //d_sock = open_socket("time-a.nist.gov", "13");
    //d_sock = open_socket("time-b.nist.gov", "13");
    d_sock = open_socket("time-c.nist.gov", "13");

    char buf[255];

    //printf(buf);
    //say(d_sock, buf);

    say(d_sock, "Host: ntp2.usno.navy.mil\r\n\r\n");

    char rec[256];

    int bytesRcvd = recv(d_sock, myrec, 255, 0);
    //printf("\n=====BYTES is %i \n", bytesRcvd);
    while (bytesRcvd)
    {
        if (bytesRcvd == -1)
            error("Can't read from server");
        //printf("\n=====REC[] = 0 is next \n");
        rec[bytesRcvd] = '\0';
        printf("%s", myrec);
        //printf("\n=====RECIEVING next is %i \n", bytesRcvd);
        bytesRcvd = recv(d_sock, myrec, 255, 0);
        //bytesRcvd = recv(d_sock, rec, 255, MSG_DONTWAIT);
        //printf("\n=====BYTES is %i \n", bytesRcvd);
   }
    close(d_sock);
    int sizeit = sizeof(myrec);
    return sizeit;
}



int no_net_pitime(char* myrec)
{
    printf("from  no net pitime!\n");
    char tdata[] = "57523 01-01-15 00:58:16 50 0 0 257.3 UTC(NIST) *";
    strcpy(myrec,tdata);

    return 51;
}
