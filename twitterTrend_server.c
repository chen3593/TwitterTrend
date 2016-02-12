#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> 
#include <pthread.h>
#include <arpa/inet.h> 
#include <arpa/inet.h> 
#include <netdb.h>
#include "queue.c" 

pthread_mutex_t queue_mutex;
struct twitterDBEntry {
	char cityName[15];
	char keywords[200];
};
struct twitterDBEntry twitterDB[100];
int twitterDBCounter = 0;

void addToTwitterDB(struct twitterDBEntry entry)
{
	strncpy(twitterDB[twitterDBCounter].cityName, entry.cityName, strlen(entry.cityName));
	strncpy(twitterDB[twitterDBCounter].keywords, entry.keywords, strlen(entry.keywords));
	twitterDBCounter++;
}

void lookupTwitterDB(char cityName[], char keywords[])
{
	int i = 0;
        for(i = 0; i < twitterDBCounter; i++) {
		if(!strncmp(twitterDB[i].cityName, cityName, strlen(cityName))) {
			strncpy(keywords, twitterDB[i].keywords,strlen(twitterDB[i].keywords));
			break;
		}
	}
}


void *func(void *ptr)
{	
	int tid = *(int *) ptr; 
    struct Node* clientNode;
    char buf[200];
    char *cName;
    char cityName[20];
    char keywords[100];
    char message[200];
	while(1) {
        pthread_mutex_lock(&queue_mutex);
        if (front == NULL){
             pthread_mutex_unlock(&queue_mutex);
             continue;
        }
        else{ 
            clientNode = Dequeue();
        }
        pthread_mutex_unlock(&queue_mutex);
        fprintf(stdout, "Thread %d is handling client %s, %d\n", tid, inet_ntoa(clientNode->clientAddr.sin_addr), clientNode->clientAddr.sin_port);
        write(clientNode->clientFD, "(100,0,)", strlen("(100,0,)"));
        fprintf(stdout, "server sends handshaking: (100,0,)\n"); 
        read(clientNode->clientFD, buf, 8);
        buf[8] = '\0';
        //Server wait for twitterTrend request
		read(clientNode->clientFD, buf, 200);
        char tbuf[200];
        strcpy(tbuf, buf);
		strtok(tbuf, ",");
		char *content_size = strtok(NULL, ",");
		int size = strlen(content_size) + atoi(content_size) + 9;	
        buf[size] = '\0';
        while (1) {
            if(strcmp(buf, "(104,0,)") == 0)
                break;
            //Reset keywords and message
            memset(keywords, 0, 100);
            memset(message, 0, 200);
            cName = strtok(buf, "\"");
            cName = strtok(NULL, "\""); 
            strncpy(cityName, cName, strlen(cName));
		    cityName[strlen(cName)] = '\0';        
            lookupTwitterDB(cityName, keywords);
            if(keywords) {
                strcpy(message, "(103,");
                sprintf(tbuf,"%d",strlen(keywords));   
                strcat(message, tbuf);
                strcat(message, ",\"");
                strcat(message, keywords);
                strcat(message, "\")"); 
            }
            else{                    
                strcpy(message, "(103,0,)");
            }
            write(clientNode->clientFD, message, strlen(message));
            fprintf(stdout, "server sends twitterTrend response: %s\n", message);
            write(clientNode->clientFD, "(105,0,)", strlen("(105,0,)"));
            fprintf(stdout, "server sends end of response: (105,0,)\n");
            //Server waits for another request
            read(clientNode->clientFD, buf, 200);
            strcpy(tbuf, buf);
		    strtok(tbuf, ",");
		    char *content_size = strtok(NULL, ",");
		    int size = (atoi(content_size) == 0) ? 8 : strlen(content_size) + atoi(content_size) + 9;
		    buf[size] = '\0';       
        }
        close(clientNode->clientFD);
        fprintf(stdout, "server closes the connection\n");
        fprintf(stdout, "Thread %d finished handling client %s, %d\n", tid, inet_ntoa(clientNode->clientAddr.sin_addr), clientNode->clientAddr.sin_port);
    }
}

int main(int argc, char *argv[])
{
	// read from Twitter trend, populate the TwitterDB
	FILE *fp = NULL;
	char buf[120];
	struct twitterDBEntry entry;
	char *token;
	int numThreads = 5;
	pthread_t tid[numThreads];
	int tids[numThreads];
    int sock;
    struct sockaddr_in server;         
    if (argc == 3)
        numThreads = atoi(argv[2]);
	// ./twitterTrend client_file num_threads
	fp = fopen("TwitterDB.txt", "r");
	if(fp == NULL)
	{
		printf("\nError opening file: TwitterDB.txt");
		exit(1);
	}
	while (1) { 
		if(fgets (buf, sizeof(buf), fp) == NULL)
            break;
        token = strtok(buf, ",");
		strncpy(entry.cityName, token, strlen(token));
		entry.cityName[strlen(token)]='\0';
		token = strtok(NULL, " ");
		strncpy(entry.keywords, token, strlen(token));
		entry.keywords[strlen(token) - 1]='\0';
		addToTwitterDB(entry);
	}
	fclose(fp);
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        perror("Failed to create socket");
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons((short)atoi(argv[1]));
    if(bind(sock, (struct sockaddr *)&server, sizeof(server)) == -1)
        perror("Failed to bind the socket to port");
    listen(sock, 10);
    fprintf(stdout, "server listens\n");
	// Spawn num_threads threads
	int i = 0;
    for(i = 0; i < numThreads; i++)
	{
		tids[i] = i + 1;
		pthread_create(&tid[i], NULL, func, (void *) &tids[i]);
	}
    while(1) {
        struct sockaddr_in netclient;
        int retval;
        int len = sizeof(struct sockaddr);
        retval = accept(sock, (struct sockaddr *)(&netclient), &len);
        if(retval == -1)
            perror("Failed to accept connection");
        fprintf(stdout, "server accepts connection\n");
        struct Node* x = (struct Node*)malloc(sizeof(struct Node));
        x->clientFD = retval; 
        memcpy (&(x->clientAddr), &netclient, sizeof(struct sockaddr_in));
        pthread_mutex_lock(&queue_mutex);
        Enqueue(x);
        pthread_mutex_unlock(&queue_mutex);
    }
}
