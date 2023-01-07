//
// space.c
//
// created by Aarch-64 on 28/12/2022.
// copyright (C) Agustín Gutiérrez.
//

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FPS 60

const char *const font_IMG = "images/space.bmp";
const char *const prot_IMG = "images/rocket.bmp";
const char *const enem_IMG = "images/missile.bmp";

static char *music_MP3 = "sounds/music.mp3";

/*
- x, y: upper left corner.
- texture, rect: outputs.
*/
void get_text_and_rect(SDL_Renderer *renderer, int x, int y, char *text,
        TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect) {
    int text_width;
    int text_height;
    SDL_Surface *surface;
    SDL_Color textColor = {255, 255, 255, 0};

    surface = TTF_RenderText_Solid(font, text, textColor);
    *texture = SDL_CreateTextureFromSurface(renderer, surface);
    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);
    rect->x = x;
    rect->y = y;
    rect->w = text_width;
    rect->h = text_height;
}

int main(int argc, char *argv[])
{
   SDL_Window   *screen;
   SDL_Renderer *render;
   SDL_Surface  *sprite_f;
   SDL_Surface  *sprite_p;
   SDL_Surface  *sprite_e;
   SDL_Texture  *texture_f;
   SDL_Texture  *texture_p;
   SDL_Texture  *texture_e;
   Mix_Music    *mod;
   SDL_Rect     velocity_p;
   SDL_Rect     velocity_e;   
   Uint32       render_flags;
 		
    SDL_Event event;
    SDL_Rect rect_ttf_up_y;
    SDL_Rect rect_ttf_left_x;
    SDL_Rect rect_ttf_down_y;
    SDL_Rect rect_ttf_right_x;
    SDL_Texture *texture_ttf_up_y;
    SDL_Texture *texture_ttf_left_x;
    SDL_Texture *texture_ttf_down_y;
    SDL_Texture *texture_ttf_right_x;
    char *font_path;

    font_path = "fonts/BruceForeverRegular-X3jd2.ttf";
    
   //
   // Init SDL
   //
   fprintf(stdout, "Init SDL\n");
   if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) 
   {
     fprintf(stderr, "No se pudo inicializar SDL: %s\n", SDL_GetError());
     exit(1);
   }
   
   //
   // Init TTF
   //
   TTF_Init();
   
   //
   // Init Music
   //
   fprintf(stdout, "Init Music\n");
   if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
   {
     fprintf(stderr, "sound init failed (SDL_mixer)", Mix_GetError());
     exit(1);
   }
     Mix_AllocateChannels(2);
     mod = Mix_LoadMUS(music_MP3);
   if (mod == NULL)
   {
     fprintf(stderr, "sound init failed (SDL_mixer)", Mix_GetError());
     exit(1);
   }
   else
     Mix_PlayMusic(mod, -1);
   
   //
   // Create Window
   //
   fprintf(stdout, "Create Window (%dx%d)\n",WINDOW_WIDTH,WINDOW_HEIGHT);
   screen = SDL_CreateWindow("Space",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH , WINDOW_HEIGHT, SDL_SWSURFACE);
   if (screen == NULL)
   {
      fprintf(stderr, "Error al entrar a modo grafico: %s\n", SDL_GetError() );
      SDL_Quit();
      return -1;
   }
   
   //
   // Init text
   //
   TTF_Font *font = TTF_OpenFont(font_path, 20);
   
   //
   // Init render flags
   //
   render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;

   //
   // creates a renderer to render our images
   //
   render = SDL_CreateRenderer(screen, -1, render_flags);
   if (!render)
    {
      fprintf(stderr, "error creating renderer: %s\n", SDL_GetError());
      SDL_DestroyWindow(screen);
      SDL_Quit();
      return 1;
    }
   
   //
   // Save images
   //
   sprite_f = SDL_LoadBMP(font_IMG);
   sprite_p = SDL_LoadBMP(prot_IMG);
   sprite_e = SDL_LoadBMP(enem_IMG);
   if (!sprite_f & !sprite_p & !sprite_e)
    {
        fprintf(stderr, "error creating surface\n");
        SDL_DestroyRenderer(render);
        SDL_DestroyWindow(screen);
        SDL_Quit();
        return 1;
    }
    
    //
    // Set color
    //
    if((SDL_SetColorKey(sprite_p, SDL_TRUE, SDL_MapRGB(sprite_p->format, 0, 0, 0)) != 0) &&
       (SDL_SetColorKey(sprite_e, SDL_TRUE, SDL_MapRGB(sprite_e->format, 0, 0, 0)) != 0))
    {
        printf("Unable to set colourkey: %s", SDL_GetError());
    }
   
   //
   // Create textures
   //
   texture_f = SDL_CreateTextureFromSurface(render, sprite_f);
   SDL_FreeSurface(sprite_f);
   
   texture_p = SDL_CreateTextureFromSurface(render, sprite_p);
   SDL_FreeSurface(sprite_p);
   
   texture_e = SDL_CreateTextureFromSurface(render, sprite_e);
   SDL_FreeSurface(sprite_e);
   if (!texture_f & !texture_p & !texture_e)
    {
        fprintf(stderr, "error creating texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(render);
        SDL_DestroyWindow(screen);
        SDL_Quit();
        return 1;
    }
    
    // Jugador
    
        //
   	// connects our texture with dest to control position
   	//

	SDL_QueryTexture(texture_p, NULL, NULL, &velocity_p.w, &velocity_p.h);
	SDL_QueryTexture(texture_e, NULL, NULL, &velocity_e.w, &velocity_e.h);

        //
	// adjust height and width of our image box.
	//
	velocity_p.w = sprite_p->w;
	velocity_p.h = sprite_p->h;

        //
	// sets initial x-position of object
	//
	velocity_p.x = WINDOW_WIDTH / 2 - velocity_p.w;

        //
	// sets initial y-position of object
	//
	velocity_p.y = WINDOW_HEIGHT / 1 - velocity_p.h;
	
    // ENEMY
	
        //
	// adjust height and width of our image box.
	//
	velocity_e.w = sprite_e->w;
	velocity_e.h = sprite_e->h;

        //
	// sets initial x-position of object
	//
	velocity_e.x = WINDOW_WIDTH / 2 - velocity_e.w;

        //
	// sets initial y-position of object
	//
	velocity_e.y = WINDOW_HEIGHT / 2 - velocity_e.h;
   
   //
   // Clear render
   //
   SDL_RenderClear(render);

// close = 0: while on
// close = 1: while off
int close = 0;

// speed of box
int speed = 200;

char text_up_y[40] = "(null)";
char text_left_x[40] = "(null)";
char text_down_y[40] = "(null)";
char text_right_x[40] = "(null)";

// animation loop
while (!close) 
{ // WHILE INIT
  SDL_Event event;

  // Events management
  if (SDL_PollEvent(&event)) 
    if (event.type == SDL_QUIT)
    {
          // handling of close button
          close = 1; // while close
          Mix_HaltMusic(); // pause music
    }

    if (event.type == SDL_KEYDOWN) 
  { // SDL_KEYDOWN INIT
    // keyboard API for key pressed
    if (event.key.keysym.scancode == SDL_SCANCODE_UP || 
        event.key.keysym.scancode == SDL_SCANCODE_W) 
    {
          velocity_p.y -= speed / 30;
          sprintf(text_up_y, "UP (y): %d", velocity_p.y);
    }
    else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT ||
             event.key.keysym.scancode == SDL_SCANCODE_A)
    {
          velocity_p.x -= speed / 30;
	  sprintf(text_left_x, "LEFT (x): %d", velocity_p.x);
    }
    else if (event.key.keysym.scancode == SDL_SCANCODE_DOWN ||
             event.key.keysym.scancode == SDL_SCANCODE_S)
    {
          velocity_p.y += speed / 30;
	  sprintf(text_down_y, "DOWN (y): %d", velocity_p.y);
    }
    else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT ||
             event.key.keysym.scancode == SDL_SCANCODE_D)
    {	
          velocity_p.x += speed / 30;
	  sprintf(text_right_x, "RIGHT (x): %d", velocity_p.x);
    }
    else
    {
        //NULL
    }

  } // SDL_KEYDOWN EXIT
    
    // right boundary
    if (velocity_p.x + velocity_p.w > WINDOW_WIDTH)
     velocity_p.x = WINDOW_WIDTH - velocity_p.w;

    // left boundary
    if (velocity_p.x < 0)
      velocity_p.x = 0;

    // bottom boundary
    if (velocity_p.y + velocity_p.h > WINDOW_HEIGHT)
      velocity_p.y = WINDOW_HEIGHT - velocity_p.h;

    // upper boundary
    if (velocity_p.y < 0)
      velocity_p.y = 0;
    
   //
   // Print in render
   //

   get_text_and_rect(render, 0, 0, text_up_y, font, &texture_ttf_up_y, &rect_ttf_up_y);
   get_text_and_rect(render, 0, 25, text_left_x, font, &texture_ttf_left_x, &rect_ttf_left_x);
   get_text_and_rect(render, 0, 50, text_down_y, font, &texture_ttf_down_y, &rect_ttf_down_y);
   get_text_and_rect(render, 0, 75, text_right_x, font, &texture_ttf_right_x, &rect_ttf_right_x);
        // clears the screen
     SDL_RenderCopy(render, texture_f, NULL, NULL);
     SDL_RenderCopy(render, texture_e, NULL, &velocity_e);
     SDL_RenderCopy(render, texture_p, NULL, &velocity_p);

     /* Use TTF textures. */
     SDL_RenderCopy(render, texture_ttf_up_y, NULL, &rect_ttf_up_y);
     SDL_RenderCopy(render, texture_ttf_left_x, NULL, &rect_ttf_left_x);
     SDL_RenderCopy(render, texture_ttf_down_y, NULL, &rect_ttf_down_y);
     SDL_RenderCopy(render, texture_ttf_right_x, NULL, &rect_ttf_right_x);
     
     /* Dibujamos el protagonista */
     SDL_RenderPresent(render);

     // calculate fps
     SDL_Delay(1000 / FPS);
    
    } // WHILE EXIT
  
   /* Finalmente, preparamos para salir */
   SDL_DestroyTexture(texture_f);
   SDL_DestroyTexture(texture_p);
   SDL_DestroyTexture(texture_ttf_up_y);
   SDL_DestroyTexture(texture_ttf_left_x);
   SDL_DestroyTexture(texture_ttf_down_y);
   SDL_DestroyTexture(texture_ttf_right_x);
   SDL_DestroyRenderer(render);
   SDL_DestroyWindow(screen);
   
   TTF_Quit();
   Mix_CloseAudio();
   SDL_Quit();
   return 0;
}
