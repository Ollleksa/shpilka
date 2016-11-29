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

//The atributes of the level
const int LEVEL_WIDTH = 3840;
const int LEVEL_HEIGHT = 2160;

//The atributes of block
const int BLOCK_SIZE = 80;
SDL_Rect block = {640, 480, BLOCK_SIZE, BLOCK_SIZE};

//The artibutes of drone
const int DRONE_SIZE = 128;

//Surfaces (elements)
SDL_Surface* screen = NULL;
SDL_Surface* background = NULL;
SDL_Surface* drone = NULL;
SDL_Surface* info = NULL;

//Camera
SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

//Events queue
SDL_Event event;

//Some indicator
bool presssed = false;

//The font
TTF_Font *font = NULL;
SDL_Color textColor = { 0, 0, 0};

bool check_collision(SDL_Rect &A, SDL_Rect &B);

class Drone
{
    private:
        SDL_Rect box;     //Box for our drone
        float Vx, Vy;     //velocities
        float ax, ay;     //forses;

    public:
        Drone();
        //read keyboard
        void handle_keys();
        //Move system
        void move(int dt);
        //show system
        void show();
        //set camera in right position
        void set_camera();
};

class Timer
{
    private:
        int startTicks;         //clock time when started
        int pausedTicks;        //clock time when paused
        bool started;           //is started/paused?
        bool paused;
    public:
        Timer();
        //clock actions
        void start();
        void stop();
        void pause();
        void unpause();
        //Get time
        int get_Ticks();
        //Status?
        bool is_started();
        bool is_paused();
};

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
        optimizedImage = SDL_DisplayFormatAlpha( loadedImage );
        SDL_FreeSurface( loadedImage );
    }

    //if( optimizedImage != NULL )
    //{
    //Map the color key //set all picsels of color R 0, G 0xFF, B 0xFF to be transparent
    //    Uint32 colorkey = SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF);
    //    SDL_SetColorKey(optimizedImage, SDL_SRCCOLORKEY, colorkey);
    //}

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
    background = load_image("field.png");
    //drone =/
    drone = load_image("drone.png");
    //Font
    font = TTF_OpenFont("font.ttf", 36);

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
    SDL_FreeSurface( drone );
    SDL_FreeSurface( info );

    //Close font
    TTF_CloseFont( font );

    //Quit TTF and SDL
    TTF_Quit();
    SDL_Quit();
}

Drone::Drone()
{
    box.x = SCREEN_WIDTH/2;
    box.y = SCREEN_HEIGHT/2;
    box.w = DRONE_SIZE;
    box.h = DRONE_SIZE;

    Vx = 0;
    Vy = 0;
    ax = 0;
    ay = 0;
}

void Drone::handle_keys()
{
    //If key was pressed
    if( event.type == SDL_KEYDOWN)
    {
        presssed = true;
        switch( event.key.keysym.sym )
        {
            case SDLK_UP: ay--; break;
            case SDLK_DOWN: ay++; break;
            case SDLK_LEFT: ax--; break;
            case SDLK_RIGHT: ax++; break;
            default: ;
        }
    }
    //If key was relized
    else if( event.type == SDL_KEYUP )
    {
        presssed = false;
        switch( event.key.keysym.sym )
        {
            case SDLK_UP: ay++; break;
            case SDLK_DOWN: ay--; break;
            case SDLK_LEFT: ax++; break;
            case SDLK_RIGHT: ax--; break;
            default: ;
        }
    }
}

void Drone::move(int dt)
{
    Vx += 10 * ax * (dt/1000.f);
    Vy += 10 * ay * (dt/1000.f);

    box.x += Vx* (dt/1000.f);
    box.y += Vy* (dt/1000.f);

    if( (box.x < 0) || ( box.x > LEVEL_WIDTH ) )
    {
        box.x -= Vx* (dt/1000.f);
        Vx = 0;
    }

    if( (box.y < 0) || ( box.y > LEVEL_HEIGHT ) )
    {
        box.y -= Vy* (dt/1000.f);
        Vy = 0;
    }

    if (check_collision(box, block))
    {
        box.x -= Vx* (dt/1000.f);
        box.y -= Vy* (dt/1000.f);
        Vx = 0;
        Vy = 0;
    }
}

void Drone::show()
{
    apply_surface(box.x - camera.x, box.y - camera.y, drone, screen );
    if(!presssed)
    {
        apply_surface(0,0, info, screen );
    }
}

void Drone::set_camera()
{
    //center camera
    camera.x = box.x - SCREEN_WIDTH/2;
    camera.y = box.y - SCREEN_HEIGHT/2;

    //keep camera in bounds
    if(camera.x < 0)
    {
        camera.x = 0;
    }
    if(camera.x > LEVEL_WIDTH - camera.w)
    {
        camera.x = LEVEL_WIDTH - camera.w;
    }

    if(camera.y < 0)
    {
        camera.y = 0;
    }
    if(camera.y > LEVEL_HEIGHT - camera.h)
    {
        camera.y = LEVEL_HEIGHT - camera.h;
    }
}

Timer::Timer()
{
    startTicks = 0;
    pausedTicks = 0;
    paused = false;
    started = false;
}

void Timer::start()
{
    started = true;
    paused = false;

    startTicks = SDL_GetTicks();
}

void Timer::stop()
{
    started = false;
    paused = false;
}

void Timer::pause()
{
    if( started == true && paused == false )
    {
        paused = true;

        pausedTicks = SDL_GetTicks() - startTicks;
    }
}

void Timer::unpause()
{
    if( paused == true )
    {
        paused = false;

        startTicks = SDL_GetTicks() - pausedTicks;

        pausedTicks = 0;
    }
}

int Timer::get_Ticks()
{
    if( started == true )
    {
        if( paused == true )
        {
            return pausedTicks;
        }
        else
        {
            return SDL_GetTicks() - startTicks;
        }
    }
    else
    {
        return 0;
    }
}

bool Timer::is_started()
{
    return started;
}
bool Timer::is_paused()
{
    return paused;
}

bool check_collision(SDL_Rect &A, SDL_Rect &B)
{
    //borders of rectangles
    int leftA, rightA, topA, botA;
    int leftB, rightB, topB, botB;

    //Numbers
    leftA = A.x;
    rightA = A.x + A.w;
    topA = A.y;
    botA = A.y + A.h;

    leftB = B.x;
    rightB = B.x + B.w;
    topB = B.y;
    botB = B.y + B.h;

    //check if A is out of B
    if( ( (botA <= topB) || (topA >= botB ) || (rightA <= leftB) || (leftA >= rightB) ) == false )
    {
        return true;
    }
    else
    {
        return false;
    }
}


int main(int argc, char* args[])
{
	//main loop flag
	bool quit = false;

	Drone myDrone;

	Timer delta;

    //Initialize
    if( init() == false )
    {
        return 1;
    }
    if( load_files() == false )
    {
        return 1;
    }

    //create text
    info = TTF_RenderText_Solid( font, "Inertion", textColor );

    //start timer first time
    delta.start();

    //while isnt closed
    while( !quit )
    {
        //If something happens
        while( SDL_PollEvent( &event ))
        {
            //Read the keyboard
            myDrone.handle_keys();

            //User request quit
            if( event.type == SDL_QUIT )
            {
                quit = true;
            }
        }

        //set camera
        myDrone.set_camera();

        //Moving
        myDrone.move(delta.get_Ticks());

        //Restart timer for next iteration
        delta.start();

        //apply background part with camera
        apply_surface(0,0,background,screen,&camera);

        //Show block
        SDL_FillRect(screen, &block, SDL_MapRGB(screen->format, 0x77, 0x77, 0x77));

        myDrone.show();

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
