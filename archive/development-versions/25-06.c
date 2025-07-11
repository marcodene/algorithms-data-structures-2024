#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BLACK 0
#define RED 1


//--------------------------------	DICHIARAZIONE TIPI DI DATO	--------------------------------
//corriere
typedef struct{
	int periodicità;
	int capienza;
} corriere_struct;



//dispensa
typedef struct {
	int scadenza;
	int quantità;
}lotto;

typedef struct {
	lotto * data;
	int length;
	int heapsize;
}lotti_heap;

typedef struct almnt{
	char nome[255];
	lotti_heap lotti;
	int color; //0=BLACK and 1=RED
	struct almnt * left;
	struct almnt * right;
	struct almnt * parent;
}alimento_nodo;

//ricette
typedef struct ingr{
	alimento_nodo * alimento;
	int quantità;
	struct ingr * next;
}ingrediente_nodo;

typedef struct rct{
	char nome[255];
	ingrediente_nodo * ingredienti;
	int peso;
	int color;	//0=BLACK and 1=RED
	struct rct * left;
	struct rct * right;
	struct rct * parent;
}ricetta_nodo;







//--------------------------------	VARIABILI GLOBALI	--------------------------------
char istruzione[255];
int now = 0;
corriere_struct corriere;

ricetta_nodo* ricettario;
alimento_nodo* dispensa;



//--------------------------------	FUNZIONI AUSILIARIE	--------------------------------

// controlla che la stringa passata per input sia un'istruzione valida
int is_istruzione(char*str){
	return !(strcmp(str, "aggiungi_ricetta") && strcmp(str, "rimuovi_ricetta") && strcmp(str, "ordine") && strcmp(str, "rifornimento") && strcmp(str, "stampa_dispensa") && strcmp(str, "stampa_ricettario"));
}

void stampa_dispensa(alimento_nodo*alimento, int livello){
	if (alimento == NULL)
        return;
    
    printf("livello %d, value: %s, colore: %c padre:%s   %s\n",
        livello, 
        alimento->nome,
        ((alimento->color)==BLACK?'b':'r'), 
        alimento->parent!=NULL?(alimento->parent)->nome:"-1",
        (alimento->parent!=NULL&&alimento->parent->left==alimento)?"sinistro":"destro"
    );
    
    
    stampa_dispensa(alimento->left, livello+1);
    stampa_dispensa(alimento->right, livello+1);
}

void stampa_ricettario(ricetta_nodo*ricetta, int livello){
	if (ricetta == NULL)
        return;
    
    printf("livello %d, value: %s, colore: %c padre:%s   %s\n",
        livello, 
        ricetta->nome,
        ((ricetta->color)==BLACK?'b':'r'), 
        ricetta->parent!=NULL?(ricetta->parent)->nome:"-1",
        (ricetta->parent!=NULL&&ricetta->parent->left==ricetta)?"sinistro":"destro"
    );
    
    
    stampa_ricettario(ricetta->left, livello+1);
    stampa_ricettario(ricetta->right, livello+1);
}



ricetta_nodo* ricerca_ricetta_in_ricettario(char* nome_ricetta){
	return NULL;
}

alimento_nodo* ricerca_alimento_in_dispensa(char* nome_alimento){
	return NULL;
}


void LeftRotate_ricettario(ricetta_nodo**root,ricetta_nodo*x){
    ricetta_nodo*y = x->right;
    x->right = y->left;
    if (x->right != NULL)
        x->right->parent = x;
    y->parent = x->parent;
    if (x->parent == NULL)
        (*root) = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else    x->parent->right = y;
    y->left = x;
    x->parent = y;
}

void RightRotate_ricettario(ricetta_nodo**root,ricetta_nodo*y){
    ricetta_nodo*x = y->left;
    y->left = x->right;
    if (x->right != NULL)
        x->right->parent = y;
    x->parent =y->parent;
    if (x->parent == NULL)
        (*root) = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else y->parent->right = x;
    x->right = y;
    y->parent = x;
}

void aggiungi_nuova_ricetta_in_ricettario_FIXUP(ricetta_nodo **root,ricetta_nodo *z){
    while (z->parent != NULL && z->parent->parent != NULL && z->parent->color == RED)
    {
        ricetta_nodo *y;
        if (z->parent == z->parent->parent->left){
            y = z->parent->parent->right;
            if (y!= NULL && y->color == RED){
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }else{
                if (z == z->parent->right){
                    z = z->parent;
                    LeftRotate_ricettario(root, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                RightRotate_ricettario(root, z->parent->parent);
            }
        }
        else{
           y = z->parent->parent->left;
            if (y!= NULL && y->color == RED){
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }else{
                if (z == z->parent->left){
                    z = z->parent;
                    RightRotate_ricettario(root, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                LeftRotate_ricettario(root, z->parent->parent);
            } 
        }
    }   
    
    (*root)->color = BLACK;
}

ricetta_nodo* aggiungi_nuova_ricetta_in_ricettario (ricetta_nodo**root, char*nome_ricetta){
    ricetta_nodo*z = (ricetta_nodo*)malloc(sizeof(ricetta_nodo));
    strcpy(z->nome, nome_ricetta);
    z->left = z->right = z->parent = NULL;
    
    ricetta_nodo*x = *root;
    ricetta_nodo*y= NULL;
    while(x!=NULL){
        y=x;
        if(strcmp(z->nome, x->nome)<0)
            x = x->left;
        else
            x = x->right;
    }
    z->parent = y;
    if(y==NULL)
        *root=z;
    else if(strcmp(z->nome, y->nome)<0)
        y->left = z;
    else
        y->right = z;
    z->left = NULL;
    z->right = NULL;
    z->color = RED;
    
    aggiungi_nuova_ricetta_in_ricettario_FIXUP(root, z);

    return z;
}



void LeftRotate_dispensa(alimento_nodo**root,alimento_nodo*x){
    alimento_nodo*y = x->right;
    x->right = y->left;
    if (x->right != NULL)
        x->right->parent = x;
    y->parent = x->parent;
    if (x->parent == NULL)
        (*root) = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else    x->parent->right = y;
    y->left = x;
    x->parent = y;
}

void RightRotate_dispensa(alimento_nodo**root,alimento_nodo*y){
    alimento_nodo*x = y->left;
    y->left = x->right;
    if (x->right != NULL)
        x->right->parent = y;
    x->parent =y->parent;
    if (x->parent == NULL)
        (*root) = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else y->parent->right = x;
    x->right = y;
    y->parent = x;
}

void aggiungi_nuovo_alimento_in_dispensa_FIXUP(alimento_nodo **root,alimento_nodo *z){
    while (z->parent != NULL && z->parent->parent != NULL && z->parent->color == RED)
    {
        alimento_nodo *y;
        if (z->parent == z->parent->parent->left){
            y = z->parent->parent->right;
            if (y!= NULL && y->color == RED){
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }else{
                if (z == z->parent->right){
                    z = z->parent;
                    LeftRotate_dispensa(root, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                RightRotate_dispensa(root, z->parent->parent);
            }
        }
        else{
           y = z->parent->parent->left;
            if (y!= NULL && y->color == RED){
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }else{
                if (z == z->parent->left){
                    z = z->parent;
                    RightRotate_dispensa(root, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                LeftRotate_dispensa(root, z->parent->parent);
            } 
        }
    }   
    
    (*root)->color = BLACK;
}

alimento_nodo* aggiungi_nuovo_alimento_in_dispensa(alimento_nodo**root, char* nome_alimento){
	alimento_nodo*z = (alimento_nodo*)malloc(sizeof(alimento_nodo));
    strcpy(z->nome, nome_alimento);
    z->left = z->right = z->parent = NULL;
    
    alimento_nodo*x = *root;
    alimento_nodo*y= NULL;
    while(x!=NULL){
        y=x;
        if(strcmp(z->nome, x->nome)<0)
            x = x->left;
        else
            x = x->right;
    }
    z->parent = y;
    if(y==NULL)
        *root=z;
    else if(strcmp(z->nome, y->nome)<0)
        y->left = z;
    else
        y->right = z;
    z->left = NULL;
    z->right = NULL;
    z->color = RED;
    
    aggiungi_nuovo_alimento_in_dispensa_FIXUP(root, z);

    return z;
}

void inserisci_ingrediente_in_testa(ingrediente_nodo*lista, ingrediente_nodo*ingrediente){
	ingrediente->next = lista;
	lista = ingrediente;
	return;
}


// gestisce l'istruzione aggiungi_ricetta
void aggiungi_ricetta(){	// suppongo la stringa istruzione sia già stata letta
	char nome_ricetta[255];
	// leggo nome ricetta e, dopo aver controllato che non esista già, creo la struttura ricetta e la inserisco nel ricettario
	if(scanf("%s", nome_ricetta)!=1){
		printf("non ho letto nome ricetta");
		return;
	}

	ricetta_nodo* ricetta = ricerca_ricetta_in_ricettario(nome_ricetta);

	int alreadyExists = (ricetta != NULL); 

	if(!alreadyExists)
		ricetta = aggiungi_nuova_ricetta_in_ricettario(&ricettario, nome_ricetta);	

	//finchè sono in grado di leggere sia un ingrediente che una quantità allora faccio un inserimento in testa alla lista degli ingrediente per quella specifica ricetta. Costruisco poi l'oggetto ingrediente: imposto la quantità pari a quella fornita in input, mentre per l'alimento devo fare un collegamento con l'alimento presente in dispensa. In caso l'alimento non dovesse esistere in dispensa lo creo.
	// N.B. anche se alreadyExists fosse uguale a 1 comunque devo leggere input fino alla nuova istruzione, ma in questo caso semplicemente leggo e non faccio nulla 

	char nome_ingrediente[255];
	int quantità;
	ingrediente_nodo * ingrediente;
	
	while( scanf("%s %d", nome_ingrediente, &quantità) == 2){
		
		if(!alreadyExists){
			// creo un ingrediente e lo aggiungo in testa alla lista
			ingrediente = (ingrediente_nodo*)malloc(sizeof(ingrediente_nodo));

			inserisci_ingrediente_in_testa(ricetta->ingredienti, ingrediente);
			
			ingrediente->quantità = quantità;

			alimento_nodo* alimento = ricerca_alimento_in_dispensa(nome_ingrediente);
			if (alimento)
				ingrediente->alimento = alimento;
			else
				ingrediente->alimento = aggiungi_nuovo_alimento_in_dispensa(&dispensa, nome_ingrediente);
		}
	}

	// esco dal ciclo quando non sono riuscito a leggere un numero e ho letto solo la nuova istruzione, che è salvata nella variabile nome_ingrediente

	// stampo messaggio in output in base a valore di alreadyExists
	if(alreadyExists)
		printf("ignorato");
	else
		printf("aggiunta");

	// faccio strcpy da nome_ingrediente a istruzione e ritorno
	strcpy(istruzione, nome_ingrediente);

	return;
}


// gestisce l'istruzione rimuovi_ricetta
void rimuovi_ricetta(){
	printf("rimuovi ricetta\n");
	return;
}


// gestisce l'istruzione ordine
void ordine(){
	printf("ordine\n");
	return;
}


// gestisce l'istruzione rifornimento
void rifornimento(){
	printf("rifornimento\n");
	return;
}




//--------------------------------	MAIN	--------------------------------
int main(){
	// CONFIGURAZIONE DEL CORRIERE
	printf("Inserire configurazione corriere: ");
	if(scanf("%d %d", &(corriere.periodicità), &(corriere.capienza))!=2){
			printf("mancata configurazione corriere");
			return 0;
	}

	printf("\nInserire istruzione: ");
	if(scanf("%s", istruzione) != 1){
			printf("mancata lettura istruzione");
			return 0;
	}

	while(is_istruzione(istruzione)){

		// controllo se arriva corriere
		if(now % corriere.periodicità == 0)
			printf("arrivato il corriere\n");


		//chiamo la funzione corrispondente all'istruzione
		if(strcmp(istruzione, "aggiungi_ricetta")==0)
			aggiungi_ricetta();

		else if(strcmp(istruzione, "rimuovi_ricetta")==0)
			rimuovi_ricetta();

		else if(strcmp(istruzione, "ordine")==0)
			ordine();

		else if(strcmp(istruzione, "rifornimento")==0)
			rifornimento();

		else if(strcmp(istruzione, "stampa_dispensa")==0){
			stampa_dispensa(dispensa, 0);
			printf("\nInserire istruzione: ");
			if(scanf("%s", istruzione) != 1){
				printf("mancata lettura istruzione");
				return 0;
			}
		}
		else if(strcmp(istruzione, "stampa_ricettario")==0){
			stampa_ricettario(ricettario, 0);
			printf("\nInserire istruzione: ");
			if(scanf("%s", istruzione) != 1){
				printf("mancata lettura istruzione");
				return 0;
			}
		}

		// incremento istante di tempo
		now++;
	}



	return 0;
}