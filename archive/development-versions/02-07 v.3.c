#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

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
    char *nome;
    lotti_heap lotti;
    int quantita_disponibile;
    int color; //0=BLACK and 1=RED
    struct almnt * left;
    struct almnt * right;
    struct almnt * parent;
}alimento_nodo;



//ricette
typedef struct ingr{
    alimento_nodo * alimento;
    int quantita;
}ingrediente_nodo;

typedef struct{
    int istante_arrivo;
    int quantita;
}ultimo;

typedef struct rct{
    char *nome;
    ingrediente_nodo * ingredienti;
    int num_ingredienti;
    int peso;
    ultimo ultimo_ordine_rifiutato;
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
int istante_ultimo_rifornimento = 0;
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
    z->nome = (char*)malloc(strlen(nome_ricetta)+1);
    strcpy(z->nome, nome_ricetta);
    z->ultimo_ordine_rifiutato.istante_arrivo=-1;
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

    free(z->nome);
    free(z->ingredienti);
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
    z->nome = (char*)malloc(strlen(nome_alimento)+1);
    z->quantita_disponibile=0;
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


void aggiungi_lotto_ad_alimento(alimento_nodo* alimento, int quantita, int scadenza){
    if(alimento->lotti.length==0){
        alimento->lotti.data = realloc(alimento->lotti.data, (alimento->lotti.length+2)*sizeof(lotto));
        alimento->lotti.length = 2;
    } else if(alimento->lotti.length==alimento->lotti.heapsize){
        // se non ho spazio raddoppio la dimensione dello heap
        alimento->lotti.data = realloc(alimento->lotti.data, (alimento->lotti.length + 5)*sizeof(lotto));
        alimento->lotti.length += 5;
    } else if(alimento->lotti.heapsize>0 && alimento->lotti.heapsize <= alimento->lotti.length / 4){
        // ridimensione la lunghezza se essa è troppo grande rispetto a heapsize
        alimento->lotti.data = realloc(alimento->lotti.data, (alimento->lotti.length / 2)*sizeof(lotto));
        alimento->lotti.length /= 2;
    }


    alimento->quantita_disponibile += quantita;
    alimento->lotti.data[alimento->lotti.heapsize].quantita = quantita;
    alimento->lotti.data[alimento->lotti.heapsize].scadenza = scadenza;

    alimento->lotti.heapsize++;

    // aggiusto lo heap visto che ho aggiunto un elem in fondo
    int i=alimento->lotti.heapsize-1;
    int parent_index = (int)floor((i-1)/2);
    while(i!=0 && alimento->lotti.data[parent_index].scadenza > alimento->lotti.data[i].scadenza){
        lotto temp = alimento->lotti.data[i];
        alimento->lotti.data[i] = alimento->lotti.data[parent_index];
        alimento->lotti.data[parent_index] = temp;
        i = parent_index;
        parent_index = (int)floor((i-1)/2);
    }


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
    while (alimento->lotti.heapsize > 0 && alimento->lotti.data[0].scadenza <= now ){
        alimento->quantita_disponibile -= alimento->lotti.data[0].quantita;
        cancella_lotto_max(&alimento->lotti);
    }

    if(alimento->lotti.heapsize>0 && alimento->lotti.heapsize <= alimento->lotti.length / 4){
        // ridimensione la lunghezza se essa è troppo grande rispetto a heapsize
        alimento->lotti.data = realloc(alimento->lotti.data, (alimento->lotti.length / 2)*sizeof(lotto));
        alimento->lotti.length /= 2;
    }
    return;
}


int controlla_disponibilita_dispensa(alimento_nodo* alimento, int quantita_richiesta){
    // controllo ci sia almeno un lotto
    if (alimento->lotti.data == NULL || alimento->lotti.heapsize == 0) {
        return 0;  
    }

    // controllo che il primo lotto non sia scaduto, is_scaduto(alimento->lotti.data[0]), in caso lo fosse chiamo elimina_lotti_scaduti(alimento) che elimina tutti i lotti scaduti. Se invece il primo non fosse scaduto allora anche nessuno dei successivi è scaduto. 

    if( alimento->lotti.data[0].scadenza <= now )
        elimina_lotti_scaduti(alimento);

    if(alimento->lotti.heapsize == 0)   // ovveso se con elimina_lotti_scaduti ho cancellato l'ultimo lotto
        return 0;

    // se alimento->lotti.data[0].quantita > quantita_richiesta allora restituisco 1 altrimento pongo una variabile sum inizializzata a 0 uguale a alimento->lotti.data[0].quantita. 

    if(alimento->quantita_disponibile >= quantita_richiesta)
        return 1; 

    return 0;

}

int confronta_con_ultimo_ordine_rifiutato(ordine_nodo*ordine){

    // se l'ultimo rifornimento c'è stato prima dell'ultimo ordine rifiutato allora le informazioni sono valide indipendentemente da now. Se invece tra l'istante dell ultimo ordine rifiutato e now c'è stato un rifornimento allora 

    if(istante_ultimo_rifornimento < ordine->ricetta->ultimo_ordine_rifiutato.istante_arrivo){
        if(ordine->ricetta->ultimo_ordine_rifiutato.quantita <= ordine->quantita)
            return 0;
        else
            return 1;
    }else{
        if(ordine->ricetta->ultimo_ordine_rifiutato.istante_arrivo < now)
        return 1;
        else{
            if(ordine->ricetta->ultimo_ordine_rifiutato.quantita <= ordine->quantita)
                return 0;
            else
                return 1;
        }
    }
}


int controlla_ingredienti(ordine_nodo* ordine){
    ingrediente_nodo curr;

    if(!confronta_con_ultimo_ordine_rifiutato(ordine))
        return 0;


    // per ogni ingrediente devo controllare se la quantita richiesta dalla ricetta * il numero di ricette da fare è disponibile in dispensa tra i vari lotti che non sono ancora scaduti
    for(int i=0; i < ordine->ricetta->num_ingredienti; i++){
        curr = ordine->ricetta->ingredienti[i];
        if (!controlla_disponibilita_dispensa(curr.alimento, curr.quantita * ordine->quantita)){
            // se controlla_disponibilita_dispensa non restituisce mai 0 allora vuol dire che gli ingredienti ci sono tutti e posso restituire 1, altrimenti restituisco 0 appena ricevo uno 0
            ordine->ricetta->ultimo_ordine_rifiutato.istante_arrivo = now;
            ordine->ricetta->ultimo_ordine_rifiutato.quantita = ordine->quantita;
            return 0;
        }
            
    }

    return 1;
}


void preleva_da_dispensa(alimento_nodo* alimento, int quantita_richiesta){
    alimento->quantita_disponibile -= quantita_richiesta;

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

    ingrediente_nodo curr;

    for(int i=0; i < ordine->ricetta->num_ingredienti; i++){
        curr = ordine->ricetta->ingredienti[i];
       
        preleva_da_dispensa(curr.alimento, curr.quantita * ordine->quantita);      
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

ordine_nodo* trova_primo_ordine_da_non_caricare(){
    int sum = 0;
    ordine_nodo * curr = ordini.pronti, *prec = NULL;
    while (curr!=NULL && sum <= corriere.capienza){
        sum += curr->peso;
        prec = curr;
        curr = curr->next;
    }
    if(sum <= corriere.capienza)
        return curr;
    else
        return prec;
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

ordine_nodo* peso_partition(ordine_nodo * start, ordine_nodo * end){
    ordine_nodo *p, *tempstart, *tempp, *pivot;
    if (start == NULL)
        return NULL;
    pivot = start;
    p = start;
    while(p->next != end){
        if( p->next->peso > pivot->peso  ||  ((p->next->peso == pivot->peso)&&(p->next->istante_arrivo < pivot->istante_arrivo))){
            tempp = p->next;
            tempstart = start;
            p->next = tempp->next;
            start = tempp;
            start->next = tempstart;
        }else
            p = p->next;
    }
    return start;
}

ordine_nodo * peso_quicksort(ordine_nodo * start, ordine_nodo * end){
    ordine_nodo * pivot;
    if(start == end)
        return start;
    pivot = start;
    start = peso_partition(start, end);
    start = peso_quicksort(start, pivot);
    pivot->next = peso_quicksort(pivot->next, end);
    return start;
}





//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------     ISTRUZIONI     ---------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
char* avanza_pnt_di_n_parole(char * pnt, int n){
    for(int i=0; i<n; i++){
        while (*pnt != ' ' && *pnt != '\0' && *pnt!='\n') pnt++;
        if (*pnt == ' ') pnt++;
    }
    return pnt;
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
    pos_input = avanza_pnt_di_n_parole(pos_input, 1);

    ricetta_nodo* ricetta = ricerca_ricetta_in_ricettario(ricettario, nome_ricetta);

    int alreadyExists = (ricetta != NULL);    


    
    if(alreadyExists)
        printf("ignorato");
    else{
        ricetta = aggiungi_nuova_ricetta_in_ricettario(&ricettario, nome_ricetta); 


        char nome_ingrediente[256];
        int quantita;
        ricetta->peso = 0;

        // conto il numero di ingredienti e dopo alloca un array che conterrà gli ingredienti
        char * temp_pos = pos_input;
        int cnt=0;
        while( sscanf(temp_pos, "%s %d", nome_ingrediente, &quantita) == 2 ){
            temp_pos = avanza_pnt_di_n_parole(temp_pos, 2);
            cnt++;
        }
        ricetta->ingredienti = (ingrediente_nodo*)malloc(cnt*sizeof(ingrediente_nodo));
        ricetta->num_ingredienti = cnt;


        int i=0;
        
        while( sscanf(pos_input, "%s", nome_ingrediente) == 1 ){
            pos_input = avanza_pnt_di_n_parole(pos_input, 1);
            if(sscanf(pos_input, "%d", &quantita)==0)
                printf("mancata lettura di quantita");
            pos_input = avanza_pnt_di_n_parole(pos_input, 1);


            ricetta->peso+=quantita;
            
            ricetta->ingredienti[i].quantita = quantita;

            alimento_nodo* alimento = ricerca_alimento_in_dispensa(dispensa, nome_ingrediente);
            if (alimento)
                ricetta->ingredienti[i].alimento = alimento;
            else
                ricetta->ingredienti[i].alimento = aggiungi_nuovo_alimento_in_dispensa(&dispensa, nome_ingrediente);
            
            i++;
        }

        printf("aggiunta");
    }

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
    pos_input = avanza_pnt_di_n_parole(pos_input, 2);
    

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
    istante_ultimo_rifornimento = now;


    char nome_alimento[256];
    int quantita;
    int scadenza;

    // finchè sono in grado di leggere un nome_alimento, una quantita e una scadenza
    while (sscanf(pos_input, "%s", nome_alimento)==1 && !is_istruzione(nome_alimento)){
        pos_input = avanza_pnt_di_n_parole(pos_input, 1);
        if(sscanf(pos_input, "%d %d", &quantita, &scadenza)!=2)
            printf("mancata lettura di quantita o scadenza");
        pos_input = avanza_pnt_di_n_parole(pos_input, 2);

        // cerco l'alimento all'interno della dispensa e aggiungo un nuovo lotto con la quantita e scadenza appena letta. Se l'alimento non dovesse esistere allora lo creo
        alimento_nodo* alimento = ricerca_alimento_in_dispensa(dispensa, nome_alimento);
        if (!alimento)
            alimento = aggiungi_nuovo_alimento_in_dispensa(&dispensa, nome_alimento);
        
        aggiungi_lotto_ad_alimento(alimento, quantita, scadenza);
        //build_scadenza_heap(&alimento->lotti, alimento->lotti.heapsize);
        
    } // esco dal while quando leggo il nome della nuova istruzione, il nome dell'istruzione sara conservata nella variabile alimento


    // visto che ho aggiunto elementi in dispensa chiamo funzione controlla_ordini_sospesi che controlla se adesso sono presenti tutti gli elementi per poterli eseguire
    controlla_ordini_sospesi();

    printf("rifornito");
    return;
}

void carica_corriere(){
    if(ordini.pronti == NULL || ordini.pronti->peso > corriere.capienza){
        printf("\ncamioncino vuoto");
        return;
    }

    //trovo il primo ordine che non dovrà essere caricato sul camioncino
    ordine_nodo * end = trova_primo_ordine_da_non_caricare();


    ordini.pronti = peso_quicksort(ordini.pronti, end);

    ordine_nodo*curr=ordini.pronti;
    ordine_nodo*next;
    while(curr!=end){
        printf("\n%d %s %d", curr->istante_arrivo, curr->ricetta->nome, curr->quantita);
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
    
    printf("livello %d, value: %s, colore: %c padre:%s - %s  {l:%d, h:%d}\n",
        livello, 
        alimento->nome,
        ((alimento->color)==BLACK?'b':'r'), 
        alimento->parent!=NULL?(alimento->parent)->nome:"-1",
        alimento->parent==NULL?"radice":(alimento->parent->left==alimento?"sinistro":"destro"),
        alimento->lotti.length,
        alimento->lotti.heapsize
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
        ingrediente_nodo curr;

        for(int i=0; i<ricetta->num_ingredienti; i++){
            curr = ricetta->ingredienti[i];
            lotti_heap lotti = curr.alimento->lotti;
            printf("        %s", curr.alimento->nome);
            if (lotti.heapsize == 0)
                printf(" {vuoto} ");
           
            for(int i =0; i < lotti.heapsize; i++){
                printf(" {q:%d, s:%d} ", lotti.data[i].quantita, lotti.data[i].scadenza);
            }
            printf("richiesta: %d", curr.quantita);
            printf("\n");
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

    printf("- %s, peso: %d. ultimo_ordine_rifiutato: {t:%d, q:%d}\n", ricetta->nome, ricetta->peso, ricetta->ultimo_ordine_rifiutato.istante_arrivo, ricetta->ultimo_ordine_rifiutato.quantita);
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
        pos_input = avanza_pnt_di_n_parole(pos_input, 1);

        
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