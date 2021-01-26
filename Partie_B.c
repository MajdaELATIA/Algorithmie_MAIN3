#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE1 50
#define SIZE2 100




// Tous les graphes sont orientés
/******************************************************************************************************************
 *STRUCTURES
 *****************************************************************************************************************/
typedef struct{
  int depart ;
  int arrive ;
} Aretes ;

typedef struct{
  int index ; // Remplissage
  int voisins   ; // Nombre de voisin pour le sommet
  int * ptrvois ; // Pointeur vers un tableau qui contient les voisins du sommet (orienté)
  
} listeAdjacence ;

// On met ensemble le graphe et les éléments pour l'algo des composants fortement connexes
typedef struct{
  int nbrSommets ;
  int nbrAretes  ;  
  Aretes *larcs;
  listeAdjacence * direct ;
  listeAdjacence * transpose ;
  int * sommetvisite ; // On va noter les sommets visités dans le premier dfs
  int * listeNoeuds ; // Liste des noueds via le premier dfs
  int indexNoeud ;
  int * cycle   ; // On va noter les sommets visités dans le deuxieme DFS (on ne peut pas réutiliser listeNoued)
  int indCycle  ; // Index pour écrire dans le tableau cycle
  int nbBloc    ; // Nb de bloc ou de composantes fortement connexes trouvés. Ils sont reportés dans sommetvisite
} Graphe ;

Graphe graphe ;

/******************************************************************************************************************
 *Permet de lire les données du fichier .txt forunies
 *****************************************************************************************************************/

void readFile(char * nomFichier){
  FILE* f ;
  char len[SIZE1]; 
  char * buff = len ;
  char ** larcs = &buff ;
  size_t taille = SIZE1;
  int    nbAr  ;
  Aretes * ptraretes ;
  int i;
  
  if ((f=fopen(nomFichier,"r")) == NULL){
    printf("Erreur, lecture fichier (%s)\n",nomFichier) ;
    exit(1) ;
  }
  
  // On connait le format du fichier et on fait confiance
  // On commence par le nombre de sommet
  getline(larcs,&taille,f);
  int nb1 = atoi(len);
  graphe.nbrSommets = nb1 ;
  graphe.nbBloc = 0 ; // On a trouvé aucun bloc ou composant connexe pour l'instant
  
  // On Lit le nombre d'arrete
  getline(larcs,&taille,f);
  int nb2 = atoi(len);
  graphe.nbrAretes = nbAr = nb2  ;
  graphe.indexNoeud = 0 ;
  
  if ((graphe.larcs = (Aretes *)calloc((size_t)(graphe.nbrAretes),sizeof(Aretes))) == NULL){
    printf("Erreur dans l'allocation memoire de %d aretes \n",graphe.nbrAretes) ;
    exit(2) ;
  }
  
  if ((graphe.direct = (listeAdjacence *)calloc((size_t)(graphe.nbrSommets+1),sizeof(listeAdjacence))) == NULL){
      printf("Erreur dans l'allocation memoire de %d\n",graphe.nbrSommets) ;
      exit(3) ;
  }
  
    if ((graphe.transpose = (listeAdjacence *)calloc((size_t)(graphe.nbrSommets+1),sizeof(listeAdjacence))) == NULL){
      printf("Erreur dans l'allocation pour transpose memoire de %d\n",graphe.nbrSommets) ;
      exit(4) ;
    }
    
    if ((graphe.sommetvisite = (int *)calloc((size_t)(graphe.nbrSommets+1),sizeof(int))) == NULL){
      printf("Erreur dans l'allocation pour sommetvisite memoire de %d\n",graphe.nbrSommets) ;
      exit(5) ;
    }
    
    if ((graphe.cycle   = (int *)calloc((size_t)(graphe.nbrSommets+1),sizeof(int))) == NULL){
     printf("Erreur dans l'allocation pour cycle memoire de %d\n",graphe.nbrSommets) ;
     exit(6) ;
    }
    
    if ((graphe.listeNoeuds   = (int *)calloc((size_t)(graphe.nbrSommets+1),sizeof(int))) == NULL){
      printf("Erreur dans l'allocation pour cycle liste des noeuds de %d\n",graphe.nbrSommets) ;
      exit(7) ;
    }
    
    ptraretes = graphe.larcs ;
    while (nbAr > 0){
      // On lit toute les lignes du fichers (sans controle d'erreur))
      getline(larcs,&taille,f);
      char* sommet1=strtok(*larcs," ");
      int debut = atoi(sommet1);
      char* sommet2 = strtok(NULL," ");
      int arrive = atoi(sommet2);
      ptraretes->depart = debut ;
      ptraretes->arrive = arrive ;
      ptraretes ++ ;
      nbAr = nbAr - 1 ;
       // Compter le nombre de voisins
      graphe.direct[debut].voisins ++ ; // Direct on part du dep
      graphe.transpose[arrive].voisins ++ ; // // Tranpose on part de l'arrivée
    }
    
    fclose(f);
    
    // On construit les listes adjacentes - On commence par allouer les tableaux des lignes
    for (int i = 0 ; i < graphe.nbrSommets+1 ; i ++){
      if ((graphe.direct[i].ptrvois = (int *) malloc(graphe.direct[i].voisins* sizeof(int))) == NULL){ // pour la ième ligne on reserve la mémoire suffisante
	printf("Erreur direct %d %d \n",i,graphe.direct[i].voisins) ;
	exit(11) ;
      }
      if ((graphe.transpose[i].ptrvois = (int *) malloc(graphe.direct[i].voisins* sizeof(int))) == NULL){ // pour la ième ligne on reserve la mémoire suffisante
	printf("Erreur transpose %d %d \n",i,graphe.direct[i].voisins) ;
	exit(12) ;
      }
    }
    
    // On va remplir les listes d'adjacence
    for ( i = 0 , ptraretes = graphe.larcs ; i < graphe.nbrAretes ; i ++){
      graphe.direct[ptraretes->depart].ptrvois[graphe.direct[ptraretes->depart].index] = ptraretes->arrive ;
      graphe.direct[ptraretes->depart].index = graphe.direct[ptraretes->depart].index + 1 ;
      graphe.transpose[ptraretes->arrive].ptrvois[graphe.transpose[ptraretes->arrive].index] = ptraretes->depart ;
      graphe.transpose[ptraretes->arrive].index = graphe.transpose[ptraretes->arrive].index + 1 ;
      ptraretes ++ ;
    }
}


/******************************************************************************************************************
 *Affichage des données du Graphe
 *****************************************************************************************************************/
void printGraphe(Graphe G){
  int j=0;
  Aretes * larcs = G.larcs ;
  printf("Nombre de sommets : %2d \n Nombre d'arêtes:  %2d\n",G.nbrSommets, G.nbrAretes) ;
  printf("\nLes aretes lues sont dans cet ordre:\n") ;
  for (int i = 0  ; i < G.nbrAretes ; i ++) {
    printf("(%3d , %3d) ",larcs->depart,larcs->arrive) ;
    larcs ++ ; // On passe a l'arrete suivante
    j = j + 1 ;
    if (j == SIZE1) 
      { printf("\n") ; j = 0 ; }
  }
  if (j != 0){
    printf("\n") ; 
  }
  
  printf("Les lignes d'adjacence directes : \n") ;
  for (int i = 1 ; i <= G.nbrSommets ; i ++){
    listeAdjacence ladja  = graphe.direct[i] ;
    printf("\n Arcs sortant du sommet %d dirigé vers le sommet voisin %d:  ",i,ladja.voisins) ;
    for (j = 0 ; j < ladja.voisins ; j ++){
      printf("%2d ,",ladja.ptrvois[j]) ;
    }
    printf("\n") ;
  }
  printf ("\n-----------------------------------------------\n");
  printf("\n Les lignes d'adjacence transpose : \n") ;
  for (int i = 1 ; i <= G.nbrSommets ; i ++){
    listeAdjacence ladja  = graphe.transpose[i] ;
    printf("\n Arcs entrant vers le sommet  %d sortant de %d :  ",i,ladja.voisins) ;
    for (int j = 0 ; j < ladja.voisins ; j ++){
      printf("%2d ,",ladja.ptrvois[j]) ;
    }
    printf("\n") ;
  }
} 

/******************************************************************************************************************
 *Affichage de la liste de noeuds. Dans l'ordre du parcours dfs
 *****************************************************************************************************************/
void printListeNoeuds(Graphe G){
  printf("\n Ordre des noeuds obtenu par le premier DFS:\n") ;
  for (int i = 0 ; i < graphe.indexNoeud ; i ++){
    	printf("%d , ",graphe.listeNoeuds[i]) ;
  }
  printf("\n") ;
}

/******************************************************************************************************************
 *Affichage de la liste de noeuds. Dans l'ordre du parcours dfs
 *****************************************************************************************************************/
void DFS(Graphe G,int sommet_actuel, int * idx, int * lst){
   int i ;
   G.sommetvisite[sommet_actuel] = 1 ;
  for (i = 0 ; i < G.direct[sommet_actuel].voisins ; i ++){
    int voisin = G.direct[sommet_actuel].ptrvois[i] ;
    if  (G.sommetvisite[voisin] == 0){
      DFS(G,voisin, idx, lst) ;
    }
  }
  lst[*idx] = sommet_actuel ;
  *idx = * idx + 1 ;
} 

/******************************************************************************************************************
 *Parcours du dfs en sens inverse en utilisant la matrice d'adjacence
 *****************************************************************************************************************/
void dfsTranspose(Graphe G,int sommet_actuel, int * idx, int * lst){
  int i ;
  printf("DFStranspo %d\n",sommet_actuel) ;
  G.sommetvisite[sommet_actuel] = -1 ; // -1 = visité, 0 non visité , 1 à N numéro du bloc
  for (i = 0 ; i < G.transpose[sommet_actuel].voisins ; i ++){
    int voisin = G.transpose[sommet_actuel].ptrvois[i] ;
    if  (G.sommetvisite[voisin] == 0){
      dfsTranspose(G,voisin, idx, lst) ;
    }
  }
  lst[*idx] = sommet_actuel ;
  *idx = * idx + 1 ;
} 


/******************************************************************************************************************
 *Récupération de la liste de noeuds lors de DFS
 *****************************************************************************************************************/
void dfs1(Graphe *G){
  int i ;
  for (i = 1 ; i <= G->nbrSommets ; i ++){
    if (G->sommetvisite[i] == 0){
      DFS(*G,i,&G->indexNoeud, G->listeNoeuds) ; }
  }
}

/******************************************************************************************************************
 *TEST
 *****************************************************************************************************************/
int main(){
  int cycleNum ;
  FILE * f  ;
  char inBloc [SIZE2]  ; 
  char outBloc[SIZE2] ;
  readFile("/home/majda/Bureau/S2_Main3/Algorithmique/TP1_2_Majda_ELATIA/Interfaces/digraph-1.txt") ;
  printGraphe(graphe) ;
  dfs1(&graphe) ;
  printListeNoeuds(graphe) ;
   // On remet à zéro sommetvisite pour le deuxieme parcours DFS
  for (int i = 1 ; i <= graphe.nbrSommets ; i ++){
    graphe.sommetvisite[i] = 0 ;
  }
  for (int i = graphe.indexNoeud -1 ; i >=0 ; i --) // On parcours dans l'ordre inverse pour le deuxième DFS
    {
      int sommet = graphe.listeNoeuds[i] ;
      printf("dfs départ en %d\n",sommet) ;
      graphe.indCycle = 0 ; // On recherche un nouveau composant connexe
      if (graphe.sommetvisite[sommet] == 0){
	dfsTranspose(graphe, sommet, &graphe.indCycle, graphe.cycle) ;
	graphe.nbBloc = graphe.nbBloc + 1 ; // On a trouvé le nouveau bloc Graph.nbBloc (on commence à 1)
	// On a trouvé un cycle, ou un bloc connexe
	for (int k = 0 ; k < graphe.indCycle ; k ++){
	  graphe.sommetvisite[graphe.cycle[k]] = graphe.nbBloc ; // Le sommet graphe.cycle[k] appartient au bloc graphe.nbBloc
	  printf("Le bloc %d contient le sommet %d\n",graphe.nbBloc,graphe.cycle[k]) ;
	}
	printf ("\n ----------------------\n");
      }
    }

  // On va créer un fichier pour graphViz
  f = fopen("/home/majda/Bureau/S2_Main3/Algorithmique/TP1_2_Majda_ELATIA/Images/digraph-1_fortconnexe.txt","w") ;
  fprintf(f,"digraph {\n") ;
  //printf("On va créer le fichier dot pour %d blocs\n",Graph.nbBloc) ;
  for (cycleNum = 1 ; cycleNum <= graphe.nbBloc ; cycleNum ++){
    inBloc[0]  = (char)'\0' ;
    outBloc[0] = (char)'\0' ;
    int sommet_actuel ;
    for (int k = 1 ; k <= graphe.nbrSommets ; k ++) // On recherche tout les sommets du bloc k
	  {
	    if (graphe.sommetvisite[k] == cycleNum) // Le sommet k est dans le cycle cycleNum
	      {
		sommet_actuel = k ;
		int nbVoisin  = graphe.direct[sommet_actuel].voisins ;
		for (int l = 0 ; l < nbVoisin ; l ++) // On regarde les voisins du sommet courant
		  {
		    int sv = graphe.direct[sommet_actuel].ptrvois[l] ; // Valeur du L ieme voisin de sommet_actuel, c'est un sommet voisin
		    if (graphe.sommetvisite[sv] == cycleNum)
		      { 
			sprintf(inBloc,"%s %d -> %d[color=red]\n",inBloc,sommet_actuel,sv) ; // en rouge
		      }
		    else{
		      sprintf(outBloc,"%s %d -> %d [color=green]\n",outBloc,sommet_actuel,sv) ;//en vert
		    }
		  }
	      }
	    
    	}//Si on est dans la composante/bloc connexe alors:
    if (strlen(inBloc) == 0) 
      {
	       fprintf(f,"subgraph cluster_%d {\n label=\"Bloc n°%d fortement connexe\"\n%d\n}\n%s\n",cycleNum,cycleNum,sommet_actuel,outBloc) ;// je renomme les sommets (de l'esthétique)
        
      }
    else
      {
        fprintf(f,"subgraph cluster_%d {\n label=\"Bloc n°%d fortement connexe\"\n%s\n}\n%s\n",cycleNum,cycleNum,inBloc,outBloc) ;
      }
  }
  fprintf(f,"}\n") ;


  fclose(f) ;
  printf("Fin de la partie B\n") ;
  return(0) ;
}
