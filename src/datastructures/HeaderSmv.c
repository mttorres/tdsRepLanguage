#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../headers/HeaderSmv.h"

/**
 * Adiciona a um header as informaçoes de filter (caso seja necessario)
 * @param header o header corrente
 * @param finalAutomataFilterModel a informaçao de contexto fornecida pelo controlador de ambiente que diz se o modelo lido tem filter no final automata
 */
void filterAutomataHeaderCheck(HeaderSmv* header, int finalAutomataFilterModel);

/**
 * Adiciona os pontos de interesse da transiçao de um automato as informaçoes do header.
 * NOTA: Assume que as strings do modelo sejam da forma xxxxxxxxxx & TRUE) ... Onde o true é uma condição parametrizável,
 * o primeiro a ser encontrado deve a condição de negação e o segundo a condição em si
 * @param pHeadersmv o header tendo a transiçao avaliada
 */
void getFilterPoints(HeaderSmv *pHeadersmv);

HeaderSmv * createHeader(enum smvtype type, char *moduleName, int varP, int assignP, int transP, int finalAutomataFilterModel) {

  HeaderSmv* header = (HeaderSmv*) malloc(sizeof(HeaderSmv));
  if(type != MAIN){
      header->PARAM_MAP = malloc(sizeof(int)*MAX_PARAM); // NOTE ! ele não inicia com zeros! Deve fazer limpeza.
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
      int mapTime = hash("time",MAX_PARAM);
      header->PARAM_MAP[mapTime] = 1;
  }
  else{
      if(type == AUTOMATA){
          header->moduleName = overwriteParam(moduleName,"ports");
          int mapPorts = hash("ports",MAX_PARAM);
          header->PARAM_MAP[mapPorts] = 1;
      }
      else{
          char* name = malloc((strlen(moduleName)+1) * sizeof(char));
          strcpy(name, moduleName);
          header->moduleName = name;
          header->expectFilter = -1;
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
  filterAutomataHeaderCheck(header,finalAutomataFilterModel);

//  if(type == MAIN){
//      header->VAR_RENAME_POINTER = 2;
//  }
//  else{
      header->VAR_RENAME_POINTER = -1; // ref a ports Module ficará em main e filter de finalAutomata
//  }

  return header; 
}

void letgoHeader(HeaderSmv* h){

	if(!h){
		return;
	}
  	//printf("[letgoHeader] DEBUG: liberando %s \n",h->moduleName);
	free(h->moduleName);
	h->moduleName = NULL;
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
	h->varBuffer = NULL;

 	for(i = 0; i< h->ASSIGN_POINTER; i++) {
		if(h->assignBuffer[i]){
			free(h->assignBuffer[i]); 
		}					 
  	}
  	//printf("[letgoHeader] DEBUG: liberando buffer ASSIGN! \n");
  	free(h->assignBuffer);		
    h->assignBuffer = NULL;

  	if(h->transBuffer) {
        for (i = 0; i < h->TRANS_POINTER; i++) {
            if (h->transBuffer[i]) {
                free(h->transBuffer[i]);
            }
        }
        //printf("[letgoHeader] DEBUG: liberando buffer TRANS! \n");
        free(h->transBuffer);
        h->transBuffer = NULL;
    }
  	if(h->PARAM_MAP){
  	    free(h->PARAM_MAP);
  	    h->PARAM_MAP = NULL;
  	}
  	if(h->filterPosNeg){
  	    free(h->filterPosNeg);
  	    h->filterPosNeg = NULL;
  	}
  	if(h->filterPosCond){
  	    free(h->filterPosCond);
  	    h->filterPosCond = NULL;
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
    int sizeLine = strlen(line);
    char* aloc = malloc(sizeLine*sizeof(char)+2); //  o +2 é estritamente para acomodar tamanho extra do buffer
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
            if(header->expectFilter){
                getFilterPoints(header);
            }
        }
        header->TRANS_POINTER += 1;

    }
}

void getFilterPoints(HeaderSmv *pHeadersmv) {
    char* transString = pHeadersmv->transBuffer[pHeadersmv->TRANS_POINTER];
    char* ref = strstr(transString," TRUE");
    if(ref){
        int ini = (ref-transString)+1;
        int end = ini+3;

        int negPos = pHeadersmv->filterPosNeg[0];

        if(!negPos){
            pHeadersmv->filterPosNeg[0] = pHeadersmv->TRANS_POINTER;
            pHeadersmv->filterPosNeg[1] = ini;
            pHeadersmv->filterPosNeg[2] = end;
        }
        else{
            pHeadersmv->filterPosCond[0] = pHeadersmv->TRANS_POINTER;
            pHeadersmv->filterPosCond[1] = ini;
            pHeadersmv->filterPosCond[2] = end;
        }
    }
}

void filterAutomataHeaderCheck(HeaderSmv* header, int finalAutomataFilterModel){
    int nameFilter = 0;
    int nameFinal = 0;
    if(header->type == AUTOMATA){
        if(header->moduleName[7] == 'f'){
            nameFilter = !!strstr(header->moduleName,"filter");
            if(finalAutomataFilterModel && !nameFilter){
                nameFinal = !!strstr(header->moduleName,"final");
            }
        }
    }
    header->expectFilter = nameFilter || nameFinal;
    if(header->expectFilter){
        header->filterPosNeg = malloc(sizeof(int)*3);
        header->filterPosCond = malloc(sizeof(int)*3);
    }
    else{
        header->filterPosNeg = NULL;
        header->filterPosCond = NULL;
    }
}


void updateAutomataFilterCond(const char *cond, const HeaderSmv *automata, int type) {
    int pos;
    int newIni;
    int newEnd;
    if(type){
        pos = automata->filterPosCond[0];
        newIni = automata->filterPosCond[1];
        newEnd = automata->filterPosCond[2];
    }
    else{
        pos = automata->filterPosNeg[0];
        newIni = automata->filterPosNeg[1];
        newEnd = automata->filterPosNeg[2];
    }
    char* refLine = automata->transBuffer[pos];
    int sizeOld = strlen(refLine);
    int sizeCond = strlen(cond);
    automata->transBuffer[pos] = updateSubStringInterval(cond,refLine,sizeCond,newIni,newEnd,
                                                         sizeOld,&newIni,&newEnd,0);
    if(type){
        automata->filterPosCond[1] = newIni;
        automata->filterPosCond[2] = newEnd;
    }
    else{
        automata->filterPosNeg[1] = newIni;
        automata->filterPosNeg[2] = newEnd;
    }
}

int addParamToModule(HeaderSmv* updated , char* param){
    int possibleParamPos = hash(param,MAX_PARAM);
    if(updated->PARAM_MAP[possibleParamPos]){
        return 0;
    }
    char* newName = addParams(updated->moduleName, param, "(", ")", 0);
    free(updated->moduleName);
    updated->moduleName = newName;
    updated->PARAM_MAP[possibleParamPos] = 1;
    return 1;
}


void addParamInterestPointHeader(HeaderSmv* header, char *paramName, int offSetPointer) {
    char* refOldPt;
    refOldPt = header->varBuffer[header->VAR_RENAME_POINTER-offSetPointer];
    char* newDeclaration = addParams(refOldPt, paramName, "(", ")", 0);
    free(refOldPt);
    header->varBuffer[header->VAR_RENAME_POINTER-offSetPointer] = newDeclaration;
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

