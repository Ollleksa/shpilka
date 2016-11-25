#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <iostream>
#include <string>

using namespace std;

//The atributes of the screen
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//Surfaces (elements)
SDL_Surface* screen = NULL;
SDL_Surface* background = NULL;
SDL_Surface* unit = NULL;

//Events queue
SDL_Event event;

//The font
TTF_Font *font = NULL;
SDL_Color textColor = { 0, 0, 0};

//Load images from file. Improve them in some way (I do not know), and delete colorkey
SDL_Surface *load_image (string filename)
{
    //Temporary image and optimized one;
    SDL_Surface* loadedImage = NULL;
    SDL_Surface* optimizedImage = NULL;

    //Load the image
    loadedImage = IMG_Load( filename.c_str() );

    if( loadedImage != NULL )
    {
        optimizedImage = SDL_DisplayFormat( loadedImage );
        SDL_FreeSurface( loadedImage );
    }

    if( optimizedImage != NULL )
    {
    //Map the color key //set all picsels of color R 0, G 0xFF, B 0xFF to be transparent
        Uint32 colorkey = SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF);
        SDL_SetColorKey(optimizedImage, SDL_SRCCOLORKEY, colorkey);
    }

    return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
    //Create rectangle to hold the offsets
    SDL_Rect offset;

    //Give the offsets to rectangle
    offset.x = x;
    offset.y = y;

    //Blit the surface (some graphics shit), clip is for cutting images, probably will not use.
    SDL_BlitSurface ( source, clip, destination, &offset );
}

bool init()
{
    //start SDL, if doesnt start - close all
	if( SDL_Init( SDL_INIT_EVERYTHING) == -1)
	{
        cout << "SDL_Init" << endl;
        return false;
	}

	//set up screen, if error - shut it down
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );
    if (screen == NULL)
    {
        cout << "SDL_vIDEO" << endl;
        return false;
    }

    //Initialize SDL_ttf
    if( TTF_Init() == -1 )
    {
        cout << "TTF_Init" << endl;
        return false;
    }

    //set the windows caption
    SDL_WM_SetCaption("Game", NULL);

    //If everything is good
    return true;
}

bool load_files()
{
    //Background
    background = load_image("background.png");
    //Font
    font = TTF_OpenFont("lazy.ttf", 72);

    //if problems, go down
    if( background == NULL )
        {cout << "background" << endl; return false;}

    if( font == NULL )
        {cout << "font" << endl; return false;}

    //If all alright
    return true;
}

void close()
{
    //Free the surfaces
    SDL_FreeSurface(background);
    SDL_FreeSurface( unit );

    //Close font
    TTF_CloseFont( font );

    //Quit TTF and SDL
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char* args[])
{
	//main loop flag
	bool quit = false;

    //Initialize
    if( init() == false )
    {
        return 1;
    }
    if( load_files() == false )
    {
        return 1;
    }

    //create unit
    unit = TTF_RenderText_Solid( font, "Unit!", textColor );

    //apply background
    apply_surface(0,0,background,screen);

    //coordinates
    int xoff = 320;
    int yoff = 240;
    int x0 = xoff;
    int y0 = yoff;

    //while isnt closed
    while( !quit )
    {
        //If something happens
        while( SDL_PollEvent( &event ))
        {
            //if key was pressed
            if( event.type == SDL_KEYDOWN )
            {
                switch( event.key.keysym.sym )
                {
                    case SDLK_UP: yoff--; break;
                    case SDLK_DOWN: yoff++; break;
                    case SDLK_LEFT: xoff--; break;
                    case SDLK_RIGHT: xoff++; break;
                    default: ;
                }
            }
            //User request quit
            else if( event.type == SDL_QUIT )
            {
                quit = true;
            }
        }

        //if something moving - it moving;
        if( (xoff != x0) || (yoff != y0) )
        {
            apply_surface(0,0,background,screen);
            apply_surface(xoff, yoff, unit, screen);

            x0 = xoff;
            y0 = yoff;
        }

    //Update screen
        if(SDL_Flip( screen ) == -1)
        {
            return 1;
        }
    }
	//quit SDL
	close();

	return 0;
}
