#include "main.h"

#define FICHIER_OBJ		"tileset.xml"

typedef struct objet objet;
struct objet
{
	char nom[256];
	int tileset,
		posx,
		posy,
		largeur,
		hauteur,
		niveau,
		numero;
};

typedef struct niveau niveau;
struct niveau
{
	Uint8 *map_x,
		  *map_y,
		  *n,
		  *t;
};

typedef struct personnage personnage;
struct personnage
{
	SDL_Rect source,source2,source2_copie;
	int x,y,sens;
	double pas;
};

personnage init_personnage(int source2_x,int source2_y)
{
	personnage perso;

	memset(&perso,0,sizeof(personnage));

	perso.source2.x = source2_x;
	perso.source2.y = source2_y;
	perso.source2.w = 32;
	perso.source2.h = 32;

	memcpy(&perso.source2_copie,&perso.source2,sizeof(SDL_Rect));

	return perso;
}

void ajouter_pas_personnage(personnage* perso)
{
	perso->pas += 0.2;

	if ((int)perso->pas == 3)
		perso->pas = 0;
}

void afficher_personnage(SDL_Surface* images[NB_IMG],personnage* perso,int source_x,int source_y)
{
	int x,y,pas = (int)perso->pas;

	if (perso->sens == 0)
	{
		x = 2;
		y = 1 + pas;
	}
	else if (perso-> sens == 1)
	{
		x = 1;
		y = pas;
	}
	else if (perso->sens == 2)
	{
		x = 0;
		if (pas == 0)
			y = 2;
		else if (pas == 1)
			y = 1;
		else if (pas == 2)
			y = 3;
	}
	else if (perso->sens == 3)
	{
		if (pas == 0)
		{
			x = 0; y = 0;
		}
		else if (pas == 1)
		{
			x = 1; y = 3;
		}
		else if (pas == 2)
		{
			x = 2; y = 0;
		}
	}

	perso->source.x = source_x;
	perso->source.y = source_y;

	perso->source2.x = perso->source2_copie.x + 32*x;
	perso->source2.y = perso->source2_copie.y + 32*y;

	SDL_BlitSurface(images[2],&perso->source2,ecran0,&perso->source);
}

niveau init_niveau(void)
{
	niveau niv;

	niv.map_x = (Uint8*)s_malloc(65536);
	niv.map_y = (Uint8*)s_malloc(65536);
	niv.n = (Uint8*)s_malloc(65536);
	niv.t = (Uint8*)s_malloc(65536);

	memset(niv.map_x,0,65536);
	memset(niv.map_y,0,65536);
	memset(niv.n,0,65536);
	memset(niv.t,1,65536);

	return niv;
}

void save_niveau(niveau niv,const char* nom_fichier_map)
{
	FILE* fmap_pkmn = s_fopen(nom_fichier_map,"wb");

	fwrite(niv.map_x,512,128,fmap_pkmn);
	fwrite(niv.map_y,512,128,fmap_pkmn);
	fwrite(niv.n,512,128,fmap_pkmn);
	fwrite(niv.t,512,128,fmap_pkmn);

	fclose(fmap_pkmn);
}

void charger_niveau(niveau* niv,const char* nom_fichier_map)
{
	FILE* fmap_pkmn = s_fopen(nom_fichier_map,"rb");

	fread(niv->map_x,512,128,fmap_pkmn);
	fread(niv->map_y,512,128,fmap_pkmn);
	fread(niv->n,512,128,fmap_pkmn);
	fread(niv->t,512,128,fmap_pkmn);

	fclose(fmap_pkmn);
}

void free_niveau(niveau niv)
{
	free(niv.map_x);
	free(niv.map_y);
	free(niv.n);
	free(niv.t);
}

void charger_nombre_objets(int* nombre_objets_ptr)
{
	FILE* fichier_nombre_objets = s_fopen("nombre_objets","r");

	fscanf(fichier_nombre_objets,"%d",nombre_objets_ptr);

	fclose(fichier_nombre_objets);
}

objet* charger_objets(const char* nom_fichier,int* nombre_objets_ptr)
{
	size_t s = sizeofFile(nom_fichier);
	char* tileset = (char*)s_malloc(s+1);
	FILE* fichier = s_fopen(nom_fichier,"rb"),*fichier_tmp = s_fopen("map_tmp","wb");

	fread(tileset,512,s/512,fichier);
	fread(tileset+s-(s%512),s%512,1,fichier);

	fclose(fichier);

	int i,nombre_objets = 0;

	charger_nombre_objets(nombre_objets_ptr);
	nombre_objets = *nombre_objets_ptr;

	for (i = 0 ; i < s ; i++)
	{
		if ((tileset[i] != '\n') && (tileset[i] != '\r') && (tileset[i] != '\t'))
			fwrite(tileset+i,1,1,fichier_tmp);
	}

	fclose(fichier_tmp);

	objet* obj = (objet*)s_malloc(nombre_objets*sizeof(objet));

	FILE* fichier_tmp2 = s_fopen("map_tmp","r");

	i = 0;
	int lu = 0,f = 0;

	do
	{
		f = fgetc(fichier_tmp2);

		if ((f == EOF) || (f != '<'))
			break;

		lu = fscanf(fichier_tmp2,"objet><nom>%[^\n^<]</nom><tileset>%d</tileset><posx>%d</posx><posy>%d</posy><largeur>%d</largeur><hauteur>%d</hauteur><niveau>%d</niveau><numero>%d</numero></objet>",
							obj[i].nom,&obj[i].tileset,
							&obj[i].posx,&obj[i].posy,
							&obj[i].largeur,&obj[i].hauteur,
							&obj[i].niveau,&obj[i].numero);

		i++;
	}while((lu == 8) && (i < nombre_objets));

	fclose(fichier_tmp2);

	free(tileset);

	return obj;
}

int trouver_objet_par_numero(objet* obj,int numero,int nombre_objets)
{
	int o;
	for (o = 0 ; o < nombre_objets ; o++)
	{
		if (obj[o].numero == numero)
				break;
	}

	return o;
}

int trouver_objet_par_nom(objet* obj,char nom[256],int nombre_objets)
{
	int o;
	for (o = 0 ; o < nombre_objets ; o++)
	{
		if (!strcmp(obj[o].nom,nom))
				break;
	}

	return o;
}

int traversable(SDL_Surface* images[NB_IMG],niveau* niv,objet* obj,int nombre_objets,int x,int y)
{
	int lvl,p,r = 0;

	for (lvl = 0 ; lvl < 4 ; lvl++)
	{
		int n = trouver_objet_par_numero(obj,niv[lvl].n[256*y + x],nombre_objets);

		if (obj[n].posx || obj[n].posy)
		{
			p = getpixel(images[3],obj[n].posx,obj[n].posy);

			printf("%d %d %x\n",obj[n].posx,obj[n].posy,p);

			if (p == 0xa0a0a0)
				return 1;

			/*if (p == 0x0000ff)
				return 1;*/
		}
	}

	printf("\n");

	return 1;
}

void editer_map(SDL_Surface* images[NB_IMG],const char* nom_fichier)
{
	int nombre_objets = 0;

	objet* obj = charger_objets(nom_fichier,&nombre_objets);

	niveau niv[4] = {init_niveau(),init_niveau(),init_niveau(),init_niveau()};

	FILE* fmap = s_fopen("map","r");

	do
	{
		char nom[256],chaine_vide[100];
		int numero,posx,posy,nombre_x,nombre_y,type = fgetc(fmap);

		if (type =='-')
		{
						fscanf(fmap,"%d : x = %d -> %d ; y = %d -> %d\n",&numero,&posx,&nombre_x,&posy,&nombre_y);
			numero = trouver_objet_par_numero(obj,numero,nombre_objets);

		}
		else if (type == '<')
		{
			fscanf(fmap,"%[^\n^>]> : x = %d -> %d ; y = %d -> %d\n",nom,&posx,&nombre_x,&posy,&nombre_y);
			numero = trouver_objet_par_nom(obj,nom,nombre_objets);
		}
		else
		{
			fgets(chaine_vide,100,fmap);

			if (strlen(chaine_vide) > 1)
				fgetc(fmap);
		}

		int x,y,r,r2,lvl;

		for (lvl = 0 ; lvl < 4 ; lvl++)
		{
			if (obj[numero].niveau == lvl)
			{
				for (r2 = 0 ; r2 < nombre_y ; r2++)
				{
					for (r = 0 ; r < nombre_x ; r++)
					{
						for (y = 0 ; y < obj[numero].hauteur ; y++)
						{
							for (x = 0 ; x < obj[numero].largeur ; x++)
							{
								int c = 256*(posy + y + r2*obj[numero].hauteur) + posx + x + r*obj[numero].largeur;

								niv[lvl].n[c] = obj[numero].numero;

								if ((lvl == 2) && (niv[lvl].map_x[c] != 0) && (niv[lvl].map_y[c] != 0))
								{
									niv[lvl+1].map_x[c] = obj[numero].posx + x;
									niv[lvl+1].map_y[c] = obj[numero].posy + y;
								}
								else
								{
									niv[lvl].map_x[c] = obj[numero].posx + x;
									niv[lvl].map_y[c] = obj[numero].posy + y;
								}
							}					
						}
					}
				}
			}
		}

	}while(!feof(fmap));

	save_niveau(niv[0],"map0.pkmn");
	save_niveau(niv[1],"map1.pkmn");
	save_niveau(niv[2],"map2.pkmn");
	save_niveau(niv[3],"map3.pkmn");

	fclose(fmap);

	free(obj);
	free_niveau(niv[0]);
	free_niveau(niv[1]);
	free_niveau(niv[2]);
	free_niveau(niv[3]);
}

void rafraichir_map(SDL_Surface* images[NB_IMG],niveau niv[4],objet* obj,personnage perso,int Xm,int Ym,int decalage_x,int decalage_y)
{
		int i,j,lvl,X = Xm/8,Y = Ym/8;

		for (lvl = 0 ; lvl < 4 ; lvl++)
		{
			if (lvl == 3)
				afficher_personnage(images,&perso,122,64);

			for (j = Y - 6 ; j < Y + 7 ; j++)
			{
				for (i = X - 7 ; i < X + 10 ; i++)
				{
					int x = niv[lvl].map_x[(256*j) + i],y = niv[lvl].map_y[(256*j) + i];
					
					if ((x != 0) || (y != 0))	
					{
						SDL_Rect source  = {2*(8*i-(Xm-56)) + decalage_x,2*(8*j-(Ym-48)) + decalage_y,16,16},
							 	 source2 = {x*16,y*16,16,16};

						SDL_BlitSurface(images[1],&source2,ecran0,&source);
					}
				}
			}
		}

		agrandir_image(ecran0,ecran,256,192);
}

void afficher_map(SDL_Surface* images[NB_IMG],const char* nom_fichier,Mix_Music** musiques,const char* nom_fichier_map)
{
	int nombre_objets = 0;

	objet* obj = charger_objets(nom_fichier,&nombre_objets);

	int i;

	char chaine0[256],chaine1[256],chaine2[256],chaine3[256];
	sprintf(chaine0,"%s0.pkmn",nom_fichier_map);
	sprintf(chaine1,"%s1.pkmn",nom_fichier_map);
	sprintf(chaine2,"%s2.pkmn",nom_fichier_map);
	sprintf(chaine3,"%s3.pkmn",nom_fichier_map);

	niveau niv[4] = {init_niveau(),init_niveau(),init_niveau(),init_niveau()};

	charger_niveau(&niv[0],chaine0);
	charger_niveau(&niv[1],chaine1);
	charger_niveau(&niv[2],chaine2);
	charger_niveau(&niv[3],chaine3);

	int continuer = 1,c,Xm = 1024,Ym = 1024,X,Y,V,W,sens = 0,inactif = 0;
    SDL_Event event;

	personnage perso = init_personnage(3*32*4,4*32*1);

    while (continuer)
    {
		V = Xm; W = Ym;
		X = Xm / 8;
		Y = (Ym + 1) / 8;

		inactif = 0;

		sens = perso.sens;

        SDL_PollEvent(&event);

		if ((event.type == SDL_QUIT) || ((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_RETURN)))
			continuer = 0;

		if (event.type == SDL_KEYDOWN)
		{
			switch(event.key.keysym.sym)
			{
				case SDLK_UP:
					perso.sens = 3;
					if ((sens == perso.sens)  && (Xm < 2047) && (traversable(images,niv,obj,nombre_objets,X,Y-1)))
					{
						int y;
						for (y = Ym ; y > Ym - 8 ; y--)
						{
							ajouter_pas_personnage(&perso);	
							rafraichir_map(images,niv,obj,perso,Xm,y,0,0);
							SDL_Flip(ecran);
						}
						Ym -= 8;
					}
					break;
				case SDLK_DOWN:
					perso.sens = 0;
					if ((sens == perso.sens)  && (Xm < 2047) && (traversable(images,niv,obj,nombre_objets,X,Y+1)))
					{
						int y;
						for (y = Ym ; y < Ym + 8 ; y++)
						{
							ajouter_pas_personnage(&perso);					
							rafraichir_map(images,niv,obj,perso,Xm,y,0,0);
							SDL_Flip(ecran);
						}
						Ym += 8;
					}
					break;
				case SDLK_RIGHT:
					perso.sens = 1;
					if ((sens == perso.sens)  && (Xm < 2047) && (traversable(images,niv,obj,nombre_objets,X-1,Y)))
					{
						int x;
						for (x = Xm ; x < Xm + 8 ; x++)
						{
							ajouter_pas_personnage(&perso);
							rafraichir_map(images,niv,obj,perso,x,Ym,0,0);
							SDL_Flip(ecran);
						}		
						Xm += 8;
					}
					break;
				case SDLK_LEFT:
					perso.sens = 2;
					if ((sens == perso.sens)  && (Xm < 2047) && (traversable(images,niv,obj,nombre_objets,X+1,Y)))
					{
						int x;
						for (x = Xm ; x > Xm - 8 ; x--)
						{
							ajouter_pas_personnage(&perso);
							rafraichir_map(images,niv,obj,perso,x,Ym,0,0);
							SDL_Flip(ecran);
						}		
						Xm -= 8;
					}
					break;
			}
		}
		else if (++inactif >= 1)
		{
			perso.pas = 0;
			rafraichir_map(images,niv,obj,perso,Xm,Ym,0,0);
			SDL_Flip(ecran);
		}

		SDL_Delay(1);
    }

	free_niveau(niv[0]);
	free_niveau(niv[1]);
	free_niveau(niv[2]);
	free_niveau(niv[3]);
	free(obj);
}

int main(int argc, char** argv)
{
	freopen("/dev/tty","w",stdout);
	freopen("/dev/tty","w",stderr);

    srand(time(NULL));

    SDL_Event event;

    SDL_Surface* images[NB_IMG] = {NULL,NULL,NULL};
	Mix_Music*   musiques[NB_M] = {NULL};

	images[0] = init(LARGEUR,HAUTEUR,"Pokemon");
    images[1] = s_IMG_Load("tileset1.png");
	images[2] = s_IMG_Load("personnage_tileset.png");
	images[3] = s_IMG_Load("tileset1-calque.bmp");
	images[4] = SDL_CreateRGBSurface(0,256,192,32,0,0,0,0);

	musiques[0] = Mix_LoadMUS("10981.mp3");

	//Mix_PlayMusic(musiques[0],1);

	editer_map(images,FICHIER_OBJ);

	afficher_map(images,FICHIER_OBJ,musiques,"map");


    cleanup(images,musiques);
    SDL_Quit();
    return EXIT_SUCCESS;
}
