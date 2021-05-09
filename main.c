#include "main.h"

#define FICHIER_OBJ		"tileset.xml"

const gchar* chemin;

void recuperer_chemin(GtkWidget *bouton,GtkWidget *file_selection);
void creer_file_selection(void);

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

void charger_nombre_objets(int* nombre_objets_ptr)
{
	FILE* fichier_nombre_objets = s_fopen("nombre_objets","r");

	fscanf(fichier_nombre_objets,"%d",nombre_objets_ptr);

	fclose(fichier_nombre_objets);
}

void charger_nombre_objets2(char* f,int* nombre_objets_ptr)
{
	FILE* fichier_nombre_objets = s_fopen(f,"r");

	fscanf(fichier_nombre_objets,"%d",nombre_objets_ptr);

	fclose(fichier_nombre_objets);
}

void ajouter_nombre_objets(void)
{
	int nombre_objets;

	charger_nombre_objets(&nombre_objets);

	FILE* fichier_nombre_objets = s_fopen("nombre_objets","w");

	fprintf(fichier_nombre_objets,"%d",nombre_objets+1);

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

int trouver_objet_par_coordonnees(objet* obj,int nombre_objets,int x,int y)
{
	int o;
	for (o = 0 ; o < nombre_objets ; o++)
	{
		if (((x >= obj[o].posx) && (x < obj[o].posx + obj[o].largeur)) && ((y >= obj[o].posy) && (y < obj[o].posy + obj[o].hauteur)))
			break;
	}

	return o;
}

int trouver_objet_par_coordonnees2(objet* obj,int nombre_objets,int x,int y)
{
	int o;
	for (o = 0 ; o < nombre_objets ; o++)
	{
		if ((x == obj[o].posx) && (y == obj[o].posy))
			break;
	}

	return o;
}

typedef struct groupe groupe;
struct groupe
{
	int son_active,objets[1900],n;
};

groupe* charger_groupes(int* nombre_groupes)
{
	FILE* f = s_fopen("groupes","r");

	fscanf(f,"%d",nombre_groupes);

	groupe* gr = s_malloc(*nombre_groupes*sizeof(groupe));

	int i,j;

	for (i = 0 ; i < *nombre_groupes ; i++)
	{
		fscanf(f,"%d %d",&gr[i].son_active,&gr[i].n);

		for (j = 0 ; j < gr[i].n ; j++)
			fscanf(f,"%d",&gr[i].objets[j]);

		fscanf(f,"\n");
	}

	fclose(f);

	return gr;
}

void sauvegarder_groupes(groupe* gr,int nombre_groupes)
{
	FILE* f = s_fopen("groupes","w");

	fprintf(f,"%d\n",nombre_groupes);

	int i,j;

	for (i = 0 ; i < nombre_groupes ; i++)
	{
		fprintf(f,"%d	%d	",gr[i].son_active,gr[i].n);

		for (j = 0 ; j < gr[i].n - 1 ; j++)
			fprintf(f,"%d	",gr[i].objets[j]);

		fprintf(f,"%d\n",gr[i].objets[gr[i].n - 1]);
	}

	fclose(f);
}

int recup = 0;
char fichier1[1024],fichier2[1024];

void btn_fichier1(void)
{
	recup = 1;
	creer_file_selection();	
}

void btn_fichier2(void)
{
	recup = 2;
	creer_file_selection();
}

void btn_fichier3(void)
{
	recup = 3;
	creer_file_selection();
}

void btn_ajouter(void)
{
	char fichier_nombre_objets1[1024],fichier_nombre_objets2[1024];
	memset(fichier_nombre_objets1,0,1024);
	memset(fichier_nombre_objets2,0,1024);

	int i,s1 = strlen(fichier1),s2 = strlen(fichier2);
	for (i = s1 ; i >= 0 ; i--)
	{
		if ((fichier1[i] == '/') || (fichier1[i] == '\\'))
			break;
	}

	memcpy(fichier_nombre_objets1,fichier1,i+1);
	memcpy(fichier_nombre_objets1+i+1,"nombre_objets",strlen("nombre_objets"));

	for (i = s2 ; i >= 0 ; i--)
	{
		if ((fichier2[i] == '/') || (fichier2[i] == '\\'))
			break;
	}

	memcpy(fichier_nombre_objets2,fichier2,i+1);
	memcpy(fichier_nombre_objets2+i+1,"nombre_objets",strlen("nombre_objets"));

	FILE *f = s_fopen("nouveau_tileset.xml","w");

	int nombre_objets1,nombre_objets2;

	charger_nombre_objets2(fichier_nombre_objets1,&nombre_objets1);
	charger_nombre_objets2(fichier_nombre_objets2,&nombre_objets2);

	objet* obj1,*obj2;

	obj1 = charger_objets(fichier1,&nombre_objets1);
	obj2 = charger_objets(fichier2,&nombre_objets2);

	for (i = 0 ; i < min(nombre_objets1,nombre_objets2) ; i++)
	{
		int n1 = trouver_objet_par_numero(obj1,i,nombre_objets1),
			n2 = trouver_objet_par_numero(obj2,i,nombre_objets2);

		if ((n1 == nombre_objets1) && (n2 != nombre_objets2))
			fprintf(f,"<objet>\n\t<nom>%s</nom>\n\t<tileset>1</tileset>\n\t<posx>%d</posx>\n\t<posy>%d</posy>\n\t<largeur>%d</largeur>\n\t<hauteur>%d</hauteur>\n\t<traversable>%d</traversable>\n\t<niveau>%d</niveau>\n\t<porte>0</porte>\n\t<numero>%d</numero>\n\t<son>%s</son>\n\t<son_active>%d</son_active>\n</objet>\n\n",obj2[n2].nom,obj2[n2].posx,obj2[n2].posy,obj2[n2].largeur,obj2[n2].hauteur,obj2[n2].traversable,obj2[n2].niveau,obj2[n2].numero,obj2[n2].son,obj2[n2].son_active);
		else if (n2 != nombre_objets2)
			fprintf(f,"<objet>\n\t<nom>%s</nom>\n\t<tileset>1</tileset>\n\t<posx>%d</posx>\n\t<posy>%d</posy>\n\t<largeur>%d</largeur>\n\t<hauteur>%d</hauteur>\n\t<traversable>%d</traversable>\n\t<niveau>%d</niveau>\n\t<porte>0</porte>\n\t<numero>%d</numero>\n\t<son>%s</son>\n\t<son_active>%d</son_active>\n</objet>\n\n",obj2[n2].nom,obj2[n2].posx,obj2[n2].posy,obj2[n2].largeur,obj2[n2].hauteur,obj2[n2].traversable,obj2[n2].niveau,obj2[n2].numero,obj1[n1].son,obj1[n1].son_active);

	}

	free(obj1);
	free(obj2);
	fclose(f);
}

void fichiers(int argc,char** argv)
{
    GtkWidget* pWindow0;
	GtkWidget *btn,*btn2,*btn3;
	GtkWidget *box;
 
    gtk_init(&argc,&argv);
 
    pWindow0 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(pWindow0),5);
    gtk_window_set_title(GTK_WINDOW(pWindow0),"Fichiers");
    gtk_window_set_default_size(GTK_WINDOW(pWindow0),160*AGRANDISSEMENT,100*AGRANDISSEMENT);
 
	btn = gtk_button_new_with_label("Fichier son");
	btn2 = gtk_button_new_with_label("Fichier");
	btn3 = gtk_button_new_with_label("Ajouter");

	box = gtk_vbox_new(TRUE,0);

	gtk_box_pack_start(GTK_BOX(box),btn,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),btn2,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(box),btn3,TRUE,TRUE,0);

    gtk_container_add(GTK_CONTAINER(pWindow0),box);
 
    gtk_widget_show_all(pWindow0);
 
    g_signal_connect(G_OBJECT(pWindow0),"destroy",G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(G_OBJECT(btn),"clicked",G_CALLBACK(btn_fichier1),NULL);
	g_signal_connect(G_OBJECT(btn2),"clicked",G_CALLBACK(btn_fichier2),NULL);
	g_signal_connect(G_OBJECT(btn3),"clicked",G_CALLBACK(btn_ajouter),NULL);

    gtk_main();

	recup = 0;
}

int nombre_groupes;
groupe* gr;

void sauvegarder_gr(GtkWidget* pButton,gpointer data)
{
	GtkWidget** pScale = data;

	int i;

	for (i = 0 ; i < nombre_groupes ; i++)
		gr[i].son_active = gtk_range_get_value(GTK_RANGE(pScale[i]));

	sauvegarder_groupes(gr,nombre_groupes);
}

void sons(int argc,char** argv)
{
    GtkWidget *pWindow0;
	GtkWidget *btn;
	GtkWidget *box;
	GtkWidget *pFrame[N_GROUPE];
	GtkWidget *pScale[N_GROUPE];
	GtkWidget *scrollbar0;
 
    gtk_init(&argc,&argv);
 
    pWindow0 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(pWindow0),5);
    gtk_window_set_title(GTK_WINDOW(pWindow0),"Console");
    gtk_window_set_default_size(GTK_WINDOW(pWindow0),320*AGRANDISSEMENT,200*AGRANDISSEMENT);
 
	scrollbar0 = gtk_scrolled_window_new(NULL,NULL);
    gtk_container_add(GTK_CONTAINER(pWindow0),scrollbar0);

	btn = gtk_button_new_with_label("Modifier");

	box = gtk_vbox_new(TRUE,0);

    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrollbar0),box);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollbar0),GTK_POLICY_NEVER,GTK_POLICY_ALWAYS);

	gtk_box_pack_start(GTK_BOX(box),btn,TRUE,TRUE,0);
 
	int i;

	for (i = 0 ; i < N_GROUPE ; i++)
	{
		char chaine[256];
		sprintf(chaine,"Groupe %d",i);
		pFrame[i] = gtk_frame_new(chaine);
		pScale[i] = gtk_hscale_new_with_range(0,100,1);
		gtk_scale_set_value_pos(GTK_SCALE(pScale[i]),GTK_POS_BOTTOM);
		gtk_range_set_value(GTK_RANGE(pScale[i]),gr[i].son_active);
		gtk_container_add(GTK_CONTAINER(pFrame[i]),pScale[i]);
		gtk_box_pack_start(GTK_BOX(box),pFrame[i],FALSE,FALSE,0);
	}

    gtk_widget_show_all(pWindow0);
 
    g_signal_connect(G_OBJECT(pWindow0),"destroy",G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(G_OBJECT(btn),"clicked",G_CALLBACK(sauvegarder_gr),pScale);

    gtk_main();
}

typedef struct SDL_Rect2 SDL_Rect2;
struct SDL_Rect2
{
	int x,y,w,h;
};

SDL_Rect2 convertir_pos(SDL_Rect2 pos)
{
	int dx = pos.w,dy = pos.h;

	if ((dx >= 0) && (dy >= 0))
	{
		SDL_Rect2 pos2 = {pos.x,pos.y,dx,dy};
		return pos2;
	}
	else if ((dx >= 0) && (dy <= 0))
	{
		SDL_Rect2 pos2 = {pos.x,pos.y + dy + 16,abs(dx),abs(dy)};
		return pos2;
	}
	else if ((dx <= 0) && (dy >= 0))
	{
		SDL_Rect2 pos2 = {pos.x + dx + 16,pos.y,abs(dx),abs(dy)};
		return pos2;
	}
	else if ((dx <= 0) && (dy <= 0))
	{
		SDL_Rect2 pos2 = {pos.x + dx + 16,pos.y + dy + 16,abs(dx),abs(dy)};
		return pos2;
	}
	else
		return pos;
}

int n = -1,nombre_objets,px = 0,py = 0,liste[1900],l = 0,l_son_active = 0;
char fichier3[1900];
objet* obj;

void sauvegarder_liste0(GtkWidget* pButton,GtkWidget* pScale)
{
	int i,son_active = gtk_range_get_value(GTK_RANGE(pScale));

	for (i = 0 ; i < l ; i++)
		obj[liste[i]].son_active = son_active;

	FILE* f = s_fopen("tileset.xml","w");

		for (i = 0 ; i < nombre_objets ; i++)
			fprintf(f,"<objet>\n\t<nom>%s</nom>\n\t<tileset>1</tileset>\n\t<posx>%d</posx>\n\t<posy>%d</posy>\n\t<largeur>%d</largeur>\n\t<hauteur>%d</hauteur>\n\t<traversable>%d</traversable>\n\t<niveau>%d</niveau>\n\t<porte>0</porte>\n\t<numero>%d</numero>\n\t<son>%s</son>\n\t<son_active>%d</son_active>\n</objet>\n\n",obj[i].nom,obj[i].posx,obj[i].posy,obj[i].largeur,obj[i].hauteur,obj[i].traversable,obj[i].niveau,obj[i].numero,obj[i].son,obj[i].son_active);

	fclose(f);
}

void sauvegarder_liste1(void)
{
	int i;

	for (i = 0 ; i < l ; i++)
		strcpy(obj[liste[i]].son,fichier3);

	FILE* f = s_fopen("tileset.xml","w");

		for (i = 0 ; i < nombre_objets ; i++)
			fprintf(f,"<objet>\n\t<nom>%s</nom>\n\t<tileset>1</tileset>\n\t<posx>%d</posx>\n\t<posy>%d</posy>\n\t<largeur>%d</largeur>\n\t<hauteur>%d</hauteur>\n\t<traversable>%d</traversable>\n\t<niveau>%d</niveau>\n\t<porte>0</porte>\n\t<numero>%d</numero>\n\t<son>%s</son>\n\t<son_active>%d</son_active>\n</objet>\n\n",obj[i].nom,obj[i].posx,obj[i].posy,obj[i].largeur,obj[i].hauteur,obj[i].traversable,obj[i].niveau,obj[i].numero,obj[i].son,obj[i].son_active);

	fclose(f);

	recup = 0;
}

void parametres_liste(int argc,char** argv,int parametre)
{
	GtkWidget *pWindow0;
	GtkWidget *label;
	GtkWidget *btn;
	GtkWidget *box;
	GtkWidget *pFrame;
	GtkWidget *pScale;
	GtkWidget *scrollbar0;
	GtkWidget *bouton_explorer0;
 
    gtk_init(&argc,&argv);
 
    pWindow0 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(pWindow0),5);
    gtk_window_set_title(GTK_WINDOW(pWindow0),"Liste");
    gtk_window_set_default_size(GTK_WINDOW(pWindow0),320*AGRANDISSEMENT,300*AGRANDISSEMENT);
 
	scrollbar0 = gtk_scrolled_window_new(NULL,NULL);
    gtk_container_add(GTK_CONTAINER(pWindow0),scrollbar0);

	btn = gtk_button_new_with_label("Modifier");

	box = gtk_vbox_new(TRUE,0);

    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrollbar0),box);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollbar0),GTK_POLICY_NEVER,GTK_POLICY_ALWAYS);

	gtk_box_pack_start(GTK_BOX(box),btn,TRUE,TRUE,0);
 
	int i;

	for (i = 0 ; i < l ; i++)
	{
		label = gtk_label_new(obj[liste[i]].nom);
		gtk_box_pack_start(GTK_BOX(box),label,TRUE,TRUE,0);
	}

	if (parametre == 0)
	{
		pFrame = gtk_frame_new("Liste de sons");
		pScale = gtk_hscale_new_with_range(0,100,1);
		gtk_scale_set_value_pos(GTK_SCALE(pScale),GTK_POS_BOTTOM);
		gtk_container_add(GTK_CONTAINER(pFrame),pScale);
		gtk_box_pack_start(GTK_BOX(box),pFrame,FALSE,FALSE,0);
		g_signal_connect(G_OBJECT(btn),"clicked",G_CALLBACK(sauvegarder_liste0),pScale);
	}
	else
	{
		bouton_explorer0 = gtk_button_new_with_label("Fichier audio");
		gtk_box_pack_start(GTK_BOX(box),bouton_explorer0,TRUE,FALSE,0);
    	g_signal_connect(G_OBJECT(bouton_explorer0),"clicked",G_CALLBACK(creer_file_selection),NULL);
		g_signal_connect(G_OBJECT(btn),"clicked",G_CALLBACK(sauvegarder_liste1),pScale);
		recup = 3;
	}


    gtk_widget_show_all(pWindow0);
 
    g_signal_connect(G_OBJECT(pWindow0),"destroy",G_CALLBACK(gtk_main_quit),NULL);

    gtk_main();
}

int objet_liste(int n,int liste[1900],int l)
{
	int i;

	for (i = 0 ; i < l ; i++)
	{
		if (liste[i] == n)
			return 1;
	}

	return 0;
}

int selection = -2;

void afficher_filtre(SDL_Surface* images[NB_IMG],SDL_Rect source2,int filtre)
{
	int x,y,n,test = 0;

	for (x = source2.x / 16 ; x < (source2.x + 256) / 16 ; x++)
	{
		for (y = source2.y / 16 ; y < (source2.y + 192) / 16 ; y++)
		{
			if ((n = trouver_objet_par_coordonnees2(obj,nombre_objets,x,y)) != nombre_objets)
			{
				if (selection == -1)
					test = objet_liste(n,liste,l);
				else if (selection >= 0)
					test = objet_liste(n,gr[selection].objets,gr[selection].n);

				if (test)
				{
					int i,j,
						X = (obj[n].posx - (source2.x/16)),
						Y = (obj[n].posy - (source2.y/16));

					for (i = X ; i < X + obj[n].largeur ; i++)
					{
						for (j = Y ; j < Y + obj[n].hauteur ; j++)
						{
							SDL_Rect source = {16*i,16*j,16,16};
							SDL_BlitSurface(images[5],NULL,ecran0,&source);
						}
					}
				}
			}
		}
	}
}

void OnValider(GtkWidget *pBtn, gpointer data)
{
    GtkWidget *pInfo;
    GtkWidget *pWindow0;
    GSList *pList;
    const gchar *sLabel;
 
    pList = gtk_radio_button_get_group(GTK_RADIO_BUTTON(data));
 
	selection = N_GROUPE;

    while(pList)
    {
        if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pList->data)))
        {
            sLabel = gtk_button_get_label(GTK_BUTTON(pList->data));
            pList = NULL;
        }
        else
            pList = g_slist_next(pList);

		selection--;
    }
 
    pWindow0 = gtk_widget_get_toplevel(GTK_WIDGET(data));
 
    pInfo = gtk_message_dialog_new(GTK_WINDOW(pWindow0),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "Vous avez choisi : %s", sLabel);
 
    gtk_dialog_run(GTK_DIALOG(pInfo));
 
    gtk_widget_destroy(pInfo);
}

void selection_groupe(int argc,char** argv)
{
    GtkWidget *pWindow0;
    GtkWidget *pVBox0;
	GtkWidget *pRadio0;
    GtkWidget *pRadio1;
    GtkWidget *pRadio2[N_GROUPE];
	GtkWidget *pValider;
    GtkWidget *pLabel0;
 
    gtk_init(&argc,&argv);
 
    pWindow0 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(pWindow0),"Selection groupe");
    gtk_window_set_default_size(GTK_WINDOW(pWindow0),160*AGRANDISSEMENT,100*AGRANDISSEMENT);
 
    pVBox0 = gtk_vbox_new(TRUE,0);
    gtk_container_add(GTK_CONTAINER(pWindow0),pVBox0);
 
    pLabel0 = gtk_label_new("Groupe :");
    gtk_box_pack_start(GTK_BOX(pVBox0),pLabel0,FALSE,FALSE,0);
 
    pRadio0 = gtk_radio_button_new_with_label(NULL,"Aucun");
    gtk_box_pack_start(GTK_BOX(pVBox0),pRadio0,FALSE,FALSE,0);

    pRadio1 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(pRadio0),"Liste");
    gtk_box_pack_start(GTK_BOX(pVBox0),pRadio1,FALSE,FALSE,0);

	int i;

	for (i = 0 ; i < N_GROUPE ; i++)
	{
		char nom_groupe[256];

		sprintf(nom_groupe,"Groupe %d",i);

		if (i == 0)
			pRadio2[i] = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (pRadio1),nom_groupe);
    	else
			pRadio2[i] = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (pRadio2[i-1]),nom_groupe);

    	gtk_box_pack_start(GTK_BOX(pVBox0),pRadio2[i],FALSE,FALSE,0);
	}

	if (selection == -2)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pRadio0),TRUE);
	else if (selection == -1)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pRadio1),TRUE);
	else if (selection >= 0)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pRadio2[selection]),TRUE);

    pValider = gtk_button_new_from_stock(GTK_STOCK_OK);
    gtk_box_pack_start(GTK_BOX (pVBox0),pValider,FALSE,FALSE,0);
 
    gtk_widget_show_all(pWindow0);
 
    g_signal_connect(G_OBJECT(pWindow0),"destroy",G_CALLBACK(gtk_main_quit),NULL);
    g_signal_connect(G_OBJECT(pValider),"clicked",G_CALLBACK(OnValider),pRadio1);
 
    gtk_main();
}

SDL_Rect2 selectionner_image(SDL_Surface* images[NB_IMG],SDL_Rect2 pos0,int* continuer,int argc,char** argv)
{
	int x = px,y = py,dx,dy,DX,DY,direction = 0,mousex = 0,mousey = 0;
	SDL_Rect source = {0,0,256,192},source2 = {x,y,256,192};
	SDL_Rect2 pos = {0,0,0,0};

	*continuer = 1;

	SDL_Event event;

	do
	{
		SDL_WaitEvent(&event);

		if ((event.type == SDL_QUIT) || ((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_ESCAPE)))
			*continuer = -1;

		if (event.type == SDL_MOUSEMOTION)
		{
			mousex = event.motion.x;
			mousey = event.motion.y;
		}

		if (event.type == SDL_KEYDOWN)
		{
			switch(event.key.keysym.sym)
			{
				case SDLK_DOWN:
					if (y <= (images[1]->h - 192 -16)) y += 16;
					break;
				case SDLK_UP:
					if (y >= 16) y -= 16;
					break;
				case SDLK_LEFT:
					if (x >= 16) x -= 16;
					break;
				case SDLK_RIGHT:
					if (x <= (images[1]->w - 256 - 16)) x += 16;
					break;
				case SDLK_SPACE:
					direction = 1 - direction;
					break;
				case SDLK_F12:
					fichiers(argc,argv);
					break;
				case SDLK_F5:
					sons(argc,argv);
					break;
				case SDLK_s:
					parametres_liste(argc,argv,1);
					break;
				case SDLK_F6:
					selection_groupe(argc,argv);
					break;
				case SDLK_e:
					selection = -2;
					l = 0;
					break;
				case SDLK_t:
					parametres_liste(argc,argv,0);
					break;
				case SDLK_i:
					if ((mousex == 0) || (mousey == 0))
						break;

					SDL_Rect pos_curseur = {COTE * ((mousex) / COTE) / AGRANDISSEMENT,COTE * (mousey / COTE) / AGRANDISSEMENT,16,16};

					int o = trouver_objet_par_coordonnees(obj,nombre_objets,(x + pos_curseur.x)/16,(y + pos_curseur.y)/16);

					if (o != nombre_objets)
					{
						char chaine[256];
						sprintf(chaine,"Ajouter %s a la liste ? (o / n)",obj[o].nom);

						afficher_console(ecran,"pkmndp.ttf",COTE,chaine);

						if (pause2('o','n') == 1)
							liste[l++] = o;

						selection = -1;
					}
					else
					{
						afficher_console(ecran,"pkmndp.ttf",COTE,"Aucun objet trouve (enter)");
						pause0();
					}

					SDL_Flip(ecran);
					break;
			}

			px = x;
			py = y;

			source2.x = x;
			source2.y = y;
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN)
		{
		    if (event.button.button == SDL_BUTTON_WHEELDOWN)
			{
				if (direction == 0)
				{
					if (y <= (images[1]->h - 192 -16))
						y += 16;
				}
				else
				{
					if (x <= (images[1]->w - 256 - 16))
						x +=16;
				}
			}
		    else if (event.button.button == SDL_BUTTON_WHEELUP)
			{
				if (direction == 0)
				{
					if (y >= 16) y -= 16;
				}
				else
				{
					if (x >= 16) x -= 16;
				}
			}

			px = x;
			py = y;

			source2.x = x;
			source2.y = y;

			SDL_Rect pos_curseur = {COTE * ((event.motion.x) / COTE) / AGRANDISSEMENT,COTE * (event.motion.y / COTE) / AGRANDISSEMENT,16,16};

			if (event.button.button == SDL_BUTTON_RIGHT)
			{
				int o = trouver_objet_par_coordonnees(obj,nombre_objets,(x + pos_curseur.x)/16,(y + pos_curseur.y)/16);

				if (o != nombre_objets)
				{
					char chaine[256];
					sprintf(chaine,"Modifier %s ? (o / n)",obj[o].nom);

					afficher_console(ecran,"pkmndp.ttf",COTE,chaine);

					if (pause2('o','n') == 1)
					{
						pos.x = x + pos_curseur.x;
						pos.y = y + pos_curseur.y;
						pos.w =  16 * (dx + signe(dx));
						pos.h =  16 * (dy + signe(dy));
						n = o;
						*continuer = 0;
					}
				}
				else
				{
					afficher_console(ecran,"pkmndp.ttf",COTE,"Aucun objet trouve (enter)");
					pause0();
				}

				SDL_Flip(ecran);
			}
			else if (event.button.button == SDL_BUTTON_LEFT)
			{
				SDL_BlitSurface(images[2],NULL,ecran0,&pos_curseur);

				do
				{
					SDL_PollEvent(&event);

					if (event.type == SDL_MOUSEMOTION)
					{
						int i,j;

						SDL_Rect pos_curseur2 = {COTE * (event.motion.x / COTE) / AGRANDISSEMENT,COTE * (event.motion.y / COTE) / AGRANDISSEMENT,16,16};

						dx = (pos_curseur2.x - pos_curseur.x) / 16;
						dy = (pos_curseur2.y - pos_curseur.y) / 16;

						if ((DX != x) || (DY != dy))
						{				
							SDL_BlitSurface(images[4],NULL,ecran0,&source);
							SDL_BlitSurface(images[1],&source2,ecran0,&source);

						}

						if ((dx >= 0) && (dy >= 0))
						{
							for (i = 0 ; i <= dx ; i++)
							{
								for (j = 0 ; j <= dy ; j++)
								{
									pos_curseur2.x = (16 * i) + pos_curseur.x;
									pos_curseur2.y = (16 * j) + pos_curseur.y;
									SDL_BlitSurface(images[2],NULL,ecran0,&pos_curseur2);
								}
							}
						}
						else if ((dx >= 0) && (dy <= 0))
						{
							for (i = 0 ; i <= dx ; i++)
							{
								for (j = dy ; j <= 0 ; j++)
								{
									pos_curseur2.x = (16 * i) + pos_curseur.x;
									pos_curseur2.y = (16 * j) + pos_curseur.y;
									SDL_BlitSurface(images[2],NULL,ecran0,&pos_curseur2);
								}
							}
						}
						else if ((dx <= 0) && (dy >= 0))
						{
							for (i = dx ; i <= 0 ; i++)
							{
								for (j = 0 ; j <= dy ; j++)
								{
									pos_curseur2.x = (16 * i) + pos_curseur.x;
									pos_curseur2.y = (16 * j) + pos_curseur.y;
									SDL_BlitSurface(images[2],NULL,ecran0,&pos_curseur2);
								}
							}
						}
						else if ((dx <= 0) && (dy <= 0))
						{
							for (i = dx ; i <= 0 ; i++)
							{
								for (j = dy ; j <= 0 ; j++)
								{
									pos_curseur2.x = (16 * i) + pos_curseur.x;
									pos_curseur2.y = (16 * j) + pos_curseur.y;
									SDL_BlitSurface(images[2],NULL,ecran0,&pos_curseur2);
								}
							}
						}

						DX = dx;
						DY = dy;

						afficher_xy(ecran0,abs(dx)+1,abs(dy)+1);

						agrandir_image(ecran0,ecran,256,192);

						SDL_Flip(ecran);
					}

				}while(event.type != SDL_MOUSEBUTTONUP);

				afficher_console(ecran,"pkmndp.ttf",COTE,"Ajouter cet objet ? (o / n)");
				SDL_Flip(ecran);

				if (pause2('o','n') == 1)
				{
					pos.x = x + pos_curseur.x;
					pos.y = y + pos_curseur.y;
					pos.w =  16 * (dx + signe(dx));
					pos.h =  16 * (dy + signe(dy));
					*continuer = 0;
				}
			}
		}

		SDL_BlitSurface(images[4],NULL,ecran0,&source);
		SDL_BlitSurface(images[1],&source2,ecran0,&source);
		afficher_filtre(images,source2,1);

		agrandir_image(ecran0,ecran,256,192);

		SDL_Flip(ecran);

	}while(*continuer == 1);

	return convertir_pos(pos);
}

void recuperer_chemin(GtkWidget *bouton,GtkWidget *file_selection)
{
    GtkWidget *dialog;

    chemin = gtk_file_selection_get_filename(GTK_FILE_SELECTION(file_selection));

	if (recup == 1)
		strcpy(fichier1,chemin);
	else if (recup == 2)
		strcpy(fichier2,chemin);
	else if (recup == 3)
		strcpy(fichier3,chemin);

    dialog = gtk_message_dialog_new(GTK_WINDOW(file_selection),
    GTK_DIALOG_MODAL,
    GTK_MESSAGE_INFO,
    GTK_BUTTONS_OK,
    "Vous avez choisi :\n%s",chemin);

	char fichier[1024];
	memset(fichier,0,1024);

	int i,s = strlen(chemin);
	for (i = s ; i >= 0 ; i--)
	{
		if ((chemin[i] == '/') || (chemin[i] == '\\'))
			break;
	}

	strcpy(fichier,chemin+i+1);

	gtk_button_set_label(GTK_BUTTON(bouton),fichier);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    gtk_widget_destroy(file_selection);
}

GtkWidget *pWindow;
GtkWidget *pVBox;
GtkWidget *pHBox;
GtkWidget *pFrame;
GtkWidget *pButton[3];
GtkWidget *pEntry;
GtkWidget *pLabel;
GtkWidget *pScale,*pScale2,*pScale3;
GtkWidget *scrollbar;
GtkWidget *bouton_explorer;
gchar *sUtf8;

void ajouter(GtkWidget *pWindow,gpointer data)
{
	SDL_Rect2* ptr_pos = (SDL_Rect2*)data;
	SDL_Rect2 pos = *ptr_pos;

	SDL_Rect pos1 = {pos.x,pos.y,abs(pos.w),abs(pos.h)};

	const gchar *nom;
	nom = gtk_entry_get_text(GTK_ENTRY(pEntry));

	int n_traversable,n_special;
	n_traversable = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pButton[0]));
	n_special = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pButton[1]));

	int son_active = gtk_range_get_value(GTK_RANGE(pScale)),n_numero = gtk_spin_button_get_value(GTK_SPIN_BUTTON(pScale2)),n_niveau = gtk_spin_button_get_value(GTK_SPIN_BUTTON(pScale3));

	if (n != -1)
	{
		FILE* f = s_fopen("tileset.xml","w");

		int i;

		for (i = 0 ; i < n ; i++)
			fprintf(f,"<objet>\n\t<nom>%s</nom>\n\t<tileset>1</tileset>\n\t<posx>%d</posx>\n\t<posy>%d</posy>\n\t<largeur>%d</largeur>\n\t<hauteur>%d</hauteur>\n\t<traversable>%d</traversable>\n\t<niveau>%d</niveau>\n\t<porte>0</porte>\n\t<numero>%d</numero>\n\t<son>%s</son>\n\t<son_active>%d</son_active>\n</objet>\n\n",obj[i].nom,obj[i].posx,obj[i].posy,obj[i].largeur,obj[i].hauteur,obj[i].traversable,obj[i].niveau,obj[i].numero,obj[i].son,obj[i].son_active);

			fprintf(f,"<objet>\n\t<nom>%s</nom>\n\t<tileset>1</tileset>\n\t<posx>%d</posx>\n\t<posy>%d</posy>\n\t<largeur>%d</largeur>\n\t<hauteur>%d</hauteur>\n\t<traversable>%d</traversable>\n\t<niveau>%d</niveau>\n\t<porte>0</porte>\n\t<numero>%d</numero>\n\t<son>%s</son>\n\t<son_active>%d</son_active>\n</objet>\n\n",nom,obj[n].posx,obj[n].posy,obj[n].largeur,obj[n].hauteur,n_traversable,n_niveau,n_numero,((chemin == NULL) ? obj[n].son : chemin),son_active);

			for (i = n+1 ; i < nombre_objets ; i++)
			fprintf(f,"<objet>\n\t<nom>%s</nom>\n\t<tileset>1</tileset>\n\t<posx>%d</posx>\n\t<posy>%d</posy>\n\t<largeur>%d</largeur>\n\t<hauteur>%d</hauteur>\n\t<traversable>%d</traversable>\n\t<niveau>%d</niveau>\n\t<porte>0</porte>\n\t<numero>%d</numero>\n\t<son>%s</son>\n\t<son_active>%d</son_active>\n</objet>\n\n",obj[i].nom,obj[i].posx,obj[i].posy,obj[i].largeur,obj[i].hauteur,obj[i].traversable,obj[i].niveau,obj[i].numero,obj[i].son,obj[i].son_active);

		fclose(f);
	}
	else
	{
		if (n_special)
		{
			if ((pos1.w / 16 == 2) && (pos1.h / 16 == 2))
			{
				FILE* f = s_fopen("tileset.xml","a");

				char* card[] = {"NO","SO","NE","SE"};

				int i,j;

				for (i = 0 ; i < 2 ; i++)
					for (j = 0 ; j < 2 ; j++)
						fprintf(f,"<objet>\n\t<nom>%s %s</nom>\n\t<tileset>1</tileset>\n\t<posx>%d</posx>\n\t<posy>%d</posy>\n\t<largeur>%d</largeur>\n\t<hauteur>%d</hauteur>\n\t<traversable>%d</traversable>\n\t<niveau>%d</niveau>\n\t<porte>0</porte>\n\t<numero>%d</numero>\n\t<son>%s</son>\n\t<son_active>%d</son_active>\n</objet>\n\n",nom,card[(2*i)+j],(pos1.x / 16) + i,(pos1.y / 16)+j,1,1,n_traversable,n_niveau,n_numero+(2*i)+j,chemin,son_active);

				fclose(f);
				ajouter_nombre_objets();
				ajouter_nombre_objets();
				ajouter_nombre_objets();
				ajouter_nombre_objets();
			}
			else if ((pos1.w / 16 == 3) && (pos1.h / 16 == 3))
			{
				FILE* f = s_fopen("tileset.xml","a");

				char* card[] = {"NO","O","SO","N","","S","NE","E","SE"};

				int i,j;

				for (i = 0 ; i < 3 ; i++)
					for (j = 0 ; j < 3 ; j++)
						fprintf(f,"<objet>\n\t<nom>%s %s</nom>\n\t<tileset>1</tileset>\n\t<posx>%d</posx>\n\t<posy>%d</posy>\n\t<largeur>%d</largeur>\n\t<hauteur>%d</hauteur>\n\t<traversable>%d</traversable>\n\t<niveau>%d</niveau>\n\t<porte>0</porte>\n\t<numero>%d</numero>\n\t<son>%s</son>\n\t<son_active>%d</son_active>\n</objet>\n\n",nom,card[(3*i)+j],(pos1.x / 16) + i,(pos1.y / 16)+j,1,1,n_traversable,n_niveau,n_numero+(3*i)+j,chemin,son_active);

				fclose(f);
				ajouter_nombre_objets();
				ajouter_nombre_objets();
				ajouter_nombre_objets();
				ajouter_nombre_objets();
				ajouter_nombre_objets();
				ajouter_nombre_objets();
				ajouter_nombre_objets();
				ajouter_nombre_objets();
				ajouter_nombre_objets();
			}
		}
		else
		{
			FILE* f = s_fopen("tileset.xml","a");

			fprintf(f,"<objet>\n\t<nom>%s</nom>\n\t<tileset>1</tileset>\n\t<posx>%d</posx>\n\t<posy>%d</posy>\n\t<largeur>%d</largeur>\n\t<hauteur>%d</hauteur>\n\t<traversable>%d</traversable>\n\t<niveau>%d</niveau>\n\t<porte>0</porte>\n\t<numero>%d</numero>\n\t<son>%s</son>\n\t<son_active>%d</son_active>\n</objet>\n\n",nom,pos1.x / 16,pos1.y / 16,pos1.w / 16,pos1.h / 16,n_traversable,n_niveau,n_numero,chemin,son_active);

			fclose(f);
			ajouter_nombre_objets();
		}
	}

	free(obj);
	charger_nombre_objets(&nombre_objets);
	obj = charger_objets("tileset.xml",&nombre_objets);
}

void creer_file_selection(void)
{
    GtkWidget *selection;
     
    selection = gtk_file_selection_new( g_locale_to_utf8( "Sélectionnez un fichier",-1,NULL,NULL,NULL));
    gtk_widget_show(selection);
  
    gtk_window_set_modal(GTK_WINDOW(selection),TRUE);
     
    g_signal_connect(G_OBJECT(GTK_FILE_SELECTION(selection)->ok_button),"clicked", G_CALLBACK(recuperer_chemin),selection);
     
    g_signal_connect_swapped(G_OBJECT(GTK_FILE_SELECTION(selection)->cancel_button),"clicked",G_CALLBACK(gtk_widget_destroy),selection);
}

int parametres(SDL_Surface* images[NB_IMG],SDL_Rect2 pos,int argc,char** argv)
{
    gtk_init(&argc,&argv);
 
    pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_container_set_border_width(GTK_CONTAINER(pWindow),5);
    gtk_window_set_title(GTK_WINDOW(pWindow),"Informations objet");
    gtk_window_set_default_size(GTK_WINDOW(pWindow),320*AGRANDISSEMENT,320*AGRANDISSEMENT);
    g_signal_connect(G_OBJECT(pWindow),"destroy",G_CALLBACK(gtk_main_quit),NULL);

	scrollbar = gtk_scrolled_window_new(NULL,NULL);
    gtk_container_add(GTK_CONTAINER(pWindow),scrollbar);

    pVBox = gtk_vbox_new(TRUE,0);
	pHBox = gtk_hbox_new(TRUE,0);

    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrollbar),pVBox);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollbar),GTK_POLICY_NEVER,GTK_POLICY_ALWAYS);

	if (n != -1)
	{
		char fichier[1024];
		memset(fichier,0,1024);

		int i,s = strlen(obj[n].son);
		for (i = s ; i >= 0 ; i--)
		{
			if ((obj[n].son[i] == '/') || (obj[n].son[i] == '\\'))
				break;
		}

		strcpy(fichier,obj[n].son+i+1);

		bouton_explorer = gtk_button_new_with_label(fichier);
	}
	else
		bouton_explorer = gtk_button_new_with_label("Fichier audio");
	gtk_box_pack_start(GTK_BOX(pVBox),bouton_explorer,TRUE,FALSE,0);
    g_signal_connect(G_OBJECT(bouton_explorer),"clicked",G_CALLBACK(creer_file_selection),NULL);

	pFrame = gtk_frame_new("Volume");
    pScale = gtk_hscale_new_with_range(0,100,1);
	gtk_scale_set_value_pos(GTK_SCALE(pScale),GTK_POS_BOTTOM);

	if (n != -1)
		gtk_range_set_value(GTK_RANGE(pScale),obj[n].son_active);

    gtk_container_add(GTK_CONTAINER(pFrame),pScale);
    gtk_box_pack_start(GTK_BOX(pVBox),pFrame,FALSE,FALSE,0);

    pLabel = gtk_label_new("Nom :");
    gtk_box_pack_start(GTK_BOX(pVBox),pLabel,TRUE,FALSE,0);
    pEntry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(pVBox),pEntry,TRUE,FALSE,0);
	if (n != -1)
		gtk_entry_set_text(GTK_ENTRY(pEntry),obj[n].nom);

	pFrame = gtk_frame_new("Niveau : ");
	pScale3 = gtk_spin_button_new_with_range(2,3,1);

	if (n != -1)
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(pScale3),obj[n].niveau);
	else
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(pScale3),0);

	gtk_container_add(GTK_CONTAINER(pFrame),pScale3);
	gtk_box_pack_start(GTK_BOX(pVBox),pFrame,FALSE,FALSE,0);

	pFrame = gtk_frame_new("Numero : ");
	pScale2 = gtk_spin_button_new_with_range(0,10000,1);

	if (n != -1)
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(pScale2),obj[n].numero);
	else
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(pScale2),nombre_objets+1);

	gtk_container_add(GTK_CONTAINER(pFrame),pScale2);
	gtk_box_pack_start(GTK_BOX(pVBox),pFrame,FALSE,FALSE,0);
 
	pButton[0] = gtk_check_button_new_with_label("Traversable");
	pButton[1] = gtk_check_button_new_with_label("Special");
	gtk_box_pack_start(GTK_BOX(pHBox),pButton[0],TRUE,FALSE,0);	
	if (n == -1)	
		gtk_box_pack_start(GTK_BOX(pHBox),pButton[1],TRUE,FALSE,0);
	
	gtk_box_pack_start(GTK_BOX(pVBox),pHBox,TRUE,FALSE,0);

	if (n != -1)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pButton[0]),((obj[n].traversable == 1) ? TRUE : FALSE));

	if (n != -1)
		pButton[2] = gtk_button_new_with_label("Modifier");
	else
		pButton[2] = gtk_button_new_with_label("Ajouter");
	gtk_box_pack_start(GTK_BOX(pVBox),pButton[2],FALSE,FALSE,0);

	g_signal_connect(G_OBJECT(pButton[2]),"clicked",G_CALLBACK(ajouter),&pos);

    gtk_widget_show_all(pWindow);
 
    gtk_main();

	n = -1;

	return -3;
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
	images[2] = s_IMG_Load("curseur.png");
	images[3] = SDL_CreateRGBSurface(0,256,192,32,0,0,0,0);
	images[4] = SDL_CreateRGBSurface(0,256,192,32,0,0,0,0);
	images[5] = s_IMG_Load("filtre.png");

	SDL_FillRect(images[4],NULL,SDL_MapRGB(images[4]->format,0x99,0x99,0x99));

	int continuer,n;

	obj = charger_objets("tileset.xml",&nombre_objets);

	gr = charger_groupes(&nombre_groupes);

	SDL_Rect2 pos = {0,0,0,0};

	do
	{
		n = -1;

		continuer = 1;

		pos = selectionner_image(images,pos,&continuer,argc,argv);

		if (continuer == -1)
			break;

	}while(parametres(images,pos,argc,argv) == -3);

	free(obj);
	free(gr);
    cleanup(images,musiques);
    SDL_Quit();
    return EXIT_SUCCESS;
}
