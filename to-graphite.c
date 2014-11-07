#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <time.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#define VERSION "0.0.1"
#define PORT 2003

static int debug = 0;
static char *server = "127.0.0.1";

void die(char *s)
{
        perror(s);
        exit(1);
}


int main( int argc, char **argv) {

    int c;
    time_t t;
    t = time(NULL);
    extern char *optarg;
    extern char *optind;
    extern char *optopt;
    char buf[BUFSIZ];

    while ((c = getopt (argc, argv, "hvs:")) != -1)
    switch (c)
    {  
       case 'v':
         debug = 1;
         break;
       case 's':
         server = optarg;
         break;
       case '?':
       case 'h':
         printf("to_graphite v%s\n",VERSION);
         printf("  Available options:\n");
         printf("    -h help\n");
         printf("    -v verbose\n");
         printf("    -s [server_ip] set graphite server ip\n");
         printf("       default ip is 127.0.0.1\n\n");

         exit(EXIT_FAILURE);
       default:
         if (isprint (optopt))
           fprintf (stderr, "Unknown option `-%c'.\n", optopt);
         else
           fprintf (stderr,
                    "Unknown option character `\\x%x'.\n",
                    optopt);
         exit(EXIT_FAILURE);
       }


    while(fgets(buf, sizeof buf, stdin))
    {   
        if (buf[strlen(buf)-1] == '\n') {
            // read full line
            strtok(buf, "\n");
            struct sockaddr_in si_other;
            int s, i, slen=sizeof(si_other);
            char graphite_message[BUFSIZ];

            if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
            {   
                die("socket");
            }

            memset((char *) &si_other, 0, sizeof(si_other));
            si_other.sin_family = AF_INET;
            si_other.sin_port = htons(PORT);

            if (inet_aton(server , &si_other.sin_addr) == 0)
            {   
                fprintf(stderr, "inet_aton() failed\n");
                exit(1);
            }

            sprintf(graphite_message,"%s %ju", buf, t);

            if (debug)
            {   
                printf("%s to %s\n",graphite_message,server);
            }

            if (sendto(s, graphite_message, strlen(graphite_message) , 0 , (struct sockaddr *) &si_other, slen)==-1)
            {   
                die("sendto()");
            }
        }
    }

    return 0;
}
