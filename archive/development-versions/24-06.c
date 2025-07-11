#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//DICHIARAZIONE TIPI DI DATO
typedef struct{
	int periodicità;
	int capienza;
} corriere;

//VARIABILI GLOBALI
char istruzione[255];
int now = 0;
corriere furgoncino;

//FUNZIONI AUSILIARIE

//controlla che la stringa passata per input sia un'istruzione valida
int is_istruzione(char*str){
	return !(strcmp(str, "aggiungi_ricetta") && strcmp(str, "rimuovi_ricetta") && strcmp(str, "ordine") && strcmp(str, "rifornimento"));
}


//gestisce l'istruzione aggiungi_ricetta
void aggiungi_ricetta(){
	printf("aggiungi ricetta\n");
	return;
}


//gestisce l'istruzione rimuovi_ricetta
void rimuovi_ricetta(){
	printf("rimuovi ricetta\n");
	return;
}


//gestisce l'istruzione ordine
void ordine(){
	printf("ordine\n");
	return;
}


//gestisce l'istruzione rifornimento
void rifornimento(){
	printf("rifornimento\n");
	return;
}

int main(){
	//CONFIGURAZIONE DEL FURGONCINO
	scanf("%d %d", &(furgoncino.periodicità), &(furgoncino.capienza));

	scanf("%s", istruzione);

	while(is_istruzione(istruzione)){

		//controllo se arriva furgoncino
		if(now % furgoncino.periodicità == 0)
			printf("arrivato il furgoncino\n");


		//chiamo la funzione corrispondente all'istruzione
		if(strcmp(istruzione, "aggiungi_ricetta")==0)
			aggiungi_ricetta();

		else if(strcmp(istruzione, "rimuovi_ricetta")==0)
			rimuovi_ricetta();

		else if(strcmp(istruzione, "ordine")==0)
			ordine();

		else if(strcmp(istruzione, "rifornimento")==0)
			rifornimento();

	
		scanf("%s", istruzione);
		//incremento istante di tempo
		now++;
	}



	return 0;
}