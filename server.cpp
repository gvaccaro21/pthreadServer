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

pthread_mutex_t updateMutex, productMutex;


using namespace std;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

typedef struct {
	bool useMutex;
	int sockfd;
} sockMutex;

void *multiply(void* threadarg) {
	stringstream ss;
	char buffer[256];
	int n, socketNum; 
	socklen_t clilen;
	double numArray[2];
     	double product;
	struct sockaddr_in cli_addr;
	sockMutex sockInfo = *((sockMutex*)threadarg);
	listen(sockInfo.sockfd,5);
    	clilen = sizeof(cli_addr);
//     	while (socketNum < 0) {
		socketNum = accept(sockInfo.sockfd, 
        	        	 (struct sockaddr *) &cli_addr, 
                		 &clilen);
     		if (socketNum < 0) 
        		  error("ERROR on accept");
//		printf("Socket: %i\n", socketNum);
//	}
	int connectionNumber = connectionCount;
	if (sockInfo.useMutex) {
		pthread_mutex_lock(&updateMutex);
//		connectionCount++;
		connectionCount = connectionNumber + 1;
		pthread_mutex_unlock(&updateMutex);
	}
	else
//		connectionCount++;
		connectionCount = connectionNumber;
	bzero(buffer,256);
     	n = read(socketNum,buffer,255);
     	if (n < 0) error("ERROR reading from socket");
	sscanf(buffer, "%lf%lf", &numArray[0], &numArray[1]);
     	printf("Here is the message: %s\n",buffer);
     	product = numArray[0] * numArray[1];
	double prodSum = productSum;
	if (sockInfo.useMutex) {
		pthread_mutex_lock(&productMutex);
//		productSum += product;
		productSum = prodSum + product;
		pthread_mutex_unlock(&productMutex);
	}
	else
//		productSum += product;
		productSum = prodSum + product;
     	ss << "The product is " <<  product;
     	string prodstring = ss.str();
	ss.str("");
     	n = write(socketNum,prodstring.c_str(),prodstring.length());
     	if (n < 0) error("ERROR writing to socket");
     	close(socketNum);
	printf("Sum of Products = %lf\n", productSum); 
	pthread_exit((void*) 0);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
//     socklen_t clilen;
     long t;
     void *status;
     bool useMutex = false;

     pthread_t socketThread[THREAD_COUNT];
     sockMutex socketInfo;
     pthread_attr_t attr;

     struct sockaddr_in serv_addr;
     int i, rc;
     if (argc < 3) {
         fprintf(stderr,"Usage:\n%s -p port_number -m\n\t-p, --port: Port number (anything above 1024)\n\t-m, --mutex: Enable mutex\n", argv[0]);
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
      /* check command line arguments for fixed seed flag */
        for (int i=0; i<argc; i++) {
                if (strstr(argv[i], "-p") || strstr(argv[i], "--port")) {
                        portno = atoi(argv[i+1]);
                        continue;
                }
		else if (strstr(argv[i], "-m") || strstr(argv[i], "--mutex")) {
			useMutex = true;
			continue;
        	}
	}
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
	printf("Running server on port %d\n", portno);
	if (useMutex) {
		printf("Running mutexes\n");
	}
     pthread_attr_init(&attr);
     pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
//     while (1) 
	socketInfo.useMutex = useMutex;
	socketInfo.sockfd = sockfd;
     for (i = 0; i < THREAD_COUNT; i++) {
/*	listen(sockfd,5);
    	clilen = sizeof(cli_addr);
     	newsockfd = accept(sockfd, 
        	         (struct sockaddr *) &cli_addr, 
                	 &clilen);
     	if (newsockfd < 0) 
        	  error("ERROR on accept");
//	int *socketNum = malloc(sizeof(*socketNum));
	socketInfo.socketNum = newsockfd; */
	int ret = pthread_create(&socketThread[i], &attr, multiply, (void *) &socketInfo);
	if (ret != 0) {
		printf("Error: pthread_create() failed\n");
		exit(EXIT_FAILURE);
	}     
     }
 
     pthread_attr_destroy(&attr);
     while (connectionCount < 10) {

	}
     sleep(1);
     printf("Total connections: %d\nTotal Sum: %f\n", connectionCount, productSum);
     close(sockfd);
     return 0; 
}
