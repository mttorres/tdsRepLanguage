#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../headers/HeaderSmv.h"


HeaderSmv* createHeader(enum smvtype type , char* moduleName, int varP, int assignP, int transP) {

  HeaderSmv* header = (HeaderSmv*) malloc(sizeof(HeaderSmv));

  if(type != MAIN){
      header->PARAM_MAP = malloc(sizeof(int)*200); // NOTE ! ele não inicia com zeros! Deve fazer limpeza.
      for (int i = 0; i < 200; i++) {
          header->PARAM_MAP[i] = 0;
      }
  }

  if(type == PORTS){
      char* refOldPt;
      // só adiciona parâmetro se for portsModule
      refOldPt = moduleName;
      char* newDeclaration = addParams(refOldPt, "time", "(", ")", 0);
      header->moduleName = newDeclaration;
      int mapTime = hash("time",200);
      header->PARAM_MAP[mapTime] = 1;
  }
  else{
      if(type == AUTOMATA){
          header->moduleName = overwriteParam(moduleName,"ports");
          int mapPorts = hash("ports",200);
          header->PARAM_MAP[mapPorts] = 1;
      }
      else{
          char* name = malloc((strlen(moduleName)+1) * sizeof(char));
          strcpy(name, moduleName);
          header->moduleName = name;
      }
  }
  
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
  else{
  	header->transBuffer = NULL;
  }

//  if(type == MAIN){
//      header->VAR_RENAME_POINTER = 2;
//  }
//  else{
      header->VAR_RENAME_POINTER = -1; // ref a ports Module ficará em main
//  }

  return header; 
}

void letgoHeader(HeaderSmv* h){

	if(!h){
		return;
	}
  	//printf("[letgoHeader] DEBUG: liberando %s \n",h->moduleName);
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

  	//rintf("[letgoHeader] DEBUG: liberando buffer VAR! \n");
  	free(h->varBuffer);
	
 	for(i = 0; i< h->ASSIGN_POINTER; i++) {
		if(h->assignBuffer[i]){
			free(h->assignBuffer[i]); 
		}					 
  	}
  	//printf("[letgoHeader] DEBUG: liberando buffer ASSIGN! \n");
  	free(h->assignBuffer);		

  	if(h->transBuffer) {
		for(i = 0; i< h->TRANS_POINTER; i++) {
			//printf("???\n");
			if(h->transBuffer[i]){
				free(h->transBuffer[i]); 
			}					 
  		}
  		//printf("[letgoHeader] DEBUG: liberando buffer TRANS! \n");
  		free(h->transBuffer);
  	}
  		

  	free(h);  				
}

void letGoRelatedHeaders(HeaderSmv** RHeader, int size){
    int i;
    if(RHeader){
        for(i = 0; i< size; i++){
            letgoHeader(RHeader[i]);
            RHeader[i] = NULL;
        }
    }
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

// 	if(h->type == PORTS){
// 		printf("ports: %s\n",h->declaredPortsNumber[0]);
// 	}

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

void writeHeaderBuffer(HeaderSmv* h, int part, FILE* smvoutput){
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
	for(i = 0; i< offsetBuffer; i++){
		if(part == VAR && h->varBuffer[i]) {
			fprintf(smvoutput,"%s",h->varBuffer[i]);
		}
		if(part == ASSIGN && h->assignBuffer[i]) {
			fprintf(smvoutput,"%s",h->assignBuffer[i]);
		}
		if(part == TRANS && h->transBuffer[i]) {
			fprintf(smvoutput,"%s",h->transBuffer[i]);
		}	
	}
}


void writeHeader(HeaderSmv* header, FILE* smvoutput){
	int sizeBuffers[] = {header->VAR_POINTER, header->ASSIGN_POINTER,header->TRANS_POINTER};
	int tamanho = sizeof(sizeBuffers)/sizeof(sizeBuffers[0]);
	int i; 
	fprintf(smvoutput,"%s",header->moduleName);
	for(i = 0; i< tamanho; i++){
		writeHeaderBuffer(header,i,smvoutput);
	}
	fprintf(smvoutput,"%s","\n");
}


void selectBuffer(headerpart part, char* line, HeaderSmv* header, int controlRename) {
    int pt;
    int tam = strlen(line);
    char* aloc = malloc((tam+2) * sizeof(char)); //  o +2 é estritamente para acomodar tamanho extra do buffer
    if(part != TRANS)
    {
        if(part == VAR)
        {

            if(header->type == AUTOMATA || (header->type == MAIN && header->VAR_POINTER != header->VAR_RENAME_POINTER) ){

                char* ref = strstr(line,"(");
                if(ref){
                    ref = overwriteParam(line,"ports");
                    strcpy(aloc,ref); // CUIDADO, strcpy copia mesmo que o buffer dest não tenha tamanho  o suficiente
                    free(ref);
                }
                else{
                    strcpy(aloc,line);
                }
            }else{
                strcpy(aloc,line);
            }
            pt = header->VAR_POINTER;
            header->varBuffer[pt] = aloc;
            header->VAR_POINTER += 1;
        }
        if(part == ASSIGN)
        {
            strcpy(aloc,line);
            pt = header->ASSIGN_POINTER;
            header->assignBuffer[pt] = aloc;
            header->ASSIGN_POINTER += 1;
        }
    }
    else
    {
        pt = header->TRANS_POINTER;
        if(!controlRename)
        {
            strcpy(aloc,line);
            header->transBuffer[pt] = aloc;
        }
        else
        {
            clearOldPortsRefs(line,aloc);
            header->transBuffer[pt] = aloc;
        }
        header->TRANS_POINTER += 1;

    }
}

void addParamToHeader(HeaderSmv* headerSmv, char* param){
	char* refOldPt = headerSmv->moduleName;
	char* newDeclaration = addParams(refOldPt, param, "(", ")", 0);
	free(headerSmv->moduleName);
	headerSmv->moduleName = newDeclaration;
}

void propagParamDependence(HeaderSmv** headers, char* param, int sizeHeaders){
    int i;
    char* refOldPt;
    for(i = 0; i < sizeHeaders; i++){
        refOldPt = headers[i]->varBuffer[headers[i]->VAR_RENAME_POINTER];
        char* newDeclaration = addParams(refOldPt, param, "(", ")", 0);
        free(refOldPt);
        headers[i]->varBuffer[headers[i]->VAR_RENAME_POINTER] = newDeclaration;
    }
}

