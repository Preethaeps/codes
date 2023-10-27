//client.c
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#define PORT 12346
#define SWS 3

typedef struct{
	int seq_no;
	char data[20];
	int ack;
}Frame;

int main()
{
	int sock,connfd;
	struct sockaddr_in sadd;
	sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
	{
		perror("error insocket");
		exit(-1);
	}
	printf("Socket created successfully\n");
	
	sadd.sin_family=AF_INET;
	sadd.sin_port=htons(PORT);
	sadd.sin_addr.s_addr=inet_addr("127.0.0.1");
	
	socklen_t len;
	len=sizeof(sadd);
	if(connect(sock,(struct sockaddr *)&sadd,len)!=0)
	{
		printf("Connection with the server failed");
		exit(0);
	}
	
	
	
	Frame frames[SWS];
	int LFS=-1,LAR=-1,i=LAR+1,flag=1,j;
	
	for(;;)
	{
		for(j=0;j<SWS;i++,j++)
		{
			if(i==SWS)
				flag=1;
			if(flag==1){
				frames[i%SWS].seq_no=++LFS;
				printf("Enter message for frame %d:",i%SWS);
				scanf("%s",frames[i%SWS].data);
			}
			else
				printf("Retransmitting frame %d:\n",i%SWS);
			
			send(sock,&frames[i%SWS],sizeof(Frame),0);
		}
		i=(LAR+1)%SWS;
		for(j=0;j<SWS;i++,j++)
		{
			recv(sock,&frames[i%SWS].ack,sizeof(int),0);
		}
		i=(LAR+1)%SWS;
		for(j=0;j<SWS;i++,j++)
		{
			
			if(frames[i%SWS].ack==0)
			{	
							
				flag=0;
				break;
			}
			LAR+=1;
			flag=1;	
		}
		i=(LAR+1)%SWS;
		if(flag==0){
			printf("Retransmit frame!\n");
			continue;
		}
		LAR+=SWS;
		i=(LAR+1)%SWS;
		printf("\n");
		
		
	}
	close(sock);
	return 0;
}




//server.c
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#define PORT 12346
#define RWS 3

typedef struct{
	int seq_no;
	char data[20];
	int ack;
}Frame;

int main()
{
	int sock,connfd;
	char buffer[1024];
	struct sockaddr_in sadd,cadd;
	sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
	{
		perror("error insocket");
		exit(-1);
 	}
	printf("Socket created successfully\n");
	sadd.sin_family=AF_INET;
	sadd.sin_addr.s_addr=htonl(INADDR_ANY);
	sadd.sin_port=htons(PORT);	
	if(bind(sock,(struct sockaddr*)&sadd,sizeof(sadd))<0)
	{
		perror("Error in binding");
		exit(-1);
	}
	if(listen(sock,1)!=0)
	{
		printf("Listen Failed");
		exit(-1);
	}
	socklen_t len;
	len=sizeof(cadd);
	connfd=accept(sock,(struct sockaddr *)&cadd,&len);
	if(connfd<0)
	{
		printf("Server accept failed");
		exit(0);
	}
	
	
	Frame frames[RWS];
	int LFR=-1,i=LFR+1,flag=1,j;
	
	for(;;)
	{
		for(j=0;j<RWS;i++,j++)
		{
			recv(connfd,&frames[i%RWS],sizeof(Frame),0);
			
		}
		i=(LFR+1)%RWS;
		for(j=0;j<RWS;i++,j++)
		{
			
			printf("Frame %d:%s\tseq:%d\n",i%RWS,frames[i%RWS].data,frames[i%RWS].seq_no);
			printf("Ack:1\nNAck:0");
			scanf("%d",&frames[i%RWS].ack);
			printf("\n");
		}
		i=(LFR+1)%RWS;
		for(j=0;j<RWS;i++,j++)
		{			
			send(connfd,&frames[i%RWS].ack,sizeof(int),0);
			
		}
		i=(LFR+1)%RWS;
		for(j=0;j<RWS;i++,j++)
		{
			
			if(frames[i%RWS].ack==0)
			{	
				
				flag=0;		
				break;
			}
			LFR+=1;
			flag=1;
		}
		i=(LFR+1)%RWS;
		if(flag==0)
		{
			continue;
		}
		LFR+=RWS;
		i=(LFR+1)%RWS;
			
	}
	close(sock);
	return 0;
}
