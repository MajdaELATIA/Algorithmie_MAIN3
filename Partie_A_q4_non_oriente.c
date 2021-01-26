#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE1 20


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
int oriente = 0 ; // 0 : pas orienté  / 1 : oriente

/******************************************************************************************************************
 *Affichage du parours dfs avec GRAPHIZ
 *****************************************************************************************************************/
void print_graphviz(int nbrAretesParcourue, Aretes * resultats, char * ptfn){
   FILE * f ;
   int  i ;
   if ((f = fopen(ptfn,"w")) == NULL){
     printf("Erreur Ouverture en ecriture %s\n",ptfn) ;
      exit(3) ;
   }
   if (oriente == 0) {
      fprintf(f,"graphe {\n") ; // fprintf pour afficher dans un fichier et pas sur le terminal(standard out)
      for (i = 0 ; i < nbrAretesParcourue ; i ++){
	fprintf(f,"%3d -- %3d\n",resultats[i].depart,resultats[i].arrive) ;
      }
      fprintf(f,"}\n") ;
   }
   else{ // Graphe orienté
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
  // Destruction des liens en double
  if (oriente == 0){
    fprintf(f,"graph {\n") ; // fprintf pour afficher dans un fichier et pas sur le terminal(standard out)
    for (i = 1 ; i <= G.nbrAretes ; i ++){
      if (larcs->sens > 0){
	fprintf(f,"%3d -- %3d\n",larcs->depart, larcs->arrive) ;
      }
      larcs++ ;
    }
    fprintf(f,"}\n") ;
  }
  //graphe non  rienté
   else {
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
   int i ;
   int j = 0 ;
   Aretes * larcs = G.larcs ;
   printf("Nombre de sommets : %2d \n Nombre d'arêtes:  %2d\n",G.nbrSommets, G.nbrAretes) ;

   for (i = 0  ; i < G.nbrAretes ; i ++){
     printf("(%3d , %3d) ",larcs->depart,larcs->arrive) ;
     larcs ++ ; // On passe a l'arrete suivante
     j = j + 1 ;

     if (j == SIZE1){
       printf("\n") ; j = 0 ;
     }
   }
   if (j != 0){
     printf("\n") ;
   }
} 
// Affichage de la matrice d'adjacance de 1 à N (creuse)

/******************************************************************************************************************
 *On construit notre matrice adjacente, s'il y a un arc entre les sommets u et v on met 1 dans la matrice, 0 sinon
 *****************************************************************************************************************/
void buildMatriceAdjacente(){
  int i ;
// On alloue les pointeurs pour la première dimension)
  if ((graphe.mat_Adj = (int **) malloc(sizeof(int **) * (graphe.nbrSommets+1))) == NULL){
    printf("Erreur Allocation n°1 %d\n",graphe.nbrSommets+1) ;
    exit(10) ;
  }
  for (i = 0 ; i < graphe.nbrSommets+1 ; i ++){
    if ((graphe.mat_Adj[i] = (int *) calloc((size_t)(graphe.nbrSommets+1), sizeof(int*))) == NULL) // pour la ième ligne on reserve la mémoire suffisante
      {
	printf("Attention : erreur d'allocation n°2 %d indice %d\n",graphe.nbrSommets+1,i) ;
	exit(11) ;
      }
  }
  // Ajout des arretes
  for (i = 0 ; i < graphe.nbrAretes ; i ++){
    graphe.mat_Adj[graphe.larcs[i].depart][graphe.larcs[i].arrive]= 1 ;
  }

} 

/******************************************************************************************************************
 *Affichage des couples de sommets (u,v) liés du graphe
 *****************************************************************************************************************/
void printlisteAdj(Graphe G){
  int k = 0 ; // Pour limiter la taille d'une ligne
   printf("\n Matrice d'adjacence") ;
   printf("\n");

   for (int i = 1 ; i <= G.nbrSommets ; i ++){
     for (int j = 1 ; j <= G.nbrSommets ; j ++){
       if (G.mat_Adj[i][j] != 0){
	 printf("(%2d , %2d) = %2d ",i,j,G.mat_Adj[i][j]) ;
        k = k + 1 ;
        if (k == SIZE1){
	  printf("\n") ;
	  k = 0 ;
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
  printf("Matrice d'adjacence\n");
  for (int i = 1 ; i <= G.nbrSommets ; i ++){
    printf("M[%2d] : ",i) ;
    for (int j = 1 ; j <= G.nbrSommets ; j ++){
      printf("%2d ",G.mat_Adj[i][j]) ;
      if ((j % 10) == 0){
	printf(" - ") ;
      }
    }
    printf("\n") ;
  }
  printf("\n") ;
} 
/******************************************************************************************************************
 * DFS
 *****************************************************************************************************************/
void dfs(Graphe G, int n, int * nonMarque, int * nbrAretesParcourue, Aretes * res_sommets_decouv){
   nonMarque[n] = 0 ; // On passe par noeud n
   int trouve = 0 ;

   // Pour tous les voisins du sommet n
   for (int i = 1 ; i <= G.nbrSommets ; i++){
     if ((i != n) && (G.mat_Adj[n][i] != 0)) // Il existe un chemin pour aller de n à i
       {
	 if (nonMarque[i] == 1) // Et de plus le sommet n'est pas pris
	   {
	     Aretes ar ;
	     ar.depart = n ;
	     ar.arrive = i ;
	     res_sommets_decouv[*nbrAretesParcourue] = ar ;
	     *nbrAretesParcourue = *nbrAretesParcourue + 1 ;
	     dfs(G,i,nonMarque,nbrAretesParcourue, res_sommets_decouv) ;
	     trouve = 1 ;
	   }
       }
   }
#ifdef NE_MAFFICHE_PAS_CA_DANS_LE_TERMINAL_CAR_TROP_LONG_MAIS_ON_LE_MET_QUAND_MEME_HISTOIRE_DE_SIGNALER_QUON_A_TROUVE_UN_CHEMIN
   if (trouve == 1) // On a trouve un chemin du dfs
   { printf("On a trouvé un chemin DFS est le voici:\n")
     for (int i = 0 ; i < *nbrAretesParcourue ; i++){
       printf("Arrive[%3d] = (%2d,%2d)\n", i, res_sommets_decouv[i].depart,res_sommets_decouv[i].arrive) ; }
   }
#endif
}

/******************************************************************************************************************
 * DFS en récursif
 *****************************************************************************************************************/
void parcoursDFS(Graphe G, char * ptfn)
{
   int i ;
   int  * nonMarque ; // On n'est pas passé par ce sommet
   Aretes * res_sommets_decouv  ; // Liste des sommets de ce parcours
   int nbrAretesParcourue  = 0 ;

   if ((nonMarque = (int*) malloc((G.nbrSommets+1)*sizeof(int))) == NULL){
     printf("Erreur Allocation non Marque\n") ;
     exit(20) ;
   }

   if ((res_sommets_decouv = (Aretes*) malloc((G.nbrAretes)*sizeof(Aretes))) == NULL){
     printf("Erreur Allocation non parcours\n") ;
     exit(21) ;
   }
   
   for (int i = 1 ; i <= G.nbrSommets ; i ++){
     nonMarque[i] = 1 ;   // On est passé par aucun noeud
   }
   
   for (i = 1 ; i <= G.nbrSommets ; i ++){
     if (nonMarque[i] == 1) // Jamais utilisé
       {
	 dfs(G, i, nonMarque, &nbrAretesParcourue, res_sommets_decouv) ;
       }
   }
   
   print_graphviz(nbrAretesParcourue,res_sommets_decouv,ptfn) ;
}



/******************************************************************************************************************
 ** On part d'un sommet au choix  et on fait un dfs. Lorsque l'exploration DFS est terminée depuis ce premier sommet, tous les points doivent
 * être marqués.
 *****************************************************************************************************************/
void connexe(Graphe G)
{
   int i ;
   int estConnexe = 1 ; // On suppose que c'est connexe
   int  * nonMarque ; // Sommet non visité
   Aretes * res_sommets_decouv  ; // Liste des sommets de ce parcours
   int nbrAretesParcourue  = 0 ;

   if ((nonMarque = (int*) malloc((G.nbrSommets+1)*sizeof(int))) == NULL){
     printf("nonMarque n'a pas été correctement alloué\n") ;
     exit(20) ;
   }

   if ((res_sommets_decouv = (Aretes*) malloc((G.nbrAretes)*sizeof(Aretes))) == NULL){
     printf("Erreur\n") ;
     exit(21) ;
   }

   for (int i = 1 ; i <= G.nbrSommets ; i ++){
     nonMarque[i] = 1 ;   // On est passé par aucun noeud
   }

   // On va faire un dfs depuis un sommet pris "au hasard" : 1
   dfs(G, 1, nonMarque, &nbrAretesParcourue, res_sommets_decouv) ;

   for (i = 2 ; i <= G.nbrSommets ; i ++){
     if (nonMarque[i] == 1) // Jamais utilisé
       {
	 printf("Le sommet %d n'est pas connexe avec le sommet 1\n",i) ;
	 estConnexe = 0 ;
       }
   }
   
   if (estConnexe == 1){
     printf("Le graphe est connexe.\n") ; }
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
  if (G.mat_Adj != NULL){
    for (i = 0 ; i < G.nbrSommets+1; i ++){
      if (G.mat_Adj[i] != NULL){
	free(G.mat_Adj[i]) ;
      }
    }
    free(G.mat_Adj) ;
  }
}


/******************************************************************************************************************
 *Permet de lire les données du fichier .txt forunies
 *****************************************************************************************************************/

void readFile(char * nomFichier)
{
    FILE* f ;
     size_t taille = SIZE1;
    int    nbAr  ;
    Aretes * ptraretes ;
    int    sens  ;
    char len[SIZE1]; 
    char * buff = len ;
    char ** larcs = &buff ;
   
    if  (oriente == 1){
      sens = 1 ; 
    }
    else{
      sens = 2 ; // chemin pour a->b et b->a  
    }
    if ((f=fopen(nomFichier,"r")) == NULL){
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
    if ((graphe.larcs = (Aretes *)malloc(nb2*sens*sizeof(Aretes))) == NULL){
      printf("Erreur dans l'allocation memoire de %d\n",nb2) ;
      exit(2) ;
    }
    if (oriente == 0){
      graphe.nbrAretes = 2*nbAr ;
    }
    //Initialisation
    graphe.mat_Adj = NULL ; 
    ptraretes = graphe.larcs ;
    while (nbAr > 0)
    { //Lecture du fichier
      getline(larcs,&taille,f);
      char* sommet1=strtok(*larcs," ");
      int debut = atoi(sommet1);
      char* sommet2 = strtok(NULL," ");
      int arrive = atoi(sommet2);
      ptraretes->depart = debut ;
      ptraretes->arrive = arrive ;
      ptraretes->sens = 1 ;
      ptraretes ++ ;
      // Creation du chemin inverse pour graphe non orienté
      if (oriente == 0){
	ptraretes->depart = arrive ;
	ptraretes->arrive = debut ;
	ptraretes->sens = -1 ;
	ptraretes ++ ;
      }
      nbAr = nbAr - 1 ;
    }
    fclose(f);
    buildMatriceAdjacente() ;
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
  parcoursDFS(graphe,"/home/majda/Bureau/S2_Main3/Algorithmique/TP1_2_Majda_ELATIA/Images/graph-1-Apres_DFS.dot") ;
  parcoursDFS(graphe,"/home/majda/Bureau/S2_Main3/Algorithmique/TP1_2_Majda_ELATIA/Images/graph-1-Apres_DFS.txt") ;
  connexe(graphe) ;
  libere_Graphe(graphe) ;
  printf("On a fini... ou presque \n") ;
}

