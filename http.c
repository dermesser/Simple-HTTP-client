# include <stdio.h>
# include <stdlib.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <netdb.h>
# include <string.h>
# include <unistd.h>

/*
 * (c) 2011 dermesser
 * This piece of software is licensed with GNU GPL 3.
 *
 */

void errExit(const char* str)
{
	fprintf(stderr,str);
	exit(1);
}

int main (int argc, char** argv)
{
	struct addrinfo *result, hints;
	int srvfd, rwerr = 42;
	char request[400], c, port[6];

	port[5] = 0;	

	if ( argc <= 2 )
		errExit("Usage: simple-http SERVER [PORT] FILE\n");
	
	if ( argc == 4 )
	{
		strcpy(port,argv[2]); // Copy the port number from the second argument 
		strcpy(argv[2],argv[3]); // copy the filename into the second argument that it seems as if there wasn't a third arg.
	} else 
	{
		strncpy(port,"80",2);
	}
	

	srvfd = socket(AF_INET,SOCK_STREAM,0);

	if ( srvfd < 0 )
		errExit("socket()\n");


	memset(&hints,0,sizeof(struct addrinfo));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ( 0 != getaddrinfo(argv[1],port,&hints,&result))
		errExit("getaddrinfo\n");

	if ( connect(srvfd,result->ai_addr,sizeof(struct sockaddr)) == -1)
		errExit("connect\n");
	
	
	// Now we have an established connection.
	
	sprintf(request,"GET %s HTTP/1.1\nHost: %s\nUser-agent: lbo's http client\n\n",argv[2],argv[1]);

	write(srvfd,request,strlen(request));
	
	while ( rwerr > 0 )
	{
		rwerr = read(srvfd,&c,1);
		putchar(c);
	}
	
	close(srvfd);

	return 0;

}
