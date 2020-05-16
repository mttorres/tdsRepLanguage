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
	
		printf("%d: MODULE() - %ld \n",h->type,h->modulePointer);
	
	
	
		printf(" VAR - %ld \n",h->varPointer);
	
		printf(" ASSIGN - %ld \n",h->assignPointer);
	
		printf(" TRANS - %ld \n",h->transPointer);
	
		printf(" OFFSET - %ld \n",h->CURR_OFFSET);
 	
 }
}

void letgoHeader(HeaderSmv* h){

	if(!h){
		return;
	}
	free(h);


}

HeaderSmv** initHeadersStruct(int size);

void letGoHeadersStruct(HeaderSmv** hs, int size);

void preProcessSmv(FILE* smvP, HeaderSmv** ds);

void postProcessSmv(FILE* smvP, int* ds);


HeaderSmv** initHeadersStruct(int size){
  
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
			if(readAutomata){
			 	printf("yare yare...\n");
			}			
		}

		if(strstr(buffer,fVarString)){
			varCursor = prevFcursor;	
		}

		if(strstr(buffer,assignString)){
			assignCursor = prevFcursor;
			if(readPortsModule){
			  clearPortsModule = 1;				
			}
			ds[PORTS-1]=createHeader(PORTS,moduleCursor,varCursor,assignCursor,0,0);
				
		}
		if(strstr(buffer,transString)){
			transCursor = prevFcursor;
			if(readAutomata){
				//renomear refs.			
			}	
		}		

		if(strstr(buffer,automataString)){
			moduleCursor = prevFcursor;			
			ds[MAIN-1] = createHeader(MAIN, 0, varCursor, assignCursor, 0, 0);	
			readMain = 0;	
			readAutomata = 1;				
			//printf("%s \n",buffer);
					
		} 	
	
		if(strstr(buffer,portsModuleString)){
			moduleCursor = prevFcursor;
		        //buffer = 
		       ds[AUTOMATA-1]=createHeader(AUTOMATA,moduleCursor,varCursor,assignCursor,transCursor,0);				
			readAutomata = 0;
			readPortsModule = 1;			
					
		}

		prevFcursor = ftell(smvP);
		
	}
	free(buffer);	
   

}

char * customCat(char* dest, char* src, int mode);

char * customCat(char* dest, char* src, int mode) {

	while(*dest){
		printf("%s \n",dest);				
		dest++;
	}
	while(*src){
		*dest = *src;
		printf("d : %s \n",dest);
		printf("s : %s \n",src);				
		dest++;
		src++;
		if(mode && *src == ')'){
			break;
		}
	}
	--dest;
	return dest;

}



char* addParamModule(char* original, char* param);

char* addParamModule(char* original, char* param) {
 
	char *newString;
	if(original[strlen(original)-1] == ')'){
		newString = (char*) malloc(sizeof(char)*(strlen(original)+2+strlen(param)+1)); // , e  \0
		newString = customCat(newString,original,1);
		newString = customCat(newString,", ",0);
		newString = customCat(newString,param,0);
		newString = customCat(newString,")",0);			
		newString = newString - ((strlen(original)+strlen(param)+2)-1);	
	}
	else {
		printf("%ld %ld \n",strlen(original),strlen(param));	   	
		newString = (char*) malloc(sizeof(char)*(strlen(original)+1+strlen(param)+1+1)); // ( , ) e  \0	   			
		newString = customCat(newString,original,0);
		newString = customCat(newString,"(",0);
		newString = customCat(newString,param,0);
		newString = customCat(newString,")",0);	
		newString = newString - ((strlen(original)+strlen(param)+2)-1); 
	}

	return newString;
}



int main()
{
//TESTE PARMETROS MODULOS
    //HeaderSmv** headers = initHeadersStruct(3);
    printf("-----------EITA----------------------\n");
    char* automataString = "MODULE finalAutomata(time)";
    char* portsModuleString = "MODULE portsModule";
    char* param1 = "modoEntrada";    
    char* param2 = "time";

    char* novo = addParamModule(automataString, param1);    
    
    printf("lets head back... %s \n",novo);
    printf("-----------EITA----------------------\n");
    free(novo);
    printf("\n");
   


//TESTE PARMETROS MODULOS

/*    
    FILE *smvP; // .smv file;
    smvP = fopen("sample/novo/merger-fifo/nuxmv.smv","r+");
    // "outra struct"  ---> localização dos principais "cabeçalhos" (3 exatamente)  e um offset que deve ser verificado...

    // onde salvar? IDEIA: MAIN (ESCOPO-MAIN), AUTOMATO (outra struct), PORTS MODULE (outra struct tmb "header") 
    //				 --> cada variavel : entrada e "pos" no arquivo			--> cada tds: entrada e "pos" no arquivo
    // fazer em "ordem" ou atualizar as entradas depois? (mesmo em ordem pode necessitar atualizar depois?)   
    
    preProcessSmv(smvP,headers); 


    char *buffer;
    size_t bufsize = 300;
    buffer = (char *) malloc(bufsize * sizeof(char));	
    fgets(buffer,bufsize,smvP);
    int i;
    for(i = 0; i < 3; i++) {
    	//printf("PARTES: %ld \n",partesArquivoSmv[i]);
	fseek(smvP,headers[i]->modulePointer,SEEK_SET);
	fgets(buffer,bufsize,smvP);
	printf("(%d) %s \n",i,buffer);
	
	fseek(smvP,headers[i]->varPointer,SEEK_SET);
	fgets(buffer,bufsize,smvP);
	printf("(%d) %s \n",i,buffer);

	fseek(smvP,headers[i]->assignPointer,SEEK_SET);
	fgets(buffer,bufsize,smvP);
	printf("(%d) %s \n",i,buffer);

	fseek(smvP,headers[i]->transPointer,SEEK_SET);
	fgets(buffer,bufsize,smvP);
	printf("(%d) %s \n",i,buffer);

    } 

    fclose(smvP);
    printHeader(headers[0]);
    printHeader(headers[1]);
    printHeader(headers[2]);
    letGoHeadersStruct(headers,3);   
//  free(buffer);	
*/
    return 0;
}
