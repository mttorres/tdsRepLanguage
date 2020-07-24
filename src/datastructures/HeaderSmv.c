#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../headers/HeaderSmv.h"
#include "../../headers/Enum.h"
#include "../../headers/textManager.h"

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
			free(h->varBuffer[i]); // note que poderiamos nos livrar de strings dinamicas nesse caso (reformular métodos CAT e refs para static string?)
		}					 // not really... STRINGS SÃO PERDIDAS A NÃO SER QUE SEJAM LITERAIS ou alocadas dentro de função!
  	}						 // ou alocar todo mundo ou tornar todas literal (provavelmente alocar todo mundo)	
  	free(h->varBuffer);
	
 	for(i = 0; i< h->ASSIGN_POINTER; i++) {
		if(h->assignBuffer[i]){
			free(h->assignBuffer[i]); 
		}					 
  	}
  	free(h->assignBuffer);		

 	for(i = 0; i< h->TRANS_POINTER; i++) {
		if(h->transBuffer[i]){
			free(h->transBuffer[i]); 
		}					 
  	}
  	free(h->transBuffer);	

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
		printf(" %s - %d \n",h->moduleName,h->type);

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



// salva o header do módulo lido anteriormente
void initPreProcessHeader(int type, char* moduleName, HeaderController* Hcontrol) {
	int transP = type == AUTOMATA? 0 : -1;
	Hcontrol->headers[Hcontrol->CURRENT_SIZE] = createHeader(type, moduleName, 0, 0, transP);
	Hcontrol->CURRENT_SIZE += 1;	
}

void selectBuffer(int part, char* line, HeaderSmv* header, int controlRename){
	int pt;
	char* aloc = malloc((strlen(line)+1) * sizeof(char));
	if(part != TRANS){
		strcpy(aloc,line);
		if(part == VAR) {
			pt = header->VAR_POINTER;
			header->varBuffer[pt] = aloc;
			header->VAR_POINTER += 1;	
		}
		if(part == ASSIGN) {
			pt = header->ASSIGN_POINTER;
			header->assignBuffer[pt] = aloc;
			header->ASSIGN_POINTER += 1;
		}
	}
	else{
		pt = header->TRANS_POINTER;
		if(!controlRename){
			strcpy(aloc,line);
			header->transBuffer[pt] = aloc;
		}
		else{
			//char** bufferAux = clearOldPortsRefs(line); 
			printf("[selectBuffer] tratamento de rename refs a portsModule ANTES:%s\n\n",line);
			//strcpy(aloc,bufferAux);
			header->transBuffer[pt] = clearOldPortsRefs(line);
			printf("[selectBuffer] tratamento de rename refs a portsModule DEPOIS:%s\n\n",header->transBuffer[pt]);			
			//if(bufferAux){
			//	free(bufferAux);	
			//}
		}
		header->TRANS_POINTER += 1;

	}
}

void saveLineOnBuffer(int pos,int part, char* line, HeaderController* Hcontrol, int controlRename) {
	selectBuffer(part,line,Hcontrol->headers[pos-1],controlRename);
}

// fases: criação, var, assign(pode não existir), trans(pode não existir) (as partes de interesse)
// as partes de interesse servem como delimitadores,  quebras de linha servem como delimitadores dos módulos
// stages são os módulos 0(main), automato(2), ports(3)
void processPhase(int stage, int part, HeaderController* Hcontrol, char * line, int controlRename) {

	// modulo
	if(part == CREATE_MODULE) {
		printf("[processPhase] Start module %d %s \n",Hcontrol->CURRENT_SIZE,line);
		initPreProcessHeader(stage,line, Hcontrol);
	}
	// VAR, ASSIGN, TRANS
	else{
		saveLineOnBuffer(Hcontrol->CURRENT_SIZE,part,line,Hcontrol,controlRename);	
	}


}

// depois(!): conversar com o felipe sobre como "diferenciar" smvs do transform 
void preProcessSmv(FILE* smvP, HeaderController* Hcontrol) {
	
	/*Strings que são usadas para a busca no arquivo*/
	char varString[] = "VAR";	
	char assignString[] = "ASSIGN";
	char transString[] = "TRANS";
	char fVarString[] = "FROZENVAR";
	char portsModuleString[] = "MODULE portsModule";
	char automataString[] = "MODULE finalAutomata(time)";
	char beginModuleString[] = "MODULE ";	
	char confirmAutomataString[] = "cs: {"; // se não encontrar isso após "começar a ler um automato" quer dizer que era um módulo qualquer
	// ATUALIZAR:  usar string "caracteristica" para detectar módulos de "função" (transform)

	/*Parâmetro a ser adicionado a ds[ultimo]->buffer[0] = PortsModule */
	char timeString[] = "time";

	/*Variáveis de controle de cursor e buffer
		-> propriedades do buffer e buffer auxiliar
	*/
	
	char* buffer;
	char* bufferAux;
    size_t bufsize = 300;
   	buffer = (char *) malloc(bufsize * sizeof(char));
	//bufferAux = (char *) malloc(bufsize * sizeof(char));

	/*Variáveis de controle de "onde no arquivo estamos"
	    -> estamos em qual "estagio" de interesse (1 = MAIN,  2 = Módulos(automato e funcoes), 3 = Ports)
		-> indice baseado no estagio corrente
		-> em qual fase dos estágios estamos (-1 = lendo inicio do módulo, 0 =var ,  1 = ASSIGN, 2= TRANS  ) (note que assign e trans podem não existir )
		-> estamos iniciando o main
		-> estamos lendo um ASSIGN
		-> estamos lendo a relação de transição do automato
		-> deve-se renomear referencias a portsModule
		-> estamos lendo automato
		-> estamos lendo portsModule
		-> renomeamos portsModule
	*/
	int stage = MAIN;
	int index;
	int phase = CREATE_MODULE;
	int readAssign = 0;
	int readTrans = 0; // o !readTrans mais abaixo é usado para evitar comparações desncessárias com a string gigante de transições 
	int controlRename = 0;
	int readAutomata = 0;
	int readPortsModule = 0;
	int renamePortsParam = 0;

   	while ((fgets(buffer,bufsize,smvP))) {
   		
   		bufferAux = buffer;

   		// ajuda a não rodar na primeira passada (do main)
   		if(stage != MAIN && !readPortsModule && !readAutomata){
   			if(strstr(buffer,portsModuleString)) {
   				stage = PORTS;
   				bufferAux = addParamModule(buffer,timeString);
   				renamePortsParam = 1;
   				readPortsModule = 1;
   			}
   			else{
   				stage = AUTOMATA;
   				readAutomata = 1;
   			}
   		}

   		if(phase == TRANS) {
   			printf("[preProcessSmv] Buffer pré computePhase 3 %s \n\n",buffer);
   			if(buffer[0] != '\n'){
   				readTrans = (buffer[0] != 'I')? readTrans : 0;
   				controlRename = readTrans? 1 : 0;
   			}
   			else{
   				phase = CREATE_MODULE;
   				stage++;
   				readAutomata = 0;
   				readPortsModule = 0;
   				printf("[preProcessSmv] Buffer pré computePhase 2  módulo acabou \n\n");
   				continue;
   			}
   		}
   		
   		if(phase == ASSIGN){
   			printf("[preProcessSmv] Buffer pré computePhase 2 %s \n\n",buffer);
   			if(!readPortsModule){
   				//isto é o módulo não acabou sem as transições 
   				if(buffer[0] != '\n'){
   					readTrans =  (buffer[0] == 'T' && strstr(buffer,transString));
   					if(readTrans){
   						phase = TRANS; // mudou de fase
   					}
   				}
   				// módulo acabou 
   				else{
   					// ignorar \n e reiniciar phases
   					phase = CREATE_MODULE;
   					stage++;
   					readAutomata = 0;
   					readPortsModule = 0;
   					printf("[preProcessSmv] Buffer pré computePhase 2  módulo acabou \n\n");
   					continue;
   				}
   			}
   			else{
   				break; // não precisa mais ler(vai ser lido da arvore)
   			}
   		}

   		// usa a primeira letra de cada parte de interesse primeiro para realizar curto circuito
   		if(phase == VAR) {
   			printf("[preProcessSmv] Buffer pré computePhase1 %s \n\n",buffer);
   			readAssign = (buffer[0] == 'A' && strstr(buffer,assignString)); 
   			readTrans =  (buffer[0] == 'T' && strstr(buffer,transString));

   			if(!(readAssign || readTrans)) {
   				if(readPortsModule){
   					printf("[preProcessSmv] debug: FVAR %s\n",buffer);
   				}
   				int iniVar = Hcontrol->headers[Hcontrol->CURRENT_SIZE-1]->VAR_POINTER == 0;
   				if(iniVar &&  strstr(buffer,fVarString)) {
   					printf("[preProcessSmv] Trocou FVAR %s\n",buffer);
   					bufferAux = varString; // tratamento de FROZEN VAR
   				}   				
   			}
   			else{
   				phase = readAssign ? ASSIGN : TRANS;
 				readAssign = 0;
   			}

   		}
   		
   		processPhase(stage,phase,Hcontrol,bufferAux,controlRename);
   		if(phase == CREATE_MODULE){
   			phase++;
   		}
   		if(renamePortsParam && phase == PORTS){
   			free(bufferAux);
   			renamePortsParam = 0;
   		}
   	}
   	free(buffer);
   	printf("terminou! \n");	
}