#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../headers/textManager.h"
#include "../headers/PreProcess.h"


/* 
	salva o headerSMV do módulo lido anteriormente. 
	se for um automato o ponteiro de transição não é vázio.
	efeito colateral: * ao criar ao salvar o header incrementa o tamanho de headers do HeaderController
					  *	cria um header (necessita de free depois) 
*/

void initPreProcessHeader(int type, char* moduleName, HeaderController* Hcontrol) {
	int transP = type == AUTOMATA? 0 : -1;
	Hcontrol->headers[Hcontrol->CURRENT_SIZE] = createHeader(type, moduleName, 0, 0, transP);
	Hcontrol->CURRENT_SIZE += 1;	
}

/*
	escolhe um buffer do header atual para salvar a linha (alocando sempre uma string para essa linha, que depois deve ser liberada).
	efeitos colaterais:  * ao ter readVarPortsModule como true, ele salva as variáveis do portsModule em uma tabela de portas
						 * ao ter controlRename como true, ele remove todas as ocorrências de determinados caracteres (no caso [])

*/
void selectBuffer(int part, char* line, HeaderSmv* header, int controlRename, int readVarsPortsModule, STable* portsSmv) { 
	int pt;
	int tam = strlen(line);
	char* aloc = malloc((tam+1) * sizeof(char));
	if(part != TRANS)
	{
		strcpy(aloc,line);
		if(part == VAR) 
		{
			pt = header->VAR_POINTER;
			header->varBuffer[pt] = aloc;

			if(readVarsPortsModule)
			{
				printf("[saveLineOnBuffer] Salvando tabela de portas (%s) \n\n",line);
				printf("[saveLineOnBuffer] Salvando tabela de portas (%c) \n\n",line[1]);

				void* po[] = {&pt, &tam};
				char name[] = {line[1],'\0'};

				//addValue(name,po,TYPE_SET,2,0,portsSmv);
				addTypeSet(name,po,TYPE_SET,2,portsSmv);
			}

			header->VAR_POINTER += 1;	
		}
		if(part == ASSIGN) 
		{
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

/*
	prepara para chamar a função de escolha de buffer, t
	ratando a posição (pos) que representa o tamanho do vetor de HEADERS do controller (LEN-1)
						 
*/
void saveLineOnBuffer(int pos,int part, char* line, HeaderController* Hcontrol, int controlRename, int readVarsPortsModule, STable* portsSmv) {
	selectBuffer(part,line,Hcontrol->headers[pos-1],controlRename,readVarsPortsModule,portsSmv);
}

/* fases: criação, var, assign(pode não existir), trans(pode não existir) (as partes de interesse)
 	as partes de interesse servem como delimitadores,  quebras de linha servem como delimitadores dos módulos
 	stages são os módulos 0(main), automato(2), ports(3)
*/
void processPhase(int stage, int part, HeaderController* Hcontrol, char * line, int controlRename, int readVarsPortsModule, STable* portsSmv) {

	// modulo
	if(part == CREATE_MODULE) {
		//printf("[processPhase] Start module %d %s \n",Hcontrol->CURRENT_SIZE,line);
		initPreProcessHeader(stage,line, Hcontrol);
	}
	// VAR, ASSIGN, TRANS
	else{
		saveLineOnBuffer(Hcontrol->CURRENT_SIZE,part,line,Hcontrol,controlRename,readVarsPortsModule, portsSmv);	
	}


}


void preProcessSmv(FILE* smvP, HeaderController* Hcontrol, STable* portsSmv) {
	
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


	/*Parâmetro a ser adicionado a PortsModule */
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
	int afterVarParPorts = 0;
	int readVarsPortsModule = 0;
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


   		if(buffer[0] != '\n') 
   		{
   			if(phase == TRANS)
   			{
   				//printf("[preProcessSmv] Buffer pré compute Phase 3 %s \n\n",buffer);
   				readTrans = (buffer[0] != 'I')? readTrans : 0;
   				controlRename = readTrans? 0 : 0; // usado para debugar o nosso renomeador de refs
   			}

   			if(phase == ASSIGN)
   			{
   				
   				if(!readPortsModule)
   				{
   					//printf("[preProcessSmv] Buffer pré computePhase 2 %s \n\n",buffer);
   					readTrans =  (buffer[0] == 'T' && strstr(buffer,transString));
   					
   					// na verdade entramos na fase de leitura de transições que veio LOGO DEPOIS DE ATRIBUIÇÕES
   					if(readTrans)
   					{
   						//printf("[preProcessSmv] terminou ASSIGN \n\n");
   						phase = TRANS; // mudou de fase
   					}

   				}
   				else 
   				{
   					break; // não precisa mais ler(vai ser lido da arvore)
   				}
   				

   			}
   			
   		}
   		// módulos são separados por quebra de linha
   		else
   		{
   			// reinicio
   			phase = CREATE_MODULE;
   			stage++;
   			readAutomata = 0;
   			//printf("[preProcessSmv] módulo acabou \n\n");
   			continue;
   		}


   		// usa a primeira letra de cada parte de interesse primeiro para realizar curto circuito
   		if(phase == VAR) 
   		{
   			//printf("[preProcessSmv] Buffer pré computePhase1 %s \n\n",buffer);
   			readAssign = (buffer[0] == 'A' && strstr(buffer,assignString)); 
   			readTrans =  (buffer[0] == 'T' && strstr(buffer,transString));

   			if(!(readAssign || readTrans)) 
   			{
   				int iniVar = Hcontrol->headers[Hcontrol->CURRENT_SIZE-1]->VAR_POINTER == 0;
   				if(readPortsModule && iniVar &&  strstr(buffer,fVarString)) {
   					//printf("[preProcessSmv] Trocou FVAR %s\n",buffer);
   					bufferAux = varString; // tratamento de FROZEN VAR
   					afterVarParPorts = 1;
   				}   				
   			}
   			else
   			{
   				//printf("[preProcessSmv] terminou VAR \n\n");
   				phase = readAssign ? ASSIGN : TRANS;
 				readAssign = 0;
   			}

   		}
   		
   		processPhase(stage,phase,Hcontrol,bufferAux,controlRename,readVarsPortsModule,portsSmv);

   		if(phase == CREATE_MODULE){
   			phase++;
   		}
   		

   		if(afterVarParPorts)
   		{
   			readVarsPortsModule = 1;
   		}

   		if(renamePortsParam && phase == PORTS){
   			free(bufferAux);
   			renamePortsParam = 0;
   		}
   	}
   	free(buffer);
   	//printf("terminou! \n");	
}