#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h> //Test
#include <SDL2/SDL_image.h> //Test
#include <SDL2/SDL_net.h>

#define WINDOW_WIDTH (640) //Test
#define WINDOW_HEIGHT (480) //Test

#define SPEED (300) //Test

int main(int argc, char *argv[])
{
    UDPsocket s;
	IPaddress saddr;
	UDPpacket *pSend;  
    UDPpacket *pRecive;
    bool close = false;

     if (SDLNet_Init() < 0) //Check if SDL_net is initialized, Jonas Willén movingTwoMenWithUDP.c
	{
		fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

    if (!(s = SDLNet_UDP_Open(0))) //Check if random port is open,  Jonas Willén movingTwoMenWithUDP.c
	{
		fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	if (SDLNet_ResolveHost(&saddr, "127.0.0.1", 2000) == -1) //Resolve servername, Jonas Willén movingTwoMenWithUDP.c
	{
		fprintf(stderr, "SDLNet_ResolveHost(127.0.0.1 2000): %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

    if (!((pSend = SDLNet_AllocPacket(512))&& (pRecive = SDLNet_AllocPacket(512)))) //Check if it's possible to allocate memory for send and recive packets
	{                                                                               // Jonas Willén movingTwoMenWithUDP.c
		fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

    /* CONTENT FROM JONAS W (movingTwoMenWithUDP.c) 
    * TO TEST COMMUNIKATION BETWEEN SERVER AND CLIENT UNTIL WE GOT OUR OWN PLAYERS HERE
    *
    *
    *
    */

    // attempt to initialize graphics and timer system
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* win = SDL_CreateWindow("Hello World!",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       WINDOW_WIDTH, WINDOW_HEIGHT,0);
    if (!win)
    {
        printf("error creating window: %s\n", SDL_GetError());
        SDL_Quit();
	    return 1;
    }

    // create a renderer, which sets up the graphics hardware
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);
    if (!rend)
    {
        printf("error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    // load the image into memory using SDL_image library function
    SDL_Surface* surface = IMG_Load("resources/fotboll.png");
    if (!surface)
    {
        printf("error creating surface\n");
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    // load the image data into the graphics hardware's memory
    SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surface);
    SDL_Texture* tex2 = SDL_CreateTextureFromSurface(rend, surface);

    SDL_FreeSurface(surface);
    if (!tex || !tex)
    {
        printf("error creating texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    // struct to hold the position and size of the sprite
    SDL_Rect dest;
    SDL_Rect secondDest;

    //get and scale the dimensions of texture
    SDL_QueryTexture(tex, NULL, NULL, &dest.w, &dest.h);
    dest.w /= 15;
    dest.h /= 15;

    secondDest.h = dest.h;
    secondDest.w = dest.w;    


    // start sprite in center of screen
    float x_pos = (WINDOW_WIDTH - dest.w) / 2;
    float y_pos = (WINDOW_HEIGHT - dest.h) / 2;
    float x_posOld = x_pos;
    float y_posOld = y_pos;
    float x_vel = 0;
    float y_vel = 0;
    secondDest.y = (int) y_pos;
    secondDest.x = (int) x_pos;
    int a, b; 

    // keep track of which inputs are given
    int up = 0;
    int down = 0;
    int left = 0;
    int right = 0;

    
    // animation loop
    while (!close)
    {
        // process events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                close = true;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    up = 1;
                    break;
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    left = 1;
                    break;
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    down = 1;
                    break;
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    right = 1;
                    break;
                default:
                    break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    up = 0;
                    break;
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    left = 0;
                    break;
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    down = 0;
                    break;
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    right = 0;
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }

        //determine velocity
        x_vel = y_vel = 0;
        if (up && !down) y_vel = -SPEED;
        if (down && !up) y_vel = SPEED;
        if (left && !right) x_vel = -SPEED;
        if (right && !left) x_vel = SPEED;

        // update positions
        x_pos += x_vel / 60;
        y_pos += y_vel / 60;

        /*INSIDE WHILELOOP SEND & RECIVE COORDINATE
        *
        * HERE WE WILL SEND TO AND RECIVE FROM SERVER
        *
        */


        // send and retrive positions  
        if(x_posOld != x_pos || y_posOld != y_pos){
            printf("%d %d\n", (int) x_pos, (int) y_pos);
            sprintf((char *)pSend->data, "%d %d\n", (int) x_pos, (int) y_pos);    
            pSend->address.host = saddr.host;	/* Set the destination host */
		    pSend->address.port = saddr.port;	/* And destination port */
		    pSend->len = strlen((char *)pSend->data) + 1;
            SDLNet_UDP_Send(s, -1, pSend);
            x_posOld = x_pos;
            y_posOld = y_pos;
        }      
        
        
        if (SDLNet_UDP_Recv(s, pRecive)){
            int a, b; 
            sscanf((char * )pRecive->data, "%d %d\n", &a, &b);
            secondDest.x = a;
            secondDest.y = b;
            //printf("UDP Packet incoming %d %d\n", secondDest.x, secondDest.y);
        }

    

        // collision detection with bounds
        if (x_pos <= 0) x_pos = 0;
        if (y_pos <= 0) y_pos = 0;
        if (x_pos >= WINDOW_WIDTH - dest.w) x_pos = WINDOW_WIDTH - dest.w;
        if (y_pos >= WINDOW_HEIGHT - dest.h) y_pos = WINDOW_HEIGHT - dest.h;

        // set the positions in the struct
        dest.y = (int) y_pos;
        dest.x = (int) x_pos;
       
        // clear the window
        SDL_RenderClear(rend);
        // draw the image to the window
        SDL_RenderCopy(rend, tex, NULL, &dest);
        SDL_RenderCopy(rend, tex2, NULL, &secondDest);
        SDL_RenderPresent(rend);

        // wait 1/60th of a second
        SDL_Delay(10000/60);
    
    }
    
    // clean up resources before exiting
    SDL_DestroyTexture(tex);
    SDL_DestroyTexture(tex2);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
}
