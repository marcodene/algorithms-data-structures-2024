#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BLACK 0
#define RED 1


//---------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------     DICHIARAZIONE TIPI DI DATO     -------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------


//corriere
typedef struct{
    int periodicita;
    int capienza;
} corriere_struct;



//dispensa
typedef struct lt{
    int scadenza;
    int quantita;
}lotto;

typedef struct {
    lotto * data;
    int length;
    int heapsize;
}lotti_heap;

typedef struct almnt{
    char nome[256];
    lotti_heap lotti;
    int color; //0=BLACK and 1=RED
    struct almnt * left;
    struct almnt * right;
    struct almnt * parent;
}alimento_nodo;



//ricette
typedef struct ingr{
    alimento_nodo * alimento;
    int quantita;
    struct ingr * next;
}ingrediente_nodo;

typedef struct rct{
    char nome[256];
    ingrediente_nodo * ingredienti;
    int peso;
    int color;  //0=BLACK and 1=RED
    struct rct * left;
    struct rct * right;
    struct rct * parent;
}ricetta_nodo;



// ordini
typedef struct ordn{
    ricetta_nodo * ricetta;
    int peso;
    int istante_arrivo;
    int quantita;
    struct ordn * next;
}ordine_nodo;

struct ordini{
    ordine_nodo * pronti;
    ordine_nodo * pronti_coda;
    ordine_nodo * sospesi;
    ordine_nodo * sospesi_coda;
};




//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------     VARIABILI GLOBALI     -----------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------


char istruzione[256];
int now = 0;
corriere_struct corriere;

ricetta_nodo* ricettario;
alimento_nodo* dispensa;

struct ordini ordini = {0};

char * input = NULL; 
char * pos_input=NULL; 
size_t len_input = 0;



//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------     FUNZIONI STRUTTURE DATI     --------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------




ricetta_nodo* ricerca_ricetta_in_ricettario(ricetta_nodo*x, char* nome_ricetta){
    if(x==NULL || strcmp(nome_ricetta, x->nome)==0)
        return x;
    if(strcmp(nome_ricetta, x->nome)<0)
        return ricerca_ricetta_in_ricettario(x->left, nome_ricetta);
    else
        return ricerca_ricetta_in_ricettario(x->right, nome_ricetta);
}

alimento_nodo* ricerca_alimento_in_dispensa(alimento_nodo*x, char* nome_alimento){
    if(x==NULL || strcmp(nome_alimento, x->nome)==0)
        return x;
    if(strcmp(nome_alimento, x->nome)<0)
        return ricerca_alimento_in_dispensa(x->left, nome_alimento);
    else
        return ricerca_alimento_in_dispensa(x->right, nome_alimento);
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
    ricetta_nodo*z = (ricetta_nodo*)calloc(1, sizeof(ricetta_nodo));
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

ricetta_nodo* trova_ricetta_minima(ricetta_nodo*x){
    while (x->left != NULL)
        x = x->left;
    return x;
}

ricetta_nodo* trova_successore_di_ricetta(ricetta_nodo*x){
    if(x->right != NULL)
        return trova_ricetta_minima(x->right);
    ricetta_nodo*y = x->parent;
    while(y!=NULL && x == y->right){
        x = y;
        y = y->parent;
    }
    return y;
}

void rimuovi_ricetta_da_ricettario_FIXUP (ricetta_nodo**root, ricetta_nodo * x){
    ricetta_nodo*w;

    if(x->color == RED || x->parent == NULL)
        // caso 0
        x->color = BLACK;
    else if(x == x->parent->left){
        w = x->parent->right;
        if(w->color == RED){
            // caso 1
            w->color = BLACK;
            x->parent->color = RED;
            LeftRotate_ricettario(root, x->parent);
            w = x->parent->right;
        }
        if(w->left->color == BLACK && w->right->color == BLACK){
            // caso 2
            w->color = RED;
            rimuovi_ricetta_da_ricettario_FIXUP(root, x->parent);
        }
        else if(w->right->color == BLACK){
            // caso 3
            w->left->color = BLACK;
            w->color = RED;
            RightRotate_ricettario(root, w);
            w = x->parent->right;
        }
        w->color = x->parent->color;
        x->parent->color = BLACK;
        w->right->color = BLACK;
        LeftRotate_ricettario(root, x->parent);
    }
    else{
        w = x->parent->left;
        if(w->color == RED){
            // caso 1
            w->color = BLACK;
            x->parent->color = RED;
            RightRotate_ricettario(root, x->parent);
            w = x->parent->left;
        }
        if(w->right->color == BLACK && w->left->color == BLACK){
            // caso 2
            w->color = RED;
            rimuovi_ricetta_da_ricettario_FIXUP(root, x->parent);
        }
        else if(w->left->color == BLACK){
            // caso 3
            w->right->color = BLACK;
            w->color = RED;
            LeftRotate_ricettario(root, w);
            w = x->parent->left;
        }
        w->color = x->parent->color;
        x->parent->color = BLACK;
        w->left->color = BLACK;
        RightRotate_ricettario(root, x->parent);
    }

    return;
}

void dealloca_ingredienti_da_ricetta(ingrediente_nodo *x){
    if(x==NULL)
        return;
    dealloca_ingredienti_da_ricetta(x->next);
    free(x);
    x=NULL;
    return;
}

void rimuovi_ricetta_da_ricettario (ricetta_nodo**root, ricetta_nodo * z){
    ricetta_nodo* y;
    ricetta_nodo*x;
    int y_color;
    if(z->left == NULL ||  z->right == NULL)
        y = z;
    else
        y = trova_successore_di_ricetta(z);
    if(y->left != NULL)
        x = y->left;
    else
        x=y->right;
    if(x!=NULL) // se y ha almeno un figlio
        x->parent = y->parent;
    if(y->parent == NULL)
        *root = x;
    else if(y->parent->left == y)
        y->parent->left = x;
    else
        y->parent->right = x;

    y_color = y->color;
    if(y!=z) { // scambio i nodi y e z
        
        if(z->parent==NULL)
            *root=y;
        else if(z->parent->left==z)
            z->parent->left=y;
        else
            z->parent->right=y;
        y->parent = z->parent;
        y->left=z->left;
        y->right=z->right;
        if(z->left!=NULL)
            z->left->parent = y;
        if(z->right!=NULL)
            z->right->parent = y;
        y->color = z->color;

    }

    dealloca_ingredienti_da_ricetta(z->ingredienti);
    free(z);
    z=NULL; 

    if(y_color == BLACK && x!=NULL)
        rimuovi_ricetta_da_ricettario_FIXUP(root, x);
    return;   
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
    alimento_nodo*z = (alimento_nodo*)calloc(1, sizeof(alimento_nodo));
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

void inserisci_ingrediente_in_testa(ingrediente_nodo**lista, ingrediente_nodo*ingrediente){
    ingrediente->next = *lista;
    *lista = ingrediente;
    return;
}

void aggiungi_lotto_ad_alimento(alimento_nodo* alimento, int quantita, int scadenza){
    if(alimento->lotti.length==alimento->lotti.heapsize){
        alimento->lotti.data = realloc(alimento->lotti.data, (alimento->lotti.length + 1)*sizeof(lotto));
        alimento->lotti.length++;
    }
    alimento->lotti.data[alimento->lotti.heapsize].quantita = quantita;
    alimento->lotti.data[alimento->lotti.heapsize].scadenza = scadenza;

    alimento->lotti.heapsize++;

    return;
}

// ordine**primo_della_lista rappresenta un puntatore a pronti oppure sospesi che contengono un puntatore alla testa delle rispettive liste di ordini
// ordine**ultimo_della_lista rappresenta un puntatore a pronti_coda oppure sospesi_coda che contengono un puntatore alla coda delle rispettive liste di ordini
void aggiungi_ordine_in_coda (ordine_nodo**primo_della_lista, ordine_nodo**ultimo_della_lista, ordine_nodo*ordine_da_inserire){
    if(*ultimo_della_lista == NULL){
        *ultimo_della_lista = *primo_della_lista = ordine_da_inserire;
        return;
    }

    (**ultimo_della_lista).next = ordine_da_inserire;
    *ultimo_della_lista = ordine_da_inserire;
    return;
}

void scadenza_heapify(lotti_heap * lotti, int i){
    int left = 2*i+1;     // left = LEFT(i);
    int right = 2*i + 2;     // right = RIGHT(i);
    int new_max;

    if(left < lotti->heapsize &&  lotti->data[left].scadenza<lotti->data[i].scadenza )
        new_max = left;
    else new_max = i;

    if(right < lotti->heapsize &&  lotti->data[right].scadenza<lotti->data[new_max].scadenza )
        new_max = right;

    if (new_max != i){
        lotto temp = lotti->data[i];
        lotti->data[i] = lotti->data[new_max];
        lotti->data[new_max] = temp;
        scadenza_heapify(lotti, new_max);
    }
    return;
}

void build_scadenza_heap(lotti_heap *A, int heapsize) {
    for (int i = heapsize / 2 - 1; i >= 0; i--)
        scadenza_heapify(A, i);
}


int is_scaduto (lotto lotto){
    return lotto.scadenza <= now;
}

void cancella_lotto_max(lotti_heap * lotti){
    if(lotti->heapsize < 1)
        return;
    lotti->data[0] = lotti->data[ lotti->heapsize-1 ];
    lotti->heapsize -= 1;
    scadenza_heapify(lotti, 0);
}

void elimina_lotti_scaduti (alimento_nodo * alimento){
    while (alimento->lotti.heapsize > 0 && alimento->lotti.data[0].scadenza <= now )
        cancella_lotto_max(&alimento->lotti);
    return;
}


int controlla_disponibilita_dispensa(alimento_nodo* alimento, int quantita_richiesta){
    // controllo ci sia almeno un lotto
    if (alimento->lotti.data == NULL || alimento->lotti.heapsize == 0) {
        return 0;  
    }

    // controllo che il primo lotto non sia scaduto, is_scaduto(alimento->lotti.data[0]), in caso lo fosse chiamo elimina_lotti_scaduti(alimento) che elimina tutti i lotti scaduti. Se invece il primo non fosse scaduto allora anche nessuno dei successivi è scaduto. 

    if( is_scaduto(alimento->lotti.data[0]) )
        elimina_lotti_scaduti(alimento);

    if(alimento->lotti.heapsize == 0)   // ovveso se con elimina_lotti_scaduti ho cancellato l'ultimo lotto
        return 0;

    // se alimento->lotti.data[0].quantita > quantita_richiesta allora restituisco 1 altrimento pongo una variabile sum inizializzata a 0 uguale a alimento->lotti.data[0].quantita. 

    if(alimento->lotti.data[0].quantita >= quantita_richiesta)
        return 1; 

    int sum = alimento->lotti.data[0].quantita;

    // inizio ad iterare sull'array (ovvero sull'heap) e aggiungendoli a sum fino a che sum > quantita_ricihiesta, in questo caso return 1.

    for(int i=1; i < alimento->lotti.heapsize; i++){
        sum += alimento->lotti.data[i].quantita;

        if(sum >= quantita_richiesta)
            return 1;
    }

    // se invece dovessi terminare lo heap senza aver raggiunto questa situazione faccio return 0;
    return 0;

}


int controlla_ingredienti(ordine_nodo* ordine){
    ingrediente_nodo * curr = ordine->ricetta->ingredienti;
    // per ogni ingrediente devo controllare se la quantita richiesta dalla ricetta * il numero di ricette da fare è disponibile in dispensa tra i vari lotti che non sono ancora scaduti
    while(curr!=NULL){
        
        if (!controlla_disponibilita_dispensa(curr->alimento, curr->quantita * ordine->quantita))
            // se controlla_disponibilita_dispensa non restituisce mai 0 allora vuol dire che gli ingredienti ci sono tutti e posso restituire 1, altrimenti restituisco 0 appena ricevo uno 0
            return 0;
        curr = curr->next;
    }


    return 1;
}


void preleva_da_dispensa(alimento_nodo* alimento, int quantita_richiesta){

    while(quantita_richiesta!=0){
        if(alimento->lotti.data[0].quantita > quantita_richiesta){
            alimento->lotti.data[0].quantita -= quantita_richiesta;
            quantita_richiesta=0;
            scadenza_heapify(&alimento->lotti, 0);
        }else{
            quantita_richiesta -= alimento->lotti.data[0].quantita;
            cancella_lotto_max(&alimento->lotti);
        }
    }

    return;
}


void prepara_ordine(ordine_nodo* ordine){
    // ho gia controllato, con la funzione controlla_ingredienti(ordine), che gli ingredienti ci siano e non siano scaduti, quindi devo solo chiamare, per ogni elementi di ordine->ricetta->ingredienti, la funzione preleva_da_dispensa(alimento, ingrediente.quantita * ordine->quantita)

    ingrediente_nodo * curr = ordine->ricetta->ingredienti;

    while(curr!=NULL){
       
        preleva_da_dispensa(curr->alimento, curr->quantita * ordine->quantita);      

        curr = curr->next;
    }


    return;
}

int is_ricetta_presente_tra_ordini(ordine_nodo* lista, ricetta_nodo*ricetta){
    if (lista==NULL)
        return 0;
    if(lista->ricetta == ricetta)
        return 1;
    if (lista->next==NULL)
        return 0;
    return is_ricetta_presente_tra_ordini(lista->next, ricetta);
}

int conta_ordini_da_caricare(){
    int sum = 0;
    int cnt = 0;
    ordine_nodo * curr = ordini.pronti;
    while (curr!=NULL && sum <= corriere.capienza){
        sum += curr->peso;
        cnt++;
        curr = curr->next;
    }
    if(sum<=corriere.capienza)
        return cnt; 
    else
        return (cnt-1);
}

// deve cancellare il nodo successivo ad elem
void cancella_ordine_in_testa(ordine_nodo*elem){
    if(elem == NULL){   // vuol dire che l'elem da cancellare è il primo della lista
        ordini.pronti = ordini.pronti->next;
        if(ordini.pronti==NULL)
            ordini.pronti_coda = NULL;
    }
    else{
        elem->next = elem->next->next;
        if(elem->next == NULL) // ho cancellato l'ultimo nodo
            ordini.pronti_coda = elem;
    }
    return;
}

void swap(ordine_nodo **A, int p, int q) {
    if (p == q) 
        return;
    ordine_nodo* temp = A[p]; 
    A[p] = A[q]; 
    A[q] = temp;
}

void peso_heapify(ordine_nodo **A, int i, int heapsize) {
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    int max = i;
    if (left < heapsize) {
        if(A[left]->peso < A[i]->peso)
            max = left;
        else if(A[left]->peso == A[i]->peso){
            if(A[left]->istante_arrivo > A[i]->istante_arrivo)
                max = left;
        }
    }
    if (right < heapsize) {
        if(A[right]->peso < A[max]->peso)
            max = right;
        else if(A[right]->peso == A[max]->peso){
            if(A[right]->istante_arrivo > A[max]->istante_arrivo)
                max = right;
        }
    }
    
    if (max != i) {
        swap(A, i, max);
        peso_heapify(A, max, heapsize);
    }
}

void build_peso_heap(ordine_nodo **A, int heapsize) {
    for (int i = heapsize / 2 - 1; i >= 0; i--)
        peso_heapify(A, i, heapsize);
}

void heapsort_per_peso(ordine_nodo** A, int heapsize){
    build_peso_heap(A, heapsize);
    for(int i=heapsize-1; i>0; i--){
        swap(A, 0, i);
        peso_heapify(A, 0, i);
    }
}

void sposta_da_sospesi_a_pronti(ordine_nodo*ordine, ordine_nodo* prec){
    // prende in ingresso l'ordine da spostare e l'ordine a lui precedente se è la testa della lista allora prec == NULL
    if (prec == NULL){
        ordini.sospesi = ordine->next;
        if(ordini.sospesi==NULL)
            ordini.sospesi_coda = NULL;
    }
    else{
        prec->next = ordine->next;
        if(prec->next==NULL)
            ordini.sospesi_coda=prec;
    }

    // ora isnerisco l'ordine in ordini.pronti al giusto posto
    ordine_nodo*curr= ordini.pronti;
    if (curr==NULL || curr->istante_arrivo >= ordine->istante_arrivo){
        //inserisci in testa
        ordine->next = ordini.pronti;
        ordini.pronti = ordine;
        if(ordine->next == NULL)
            ordini.pronti_coda = ordine;
    }else{
        while(curr->next!=NULL && curr->next->istante_arrivo < ordine->istante_arrivo)
            curr = curr->next;
        if(curr->next==NULL){       // inserisci in fondo
            curr->next=ordine;
            ordine->next = NULL;
            ordini.pronti_coda = ordine;
        }else{      // inserisci dopo curr
            ordine->next = curr->next;
            curr->next = ordine;
        }
    }

}

void controlla_ordini_sospesi(){
    ordine_nodo*prec = NULL;
    ordine_nodo*temp;
    ordine_nodo*curr = ordini.sospesi;
    while(curr!=NULL){
        if(controlla_ingredienti(curr)){
            prepara_ordine(curr);
            temp = curr;     
            curr = curr->next;
            sposta_da_sospesi_a_pronti(temp, prec);         // non posso passare curr come parametro perchè se no perderei un riferimento a curr-next 
        }
        else{
            prec = curr;
            curr=curr->next;
        }
        
    }


    return;
}



//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------     ISTRUZIONI     ---------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
void avanza_input_di_n_parole(int n){
    for(int i=0; i<n; i++){
        while (*pos_input != ' ' && *pos_input != '\0' && *pos_input!='\n') pos_input++;
        if (*pos_input == ' ') pos_input++;
    }
    
}

// controlla che la stringa passata per input sia un'istruzione valida
int is_istruzione(char*str){
    return !(strcmp(str, "aggiungi_ricetta") && strcmp(str, "rimuovi_ricetta") && strcmp(str, "ordine") && strcmp(str, "rifornimento") && strcmp(str, "stampa_dispensa") && strcmp(str, "stampa_ricettario") && strcmp(str, "stampa_ordini"));
}

// gestisce l'istruzione aggiungi_ricetta
void aggiungi_ricetta(){    // suppongo la stringa istruzione sia gia stata letta
    char nome_ricetta[256];
    
    // leggo nome ricetta e, dopo aver controllato che non esista gia, creo la struttura ricetta e la inserisco nel ricettario
    if(sscanf(pos_input, "%s", nome_ricetta)!=1){
        printf("non ho letto nome ricetta\n");
        return;
    }
    avanza_input_di_n_parole(1);

    
    ricetta_nodo* ricetta = ricerca_ricetta_in_ricettario(ricettario, nome_ricetta);

    int alreadyExists = (ricetta != NULL); 

    if(!alreadyExists)
        ricetta = aggiungi_nuova_ricetta_in_ricettario(&ricettario, nome_ricetta);  

    //finchè sono in grado di leggere sia un ingrediente che una quantita allora faccio un inserimento in testa alla lista degli ingrediente per quella specifica ricetta. Costruisco poi l'oggetto ingrediente: imposto la quantita pari a quella fornita in input, mentre per l'alimento devo fare un collegamento con l'alimento presente in dispensa. In caso l'alimento non dovesse esistere in dispensa lo creo.
    // N.B. anche se alreadyExists fosse uguale a 1 comunque devo leggere input fino alla nuova istruzione, ma in questo caso semplicemente leggo e non faccio nulla 

    char nome_ingrediente[256];
    int quantita;
    ingrediente_nodo * ingrediente;
    if(!alreadyExists)
        ricetta->peso = 0;
    
    while( sscanf(pos_input, "%s", nome_ingrediente) == 1 ){
        avanza_input_di_n_parole(1);
        
        if(sscanf(pos_input, "%d", &quantita)==0)
            printf("mancata lettura di quantita");
        avanza_input_di_n_parole(1);


        if(!alreadyExists){
            ricetta->peso+=quantita;

            // creo un ingrediente e lo aggiungo in testa alla lista
            ingrediente = (ingrediente_nodo*)calloc(1, sizeof(ingrediente_nodo));

            inserisci_ingrediente_in_testa(&ricetta->ingredienti, ingrediente);
            
            ingrediente->quantita = quantita;

            alimento_nodo* alimento = ricerca_alimento_in_dispensa(dispensa, nome_ingrediente);
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


    return;
}


// gestisce l'istruzione rimuovi_ricetta
void rimuovi_ricetta(){
    // devo controllare non ci siano ricette tra gli ordini in sospeso, in caso ci fossero stampo "ordini in sospeso" altrimenti devo controllare se la ricetta è presente nel ricettario. Se c'è la rimuovo e stampo "rimossa" altrimenti stampo "non presente".
    //all'inizio devo fare una scanf per leggere la ricetta da eliminare e alla fine devo fare un'altra scanf per leggere l'istruzione successiva
    
    char nome_ricetta[256];
    if(sscanf(pos_input, "%s", nome_ricetta)!=1){
        printf("mancata lettura di nome ricetta");
        return;
    }
    avanza_input_di_n_parole(1);


    ricetta_nodo*ricetta = ricerca_ricetta_in_ricettario(ricettario, nome_ricetta);

    if(ricetta==NULL)
        printf("non presente");
    else{
        // controllo non ci siano ordini in sospeso
        if( is_ricetta_presente_tra_ordini(ordini.sospesi, ricetta) || is_ricetta_presente_tra_ordini(ordini.pronti, ricetta) )
            printf("ordini in sospeso");
        else{
            rimuovi_ricetta_da_ricettario (&ricettario, ricetta);
            printf("rimossa");
        }
    }


    return;
}


// gestisce l'istruzione ordine
void ordine(){
    char nome_ricetta[256];
    int quantita_ordine;


    // leggi la ricetta e la quantita ordinata e salvali nelle variabili nome_ricetta e quantita_ordine
    if (sscanf(pos_input, "%s %d", nome_ricetta, &quantita_ordine) != 2) {
        printf("mancata lettura di nome_ricetta o quantita_ordine\n");
        return;
    }
    avanza_input_di_n_parole(2);
    

    // cerca la ricetta_ordinata (puntatore alla ricetta) nel ricettario. Se non esiste stampa "rifiutato" e continua, 
    ricetta_nodo* ricetta_ordinata = ricerca_ricetta_in_ricettario(ricettario, nome_ricetta);

    if(!ricetta_ordinata)
        printf("rifiutato");
    else{

        // altrimenti crea la struttura ordine e rimpila con i dati relativi all'ordine. 
        ordine_nodo* ordine = (ordine_nodo*)calloc(1, sizeof(ordine_nodo));
        ordine->ricetta = ricetta_ordinata;
        ordine->quantita = quantita_ordine;
        ordine->peso = ricetta_ordinata->peso*quantita_ordine;
        ordine->istante_arrivo = now;


        // Poi chiama controlla_ingredienti(ordine), se ha riscontro positivo, ovvero ci sono tutti gli ingredienti, chiama prepara_ordine(ordine), che aggiorna la dispensa modificando gli ingredienti usati per la ricetta. Aggiungo l'ordine in coda a ordini.pronti.
        if(controlla_ingredienti(ordine)){
            prepara_ordine(ordine);
            aggiungi_ordine_in_coda(&(ordini.pronti), &(ordini.pronti_coda), ordine);
        }else
            // In caso invece non ci fossero stati tutti gli ingredienti per la ricetta allora dovevo aggiungere l'ordine a ordini.sospesi
            aggiungi_ordine_in_coda(&(ordini.sospesi), &(ordini.sospesi_coda), ordine);
    
        printf("accettato");
    }

    
    
    return;
}


// gestisce l'istruzione rifornimento
void rifornimento(){
    char nome_alimento[256];
    int quantita;
    int scadenza;

    // finchè sono in grado di leggere un nome_alimento, una quantita e una scadenza
    while (sscanf(pos_input, "%s", nome_alimento)==1){
        avanza_input_di_n_parole(1);
        if(sscanf(pos_input, "%d %d", &quantita, &scadenza)!=2){
            printf("mancata lettura di quantita o scadenza");
            return;
        }
        avanza_input_di_n_parole(2);

        // cerco l'alimento all'interno della dispensa e aggiungo un nuovo lotto con la quantita e scadenza appena letta. Se l'alimento non dovesse esistere allora lo creo
        alimento_nodo* alimento = ricerca_alimento_in_dispensa(dispensa, nome_alimento);
        if (!alimento)
            alimento = aggiungi_nuovo_alimento_in_dispensa(&dispensa, nome_alimento);
        
        aggiungi_lotto_ad_alimento(alimento, quantita, scadenza);
        build_scadenza_heap(&alimento->lotti, alimento->lotti.heapsize);
        
    } // esco dal while quando leggo il nome della nuova istruzione, il nome dell'istruzione sara conservata nella variabile alimento

    // visto che ho aggiunto elementi in dispensa chiamo funzione controlla_ordini_sospesi che controlla se adesso sono presenti tutti gli elementi per poterli eseguire
    controlla_ordini_sospesi();

    printf("rifornito");
    return;
}

void carica_corriere(){
    //conto il numero n di ordini da caricare sul corriere attraverso la func conta_ordini_da_caricare() e alloco un array di dimensione n
    int n = conta_ordini_da_caricare();

    if(n==0){
        printf("\ncamioncino vuoto");
        return;
    }

    ordine_nodo* pnt_to_max_peso;
    ordine_nodo* curr;
    for(int i=0; i<n; i++){
        int max_peso=-2;
        pnt_to_max_peso=NULL;
        curr = ordini.pronti;

        for(int j=0; j<n; j++){
            if(curr->peso > max_peso){
                max_peso = curr->peso;
                pnt_to_max_peso = curr;
            }else if(curr->peso == max_peso){
                if(pnt_to_max_peso!=NULL && curr->istante_arrivo < pnt_to_max_peso->istante_arrivo)
                    pnt_to_max_peso = curr;
            }
            curr = curr->next;
        }
        printf("\n%d %s %d", pnt_to_max_peso->istante_arrivo, pnt_to_max_peso->ricetta->nome, pnt_to_max_peso->quantita);
        pnt_to_max_peso->peso = -1;

    }

    curr = ordini.pronti;
    ordine_nodo*next;
    for(int i=0; i<n; i++){
        next = curr->next;
        free(curr);
        curr=next;
    }
    ordini.pronti = curr;
    if(curr==NULL)
        ordini.pronti_coda=NULL;
    
    
    return;
}


//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------     FUNZIONI DI STAMPA     -----------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------

void stampa_dispensa(alimento_nodo*alimento, int livello){
    if (alimento == NULL)
        return;
    
    printf("livello %d, value: %s, colore: %c padre:%s - %s\n",
        livello, 
        alimento->nome,
        ((alimento->color)==BLACK?'b':'r'), 
        alimento->parent!=NULL?(alimento->parent)->nome:"-1",
        alimento->parent==NULL?"radice":(alimento->parent->left==alimento?"sinistro":"destro")
        
    );

    for(int i=0; i<alimento->lotti.heapsize; i++){
        printf("     quantita: %d, scadenza: %d\n", alimento->lotti.data[i].quantita, alimento->lotti.data[i].scadenza);
    }
    
    
    stampa_dispensa(alimento->left, livello+1);
    stampa_dispensa(alimento->right, livello+1);
}

void stampa_lotti_di_ricetta(char nome_ricetta[256]){
    ricetta_nodo * ricetta = ricerca_ricetta_in_ricettario(ricettario, nome_ricetta);

    if(ricetta){
        ingrediente_nodo * curr = ricetta->ingredienti;

        while(curr!=NULL){
            lotti_heap lotti = curr->alimento->lotti;
            printf("        %s", curr->alimento->nome);
            if (lotti.heapsize == 0)
                printf(" {vuoto} ");
           
            for(int i =0; i < lotti.heapsize; i++){
                printf(" {q:%d, s:%d} ", lotti.data[i].quantita, lotti.data[i].scadenza);
            }
            printf("richiesta: %d", curr->quantita);
            printf("\n");
            curr = curr->next;
        }
    }

    return;
}

void stampa_ricettario(ricetta_nodo*ricetta, int livello){
    if (ricetta == NULL)
        return;
    /*printf("livello %d, value: %s, peso: %d, colore: %c padre:%s   %s\n",
        livello, 
        ricetta->nome,
        ricetta->peso,
        ((ricetta->color)==BLACK?'b':'r'), 
        ricetta->parent!=NULL?(ricetta->parent)->nome:"-1",
        ricetta->parent==NULL?"radice":(ricetta->parent->left==ricetta?"sinistro":"destro")
    );*/

    printf("- %s, peso: %d \n", ricetta->nome, ricetta->peso);
    stampa_lotti_di_ricetta(ricetta->nome);
    
    
    stampa_ricettario(ricetta->left, livello+1);
    stampa_ricettario(ricetta->right, livello+1);
}

void stampa_ordini(){
    ordine_nodo * ordine;

    printf("--- ORDINI PRONTI ---\n");
    ordine = ordini.pronti;
    while(ordine!=NULL){
        printf("   {%s, p:%d, t:%d, q:%d}", ordine->ricetta->nome, ordine->peso, ordine->istante_arrivo, ordine->quantita);
        ordine = ordine->next;
    }



    printf("\n--- ORDINI SOSPESI ---\n");
    ordine = ordini.sospesi;
    while(ordine!=NULL){
        printf("   {%s, p:%d, t:%d, q:%d}", ordine->ricetta->nome, ordine->peso, ordine->istante_arrivo, ordine->quantita);
        ordine = ordine->next;
    }
    printf("\n");
}


//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------     MAIN     ------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------

int main(){
    // stringa input messa come var globale
    // intero len_input messo come var globale    
    if(getline(&input, &len_input, stdin) == -1)
        printf("ERRORE");

    // CONFIGURAZIONE DEL CORRIERE
    if(sscanf(input, "%d %d", &(corriere.periodicita), &(corriere.capienza))!=2){
            printf("mancata configurazione corriere");
            return 0;
    }


    while(getline(&input, &len_input, stdin) != -1){
        pos_input=input;
        // leggo istruzione da stringa input
        if(sscanf(pos_input, "%s", istruzione) != 1){
            printf("mancata lettura istruzione");
            return 0;
        }
        avanza_input_di_n_parole(1);

        
        if(strcmp(istruzione, "stampa_dispensa")==0){
            stampa_dispensa(dispensa, 0);
            continue;
        }
        else if(strcmp(istruzione, "stampa_ricettario")==0){
            stampa_ricettario(ricettario, 0);
            continue;
        }
        else if(strcmp(istruzione, "stampa_ordini")==0){
            stampa_ordini();
            continue;
        }



        //chiamo la funzione corrispondente all'istruzione
        if(strcmp(istruzione, "aggiungi_ricetta")==0)
            aggiungi_ricetta();

        else if(strcmp(istruzione, "rimuovi_ricetta")==0)
            rimuovi_ricetta();

        else if(strcmp(istruzione, "ordine")==0)
            ordine();

        else if(strcmp(istruzione, "rifornimento")==0)
            rifornimento();


        // incremento istante di tempo
        now++;


        // controllo se arriva corriere
        if(now!= 0 && now % corriere.periodicita == 0)
            //printf("arrivato il corriere\n");
            carica_corriere();

        if(feof(stdin)){
            //printf("END OF FILE: %d\n", feof(stdin));
            return 0;
        }

        printf("\n");
    }

    free(input);

    return 0;
}

//-----------------------------------------------------------------------------------------
//--------------------------------------     TODO     -------------------------------------
//-----------------------------------------------------------------------------------------