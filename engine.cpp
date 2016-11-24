#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>

using namespace std;

//The atributes of the screen
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

SDL_Surface *load_image (string filename)
{
    //Temporary image
    SDL_Surface* loadedImage = NULL;

    //Optimised image (for use)
    SDL_Surface* optimizedImage = NULL;

    //Load the image (IMG_Load - better)
    loadedImage = IMG_Load( filename.c_str() );

    //If nothing went wrong with loading, create an optimized image and delete loaded one
    if( loadedImage != NULL )
    {
        optimizedImage = SDL_DisplayFormat( loadedImage );
        SDL_FreeSurface( loadedImage );
    }

    return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination )
{
    //Create rectangle to hold the offsets
    SDL_Rect offset;

    //Give the offsets to rectangle
    offset.x = x;
    offset.y = y;

    //Blit the surface (some graphics shit)
    SDL_BlitSurface ( source, NULL, destination, &offset );
}

int main(int argc, char* args[])
{
	//images
	SDL_Surface* message = NULL;
	SDL_Surface* background = NULL;
	SDL_Surface* screen = NULL;

	//start SDL, if doesnt start - close all
	if( SDL_Init( SDL_INIT_EVERYTHING) == -1)
	{
        return 1;
	}

	//set up screen, if error - shut it down
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );
    if (screen == NULL)
    {
        return 1;
    }

    //set the windows caption
    SDL_WM_SetCaption("Caption", NULL);

    //load images
    message = load_image("hello.bmp");
    background = load_image("background.bmp");

    //Apply background to screen
    apply_surface(0, 0, background, screen);
    SDL_Flip( screen );
    SDL_Delay(2000);
    apply_surface(320, 0, background, screen);
    apply_surface(0, 240, background, screen);
    apply_surface(320, 240, background, screen);
    SDL_Flip( screen );

    //Aplly the message to the screen
    apply_surface(180, 140, message, screen);

    //Update screen
    if(SDL_Flip( screen ) == -1)
    {
        return 1;
    }

    //pause
    SDL_Delay( 3000 );

    //Free the loaded image
    SDL_FreeSurface( message );
    SDL_FreeSurface( background );

	//quit SDL
	SDL_Quit();

	return 0;
}
