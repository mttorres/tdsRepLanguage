#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum smvpart {MAIN = 1, AUTOMATA = 2 , PORTS = 3} ;

typedef struct headersmv
{
  int type;
  long modulePointer;
  long varPointer;
  long assignPointer;
  long transPointer;
  long CURR_OFFSET;

}HeaderSmv;


HeaderSmv* createHeader(int type,long moduleP, long varP, long assignP, long transP, long offset);

void printHeader(HeaderSmv* h);

void letgoHeader(HeaderSmv* h);



HeaderSmv* createHeader(int type, long moduleP, long varP, long assignP, long transP, long offset){

  HeaderSmv* header = (HeaderSmv*) malloc(sizeof(HeaderSmv));
  header->type = type;
  header->modulePointer = moduleP;
  header->varPointer = varP;
  header->assignPointer = assignP;
  header->transPointer = transP;
  header->CURR_OFFSET = offset;

  return header; 


}

void printHeader(HeaderSmv* h){

  if(h) {
	if(h->type  && h->modulePointer){
		printf("%d: MODULE() - %ld \n",h->type,h->modulePointer);
	
	}
	if(h->varPointer){
		printf(" VAR - %ld \n",h->varPointer);
	}
	if(h->assignPointer){
		printf(" ASSIGN - %ld \n",h->assignPointer);
	}
	if(h->transPointer){
		printf(" TRANS - %ld \n",h->transPointer);
	}	
	if(h->CURR_OFFSET){
		printf(" OFFSET - %ld \n",h->CURR_OFFSET);
 	} 
 }
}

void letgoHeader(HeaderSmv* h){

	if(!h){
		return;
	}
	free(h);


}

HeaderSmv** initHeadersStruct(int size);

void letGoHeadersStruct(int size);

void preProcessSmv(FILE* smvP, long* ds);

void postProcessSmv(FILE* smvP, int* ds);


HeaderSmv** initHeadersStruct(int size){
  
  HeaderSmv** headers = ( HeaderSmv**) malloc(sizeof(HeaderSmv*)*size);
  return headers;
}

void letGoHeadersStruct(HeaderSmv** hs, int size){
  
  for(i = 0; i< size; i++){
	letgoHeader(hs[i]);
	hs[i] = NULL;
  }
  free(hs);
}


void preProcessSmv(FILE* smvP, HeaderSmv** ds) {
	
	char* varString = "VAR";
	char* fVarString = "FROZENVAR";	
	char* assignString = "ASSIGN";
	char* transString = "TRANS";

	char* portsModuleString = "MODULE portsModule";
	char* automataString = "MODULE finalAutomata(time)";	
	
	char *buffer;
    	size_t bufsize = 300;
	
	long prevFcursor = 0L;

	long moduleCursor = 0L;
  	long varCursor = 0L;
	long assignCursor = 0L; 
	long transCursor = 0L;


	int readMain = 1;
	
	int readAutomata = 0;

	int readPortsModule = 0;
        int clearPortsModule = 0;	

	buffer = (char *) malloc(bufsize * sizeof(char));
   	while ((fgets(buffer,bufsize,smvP))) {
		
		if(clearPortsModule){
			fwrite("", 1, sizeof(buffer), smvP);
		}		

		if(strstr(buffer,varString)){
			varCursor = prevFcursor;			
		}

		if(strstr(buffer,fVarString){
			varCursor = preFcursor;	
		}

		if(strstr(buffer,assignString){
			assignCursor = preFcursor;
			if(readPortsModule){
			  clearPortsModule = 1;				
			}
			ds[PORTS-1]=createHeader(PORTS,moduleCursor,varCursor,assignCursor,0,0);
				
		}
		if(strstr(buffer,transString){
			transCursor = preFcursor;
			if(readAutomata){
				//renomear refs.			
			}	
		}		

		if(strstr(buffer,automataString)){
			ds[MAIN-1] = createHeader(MAIN, moduleCursor, varCursor, assignCursor, transCursor, 0);	
			readMain = 0;	
			readAutomata = 1;	
			//printf("%s \n",buffer);
					
		} 	
	
		if(strstr(buffer,portsModuleString)){
		       ds[AUTOMATA-1]=createHeader(AUTOMATA,moduleCursor,varCursor,assignCursor,transCursor,0);				
			readAutomata = 0;
			readPortsModule = 1;			
			
					
		}

		prevFcursor = ftell(smvP);
		
	}
	free(buffer);	
   

}






int main()
{
//TESTE PARMETROS MODULOS
    HeaderSmv** headers = initHeadersStruct(3);
    printf("-----------EITA----------------------\n");
    char* automataString = "MODULE finalAutomata(time)";
    char *lastParStr = (char*)malloc(sizeof(char)*(strlen(automataString)+1));
    strncpy(lastParStr, automataString,strlen(automataString));
    printf("lets head back... %s \n",lastParStr);
    printf("-----------EITA----------------------\n");
    free(lastParStr);
    printf("\n");

    

//TESTE PARMETROS MODULOS

    
    FILE *smvP; // .smv file;
    smvP = fopen("sample/novo/merger-fifo/nuxmv.smv","r+");
//    long * partesArquivoSmv = (long*) malloc(sizeof(long)*3); // MAIN, AUTOMATO, PORTS MODULE
    //long * partesArquivoSmv = (long*) malloc(sizeof(long)*2); // VAR, assign

    // "outra struct"  ---> localização dos principais "cabeçalhos" (3 exatamente)  e um offset que deve ser verificado...

    // onde salvar? IDEIA: MAIN (ESCOPO-MAIN), AUTOMATO (outra struct), PORTS MODULE (outra struct tmb "header") 
    //				 --> cada variavel : entrada e "pos" no arquivo			--> cada tds: entrada e "pos" no arquivo
    // fazer em "ordem" ou atualizar as entradas depois? (mesmo em ordem pode necessitar atualizar depois?)   
    
    //preProcessSmv(smvP,headers); 

/*
    char *buffer;
    size_t bufsize = 300;
    buffer = (char *) malloc(bufsize * sizeof(char));	
    fgets(buffer,bufsize,smvP);
    //printf("%s \n",buffer);    

    int i = 0;
    for(i = 0; i < 3; i++) {
    	printf("PARTES: %ld \n",partesArquivoSmv[i]);
	fseek(smvP,partesArquivoSmv[i],SEEK_SET);
	fgets(buffer,bufsize,smvP);
	printf("%s \n",buffer);
	
    } 
*/
    fclose(smvP);
    
//  free(buffer);	
    return 0;
}
