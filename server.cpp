/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/types.h> 
#ifdef __WIN32__
	#include <winsock2.h>
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
#endif
#include <iostream>
#include <sstream>
#include <pthread.h>

using namespace std;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     stringstream ss;
     char buffer[256];
     double numArray[2];
     double product;
     struct sockaddr_in serv_addr, cli_addr;
     int i, n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     while (1) {
	listen(sockfd,5);
    	clilen = sizeof(cli_addr);
     	newsockfd = accept(sockfd, 
        	         (struct sockaddr *) &cli_addr, 
                	 &clilen);
     	if (newsockfd < 0) 
        	  error("ERROR on accept");
     	bzero(buffer,256);
     	n = read(newsockfd,buffer,255);
     	if (n < 0) error("ERROR reading from socket");
	sscanf(buffer, "%lf%lf", &numArray[0], &numArray[1]);
     	printf("Here is the message: %s\n",buffer);
     	product = numArray[0] * numArray[1];
     	ss << "The product is " <<  product;
     	string prodstring = ss.str();
	ss.str("");
     	n = write(newsockfd,prodstring.c_str(),prodstring.length());
     	if (n < 0) error("ERROR writing to socket");
     	close(newsockfd);
     }
     close(sockfd);
     return 0; 
}
