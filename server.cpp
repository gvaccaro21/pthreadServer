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
int THREAD_COUNT = 10;
int connectionCount = 0;
double productSum = 0;

using namespace std;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void *multiply(void* threadarg) {
	stringstream ss;
	char buffer[256];
	int n;
	double numArray[2];
     	double product;
	int socketNum = *((int*)threadarg);

	connectionCount++;
	bzero(buffer,256);
     	n = read(socketNum,buffer,255);
     	if (n < 0) error("ERROR reading from socket");
	sscanf(buffer, "%lf%lf", &numArray[0], &numArray[1]);
     	printf("Here is the message: %s\n",buffer);
     	product = numArray[0] * numArray[1];
	productSum += product;
     	ss << "The product is " <<  product;
     	string prodstring = ss.str();
	ss.str("");
     	n = write(socketNum,prodstring.c_str(),prodstring.length());
     	if (n < 0) error("ERROR writing to socket");
     	close(socketNum);
	pthread_exit((void*) 0);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     long t;
     void *status;
     
     pthread_t socketThread[THREAD_COUNT];
     pthread_attr_t attr;
     
    
     struct sockaddr_in serv_addr, cli_addr;
     int i, rc;
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
     pthread_attr_init(&attr);
     pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
//     while (1) 
     for (i = 0; i < THREAD_COUNT; i++) {
	listen(sockfd,5);
    	clilen = sizeof(cli_addr);
     	newsockfd = accept(sockfd, 
        	         (struct sockaddr *) &cli_addr, 
                	 &clilen);
     	if (newsockfd < 0) 
        	  error("ERROR on accept");
//	int *socketNum = malloc(sizeof(*socketNum));
	pthread_create(&socketThread[i], &attr, multiply, (void *) &newsockfd);
     }
 
     pthread_attr_destroy(&attr);
     
     printf("Total connections: %d\nTotal Sum: %f\n", connectionCount, productSum);
     close(sockfd);
     return 0; 
}
