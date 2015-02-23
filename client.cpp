#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <iostream>
#include <pthread.h>
#include <sstream>
#include <string>
#include <time.h>
#include <pthread.h>

using namespace std;

#define N 2
#define INITIAL_SEED 1

typedef struct  {
	int x, y sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;	
} operands;

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

//Worker Thread for Pthreading
void *worker_thread(void *arg) {
	char buffer[256];
	operands *my_opers = (operands *)arg;
	int sum = my_opers->x + my_opers->y;
	printf("%i ", my_opers->x);
	printf("%i\n", my_opers->y);
	//        printf("This is worker_thread #%ld\n", (long)sum);

//--Send and receive communications--//
	//Write random values to string
	stringstream ss;
	ss << r1 << " " << r2;
	string message = ss.str();

	if (connect(my_opers->sockfd, (struct sockaddr *) &my_opers->serv_addr, sizeof(my_opers->serv_addr)) < 0)
		error("ERROR connecting");
	//sned values to server
	my_opers->n = write(my_opers->sockfd, message.c_str(), message.length());
	if (my_opers->n < 0)
		error("ERROR writing to socket");

	//Get responce from server
	bzero(buffer, 256);
	my_opers->n = read(my_opers->sockfd, buffer, 255);
	if (my_opers->n < 0)
		error("ERROR reading from socket");
	printf("%s\n", buffer);

	//close socket
	close(my_opers->sockfd);

	//Close Thread
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	operands my_operands[N];
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[256];
	if (argc < 3) {
		fprintf(stderr, "usage %s hostname port\n", argv[0]);
		exit(0);
	}

	portno = atoi(argv[2]);
	server = gethostbyname(argv[1]);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
	
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
		(char *)&serv_addr.sin_addr.s_addr,
		server->h_length);

	serv_addr.sin_port = htons(portno);
	

	//Create random variables
	//////////////////////////////////////////////////////////
	int do_fixed = 0;

	/* check command line arguments for fixed seed flag */
	for (int i = 0; i<argc; i++) {
		if (strstr(argv[i], "fixed")) {
			do_fixed = 1;
			break;
		}
	}
	pthread_t my_thread[N];
	

	if (do_fixed) {
		srand(INITIAL_SEED);
	}
	else {
		srand(time(NULL));
	}


	long id;
	for (id = 1; id <= N; id++) {
		my_operands[id].sockfd = sockfd;
		my_operands[id].portno = portno;
		my_operands[id].serv_addr = serv_addr;
		my_operands[id].server = server;
		my_operands[id].x = rand() % 100;
		my_operands[id].y = rand() % 100;
		int ret = pthread_create(&my_thread[id], NULL, &worker_thread, (void*)&(my_operands[id]));
		if (ret != 0) {
			printf("Error: pthread_create() failed\n");
			exit(EXIT_FAILURE);
		}
	}

	pthread_exit(NULL);

	/////////////////////////////////////////////////////////


	return 0;
}
