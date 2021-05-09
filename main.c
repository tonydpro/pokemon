#include "main.h"

#define FICHIER_OBJ		"tileset.xml"

typedef struct objet objet;
struct objet
{
	char nom[256],son[256];
	int tileset,
		posx,
		posy,
		largeur,
		hauteur,
		traversable,
		niveau,
		porte,
		numero,
		son_active;
};

typedef struct niveau niveau;
struct niveau
{
	Uint8 *map_x,
		  *map_y,
		  *n,
		  *t;
};

typedef struct element element;
struct element
{
	objet obj;
	int posx,posy,dx,dy;
};

typedef struct map map;
struct map
{
	element* e;
	int n_objet,taille;
};

map init_map(int n)
{
	map m;
	m.e = s_malloc((sizeof(element)*(n+128)));
	m.n_objet = 0;
	m.taille = n + 128;
	return m;
}

void free_map(map m)
{
	free(m.e);
}

void ajouter_objet_map(map* m,objet nouvel_obj,int posx,int dx,int posy,int dy)
{
	if (m->n_objet > m->taille)
	{
		m->e = s_realloc(m->e,m->taille+128);
		m->n_objet += 128;
	}

	m->e[m->n_objet].obj = nouvel_obj;

	if ((posx != -1) && (posy != -1) && (dx != -1) && (dy != -1))

	{
		m->e[m->n_objet].posx = posx;
		m->e[m->n_objet].posy = posy;
		m->e[m->n_objet].dx = dx;
		m->e[m->n_objet].dy = dy;
	}

	m->n_objet++;
}

void ajouter_objet_map2(map* m,objet nouvel_obj,int posx,int dx,int posy,int dy,niveau* niv)
{
	if (m->n_objet > m->taille)
	{
		m->e = s_realloc(m->e,m->taille+128);
		m->n_objet += 128;
	}

	m->e[m->n_objet].obj = nouvel_obj;

	if ((posx != -1) && (posy != -1) && (dx != -1) && (dy != -1))
	{
		m->e[m->n_objet].posx = posx;
		m->e[m->n_objet].posy = posy;
		m->e[m->n_objet].dx = dx;
		m->e[m->n_objet].dy = dy;
	}

	m->n_objet++;

	int lvl = nouvel_obj.niveau;

	int x,y;

	for (y = 0 ; y < nouvel_obj.hauteur ; y++)
	{
		for (x = 0 ; x < nouvel_obj.largeur ; x++)
		{
			int c = 256*(posy + y) + posx + x;

			if (nouvel_obj.traversable == 0)
				niv[lvl].t[c] = 0;

			niv[lvl].n[c] = nouvel_obj.numero;

			if ((lvl == 2) && (niv[lvl].map_x[c] != 0) && (niv[lvl].map_y[c] != 0))
			{
				niv[lvl+1].map_x[c] = nouvel_obj.posx + x;
				niv[lvl+1].map_y[c] = nouvel_obj.posy + y;
			}
			else
			{
				niv[lvl].map_x[c] = nouvel_obj.posx + x;
				niv[lvl].map_y[c] = nouvel_obj.posy + y;
			}
		}					
	}
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

		char son[256] = {0};

		lu = fscanf(fichier_tmp2,"objet><nom>%[^\n^<]</nom><tileset>%d</tileset><posx>%d</posx><posy>%d</posy><largeur>%d</largeur><hauteur>%d</hauteur><traversable>%d</traversable><niveau>%d</niveau><porte>%x</porte><numero>%d</numero><son>%[^<]</son><son_active>%d</son_active></objet>",
							obj[i].nom,&obj[i].tileset,
							&obj[i].posx,&obj[i].posy,
							&obj[i].largeur,&obj[i].hauteur,
							&obj[i].traversable,&obj[i].niveau,
							&obj[i].porte,&obj[i].numero,obj[i].son,&obj[i].son_active);

		i++;
	}while((lu == 12) && (i < nombre_objets));

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

map editer_map(SDL_Surface* images[NB_IMG],const char* nom_fichier)
{
	int nombre_objets = 0;

	objet* obj = charger_objets(nom_fichier,&nombre_objets);

	niveau niv[4] = {init_niveau(),init_niveau(),init_niveau(),init_niveau()};

	map m = init_map(0);

	FILE* fmap = s_fopen("map","r");

	do
	{
		char nom[256],chaine_vide[100];
		int numero,posx,posy,nombre_x,nombre_y,type = fgetc(fmap);

		if (type =='-')
		{
			fscanf(fmap,"%d : x = %d -> %d ; y = %d -> %d\n",&numero,&posx,&nombre_x,&posy,&nombre_y);
			numero = trouver_objet_par_numero(obj,numero,nombre_objets);
			ajouter_objet_map(&m,obj[numero],posx,nombre_x,posy,nombre_y);
		}
		else if (type == '<')
		{
			fscanf(fmap,"%[^\n^>]> : x = %d -> %d ; y = %d -> %d\n",nom,&posx,&nombre_x,&posy,&nombre_y);
			numero = trouver_objet_par_nom(obj,nom,nombre_objets);
			ajouter_objet_map(&m,obj[numero],posx,nombre_x,posy,nombre_y);
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

								if (obj[numero].traversable == 0)
									niv[lvl].t[c] = 0;

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

	return m;
}

int n_objet = 0;
SDL_Rect pos = {0,0,128,128};

void rafraichir_map(SDL_Surface* images[NB_IMG],niveau niv[4],objet* obj,int Xm,int Ym,int decalage_x,int decalage_y)
{
	int i,j,lvl,X = Xm/8, Y = Ym/8;

	for (lvl = 0 ; lvl < 4 ; lvl++)
	{
		if (LVL[lvl] == 0)
			continue;

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
				else if (lvl == 0)
				{
					SDL_Rect source  = {2*(8*i-(Xm-56)) + decalage_x,2*(8*j-(Ym-48)) + decalage_y,16,16};

					SDL_BlitSurface(images[4],NULL,ecran0,&source);
				}
			}
		}
	}

	if (n_objet)
	{
		SDL_Rect source = {obj[n_objet].posx * 16,obj[n_objet].posy * 16,obj[n_objet].largeur * 16,obj[n_objet].hauteur * 16};

		SDL_BlitSurface(images[1],&source,ecran0,&pos);
	}

	afficher_niveaux(ecran0);

	agrandir_image(ecran0,ecran,256,192);
}

void rafraichir(SDL_Surface* images[NB_IMG],niveau niv[4],objet* obj,int Xm,int Ym)
{
	rafraichir_map(images,niv,obj,Xm,Ym,0,0);
	SDL_Flip(ecran);
}

int parametres(SDL_Surface* images[NB_IMG],objet* obj,int nombre_objets,int* n_objet)
{
	int i,j;

	for (i = 0 ; i < LARGEUR; i++)
		for (j = 0 ; j < HAUTEUR ; j++)
			definirPixel(ecran,i,j,0x999999);

	SDL_Event event;
	SDL_Rect pos_numero = {32,32,200,0},pos_btn_numero = {260,34,70,70},
			 pos_nom    = {32,80,200,0},pos_btn_nom    = {265,80,70,70};

	SDL_InputBox* numero = SDL_CreateInputBox(pos_numero,BLANC,BLEU,12,30,50,"pkmndp.ttf",2,0);

	SDL_Button* btn_numero = SDL_CreateButton(pos_btn_numero,"Numero","pkmndp.ttf",18,BLANC,NOIR);

	SDL_InputBox* nom = SDL_CreateInputBox(pos_nom,BLANC,BLEU,12,30,50,"pkmndp.ttf",2,0);

	SDL_Button* btn_nom = SDL_CreateButton(pos_btn_nom,"Nom","pkmndp.ttf",18,BLANC,NOIR);

	afficher_console(ecran,"pkmndp.ttf",16,"Nom ou numero de l'objet");

	*n_objet = 0;

	debut:

	int suite = 1,n2;

	do
	{
		SDL_WaitEvent(&event);

		if (event.type == SDL_QUIT)
			suite = -1;
	
		SDL_GetInputBoxState(numero,event);
		SDL_RefreshInputBox(numero,ecran);
		SDL_GetButtonState(btn_numero,event);
		SDL_RefreshButton(btn_numero,ecran);
		SDL_GetInputBoxState(nom,event);
		SDL_RefreshInputBox(nom,ecran);
		SDL_GetButtonState(btn_nom,event);
		SDL_RefreshButton(btn_nom,ecran);

		SDL_Flip(ecran);

		SDL_Delay(1);

		if (btn_numero->state) suite = -2;
		if (btn_nom->state) suite = -3;

	}while(suite > 0);


	if (suite == -1)	
		goto fin;
	else if (suite == -2)
	{
		afficher_console(ecran,"pkmndp.ttf",16,numero->buffer);
		
		int lu,n;
		if ((lu = sscanf(numero->buffer,"%d",&n)) == 0)
		{
			afficher_console(ecran,"pkmndp.ttf",16,"Format incorrect");
			goto debut;
		}

		n2 = trouver_objet_par_numero(obj,n,nombre_objets);

		SDL_Rect source  = {obj[n2].posx * 16,obj[n2].posy * 16,obj[n2].largeur * 16,obj[n2].hauteur * 16},
							 	 source2 = {16,128,128,128};

		SDL_BlitSurface(images[1],&source,ecran,&source2);
		afficher_console(ecran,"pkmndp.ttf",16,"Ajouter cet objet ? (o / n)");

	}
	else if (suite == -3)
	{
		afficher_console(ecran,"pkmndp.ttf",16,nom->buffer);
		
		n2 = trouver_objet_par_nom(obj,nom->buffer,nombre_objets);

		if (n2 == nombre_objets)
		{
			afficher_console(ecran,"pkmndp.ttf",16,"Objet non trouve");
			goto debut;
		}

		SDL_Rect source  = {obj[n2].posx * 16,obj[n2].posy * 16,obj[n2].largeur * 16,obj[n2].hauteur * 16},
							 	 source2 = {16,128,128,128};

		SDL_BlitSurface(images[1],&source,ecran,&source2);
		afficher_console(ecran,"pkmndp.ttf",16,"Ajouter cet objet ? (o / n)");
	}

	if (pause2('o','n') == 1)
	{
		*n_objet = n2;
		goto fin;
	}
	else
	{
		afficher_console(ecran,"pkmndp.ttf",16,"Nom ou numero de l'objet");
		goto debut;
	}

	fin:

	SDL_FreeInputBox(numero);
	SDL_FreeButton(btn_numero);

	return suite;
}

void save_map(map* m,const char* fichier)
{
	int i;
	FILE* f = s_fopen(fichier,"w");

	for (i = 0 ; i < m->n_objet ; i++)
			fprintf(f,"<%s> 				: x = %d -> %d ; y = %d -> %d\n",m->e[i].obj.nom,m->e[i].posx,m->e[i].dx,m->e[i].posy,m->e[i].dy);

	fclose(f);
}

void afficher_map(SDL_Surface* images[NB_IMG],const char* nom_fichier,Mix_Music** musiques,const char* nom_fichier_map,map* m)
{
	int nombre_objets = 0;

	objet* obj = charger_objets(nom_fichier,&nombre_objets);

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

	int continuer = 1,c,Xm = 1024,Ym = 1024,V,W;

    SDL_Event event;

    while (continuer)
    {
		V = Xm;
		W = Ym;

        SDL_WaitEvent(&event);

		if ((event.type == SDL_QUIT) || ((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_ESCAPE)))
			continuer = 0;

		pos.x = 16 * (event.motion.x / 16) / AGRANDISSEMENT;
		pos.y = 16 * (event.motion.y / 16) / AGRANDISSEMENT;

		if (event.type == SDL_KEYDOWN)
		{
			switch(event.key.keysym.sym)
			{
				case SDLK_DOWN:
					Ym += 16;
					break;
				case SDLK_UP:
					Ym -= 16;
					break;
				case SDLK_LEFT:
					Xm -= 16;
					break;
				case SDLK_RIGHT:
					Xm += 16;
					break;
				case SDLK_0:
				case SDLK_KP0:
					LVL[0] = 1 - LVL[0];
					rafraichir(images,niv,obj,Xm,Ym);
					break;
				case SDLK_1:
				case SDLK_KP1:
					LVL[1] = 1 - LVL[1];
					rafraichir(images,niv,obj,Xm,Ym);
					break;
				case SDLK_2:
				case SDLK_KP2:
					LVL[2] = 1 - LVL[2];
					rafraichir(images,niv,obj,Xm,Ym);
					break;
				case SDLK_3:
				case SDLK_KP3:
					LVL[3] = 1 - LVL[3];
					rafraichir(images,niv,obj,Xm,Ym);
					break;
				case SDLK_TAB:
					if (parametres(images,obj,nombre_objets,&n_objet) == -1)
						continuer = 0;
					else
						rafraichir(images,niv,obj,Xm,Ym);
					break;
				case SDLK_p:
					if (n_objet < nombre_objets)
					{
						n_objet++;
						rafraichir(images,niv,obj,Xm,Ym);
					}
					break;
				case SDLK_m:
					if (n_objet > 0)
					{
						n_objet--;
						rafraichir(images,niv,obj,Xm,Ym);
					}
					break;
			}

			if ((V != Xm) || (W != Ym))
			{
				rafraichir_map(images,niv,obj,Xm,Ym,0,0);
				SDL_Flip(ecran);
			}
		}
		else if (n_objet && (event.type == SDL_MOUSEMOTION))
		{
			rafraichir_map(images,niv,obj,Xm,Ym,0,0);
			SDL_Flip(ecran);
		}
		else if (n_objet && (event.type == SDL_MOUSEBUTTONUP))
		{
			ajouter_objet_map2(m,obj[n_objet],(Xm / 8) + (event.motion.x / (16*AGRANDISSEMENT)) - 7,1,(Ym / 8) + (event.motion.y / (16*AGRANDISSEMENT)) - 6,1,niv);
			n_objet = 0;
		}

		SDL_Delay(1);
    }

	save_map(m,"map");

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

    SDL_Surface* images[NB_IMG] = {NULL,NULL,NULL,NULL};
	Mix_Music*   musiques[NB_M] = {NULL};

	images[0] = init(LARGEUR,HAUTEUR,"Pokemon");
    images[1] = s_IMG_Load("tileset1.png");
	//images[2] = s_IMG_Load("curseur.png");
	images[3] = SDL_CreateRGBSurface(0,256,192,32,0,0,0,0);
	images[4] = SDL_CreateRGBSurface(0,16,16,32,0,0,0,0);

	map m = editer_map(images,FICHIER_OBJ);

	afficher_map(images,FICHIER_OBJ,musiques,"map",&m);

	free_map(m);
    cleanup(images,musiques);
    SDL_Quit();
    return EXIT_SUCCESS;
}
