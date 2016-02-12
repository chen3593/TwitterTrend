#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 

char buf[200];
char tbuf[200];
char msg[200]; 
char reply[200];
char cityname[40];
char inputfilename[40];
FILE *fp;
FILE *file;

int main(int argc , char *argv[])
{
    int sock, size, i, num_clientfile;
    struct sockaddr_in server;
    num_clientfile = argc - 3;
    //this vaule solves the extra credit

    //use function socket to create socket
    if ( (sock = socket(AF_INET, SOCK_STREAM , 0)) == -1 ){
         printf("Could not create socket");
         return -1;
    }
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_family = AF_INET;
    server.sin_port = htons( atoi(argv[2]) );
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
    fprintf(stdout, "client connects\n");
    read(sock, buf, 8);
    buf[8] = '\0';
    write(sock, "(101,0,)", strlen("(101,0,)"));
    fprintf(stdout, "client sends handshake response: (101,0,)\n");
    for (i = 0; i < num_clientfile; i++){
        fp = fopen(argv[3+i],"r");
        if(fp == NULL)
        {
            perror("cannot open file argv[3]");
            return -1;
        }
        strcpy(inputfilename,argv[3 + i]);
        strcat(inputfilename,".result");
        file=fopen(inputfilename,"w+");
        while(1) {
	        if(fgets(cityname,100,fp) == NULL) 
	            break;         
            cityname[strlen(cityname)-1] = '\0';
            char *info;   
            strcpy(msg, "(102,");
            memset(tbuf, 0, 200);
            sprintf(tbuf,"%d",strlen(cityname));   
            strcat(msg, tbuf);
            strcat(msg, ",\"");
            strcat(msg, cityname);
            strcat(msg, "\")");
            write(sock, msg, strlen(msg));
            fprintf(stdout, "client sends twitterTrend request: %s\n", msg);
            memset(reply, 0, 200);
            memset(tbuf, 0, 200);
	        read(sock, reply, 100);
            strcpy(tbuf, reply);
	        strtok(reply, ",");
	        char *infosize = strtok(NULL, ",");
	        if (atoi(infosize) == 0) {
		        size = 8;
			}
	        else { 
		        size = 9 + strlen(infosize) + atoi(infosize);
			}
	        memset(reply, 0, 200);
            strcpy(reply, tbuf);
	        reply[size] = '\0';
            strtok(reply, ",");
            info = strtok(NULL, ",");        
            if (!strncmp(info, "0", strlen(info))){          
		        memset(reply, 0, 200);
                strcpy(reply, tbuf);
                strtok(reply, "\""); 
                info = strtok(NULL, "\"");
                strncpy(msg, info, strlen(info));
                msg[strlen(info)]='\0';   
                fprintf(file,"%s: %s\n",cityname, msg); 
            }
            else { 
                fprintf(file,"%s: NA\n",cityname);
            }    
            memset(buf, 0, 200);
		    read(sock, buf, 8);
		    buf[8] = '\0';
        }
        close(fp);
        close(file);
    }
    write(sock, "(104,0,)" , strlen("(104,0,)"));
    fprintf(stdout, "client sends end of request:(104,0,)\n");
    close(sock);
    fprintf(stdout, "client closes connection\n");
    return 0;
}
