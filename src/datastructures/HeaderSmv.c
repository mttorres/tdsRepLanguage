#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../headers/HeaderSmv.h"
#include "../../headers/Enum.h"


HeaderSmv* createHeader(int type, char* moduleName, int varP, int assignP, int transP) {

  HeaderSmv* header = (HeaderSmv*) malloc(sizeof(HeaderSmv));
  
  char* name = malloc((strlen(moduleName)+1) * sizeof(char));
  strcpy(name, moduleName);
  header->moduleName = name; 

  
  header->type = type;
  header->VAR_POINTER = varP;
  header->ASSIGN_POINTER = assignP;  
  header->TRANS_POINTER = transP;

  header->varBuffer = malloc(sizeof(char*)*300);
  header->assignBuffer = malloc(sizeof(char*)*300);
  if(type == AUTOMATA){
  	//printf("[createHeader] DEBUG: alocando transicoes tipo (%d) \n",type);
  	header->transBuffer = malloc(sizeof(char*)*300); // tamanho default
  }

  return header; 
}

void letgoHeader(HeaderSmv* h){

	if(!h){
		return;
	}
	free(h->moduleName);
	int i;
 	
 	for(i = 0; i< h->VAR_POINTER; i++){
		if(h->varBuffer[i]){
			free(h->varBuffer[i]); 

		}
	}
	// note que poderiamos nos livrar de strings dinamicas nesse caso (reformular métodos CAT e refs para static string?)
	// not really... STRINGS SÃO PERDIDAS A NÃO SER QUE SEJAM LITERAIS ou alocadas dentro de função!
  	// ou alocar todo mundo ou tornar todas literal (provavelmente alocar todo mundo)	

  	printf("[letgoHeader] DEBUG: liberando buffer VAR! \n");
  	free(h->varBuffer);
	
 	for(i = 0; i< h->ASSIGN_POINTER; i++) {
		if(h->assignBuffer[i]){
			free(h->assignBuffer[i]); 
		}					 
  	}
  	printf("[letgoHeader] DEBUG: liberando buffer ASSIGN! \n");
  	free(h->assignBuffer);		

  	if(h->transBuffer) {
		for(i = 0; i< h->TRANS_POINTER; i++) {
			printf("???\n");
			if(h->transBuffer[i]){
				free(h->transBuffer[i]); 
			}					 
  		}
  		printf("[letgoHeader] DEBUG: liberando buffer TRANS! \n");
  		free(h->transBuffer);
  	}
  		

  	free(h);  				
}


HeaderSmv** initHeadersStruct(int size) {
  
  HeaderSmv** headers = ( HeaderSmv**) malloc(sizeof(HeaderSmv*)*size);
  return headers;
}

void letGoHeadersStruct(HeaderSmv** hs, int size){
  
  int i;
  for(i = 0; i< size; i++){
	letgoHeader(hs[i]);
	hs[i] = NULL;
  }
  free(hs);
}

HeaderController* createController(int size) {
	HeaderController* Hcontrol = malloc(sizeof(HeaderController));
	Hcontrol->CURRENT_SIZE = 0;
	Hcontrol->headers = initHeadersStruct(size);

	return Hcontrol;
}


void letGoHeaderControl(HeaderController* Hcontrol) {
	letGoHeadersStruct(Hcontrol->headers,Hcontrol->CURRENT_SIZE);
	free(Hcontrol);
}


void printHeaderBuffer(HeaderSmv* h, int part, char* partString) {
	int i;
	int offsetBuffer;
	if(part == VAR){
		offsetBuffer = h->VAR_POINTER;
	}
	if(part == ASSIGN){
		offsetBuffer = h->ASSIGN_POINTER;
	}
	if(part == TRANS){
		offsetBuffer = h->TRANS_POINTER;
	}

 	printf("------------BUFFER %s----------- \n \n ",partString);
 	for(i = 0; i< offsetBuffer; i++){
		if(part == VAR && h->varBuffer[i]) {
			printf("%s",h->varBuffer[i]);
		}
		if(part == ASSIGN && h->assignBuffer[i]) {
			printf("%s",h->assignBuffer[i]);
		}
		if(part == TRANS && h->transBuffer[i]) {
			printf("%s",h->transBuffer[i]);
		}		
  }
  printf("------------BUFFER %s----------- \n \n ",partString);
}

void printHeader(HeaderSmv* h) {

  if(h) {
		printf("------------INFO----------- \n \n ");
		printf("(%d) %s",h->type,h->moduleName);

		printf(" VAR: %d \n",h->VAR_POINTER);
	
		printf(" ASSIGN: %d \n",h->ASSIGN_POINTER);
	
		printf(" TRANS: %d \n",h->TRANS_POINTER);

		printf("------------INFO----------- \n \n ");

 	
 	}
	char* tiposBuffers[] = {
    	"VAR",
    	"ASSIGN",
    	"TRANS",    	
	};
	int tamanho = sizeof(tiposBuffers)/sizeof(tiposBuffers[0]);
	int i;
 	for(i = 0; i< tamanho; i++){
		printHeaderBuffer(h,i,tiposBuffers[i]);
	}

}



