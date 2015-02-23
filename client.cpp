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

using namespace std;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);

	serv_addr.sin_port = htons(portno);
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR connecting");
	
	//string contConnection = "yes";
	//while (contConnection == "yes")
	//{
		//contConnection = "";
		//printf("Please enter the message: ");
		//buffer = 0;
		srand(time(NULL));
		int r1 = rand() % 100;
		int r2 = rand() % 100;

		stringstream ss;
		ss << "1 " << r1 << " " << r2;
		string message = ss.str();
	//	buffer = message.c_str();

		//cout << "Please enter the message: ";
		//cin >> buffer;

//		buffer = "1" 

//		bzero(buffer, 256);
//		fgets(buffer, 255, stdin);
//		n = write(sockfd, buffer, strlen(buffer));
		n = write(sockfd, message.c_str(), message.length());
		if (n < 0)
			error("ERROR writing to socket");
		bzero(buffer, 256);
		n = read(sockfd, buffer, 255);
		if (n < 0)
			error("ERROR reading from socket");
		printf("%s\n", buffer);
		
		//cout << "Continue?\n";
		//cin >> contConnection;
	//}
    close(sockfd);
    return 0;
}
