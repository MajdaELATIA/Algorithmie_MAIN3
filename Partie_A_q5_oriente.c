#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE1 20

// Pour partie A, question 5, tous les graphes sont orientés
int oriente = 1 ; // 0 : pas orienté  / 1 : oriente

/******************************************************************************************************************
 *STRUCTURES
 *****************************************************************************************************************/
typedef struct Aretes{
  int depart ;
  int arrive ;
  int sens;
} Aretes ;

typedef struct Graphe
{
  int oriente  ; 
  int nbrSommets ;
  int nbrAretes  ;  
  Aretes * larcs   ;  // Liste qui contient les arcs reliant les sommets entre eux du graphe 
  int ** mat_Adj ;
} Graphe ;

Graphe graphe ;
Graphe grapheInv ;

/******************************************************************************************************************
 *Affichage du parours dfs avec GRAPHIZ
 *****************************************************************************************************************/
void print_graphviz(int nbrAretesParcourue, Aretes * resultats, char * ptfn){
  FILE * f ;
  int  i ;
  if ((f = fopen(ptfn,"w")) == NULL)
    {  printf("Erreur Ouverture en ecriture %s\n",ptfn) ;
      exit(3) ;
    }
  
  if (oriente == 0){
    fprintf(f,"graphe {\n") ; // fprintf pour afficher dans un fichier et pas sur le terminal(standard out)
    for (i = 0 ; i < nbrAretesParcourue ; i ++){
      fprintf(f,"%3d -- %3d\n",resultats[i].depart,resultats[i].arrive) ;
    }
      fprintf(f,"}\n") ;
  }
  else // Graphe orienté
    {
      fprintf(f,"digraph {\n") ; // fprintf pour afficher dans un fichier et pas sur le terminal(standard out)
      for (i = 0 ; i < nbrAretesParcourue; i ++){
	fprintf(f,"%3d -> %3d\n",resultats[i].depart,resultats[i].arrive) ;
      }
      fprintf(f,"}\n") ;
    }
  fclose(f) ;
} 

/******************************************************************************************************************
 *Affichage du parours du graphe en utilsant graphviz
 *****************************************************************************************************************/
void creation_en_dot(Graphe G, char * nomFichier){
  FILE * f ;
  int  i ;
  Aretes  * larcs = G.larcs ;
  if ((f = fopen(nomFichier,"w")) == NULL){
    printf("Erreur Ouverture en ecriture %s\n",nomFichier) ;
     exit(3) ;
  }

  if (oriente == 0) // Il faut penser à éliminer nos liens dédoublés
    {
      fprintf(f,"graph {\n") ; // fprintf pour afficher dans un fichier et pas sur le terminal(standard out)
      for (i = 1 ; i <= G.nbrAretes ; i ++)
	{
          larcs++ ;
	}
      fprintf(f,"}\n") ;
    }
  else // Graphe orienté
    {
      fprintf(f,"digraph {\n") ; // fprintf pour afficher dans un fichier et pas sur le terminal(standard out)
      for (i = 1 ; i <= G.nbrAretes ; i ++){
	fprintf(f,"%3d -> %3d\n",larcs->depart, larcs->arrive) ;
	larcs++ ;
      }
      fprintf(f,"}\n") ;
    }
  fclose(f) ;
}

/******************************************************************************************************************
 *Affichage des données du Graphe
 *****************************************************************************************************************/
void printGraphe( Graphe G){
  int j = 0 ;
  Aretes * larcs = G.larcs ;
  printf("Nombre de sommets : %2d \n Nombre d'arêtes:  %2d\n",G.nbrSommets, G.nbrAretes) ;
  for (int i = 0  ; i < G.nbrAretes ; i ++){
    printf("(%3d , %3d) ",larcs->depart,larcs->arrive) ;
    larcs ++ ; // On passe a l'arrete suivante
    j = j + 1 ;
    if (j == SIZE1) // Formatage de la sortie : limitation de la longueur de la ligne
      { printf("\n") ; j = 0 ;
      }
  }
  if (j != 0){
    printf("\n") ;
  }
} 

/******************************************************************************************************************
 *On construit notre matrice adjacente, s'il y a un arc entre les sommets u et v on met 1 dans la matrice, 0 sinon
 *****************************************************************************************************************/
void buildMatriceAdjacente(Graphe * G){
  int i ;
  // On alloue les pointeurs pour la première dimension)
  if ((G->mat_Adj = (int **) malloc(sizeof(int **) * (G->nbrSommets+1))) == NULL){
    printf("Erreur Allocation n°1 %d\n",G->nbrSommets+1) ;
    exit(10) ;
  }
  for (i = 0 ; i < G->nbrSommets+1 ; i ++){
    if ((G->mat_Adj[i] = (int *) calloc((size_t)(G->nbrSommets+1), sizeof(int*))) == NULL) // pour la ième ligne on reserve la mémoire suffisante
      {
	printf("Erreur Allocation n°2 %d Indice %d\n",G->nbrSommets+1,i) ;
	exit(11) ;
      }
  }
  // On complete avec les arretes
  for (i = 0 ; i < G->nbrAretes ; i ++)
    {
      G->mat_Adj[G->larcs[i].depart][G->larcs[i].arrive]= 1 ;
    }
}


/******************************************************************************************************************
 *Affichage des couples de sommets (u,v) liés du graphe
 *****************************************************************************************************************/
void printlisteAdj(Graphe G){
  int k = 0 ; // Pour limiter la taille d'une ligne
  printf("\n Lien entre les differents sommets et aretes qui nous permettront de réaliser notre matrice d'adjacence: \n");
  for (int i = 1 ; i <= G.nbrSommets ; i ++)
    {
      for (int j = 1 ; j <= G.nbrSommets ; j ++)
	{
	  if (G.mat_Adj[i][j] != 0)
	    {
	      printf("(%2d , %2d) = %2d ",i,j,G.mat_Adj[i][j]) ;
	      k = k + 1 ;
	      if (k == SIZE1) // Formatage de la sortie : limitation de la longueur de la ligne
		{ printf("\n") ; k = 0 ;
		}
	    }
	}
    }
  if (k != 0){
    printf("\n") ;
  }
}
/******************************************************************************************************************
 *Affichage de la matrice d'adjacence
 *****************************************************************************************************************/
void printMatriceAdj(Graphe G){
   int i ;
   int j ;

   printf("Matrice d'adjacence\n");

   for (i = 1 ; i <= G.nbrSommets ; i ++)
   {
	   printf("M[%2d] : ",i) ;
	   for (j = 1 ; j <= G.nbrSommets ; j ++)
	   {
	   printf("%2d ",G.mat_Adj[i][j]) ;
           if ((j % 10) == 0)
           { printf(" - ") ; }

	   }
	   printf("\n") ;
   }

   printf("\n") ;
} 

/******************************************************************************************************************
 * DFS
 *****************************************************************************************************************/

void dfs(Graphe G, int n, int * nonMarque, int * nbrAretesParcourue, Aretes * resultats){
  nonMarque[n] = 0 ; // On passe par noeud n
  int trouve = 0 ;
  // Pour tous les voisins du sommet n
  for (int i = 1 ; i <= G.nbrSommets ; i++)
    {
      if ((i != n) && (G.mat_Adj[n][i] != 0)) // Il existe un chemin pour aller de n à i
	{
	  if (nonMarque[i] == 1) // Et de plus le sommet n'est pas pris
	    {
	      Aretes ar ;
	      ar.depart = n ;
	      ar.arrive = i ;
	      resultats[*nbrAretesParcourue] = ar ;
	      *nbrAretesParcourue = *nbrAretesParcourue + 1 ;
	      dfs(G,i,nonMarque,nbrAretesParcourue, resultats) ;
	      trouve = 1 ;
	    }
	}
    }

#ifdef NE_MAFFICHE_PAS_CA_DANS_LE_TERMINAL_CAR_TROP_LONG_MAIS_ON_LE_MET_QUAND_MEME_HISTOIRE_DE_SIGNALER_QUON_A_TROUVE_UN_CHEMIN
  if (trouve == 1) // On a trouve un chemin du dfs
    {
      int i = 0 ;
      for (i = 0 ; i < *nbrAretesParcourue ; i++)
	{ printf("Arrive[%3d] = (%2d,%2d)\n", i, resultats[i].depart,resultats[i].arrive) ;
	}
    }
#endif
}

/******************************************************************************************************************
 * DFS en récursif
 *****************************************************************************************************************/

void parcoursDFS(Graphe G, char * ptfn){
   int i ;
   int  * nonMarque ; // On n'est pas passé par ce sommet
   Aretes * resultats  ; // Liste des sommets de ce parcours
   int nbrAretesParcourue  = 0 ;
   
   if ((nonMarque = (int*) malloc((G.nbrSommets+1)*sizeof(int))) == NULL)
   { printf("Erreur Allocation non Marque\n") ;
     exit(20) ;
   }
   
   if ((resultats = (Aretes*) malloc((G.nbrAretes)*sizeof(Aretes))) == NULL)
   { printf("Erreur Allocation non parcours\n") ;
     exit(21) ;
   }
   
   for (int i = 1 ; i <= G.nbrSommets ; i ++)
   {
     nonMarque[i] = 1 ;   // On est passé par aucun noeud
   }
   
   for (i = 1 ; i <= G.nbrSommets ; i ++)
     {  if (nonMarque[i] == 1) {
	 dfs(G, i, nonMarque, &nbrAretesParcourue, resultats) ;
	 printf("\n Parcours DFS\n") ;
	 for (int j = 0 ; j < nbrAretesParcourue ; j ++)
	   {
	     printf("Arete[%d] : %d -> %d \n",j,resultats[j].depart,resultats[j].arrive) ;
	   }
       }
     }
}

/******************************************************************************************************************
 * Idée: faire un dfs à partir de la matrice adjacente pour obtenir notre graphe inverse
 *****************************************************************************************************************/
void grapheInverse(Graphe graphe, Graphe * ptGraphInverse){
  Aretes * ptlSource ;
  Aretes * ptlDest ;
  int  nbArr ;
  ptGraphInverse->oriente  = graphe.oriente ;
  ptGraphInverse->nbrSommets = graphe.nbrSommets ;
  ptGraphInverse->nbrAretes  = nbArr = graphe.nbrAretes ;
  
  // Il faut mettre à jour la liste des arêtes
  if ((ptGraphInverse->larcs = (Aretes *)malloc(ptGraphInverse->nbrAretes*sizeof(Aretes))) == NULL){
    printf("Erreur dans l'allocation memoire de %d\n",ptGraphInverse->nbrAretes) ;
    exit(20) ;
    }
  ptGraphInverse->mat_Adj = NULL ; //on initialise
  ptlSource = graphe.larcs ;
  ptlDest   = ptGraphInverse->larcs ;
  while (nbArr > 0){
    ptlDest->depart= ptlSource->arrive ;
    ptlDest->arrive = ptlSource->depart ;
    nbArr = nbArr - 1 ;
    ptlSource ++ ;
    ptlDest ++ ;
  }
  buildMatriceAdjacente(ptGraphInverse) ;
}
/******************************************************************************************************************
 ** On part d'un sommet au choix  et on fait un dfs. Lorsque l'exploration DFS est terminée depuis ce premier sommet, tous les points doivent
 * être marqués.
 *****************************************************************************************************************/
void connexe(Graphe G){
  int i ;
  int estConnexe = 1 ; // On suppose que c'est connexe
  int  * nonMarque ; // On n'est pas passé par ce sommet
  Aretes * resultats  ; // Liste des sommets de ce parcours
  int nbrAretesParcourue  = 0 ;
  
   if ((nonMarque = (int*) malloc((G.nbrSommets+1)*sizeof(int))) == NULL)
     { printf("Erreur Allocation non Marque\n") ;
     exit(20) ;
     }

   if ((resultats = (Aretes*) malloc((G.nbrAretes)*sizeof(Aretes))) == NULL)
     { printf("Erreur Allocation non parcours\n") ;
       exit(21) ;
   }
   
   for (int i = 1 ; i <= G.nbrSommets ; i ++){
     nonMarque[i] = 1 ;   // On est passé par aucun noeud
   }
   
   // On va faire un dfs depuis un sommet pris "au hasard" : 1
   dfs(G, 1, nonMarque, &nbrAretesParcourue, resultats) ;
   for (i = 2 ; i <= G.nbrSommets ; i ++){
     if (nonMarque[i] == 1) // Jamais utilisé
       {
	 printf("Le sommet %d n'est pas connexe avec le sommet 1\n",i) ;
	 estConnexe = 0 ;
       }
   }
   
   if (estConnexe == 1){
     printf("Le graph est connexe.\n") ; }
   else{
     printf("Le graphe n'est pas connexe!\n") ; }
}

/******************************************************************************************************************
 *Libère la structure Graphe
 *****************************************************************************************************************/

void libere_Graphe(Graphe G){
  int i ;
  if (G.larcs != NULL){
    free(G.larcs) ;
  }
  
  if (G.mat_Adj != NULL)
    { // On libère la dimension 2
      for (i = 0 ; i < G.nbrSommets+1; i ++){
	  if (G.mat_Adj[i] != NULL){
	    free(G.mat_Adj[i]) ;
	  }
      }
      // On libère la dimension 1
      free(G.mat_Adj) ;
    }
}

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
  int    sens  ;
  
  if   (oriente == 0){
    sens = 2 ; // On va allouer une case pour 1->2 et une pour 2->1
  }
  else
    {
      sens = 1 ; // Graphe orienté done 1,2 signifie 1 -> 2
    }
  
  if ((f=fopen(nomFichier,"r")) == NULL)
    {
      printf("Erreur, lecture fichier (%s)\n",nomFichier) ;
      exit(1) ;
    }
  
// On commence par le nombre de sommet
    getline(larcs,&taille,f);
    int nb1 = atoi(len);
    graphe.nbrSommets = nb1 ;

// On lit le nombre d'arrete
    getline(larcs,&taille,f);
    int nb2 = atoi(len);
    graphe.nbrAretes = nbAr = nb2 ;

    if (oriente == 0)
    { graphe.nbrAretes = 2*nbAr ; }

    if ((graphe.larcs = (Aretes *)malloc(nb2*sens*sizeof(Aretes))) == NULL)
    { printf("Erreur dans l'allocation memoire de %d\n",nb2) ;
      exit(2) ;
    }
    
    graphe.mat_Adj = NULL ; //on initialise

    ptraretes = graphe.larcs ;
    while (nbAr > 0)
      { // On lit toute les lignes du fichers (sans controle d'erreur))
	getline(larcs,&taille,f);
	
	char* sommet1=strtok(*larcs," ");
	int debut = atoi(sommet1);
	char* sommet2 = strtok(NULL," ");
	int arrive = atoi(sommet2);
	ptraretes->depart = debut ;
	ptraretes->arrive = arrive ;
	ptraretes ++ ;
	nbAr = nbAr - 1 ;
      }
    fclose(f);
    buildMatriceAdjacente(&graphe) ;
} 

/******************************************************************************************************************
 * TEST
 *****************************************************************************************************************/

int main(){
  readFile("/home/majda/Bureau/S2_Main3/Algorithmique/TP1_2_Majda_ELATIA/Interfaces/graph-1.txt") ;
  printGraphe(graphe) ; // Graph c'est la structure remplie par lecture du fichier graph.txt
  printlisteAdj(graphe) ;
  printMatriceAdj(graphe) ;
  creation_en_dot(graphe,"/home/majda/Bureau/S2_Main3/Algorithmique/TP1_2_Majda_ELATIA/Images/graph-1-Avant_DFS.dot") ;
  creation_en_dot(graphe,"/home/majda/Bureau/S2_Main3/Algorithmique/TP1_2_Majda_ELATIA/Images/graph-1-Avant_DFS.txt") ;
  parcoursDFS(graphe,"/home/majda/Bureau/S2_Main3/Algorithmique/TP1_2_Majda_ELATIA/Images/graph-1-Apres_DFS.txt") ;
  //connexe(Graph) ;

  grapheInverse(graphe, &grapheInv) ;
  creation_en_dot(grapheInv,"/home/majda/Bureau/S2_Main3/Algorithmique/TP1_2_Majda_ELATIA/Images/graph-1-inv.txt") ;
  creation_en_dot(grapheInv,"/home/majda/Bureau/S2_Main3/Algorithmique/TP1_2_Majda_ELATIA/Images/graph-1-inv.dot") ; 
  printf("\n Matrice d'adjacence du graphe inverse : \n");
  printMatriceAdj(grapheInv) ;
  //connexe (grapheInv);
  libere_Graphe(graphe) ;
  libere_Graphe(grapheInv) ;
  printf("On a fini la partie A  \n") ;
}
