#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXPENDING 3
#define RCVBUFSIZE 32

void handleAgentCommand(int agentSock){
	char msgBuffer[RCVBUFSIZE];
	int sizeOfMsg;
	FILE *fp;
	char str[] = " received by 147.26.231.156 and sent back\n";

	if((sizeOfMsg = recv(agentSock, msgBuffer, RCVBUFSIZE, 0)) < 0)
	{
		puts("receive message failed");
	}

	fp = fopen("log.txt", "w");
	for(int i = 0; i < sizeOfMsg; i++)
	{
		fputc(msgBuffer[i], fp);
	}
	for(int i = 0; i < sizeof(str); i++)
	{
		fputc(str[i], fp);
	}

	fclose(fp);
	
	while(sizeOfMsg > 0)
	{
		if(send(agentSock, msgBuffer, RCVBUFSIZE, 0)!= sizeOfMsg)
		{
			puts(msgBuffer);
			puts("received, sent to agent and added to log");
		}

		if((sizeOfMsg = recv(agentSock, msgBuffer, RCVBUFSIZE, 0)) <0)
		{
			puts("recv() failed");
		}
	}
	close(agentSock);
}


int main(int argc, char *argv[])
{
	int sockFd;
	int agentSocket;
	struct sockaddr_in servAddr;
	struct sockaddr_in agentAddr;
	unsigned short servPort;
	unsigned int agentLen;

	if(argc !=2)
	{
		fprintf(stderr, "usage: %s <Server Port>\n", argv[0]);
	}

	servPort = atoi(argv[1]);

	if((sockFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		puts("error with socket");
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(servPort);

	if(bind(sockFd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
	{
		puts("bind failed");
	}

	if(listen(sockFd, MAXPENDING) < 0)
	{
		puts("listen failed");
	}

	for(;;)
	{
		agentLen = sizeof(agentAddr);

		if((agentSocket = accept(sockFd, (struct sockaddr*)&agentAddr, &agentLen))<0)
		{
			puts("accept failed.");
		}
			

		printf("Connected to agent %s\n", inet_ntoa(agentAddr.sin_addr));

		handleAgentCommand(agentSocket);
	}
}
