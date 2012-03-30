# include <stdio.h>
# include <stdlib.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <netdb.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>

/*
 * (c) 2011 dermesser
 * This piece of software is licensed with GNU GPL 3.
 *
 */

void errExit(const char* str)
{
	perror(str);
	exit(1);
}

int main (int argc, char** argv)
{
	struct addrinfo *result, hints;
	int srvfd, rwerr = 42, outfile;
	char request[400], buf[16], port[6],c;

	memset(port,0,6);

	if ( argc < 3 )
		errExit("Usage: simple-http [-h] [-p PORT] [-o OUTPUT_FILE] SERVER FILE\n");
	
	strncpy(port,"80",2);

	while ( (c = getopt(argc,argv,"p:ho:")) >= 0 )
	{
		switch (c)
		{
			case 'h' : 
				printf("Usage: simple-http [-h] [-p PORT] [-o OUTPUT_FILE] SERVER FILE\n");
				exit(0);
			case 'p' :
				strncpy(port,optarg,5);
				break;
			case 'o' :
				outfile = open(optarg,O_WRONLY|O_CREAT,0644);
				close(1);
				dup2(outfile,1);
				break;
		}
	}

	memset(&hints,0,sizeof(struct addrinfo));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ( 0 != getaddrinfo(argv[optind],port,&hints,&result))
		errExit("getaddrinfo\n");

	// Create socket after retrieving the inet protocol to use (getaddrinfo)
	srvfd = socket(result->ai_family,SOCK_STREAM,0);

	if ( srvfd < 0 )
		errExit("socket()\n");

	if ( connect(srvfd,result->ai_addr,result->ai_addrlen) == -1)
		errExit("connect\n");
	
	
	// Now we have an established connection.
	
	sprintf(request,"GET %s HTTP/1.1\nHost: %s\nUser-agent: simple-http client\n\n",argv[optind+1],argv[optind]);

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
