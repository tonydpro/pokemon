#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>

#define NB_IMG      5
#define NB_M        256
#define ecran       images[0]
#define ecran0		images[4]

int AGRANDISSEMENT = 2;

int LARGEUR = 256*AGRANDISSEMENT, HAUTEUR = 192*AGRANDISSEMENT;

int n_musiques = 0;

SDL_Surface* init(int x,int y,const char* titre)
{
    SDL_Surface* screen = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) == -1)
        exit(EXIT_FAILURE);

    if ((screen =
         SDL_SetVideoMode(x,y,32,SDL_DOUBLEBUF | SDL_HWSURFACE)) == NULL)
        exit(EXIT_FAILURE);

	SDL_EnableKeyRepeat(1500,1000);
    SDL_WM_SetCaption(titre,NULL);
    SDL_EnableUNICODE(1);

	if(Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,MIX_DEFAULT_CHANNELS,1024) == -1)
	{
   		printf("%s", Mix_GetError());
		exit(EXIT_FAILURE);
	}

    return screen;
}

void cleanup(SDL_Surface** images,Mix_Music** musiques)
{
    int img,m;
    for (img = 0 ; img < NB_IMG ; img++)
		if (images[img] != NULL)    
			SDL_FreeSurface(images[img]);

    for (m = 0 ; m < n_musiques ; m++)
		if (musiques[m] != NULL)    
			Mix_FreeMusic(musiques[m]);

    SDL_Quit();
	Mix_CloseAudio();
}

FILE* s_fopen(const char* cfile,const char* mode)
{
    FILE* file = NULL;

    if ((file = fopen(cfile,mode)) == NULL)
        exit(EXIT_FAILURE);

    return file;
}

void* s_malloc(size_t size)
{
    void* ptr = NULL;

    if ((ptr = malloc(size)) == NULL)
        exit(EXIT_FAILURE);

    return ptr;
}

SDL_Surface* s_IMG_Load(const char* cimage)
{
    SDL_Surface* surface = NULL;

    if ((surface = IMG_Load(cimage)) == NULL)
    {
        fprintf(stderr,"Impossible de charger l'image \'%s\' !",cimage);
        exit(EXIT_FAILURE);
    }

    return surface;
}

size_t sizeofFile(const char* cfile)
{
    size_t size;
    FILE* file = s_fopen(cfile,"rb");
    fseek(file,0,SEEK_END);
    size = ftell(file);
    rewind(file);
    fclose(file);
    return size;
}

void definirPixel(SDL_Surface *surface,int x,int y,Uint32 pixel)
{
    int nbOctetsParPixel = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * nbOctetsParPixel;

    switch(nbOctetsParPixel)
    {

        case 1:
            *p = pixel;
            break;

        case 2:
            *(Uint16 *)p = pixel;
            break;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            }
            else
            {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4:
            *(Uint32 *)p = pixel;
            break;

		default:
			*p = 0;
			break;
    }
}

Uint32 obtenirPixel(SDL_Surface *surface,int x,int y)
{

    int nbOctetsParPixel = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * nbOctetsParPixel;

    switch(nbOctetsParPixel)
    {
        case 1:
            return *p;
			break;

        case 2:
            return *(Uint16 *)p;
			break;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
			break;

        case 4:
            return *(Uint32 *)p;
			break;

        default:
            return 0;
			break;
    }
}


Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}


int entAleat(int a,int b)
{
    return rand()%(b-a+1)+a;
}


double min(double a, double b)
{
    if (a < b)
        return a;
    return b;
}

double max(double a, double b)
{
    if (a > b)
        return a;
    return b;
}

double valeur_absolue(double n)
{
    if (n < 0) return -1*n;
    else return n;
}

double carre(double a)
{
    return a*a;
}

void pause(void)
{
    int continuer = 1;
    SDL_Event event;

    while (continuer)
    {
        SDL_PollEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                continuer = 0;
                break;
            case SDL_KEYDOWN :
                if(event.key.keysym.sym == SDLK_RETURN)
                    continuer = 0;
                break;
        }
    }
}

void agrandir_image(SDL_Surface* image0,SDL_Surface* image1,int largeur_0,int hauteur_0)
{
	int i,j;

	for (j = 0 ; j < hauteur_0 ; j++)
	{
		for (i = 0 ; i < largeur_0 ; i++)
		{
			int x = AGRANDISSEMENT*i,y = AGRANDISSEMENT*j,X,Y;
			Uint32 pixel = obtenirPixel(image0,i,j);

			for (Y = y ; Y < y + AGRANDISSEMENT ; Y++)
				for (X = x ; X < x + AGRANDISSEMENT ; X++)
					definirPixel(image1,X,Y,pixel);

		}
	}
}


#endif // MAIN_H_INCLUDED
