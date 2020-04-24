#if 0
#!/bin/sh
gcc -Wall `sdl-config --cflags` udps.c -o udps `sdl-config --libs` -lSDL_net
 
exit
#endif
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include "SDL2/SDL_net.h"

struct clients {
	Uint32 IP;
	Uint32 port;
}; 
typedef struct clients Clients;
 
int main(int argc, char **argv)
{
	UDPsocket sd;       /* Socket descriptor */
	UDPpacket *pRecive;       /* Pointer to packet memory */
	UDPpacket *pSent;
    Clients client[4];
	int quit, a, b, x; 
	int clientCount=0;
	int* pClientCount;
	pClientCount=&clientCount;
	
	//
	for(int i=0;i<4;i++)
	{
		client[i].IP=0;
		client[i].port=0;
	}
 
	/* Initialize SDL_net */
	if (SDLNet_Init() < 0)
	{
		fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
 
	/* Open a socket */
	if (!(sd = SDLNet_UDP_Open(2000)))
	{
		fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
 
	/* Make space for the packet */
	if (!((pSent = SDLNet_AllocPacket(512))&&(pRecive = SDLNet_AllocPacket(512))))
	{
		fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
 
	/* Main loop */
	quit = 0;
	while (!quit)
	{
		/* Wait a packet. UDP_Recv returns != 0 if a packet is coming */
		if (SDLNet_UDP_Recv(sd, pRecive))
		{
			//printf("UDP Packet incoming\n");
			//printf("\tData:    %s\n", (char *)pRecive->data);
			//printf("\tAddress: %x %x\n", pRecive->address.host, pRecive->address.port);
			x=0;

			for(int i=0; i<=*pClientCount; i++)
			{
				
				if(pRecive->address.port == client[i].port)
				{
					for(int j=0; j<*pClientCount;j++)
					{
						if (client[j].port != 0)
						{
							printf("Send to Client %d \n", j+1);
							pSent->address.host = client[j].IP;	/* Set the destination host */
							pSent->address.port = client[j].port;
							sscanf((char * )pRecive->data, "%d %d\n", &a, &b);
							printf("%d %d\n", a, b);
							sprintf((char *)pSent->data, "%d %d\n", a,  b);
							pSent->len = strlen((char *)pSent->data) + 1;
							SDLNet_UDP_Send(sd, -1, pSent);
						
							}
						}
					x=1;
					
				}
				if(x==0)
				{
					if(client[i].IP == 0 && client[i].port == 0)
					{
						printf("Client %d\n", *pClientCount+1);
                		client[i].IP = pRecive->address.host;
                		client[i].port = pRecive->address.port;
						(*pClientCount)++;
						printf("for1 c%d ip: %d port: %d \n", clientCount, &client[i].IP, &client[i].port);
						break;
					}
					
				}

			}
			/* Quit if packet contains "quit" */
			if (strcmp((char *)pSent->data, "quit") == 0)
				quit = 1;
		}		
	}
 
	/* Clean and exit */
	SDLNet_FreePacket(pSent);
    SDLNet_FreePacket(pRecive);
	SDLNet_Quit();
	return EXIT_SUCCESS;
} 
