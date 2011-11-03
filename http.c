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
	int srvfd, rwerr = 42, argindex;
	char request[400], buf[16], port[6];

	port[5] = 0;	

	if ( argc <= 2 )
		errExit("Usage: simple-http SERVER [PORT] FILE\n\n(c) 2011 dermesser. Licensed with GNU-GPL v3\n");
	
	if ( argc == 4 )
	{
		strcpy(port,argv[2]); // Copy the port number from the second argument 
		argindex = 3;

	}
	if (argc == 3)
	{
		argindex = 2;
		strncpy(port,"80",3);
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
	
	sprintf(request,"GET %s HTTP/1.1\nHost: %s\nUser-agent: simple-http client\n\n",argv[argindex],argv[1]);

	write(srvfd,request,strlen(request));
	
	shutdown(srvfd,SHUT_WR);

	while ( rwerr > 0 )
	{
		rwerr = read(srvfd,buf,16);
		write(1,buf,rwerr);
	}
	
	close(srvfd);

	return 0;

}
