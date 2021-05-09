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
#include <SDL/SDL_ttf.h>

#define NB_IMG      5
#define NB_M        256
#define ecran       images[0]
#define ecran0		images[3]

typedef struct SDL_InputBox SDL_InputBox;
struct SDL_InputBox
{
    SDL_Rect pos;
    SDL_Color bmp1,bmp2;
    int state,decal,mode;
    size_t sizeText,buffSize,bufflen,taille;
    TTF_Font* font;
    char* buffer;
    unsigned char*buffer2;
};

typedef struct SDL_Button SDL_Button;
struct SDL_Button
{
    SDL_Rect pos;
    char* caption;
    size_t textSize;
    TTF_Font* font;
    int state,signal;
    SDL_Color bmp1,bmp2;
};

typedef struct SDL_ButtonIMG SDL_ButtonIMG;
struct SDL_ButtonIMG
{
    SDL_Rect pos;
    SDL_Color fond;
    SDL_Surface* picture;
    int state,signal;
};

SDL_Color BLANC = {255,255,255,0};
SDL_Color GRIS = {192,192,192,0};
SDL_Color NOIR = {0,0,0,0};
SDL_Color ROUGE = {255,0,0,0};
SDL_Color BLEU = {0,0,255,0};
SDL_Color VERT = {0,255,0,0};
SDL_Color ORANGE = {255,126,0,0};
SDL_Color JAUNE = {200,200,000,0};

int LVL[4] = {1,1,1,1};

TTF_Font* s_TTF_OpenFont(const char*,int);
void ligne(SDL_Surface*,int,int,int,int,Uint32);
double distance(int x1,int y1,int x2,int y2);
double valeur_absolue(double);
double carre(double);

int truePos(SDL_Rect,int,int);
SDL_Rect SDL_SetRect(int,int,int,int);

SDL_Button* SDL_CreateButton(SDL_Rect,char*,char*,size_t,SDL_Color,SDL_Color);
SDL_ButtonIMG* SDL_CreateButtonIMG(SDL_Rect,SDL_Color,char*);

SDL_InputBox* SDL_CreateInputBox(SDL_Rect,SDL_Color,SDL_Color,size_t,size_t,size_t,char*,int,int);
void SDL_FreeInputBox(SDL_InputBox*);
int SDL_GetInputBoxState(SDL_InputBox*,SDL_Event);
void SDL_RefreshInputBox(SDL_InputBox*,SDL_Surface*);

int AGRANDISSEMENT = 2;

int LARGEUR = 256*AGRANDISSEMENT, HAUTEUR = 192*AGRANDISSEMENT;

int n_musiques = 0;

SDL_Surface* init(int x,int y,const char* titre)
{
    SDL_Surface* screen = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) == -1)
        exit(EXIT_FAILURE);

    if (TTF_Init() == -1)
        exit(EXIT_FAILURE);

    if ((screen =
         SDL_SetVideoMode(x,y,32,SDL_DOUBLEBUF | SDL_HWSURFACE)) == NULL)
        exit(EXIT_FAILURE);

	SDL_EnableKeyRepeat(500,100);
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
	TTF_Quit();
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

void* s_realloc(void*ptr,size_t size)
{
    if ((ptr = s_realloc(ptr,size)) == NULL)
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

int pause2(int k1,int k2)
{
    int continuer = 1,choix = 0;
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
                if(event.key.keysym.sym == k1)
                {
					choix = 1;
					continuer = 0;
				}
			    else if(event.key.keysym.sym == k2)
                {
					choix = 2;
					continuer = 0;
				}
                break;
        }
    }

	return choix;
}

void agrandir_image(SDL_Surface* image0,SDL_Surface* image1,int largeur_0,int hauteur_0)
{
	if (AGRANDISSEMENT <= 1)
	{
		SDL_Rect pos = {0,0,0,0};
		SDL_BlitSurface(image0,NULL,image1,&pos);
		return;
	}

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

TTF_Font* s_TTF_OpenFont(const char* police,int taille)
{
    TTF_Font* font = NULL;

    if ((font = TTF_OpenFont(police,taille)) == NULL)
    {
        fprintf(stderr,"Impossible de charger la police \'%s\' : %s\n",police,TTF_GetError());
        exit(EXIT_FAILURE);
    }

    return font;
}

void ligne(SDL_Surface* surf,int x1,int y1,int x2,int y2,Uint32 couleur)
{
    int x,y,Dx,Dy,xincr,yincr,erreur,i;

    Dx = abs(x2-x1); Dy = abs(y2-y1);

    if(x1<x2) xincr = 1;
    else xincr = -1;
    if(y1<y2) yincr = 1;
    else yincr = -1;

    x = x1; y = y1;
    if(Dx > Dy)
    {
        erreur = Dx/2;
        for(i = 0; i < Dx ; i++)
        {
            x += xincr; erreur += Dy;
            if(erreur > Dx)
            {
                erreur -= Dx;
                y += yincr;
            }
            definirPixel(surf,x,y,couleur);
        }
    }
    else
    {
        erreur = Dy/2;
        for(i = 0; i < Dy ; i++)
        {
            y += yincr; erreur += Dx;
            if(erreur > Dy)
            {
                erreur -= Dy;
                x += xincr;
            }
            definirPixel(surf,x,y,couleur);
        }
    }

    definirPixel(surf,x1,y1,couleur);
    definirPixel(surf,x2,y2,couleur);
}


void afficher_console(SDL_Surface* image,const char* nom_font,int taille,char* console)
{
    TTF_Font* font = s_TTF_OpenFont(nom_font,taille);
    char console2[54];
    memset(console2,0,54);
    strncpy(console2,console,min(strlen(console),54));

    int i,j;
    for (i = 0 ; i < LARGEUR ; i++)
    {
        for (j = HAUTEUR - 40 ; j < HAUTEUR ; j++)
            definirPixel(image,i,j,0x000000);
    }
    SDL_Rect postxt = {5,HAUTEUR - 30,LARGEUR - 5,20};

    SDL_Surface* txt = NULL;
    txt = TTF_RenderText_Shaded(font,console2,BLANC,NOIR);
    SDL_BlitSurface(txt,NULL,image,&postxt);
    TTF_CloseFont(font);
    SDL_FreeSurface(txt);
    SDL_Flip(image);
}

void afficher_niveaux(SDL_Surface* image)
{
    TTF_Font* font = s_TTF_OpenFont("pkmndp.ttf",16);
    char console[54];

	sprintf(console,"%d %d %d %d",LVL[0],LVL[1],LVL[2],LVL[3]);

 	int i,j;
    for (i = 195 ; i < 256 ; i++)
    {
        for (j = 0 ; j < 30 ; j++)
            definirPixel(image,i,j,0x000000);
    }

    SDL_Rect postxt = {205,8,100,100};

    SDL_Surface* txt = NULL;
    txt = TTF_RenderText_Shaded(font,console,BLANC,NOIR);
    SDL_BlitSurface(txt,NULL,image,&postxt);
    TTF_CloseFont(font);
    SDL_FreeSurface(txt);
}

int truePos(SDL_Rect objPos,int x,int y)
{
    return (x < (objPos.w + objPos.x) && x > objPos.x && y > objPos.y && y < (objPos.h + objPos.y));
}

SDL_Rect SDL_SetRect(int x,int y,int w,int h)
{
    SDL_Rect rect;

    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    return rect;
}

SDL_Button* SDL_CreateButton(SDL_Rect pos,char* caption,char* font,size_t textSize,SDL_Color bmp1,SDL_Color bmp2)
{
    SDL_Button* obj = NULL;

    obj = s_malloc(sizeof(SDL_Button));

    obj->state = 0;
    obj->signal = 0;
    obj->pos = pos;
    obj->bmp1 = bmp1;
    obj->bmp2 = bmp2;
    obj->textSize = textSize;
    obj->font = s_TTF_OpenFont(font,textSize);
    obj->caption = s_malloc(strlen(caption)+1);

    strcpy(obj->caption,caption);

    return obj;
}

SDL_ButtonIMG* SDL_CreateButtonIMG(SDL_Rect pos,SDL_Color fond,char* picturePath)
{
    SDL_ButtonIMG* obj = NULL;

    obj = s_malloc(sizeof(SDL_ButtonIMG));
    obj->pos = pos;
    obj->fond = fond;
    obj->picture = s_IMG_Load(picturePath);

    return obj;
}

void SDL_FreeButton(SDL_Button* obj)
{
    TTF_CloseFont(obj->font);
    free(obj->caption);
    free(obj);
}

void SDL_FreeButtonIMG(SDL_ButtonIMG* obj)
{
    SDL_FreeSurface(obj->picture);
    free(obj);
}

void SDL_RefreshButton(SDL_Button* obj,SDL_Surface* image)
{
    SDL_Surface* txt = NULL,*bord = NULL,*bord2;
    txt = TTF_RenderText_Shaded(obj->font,obj->caption,obj->bmp1,obj->bmp2);

    if (obj->state == 1)
    {
        obj->pos.x += 2;
        SDL_BlitSurface(txt,NULL,image,&(obj->pos));
        obj->pos.x -= 2;
    }
    else
        SDL_BlitSurface(txt,NULL,image,&(obj->pos));

    SDL_Rect posBord = SDL_SetRect(obj->pos.x,obj->pos.y,obj->pos.w,obj->pos.h);
    bord = SDL_CreateRGBSurface(SDL_HWSURFACE,txt->w+2,2,32,0,0,0,0);
    bord2 = SDL_CreateRGBSurface(SDL_HWSURFACE,2,txt->h,32,0,0,0,0);

    int type1,type2;

    if (!obj->state)
    {
        type1 = 235;
        type2 = 0;
    }
    else
    {
        type1 = 0;
        type2 = 235;
    }

    SDL_FillRect(bord,NULL,SDL_MapRGB(image->format,type1,type1,type1));
    SDL_BlitSurface(bord,NULL,image,&posBord);
    SDL_FillRect(bord,NULL,SDL_MapRGB(image->format,type2,type2,type2));
    posBord.y += txt->h;
    SDL_BlitSurface(bord,NULL,image,&posBord);

    posBord.y -= txt->h;
    SDL_FillRect(bord2,NULL,SDL_MapRGB(image->format,type1,type1,type1));
    SDL_BlitSurface(bord2,NULL,image,&posBord);
    SDL_FillRect(bord2,NULL,SDL_MapRGB(image->format,type2,type2,type2));
    posBord.x += txt->w;
    SDL_BlitSurface(bord2,NULL,image,&posBord);

    SDL_FreeSurface(txt);
    SDL_FreeSurface(bord);
    SDL_FreeSurface(bord2);
}

void SDL_RefreshButtonIMG(SDL_ButtonIMG* obj,SDL_Surface* image)
{
    SDL_Surface *bord = NULL,*bord2;

    if (obj->state == 1)
    {
        obj->pos.x += 2;
        SDL_BlitSurface(obj->picture,NULL,image,&(obj->pos));
        obj->pos.x -= 2;
    }
    else
        SDL_BlitSurface(obj->picture,NULL,image,&(obj->pos));

    SDL_Rect posBord = SDL_SetRect(obj->pos.x,obj->pos.y,obj->pos.w,obj->pos.h);
    bord = SDL_CreateRGBSurface(SDL_HWSURFACE,obj->picture->w+2,2,32,0,0,0,0);
    bord2 = SDL_CreateRGBSurface(SDL_HWSURFACE,2,obj->picture->h,32,0,0,0,0);

    int type1,type2;

    if (!obj->state)
    {
        type1 = 235;
        type2 = 0;
    }
    else
    {
        type1 = 0;
        type2 = 235;
    }

    SDL_FillRect(bord,NULL,SDL_MapRGB(image->format,type1,type1,type1));
    SDL_BlitSurface(bord,NULL,image,&posBord);
    SDL_FillRect(bord,NULL,SDL_MapRGB(image->format,type2,type2,type2));
    posBord.y += obj->picture->h;
    SDL_BlitSurface(bord,NULL,image,&posBord);

    posBord.y -= obj->picture->h;
    SDL_FillRect(bord2,NULL,SDL_MapRGB(image->format,type1,type1,type1));
    SDL_BlitSurface(bord2,NULL,image,&posBord);
    SDL_FillRect(bord2,NULL,SDL_MapRGB(image->format,type2,type2,type2));
    posBord.x += obj->picture->w;
    SDL_BlitSurface(bord2,NULL,image,&posBord);

    SDL_FreeSurface(bord);
    SDL_FreeSurface(bord2);
}

int SDL_GetButtonState(SDL_Button* obj,SDL_Event event)
{
    obj->signal = 0;
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT && truePos(obj->pos,event.button.x,event.button.y))
    {
        if (obj->state == 1 || !obj->state)
            obj->state = 1;
        return 1;
    }
    else if (truePos(obj->pos,event.button.x,event.button.y) && obj->state == 1 && event.type != SDL_MOUSEBUTTONUP)
    {
        obj->state = 1;
        return 1;
    }
    else
    {
        if (obj->state)
            obj->signal = 1;
        obj->state = 0;
        return 0;
    }
}

int SDL_GetButtonIMGState(SDL_ButtonIMG* obj,SDL_Event event)
{
    obj->signal = 0;
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT && truePos(obj->pos,event.button.x,event.button.y))
    {
        if (obj->state == 1 || !obj->state)
            obj->state = 1;
        return 1;
    }
    else if (truePos(obj->pos,event.button.x,event.button.y) && obj->state == 1 && event.type != SDL_MOUSEBUTTONUP)
    {
        obj->state = 1;
        return 1;
    }
    else
    {
        if (obj->state)
            obj->signal = 1;
        obj->state = 0;
        return 0;
    }
}

SDL_InputBox* SDL_CreateInputBox(SDL_Rect pos,SDL_Color bmp1,SDL_Color bmp2,size_t sizeText,size_t buffSize,size_t taille,char* font,int decal,int mode)
{
    SDL_InputBox* box = NULL;

    box = s_malloc(128);
    box->font = s_TTF_OpenFont(font,sizeText);
    box->buffer = s_malloc(buffSize+100);
    box->buffer2 = s_malloc(buffSize+100);
    box->bufflen = 0;
    box->buffer[0] = '\0';
    box->pos = pos;
    box->bmp1 = bmp1;
    box->bmp2 = bmp2;
    box->state = 0;
    box->sizeText = sizeText;
    box->buffSize = buffSize;
    box->decal = decal;
    box->mode = mode;
    box->taille = taille;

    return box;
}

void SDL_FreeInputBox(SDL_InputBox* box)
{
    TTF_CloseFont(box->font);
    free(box->buffer);
    free(box->buffer2);
    free(box);
}

int SDL_GetInputBoxState(SDL_InputBox* obj,SDL_Event event)
{
    obj->bufflen = strlen(obj->buffer);
    if (event.type == SDL_KEYDOWN && obj->state && (obj->bufflen < obj->buffSize || event.key.keysym.unicode == '\b') && event.key.keysym.unicode)
    {

        if (event.key.keysym.unicode == '\b' && obj->bufflen)
        {
            obj->bufflen--;
            obj->buffer[obj->bufflen] = '\0';
            obj->buffer2[obj->bufflen] = '\0';
        }
        else if (!iscntrl(event.key.keysym.unicode))
        {
            if (obj->mode == 0)
                obj->buffer[obj->bufflen] = event.key.keysym.unicode;
            else
            {
                obj->buffer[obj->bufflen] = '*';
                obj->buffer2[obj->bufflen] = event.key.keysym.unicode;
            }
            obj->bufflen++;
        }
        return 1;
    }
    else if ((event.type == SDL_MOUSEBUTTONDOWN  && truePos(obj->pos,event.button.x,event.button.y)) || (truePos(obj->pos,event.button.x,event.button.y) && obj->state == 1 && event.type != SDL_MOUSEBUTTONUP))
    {
        obj->state = 1;
        return 1;
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN  && !(truePos(obj->pos,event.button.x,event.button.y)))
    {
        obj->state = 0;
        return 0;
    }
    return 1;
}

void SDL_RefreshInputBox(SDL_InputBox* obj,SDL_Surface* image)
{
    long t = time(NULL);

    SDL_Surface* txt = NULL,*bord = NULL,*bord2,*mur = NULL;
    SDL_Rect posBord = SDL_SetRect(obj->pos.x,obj->pos.y,obj->pos.w,obj->pos.h);

    mur = SDL_CreateRGBSurface(SDL_HWSURFACE,obj->pos.w,obj->sizeText+6,32,0,0,0,0);
    bord = SDL_CreateRGBSurface(SDL_HWSURFACE,obj->pos.w+2,2,32,0,0,0,0);
    bord2 = SDL_CreateRGBSurface(SDL_HWSURFACE,2,obj->sizeText+6,32,0,0,0,0);

    SDL_FillRect(mur,NULL,SDL_MapRGB(image->format,obj->bmp2.r,obj->bmp2.g,obj->bmp2.b));
    SDL_BlitSurface(mur,NULL,image,&(obj->pos));

    if (t % 2 && obj->state)
        strcat(obj->buffer,"|");
    else
        strcat(obj->buffer," ");

    if (obj->taille <= obj->bufflen)
    {
        char buffer2[obj->buffSize+10];
        memset(buffer2,0,obj->buffSize+10);
        strncpy(buffer2,obj->buffer+obj->bufflen-obj->taille-1,obj->taille+1);
        txt = TTF_RenderText_Shaded(obj->font,buffer2,obj->bmp1,obj->bmp2);
    }
    else
        txt = TTF_RenderText_Shaded(obj->font,obj->buffer,obj->bmp1,obj->bmp2);

    obj->buffer[obj->bufflen] = '\0';

    posBord.y += obj->decal;
    posBord.x += 3;
    SDL_BlitSurface(txt,NULL,image,&posBord);
    posBord.y -= obj->decal;
    posBord.x -= 3;

    SDL_FillRect(bord,NULL,SDL_MapRGB(image->format,0,0,0));
    SDL_BlitSurface(bord,NULL,image,&posBord);
    SDL_FillRect(bord,NULL,SDL_MapRGB(image->format,235,235,235));
    posBord.y += obj->sizeText+6;
    SDL_BlitSurface(bord,NULL,image,&posBord);

    posBord.y -= obj->sizeText+6;
    SDL_FillRect(bord2,NULL,SDL_MapRGB(image->format,0,0,0));
    SDL_BlitSurface(bord2,NULL,image,&posBord);
    SDL_FillRect(bord2,NULL,SDL_MapRGB(image->format,235,235,235));
    posBord.x += obj->pos.w;
    SDL_BlitSurface(bord2,NULL,image,&posBord);

    SDL_FreeSurface(txt);
    SDL_FreeSurface(bord);
    SDL_FreeSurface(bord2);
    SDL_FreeSurface(mur);
}

#endif // MAIN_H_INCLUDED
