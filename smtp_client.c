#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>      // for struct hostent

#define MAX_REQUEST 1024
#define MAX_REPLY 1024
int getLine(int fd, char line[], int max);

main(int argc, char **argv)
{
    int                 sockfd;
    struct sockaddr_in  serv_addr;
    char                request[MAX_REQUEST+1];
    char                reply[MAX_REPLY+1];
    unsigned int	server_port = 25;
    struct hostent	*hostptr;
    struct in_addr      *ptr;
    unsigned short	port_number;
    char userinput[801]; //MAX_IN is initialized to 800
    char buf[MAX_REPLY];
    char tmp[MAX_REQUEST];
    // Read hostname and port from the command-line
    if (argc < 2)
    {
        printf("Usage: smtpClient <hostname> [port-number]\n");
        return(1);
    }
    if (argc > 2)
        port_number = atoi(argv[2]);
    else
        port_number = 25;

    if ( (hostptr = (struct hostent *) gethostbyname(argv[1])) == NULL) {
        perror("gethostbyname error for host");
	return(1);
    }
    ptr = (struct in_addr *) *(hostptr->h_addr_list);
    printf ("DEBUG: server address: %u %s\n",ptr->s_addr,inet_ntoa(*ptr));
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = ptr->s_addr;
    serv_addr.sin_port        = htons(port_number);

    // Create communication endpoint.
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("server: can't open stream socket");
        return(1);
    }

    // Connect to the server.
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("client: can't connect to server");
        return(1);
    }
    memset(buf,0,MAX_REQUEST);
    read(sockfd,buf,MAX_REQUEST);
    strcpy(buf,"HEY\n");
    getLine(sockfd, reply, MAX_REPLY);
    puts(reply);
    printf("SENDING HELO %s\n", argv[1]);
    sprintf(request,"HELO %s\r\n", argv[1]);
    write(sockfd, request, strlen(request));
    getLine(sockfd, reply, MAX_REPLY);
    puts(reply);
    close(sockfd);
    return(0);
}

//
// get a line of data from fd
//
int getLine(int fd, char* line, int lim)
{
    int i;
    char c;

    i =  0;
    while (--lim > 0 && read(fd,&c,1)>0 && c!='\n' && c!='\0')
    {
        line[i++] = c;
    }
    if (c=='\n')
        line[i++] = c;
    line[i] = '\0';
    return i;
}

