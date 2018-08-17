#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


//Affichage plateau
#define couleur(param) printf("\033[%sm",param)
#define taille_max  3
#define HAUTEUR 10
#define LARGEUR 9


//couleur
#define VIDE 0


//******************* Definition des pieces ********************/


typedef enum {P = 0, T, E, B, C, R, G, V} nom_piece;
typedef enum {ROUGE, NOIR} couleur_piece;
typedef struct {
    nom_piece nom;
    couleur_piece couleur;
} piece;

nom_piece MODELE[HAUTEUR/2][LARGEUR] =
    {{T, C, E, G, R, G, E, C, T},
     {V, V, V, V, V, V, V, V, V},
     {V, B, V, V, V, V, V, B, V},
     {P, V, P, V, P, V, P, V, P},
     {V, V, V, V, V, V, V, V, V},
    };


//******************* Definition du plateau ********************/


// convention :
// case vide -> NULL
// case non vide -> pointeur vers piece.

typedef struct {
    piece *contenu[HAUTEUR][LARGEUR]; //Etat du plateau
} plateau;


//******************* Affichage message d'erreurs  ********************/


typedef enum {ECHEC , MVMT , DANGER , ILLEGAL , MAUVAIS_PION} code_resultat;


char *STR_OF_CODE[] = {"Le roi est en echec \n ",
"Le mouvement est possible \n ", 
"Si vous bougez ce pion le roi sera en echec \n", 
"Le pion selectionné n'effectue pas un déplacement dans les règles du jeu, vous passez votre tour.\n",
"Vous n'avez pas joué votre pion, vous passez votre tour.\n"};
 
void afficher_message(code_resultat c) {
    printf("%s", STR_OF_CODE[c]);
}


void regles_du_jeu () {
	printf("--------------------------------------------------------------------------------------------\n");
	printf("=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*==XIANGQI==*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*\n\n");
	printf("--------------------------------------------------------------------------------------------\n");
	printf(" 					REGLES DE DEPLACEMENT           		              					\n" );	
	printf("--------------------------------------------------------------------------------------------\n");
	printf("Les pions rouges ( joueur 0 ) commencent la partie.  \n");
	printf("Les petits carrés contenant des lignes diagonales sont les palais  et le rectangle horizontal au milieu du plateau représente la rivière. \n ");
	printf("La tour  (T) se déplace verticalement ou horizontalement dans le plateau. Elle peut parcourir plusieurs cases sauf si un pion bloque son passage.  \n");
	printf(" Le cheval( C ) se déplace en deux fois : d'abord d'une première case verticalement ou horizontalement, suivit d'une deuxième case en diagonale. La position intermeiaire doit être vide .\n");
	printf("L'éléphant (E)  se déplace de deux cases en diagonale, dans la même direction. La position intermediaire doit être vide. Il ne peut pas traverser la rivière \n");
	printf("La roi (  R ) se déplace d'une case verticalement ou horizontalement et doit rester dans le palais \n " );
	printf("La bombarde ( B ) se déplace de la même façon que la tour. Si elle veut capturer une pièce adverse, elle doit le faire en sautant par dessus une autre pièce (des siennes ou de son adversaire). \n");
	printf("Le garde ( G )  se déplace d'une case en diagonal et doit rester dansle palais \n " );
	printf("Le pion  (  P ) se déplace  d'une case verticalement lorsqu'il est placé avant la rivière. Après la rivière, il peut se déplacer soit d'une case horizontalement ou verticalement \n " ) ;
	printf("Pour effectuer un mouvement, veuillez  : \n--Entrer la ligne du pion à déplacer ( de 0 à 9 ) puis -Entrée-.\n");
	printf("--La partie prend fin lorsqu'un des deux rois ne peut plus effectuer de mouvment sans être en échec.");
	printf("--Entrer la colonne du pion à déplacer ( de 0 à 8 ) puis -Entrée-\n");
	printf("--Puis, la direction qui vous sera proposée, puis -Entrée- \n");
	printf("--Puis, le nombre de case à parcourir ( pour la tour et le canon ) , puis -Entrée- \n");
	printf("-------------------------------------------------------------------------------------------\n\n");
}

//******************* Orientation des mouvements  ********************/


typedef enum {N, S, ES, W, NE, NW, SE, SW} dir;



typedef struct {
    int i;
    int j;
} pos;

// E , C : deux coups  d'un mouvement
// pions concernés : P, G , R , E , C
pos move(pos p, dir d) {
    switch(d) {
    case N  : return (pos) {p.i - 1, p.j};
    case S  : return (pos) {p.i + 1, p.j};
    case ES : return (pos) {p.i, p.j + 1};
    case W  : return (pos) {p.i, p.j - 1};
    case NE : return (pos) {p.i - 1, p.j + 1};
    case NW : return (pos) {p.i - 1, p.j - 1};
    case SE : return (pos) {p.i + 1, p.j + 1};
    case SW : return (pos) {p.i + 1, p.j - 1};
    }
    return p;
}


//deplacement multiple de n lignes ou n colonnes

pos move_mult(pos p, dir d , int n) {
    switch(d) {
    case N  : return (pos) {p.i - n, p.j};
    case S  : return (pos) {p.i + n, p.j};
    case E  : return (pos) {p.i, p.j + n};
    case W  : return (pos) {p.i, p.j - n};
    case NE : return (pos) {p.i - n, p.j + n};
    case NW : return (pos) {p.i - n, p.j - n};
    case SE : return (pos) {p.i + n, p.j + n};
    case SW : return (pos) {p.i + n, p.j - n};
    }
    return p;
}






//******************* FONCTIONS POUR MOUVEMENTS  ********************/


//******************* 3 limites du plateau ********************/


int dans_limites(pos p){
    //printf("dans limites\n ");
    return
	p.i >= 0 && p.i < HAUTEUR &&
	p.j >= 0 && p.j < LARGEUR;
}

// OPT : les comparaisons s'evaluent en 0 ou 1.
// OPT : si la couleur n'est pas rouge, elle est noire.
int dans_palais(plateau *pl, pos p,  dir d, couleur_piece couleur){
  pos q = move(p, d);
  if (q.j >= 3 && q.j <= 5)
    return
      couleur == ROUGE ?
      q.i >= 0 && q.i <= 2:
      q.i >= 7 && q.i <= 9;
  return 0;
}

// OPT : memes remarques.
int cote_riviere(plateau *pl, pos p,  couleur_piece couleur){
  return
    couleur==ROUGE ?
    p.i < HAUTEUR/2:
    p.i >= HAUTEUR/2;
}




//******************* FOnctions tests ********************/


int case_vide(plateau *pl, pos p, dir d){
    pos q = move(p, d);
    //printf("HELLO case_vide\n");
    return
	dans_limites(q) &&
	pl -> contenu [q.i][q.j] == NULL;
}

// OPT : necessaire, sinon la fonction cpt_pion risque de échecs compter +1
// pour chaque case en dehors du plateau.
int case_pleine(plateau *pl, pos p, dir d){
    pos q = move(p, d);
    //printf("HELLO CASE PLEINE direction %d \n", d);
    return
	(dans_limites(q) &&
	 pl -> contenu [q.i][q.j] != NULL);
}

int case_vide_multiple(plateau *pl, pos p, dir d, int n) {
    int i;
    for (i = 0; i < n; i++) {
	if (!case_vide(pl, p, d))
	    return 0;
	p = move(p, d);
    }
    return 1;
}

// OPT : il manquait le return (il faut compiler avec -Wall)
int ennemi(piece *f, piece *e){
    return f -> couleur == ROUGE ? e -> couleur == NOIR : e -> couleur == ROUGE ;
}



// OPT : factorisation de la partie commune des deux tests
int case_ennemi(plateau *pl, pos p, pos q){
    //printf("HELLO case_ennemi\n");
   return
       (dans_limites(q) &&
      pl -> contenu[q.i][q.j] != NULL &&
      pl -> contenu[q.i][q.j] -> couleur != pl -> contenu[p.i][p.j]->couleur);
}

int case_ennemi_roi(plateau *pl, pos p, pos q){
    return
      (dans_limites(q) &&
      pl -> contenu[q.i][q.j] != NULL &&
      pl -> contenu[q.i][q.j] -> couleur != pl -> contenu[p.i][p.j]->couleur) &&  pl -> contenu[q.i][q.j] -> nom == 5;
}


int case_ennemi_proche (plateau *pl, pos p, dir d){
    //printf("HELLO ennemi_prochv\n");
    return case_ennemi(pl, p, move(p, d));
 }

int case_ennemi_loin (plateau *pl, pos p, dir d, int n){
	//printf("HELLO case_ennemi_loin\n");
    int i;
    pos q = p;
    for (i = 0; i < n; i++)
	q = move(q, d);
    return case_ennemi(pl, p, q);
}

// compte le nb de pions entre position arrivée et départ n
// OPT : bug potentiel, corrige par l'ecriture de case_pleine.
int cpt_pion(plateau *pl, pos p, dir d, int n) {
  int i, c = 0;
  for (i = 0; i < n; i++) {
    if (case_pleine(pl, p, d))
      c++;
    p = move(p, d);
  }
  return c;
}



int detecte_roi(plateau *pl , couleur_piece joueur_courant) {
    int i, j;
    for (i = 0; i < HAUTEUR; i++) {
		for (j = 0; j < LARGEUR; j++) {
	    	if (pl -> contenu[i][j] != NULL
	    		&& pl -> contenu[i][j] -> nom == R 
	    		&& pl -> contenu[i][j] -> couleur == joueur_courant ){
	    			return(1);
	    	}
	    }
	}
	return (0) ;
}


//*******************  Fct : Déplacement des pions  ********************/


//remplace et vide après depart du  pions
void remplace_vide( plateau *pl  , pos p , pos q) {
    pl -> contenu [q.i][q.j] =  pl -> contenu [p.i][p.j] ;
    pl -> contenu [p.i][p.j] = NULL ;
}



//**************** Initiatlisation : remplissage plateau *******************/


plateau *plateau_initial() {
    int i, j;
    plateau *pl;
    pl = malloc(sizeof(plateau));
    for (i = 0; i < HAUTEUR/2; i++)
	for (j = 0; j < LARGEUR; j++) {
	    piece *fr = NULL, *fn = NULL;
	    if (MODELE[i][j] != V) {
		fr = malloc(sizeof(piece));
		fr -> nom = MODELE[i][j];
		fr -> couleur = ROUGE;
		fn = malloc(sizeof(piece));
		fn -> nom = MODELE[i][j];
		fn -> couleur = NOIR;
	    }
	    pl -> contenu[i][j] = fr;
	    pl -> contenu[HAUTEUR - 1 - i][j] = fn;
	}
    return pl;
}

char CR[] = {'P', 'T', 'E', 'B', 'C', 'R', 'G'};
char CN[] = {'p', 't', 'e', 'b', 'c', 'r', 'g'};

char char_of_piece(piece *f) {
    return
	(f -> couleur == ROUGE) ?  CR[f -> nom] : CN[f -> nom];
}

void afficher_plateau(plateau *pl) {
    int i, j;
    printf("\n");
    printf("0   1   2   3   4   5   6   7   8\n");
     printf("----------------------------------\n");
    for (i = 0; i < HAUTEUR; i++) {
	for (j = 0; j < LARGEUR; j++) {
	    char c = 'o';
	    if (pl -> contenu[i][j] != NULL){
		c = char_of_piece(pl -> contenu[i][j]);
		if(pl -> contenu[i][j]-> couleur==ROUGE){
		    couleur("31");
		    printf("%c", c);
		    couleur("37");
		     }else if (pl -> contenu[i][j]-> couleur==NOIR){
		       couleur("34");
		       printf("%c", c);
		       couleur("37");}
		}else if(pl -> contenu[i][j]==NULL){
			couleur("37");
			printf("%c", c);}

		if (j < LARGEUR - 1)
		    printf(" - ");}
			    printf("  |%d " , i);


	    if (i == 0 || i == HAUTEUR - 3)
		printf("\n|   |   |   | \\ | / |   |   |   |");
	    else if (i == 1 || i==HAUTEUR - 2)
		printf("\n|   |   |   | / | \\ |   |   |   |");
	    else if (i != HAUTEUR/2 - 1 && i != HAUTEUR - 1 )
		printf("\n|   |   |   |   |   |   |   |   |");
	    else if (i != HAUTEUR/2 - 1 && i != HAUTEUR - 1 )
		printf("\n|   |   |   |   |   |   |   |   |");
	    else if(i==5){
		printf("                                 ");
		printf("\n");}
	    printf("\n");
	}
	printf("\n");
    }




//******************* MOUVEMENTS DES PIECES  ********************/


// OPT : ecriture plus concise par factorisation des conditions.
int pion_mouvtest(plateau *pl, pos p, dir d ){
    int coteriv = cote_riviere(pl,  p,  pl -> contenu[p.i][p.j] -> couleur);
    return
	(case_ennemi_proche(pl, p, d) ||  case_vide(pl , p,  d)) && // Case vide : ok  && case_ennemi : ok
	((!coteriv && (d == ES || d == W)) ||
	 (pl -> contenu[p.i][p.j] -> couleur == ROUGE && d == S) ||
	 (pl -> contenu[p.i][p.j] -> couleur == NOIR  && d == N));
}

// OPT : concision. le flag est inutile.
int roi_mouvtest(plateau *pl, pos p, dir d){
    int palais = dans_palais(pl, p, d, pl -> contenu[p.i][p.j] -> couleur);
    return
      palais &&
      (d == S || d == N || d == ES || d == W) &&
      (case_ennemi_proche(pl , p, d) ||  case_vide(pl, p, d));
}

// OPT : idem pour toutes les suivantes.
int garde_mouvtest( plateau *pl, pos p, dir d ){
    int palais = dans_palais(pl, p, d, pl -> contenu[p.i][p.j] -> couleur);
    return
      palais &&
      (d == NW || d == NE || d == SE || d== SW) &&
      (case_ennemi_proche(pl , p, d) ||  case_vide(pl, p, d));
}

int tour_mouvtest( plateau *pl, pos p, dir d , int n ){
    return
      (d == N || d == S || d == ES || d == W) &&
      (case_vide_multiple(pl , p, d, n) ||
       (case_vide_multiple(pl, p, d, n - 1) &&
	case_ennemi_loin(pl, p, d, n)));
}

int elephant_mouvtest(plateau *pl, pos p, dir d ){
    pos p1 = move(p,d);
    pos p2 = move(p1,d);
    int coteriv = cote_riviere(pl, p2, pl -> contenu[p.i][p.j] -> couleur);
    return
      (d == NE || d == NW || d == SE || d == SW) &&
      coteriv &&
      case_vide(pl, p, d) &&
      (case_vide_multiple(pl, p, d, 1) || case_ennemi_loin(pl, p, d, 1));
}



int cheval_mouvtest(plateau *pl, pos p, dir d1, dir d2){
  pos p1 = move(p, d1);
  pos p2 = move(p1, d2);
  return 
	((d1 == N && ( d2 == NE || d2 == NW ) ) ||
	(d1 == S && ( d2 == SE || d2 == SW ) ) ||
	(d1 == ES && ( d2 == SE || d2 == NE ) ) ||
	(d1 == W && ( d2 == SW || d2 == NW ) ) ) &&
	case_vide(pl, p, d1) &&
    (case_ennemi(pl, p , p2 ) ||  case_vide(pl , p1,  d2));
}




int canon_mouvtest( plateau *pl, pos p, dir d , int n ){
  int nb_pions = cpt_pion(pl, p, d, n - 1);
  return
      (d == N || d == S || d == ES || d == W) &&
      ((nb_pions == 0 && case_vide_multiple (pl, p, d, n)) ||
       (nb_pions == 1 && case_ennemi_loin(pl , p,  d , n)));
}



//******************* GESTION DES MOUVEMENTS DES PIECES  ********************/


pos choix_pion(int joueur_courant){
    pos p;
    int l,c;
    printf("\nEntrez les numeros de ligne et de colonne du pion à déplacer : \n");
    scanf("%d", &l);
    scanf("%d", &c);
    p.i = l;
    p.j = c;
    return(p);
}

int mon_pion(plateau *pl, couleur_piece joueur_courant, pos p){
    if (pl->contenu[p.i][p.j]->couleur == joueur_courant){
	return 1;}
    else {
	afficher_message(MAUVAIS_PION);
	return 0;
    }
}

int choix_direction(int joueur_courant){                //Sélection d'une direction par le joueur
    int dir;
    printf("Dans quelle direction voulez-vous déplacer votre pion ? : \n\n 0-N \n 1-S \n 2-ES \n 3-W \n 4-NE \n 5-NW \n 6-SE \n 7-SW \n");
    printf("\n");
    scanf("%d", &dir);
    return(dir);
}


int choix_n(int joueur_courant){ //Sélection d'une direction par le joueur
    int n;
    printf("De combien de cases voulez vous avancer ? ");
    printf("\n");
    scanf("%d", &n);
    return(n);
}



int echec_par_pion( plateau *pl, couleur_piece joueur_courant, pos p ){
	int k ; 
	for(k=0; k<4; k++){
	    pos q = move(p, k);
	    if( ! case_vide(pl, p, k) &&  pion_mouvtest(pl, p, k) ){
			if(pl->contenu[q.i][q.j]->couleur != joueur_courant &&  pl -> contenu[q.i][q.j] -> nom == 5){
			    return(1) ;
			}
		}
	}
	return (0)	; 
}



int echec_par_tour( plateau *pl, couleur_piece joueur_courant, pos p ){
	int k ; 
	for ( k=0 ; k<4 ; k++ ) {
		switch (k){
			case 0 :
			for (int w=p.i; w>=0; w--){
			    if (case_vide_multiple(pl, p, 0, w-1) && case_ennemi_loin(pl, p, 0, w) && tour_mouvtest(pl , p , 0 , w) && (pl -> contenu[p.i - w][p.j] -> nom == 5)){
				    return(1) ;
			    }
			}
			break ;
			case 1 :
			for (int w=p.i; w<HAUTEUR; w++){
			    if (case_vide_multiple(pl, p, 1, HAUTEUR - w -1) && case_ennemi_loin(pl, p, 1, HAUTEUR - w) && tour_mouvtest(pl , p , 1 , HAUTEUR - w) && (pl -> contenu[HAUTEUR - w +p.i][p.j] -> nom == 5)){
					return(1) ;
			    }
			}
			break;
			case 2:
			for (int w=p.j; w<LARGEUR; w++){
			    if (case_vide_multiple(pl, p, 2, LARGEUR - w -1) && case_ennemi_loin(pl, p, 2, LARGEUR - w) && tour_mouvtest(pl , p , 2 , LARGEUR - w) && (pl -> contenu[p.i][LARGEUR - w +p.j] -> nom == 5)){
				return(1) ;
			    }
			}
			break;
			case 3:
			for (int w=p.j; w>=0; w--){
			    if (case_vide_multiple(pl, p, 3, w-1) && case_ennemi_loin(pl, p, 3, w) && tour_mouvtest(pl , p , 3 , w) && (pl -> contenu[p.i][p.j-w] -> nom == 5)){
				return(1) ;
			    }
			}
			break;
	    }
	}
	return (0)	; 
}




int echec_par_canon( plateau *pl, couleur_piece joueur_courant, pos p ){
	int k ;
    for(k=0; k<=3; k++){
	    switch (k){
	    case 0 :
	      for (int w = p.i  ; w >= 0; w--){
	        if (  cpt_pion(pl,  p, 0, w - 1) == 1 &&
	        case_ennemi_loin(pl, p, 0, w) &&
	        canon_mouvtest(pl , p , 0 , w) &&
	        (pl -> contenu[p.i - w][p.j] -> nom == 5)){
	         return(1) ;
	        }
	      }
	    break ;
	    case 1 :
	      for (int w = p.i ; w < HAUTEUR; w++){
	        if ( cpt_pion(pl,  p, 1, HAUTEUR - w -1) == 1 &&
	        case_ennemi_loin(pl, p, 1, HAUTEUR - w) &&
	        canon_mouvtest(pl , p , 1 , HAUTEUR - w) &&
	        (pl -> contenu[HAUTEUR - w +p.i][p.j] -> nom == 5)){
	      		return(1) ;
	        }
	      }
	    break;
	    case 2:
	      for (int w = p.j ; w < LARGEUR; w++){
	        if ( cpt_pion(pl,  p, 2 ,   LARGEUR - w -1) == 1 &&
	          case_ennemi_loin(pl, p, 2, LARGEUR - w) &&
	          canon_mouvtest(pl , p , 2 , LARGEUR - w) &&
	          (pl -> contenu[p.i][LARGEUR - w +p.j] -> nom == 5)){
	        	return(1) ;
	        }
	      }
	    break;
	    case 3:
	      for (int w = p.j ; w >=0; w--){
	        if (
	          cpt_pion(pl,  p, 3,  w-1 ) == 1 &&
	         case_ennemi_loin(pl, p, 3, w) &&
	         canon_mouvtest(pl , p , 3 , w) &&
	         (pl -> contenu[p.i][p.j-w] -> nom == 5)){
	       	return(1) ;
	         }
	      }
	    break;
	    }
	}
	return (0)	; 

}


int echec_par_cheval (  plateau *pl, couleur_piece joueur_courant, pos p ) {
	int k1 , k2 ;
    for(k1=0; k1<=3; k1++){
      pos p1 = move(p, k1);
      //printf("ligne cheval p1  %d col cheval p1 %d \n", p1.i , p1.j);
      if ( case_vide( pl , p , k1)){
      	for(k2=4; k2<=7; k2++){
        	pos p2= move(p1, k2);
        	//printf("ligne cheval p2  %d col cheval p2 %d \n", p2.i , p2.j);
	        if (  case_pleine(pl, p1 , k2)  &&
	         cheval_mouvtest(pl,  p, k1, k2 ) &&
	         (pl -> contenu[p2.i][p2.j] -> nom == 5 )) {
	      		return(1) ;
      		}
        }
      }
    }
    return (0)	; 
}




int  roi_en_echec(plateau *pl, couleur_piece joueur_courant, pos p){
    nom_piece  figure = pl -> contenu[p.i][p.j]-> nom;
    switch(figure){
	    case P: echec_par_pion( pl, joueur_courant,  p ) ;
		break;
	  	case T : echec_par_tour(  pl,  joueur_courant,  p ) ;
		break;
		case B: echec_par_canon(  pl, joueur_courant,  p ) ;
	  	break;
	  	case C : echec_par_cheval (  pl, joueur_courant,  p ) ;
	  	break;
  		default : return ( 0 );
	}
}


int alerte(plateau *pl , couleur_piece joueur_courant){
	int i, j;
    for (i = 0; i < HAUTEUR; i++) {
		for (j = 0; j < LARGEUR; j++) {
	    	if (pl -> contenu[i][j] != NULL
	    		&& pl -> contenu[i][j] -> couleur == joueur_courant ){
	    			pos pos_fig ;
	    			pos_fig.i = i ;
	    			pos_fig.j= j ;
	    			if ( roi_en_echec ( pl , joueur_courant , pos_fig) == 1 ){
	    				return(1);
	    			}
	    	}
	    }
	}
	return (0) ;
}



//////////////


plateau *copie( plateau *plateauOriginal )
{
	int i , j;
	plateau* copie=malloc(sizeof(plateau));
    for ( i = 0  ; i<HAUTEUR ; i++)
    	{
    	for (  j = 0 ; j<LARGEUR ;j++ ){
       		copie -> contenu[i][j] = plateauOriginal -> contenu[i][j] ;
       		
    	}
    } 
    return( copie);
}


//////////////////////

void mvmt_sans_danger(pos p , pos q , plateau *pl , couleur_piece joueur_courant ){
	plateau *copie_pl ;
   	copie_pl = copie (pl);
	remplace_vide(copie_pl, p, q);
	afficher_plateau(copie_pl) ;
	int danger = alerte( copie_pl , !joueur_courant) ;
    if ( danger == 0 ) {
    	afficher_message(MVMT) ;
		remplace_vide(pl, p, q);
		roi_en_echec(pl, joueur_courant, q);
		if (roi_en_echec(pl, joueur_courant, q) == 1 ) {
			afficher_message(ECHEC);}
	} 
	else{ 
		afficher_message(DANGER);
		afficher_plateau(pl);
		pos p = choix_pion(joueur_courant);
		mouvement (p , pl , joueur_courant);
	}
	free(copie_pl);
}

// OPT : il y a encore moyen de factoriser des elements dans celle-ci,
// mais ce sera moins simple (il faudrait des pointeurs de fonctions).
// d'autre part, pour la bonne gestion de la partie, la fonction devrait
// renvoyer un entier (au moins 0 ou 1) specifiant si le mouvement a reussi.
int mouvement(pos p, plateau *pl, couleur_piece joueur_courant){
	regles_du_jeu();
    dir d1; dir d2;
    int n;
    int figure = pl -> contenu[p.i][p.j]-> nom;
    switch(figure){
    case P:
	d1 = choix_direction(joueur_courant);
	if(pion_mouvtest(pl, p, d1)){
	   	pos q = move(p, d1);
		mvmt_sans_danger(p , q , pl ,  joueur_courant ) ;
	}
	afficher_message(ILLEGAL);
	break;
    case T:
	d1 = choix_direction(joueur_courant);
	n = choix_n(joueur_courant);
	if (tour_mouvtest(pl, p, d1, n)){
	    pos q = move_mult(p, d1, n);
	    mvmt_sans_danger( p , q , pl ,  joueur_courant ) ;
	}
	afficher_message(ILLEGAL);
	break;

    case C:
	d1 = choix_direction(joueur_courant);
	d2 = choix_direction(joueur_courant);
	if (cheval_mouvtest(pl,  p, d1, d2)){
	    pos q1 = move(p,  d1);
	    pos q2 = move(q1, d2);
		mvmt_sans_danger( p , q2, pl , joueur_courant ) ;
	}
	afficher_message(ILLEGAL);
	break;

    case E:
	d1 = choix_direction(joueur_courant);
	if (elephant_mouvtest(pl, p, d1)){
	  	pos q = move_mult(p, d1, 2) ;
		mvmt_sans_danger( p , q, pl , joueur_courant ) ;
	}
	afficher_message(ILLEGAL);
	break;
    case G:
	d1 = choix_direction(joueur_courant);
	if (garde_mouvtest(pl, p, d1)){
	    pos q = move(p, d1) ;
	    mvmt_sans_danger( p , q, pl , joueur_courant ) ;
	}
	afficher_message(ILLEGAL);
	break;
    case R:
	d1 = choix_direction(joueur_courant);
	if (roi_mouvtest(pl, p, d1)){
	    pos q = move(p, d1) ;
	    mvmt_sans_danger( p , q, pl , joueur_courant ) ;
	}
	afficher_message(ILLEGAL);
	break;
    case B:
	d1 = choix_direction(joueur_courant);
	n  = choix_n(joueur_courant);
	if (canon_mouvtest(pl, p,  d1, n)){
	    pos q = move_mult(p, d1, n) ;
	    mvmt_sans_danger( p , q, pl , joueur_courant ) ;
	}
	afficher_message(ILLEGAL);
	break;
    }
    return figure;

}


int main(){
	regles_du_jeu();
    couleur_piece joueur_1;
    couleur_piece joueur_2;
    couleur_piece joueur_courant;
    joueur_1=0;
    joueur_2=1;
    joueur_courant=joueur_1;
    plateau* plat=malloc(sizeof(plateau));
    plat=plateau_initial();
    afficher_plateau(plat);
    do{
	printf("\nC'est au tour du joueur %d :\n \n", joueur_courant);
	pos p = choix_pion(joueur_courant);
	if(mon_pion(plat, joueur_courant, p)==1){
	    mouvement (p , plat , joueur_courant);
	    afficher_plateau(plat);}
        if (joueur_courant==joueur_1){
	    joueur_courant=joueur_2;
	}else{
	    joueur_courant=joueur_1;
	}
    }while(detecte_roi(plat ,  joueur_courant)  == 1 );
    if ( detecte_roi(plat ,  joueur_courant)  == 0 ){
    	printf("Fin de la partie , le joueur %d a gagné  \n " , !joueur_courant ) ;
    }
    return 0;
}

