  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
  #include "../headers/STable.h"
  #include "../headers/Node.h"
  #include "../headers/HeaderSmv.h"
  #include "../headers/PreProcess.h"
  #include "../headers/Interpreter.h"

  extern int yylex(void);
  extern int yyparse();
  extern FILE *yyin;
  FILE *astout;
  extern Node* root;
  extern void yyerror(const char *s);

int main(int argc, char* argv[]) {
  
  
	FILE *fp; // .tds file
	FILE *smvP; // .smv file;
	fp = fopen(argv[1], "r");
	astout = fopen("results/astOutput", "w");
	smvP = fopen(argv[2], "r+");
	//printf("%s \n",argv[1]);
	//printf("%s \n",argv[2]);

	if(!fp){
		printf("CADE MEU CODIGO!?");
		return -1;
	} 
	yyin = fp;
	yyparse();

	fprintf(astout,"--------------TREE--------------------\n");
	filePrintNode(root,astout);
	fclose(astout);
	fclose(fp);

	// tabelas e componentes globais
	HeaderController* controller = createController(5);  

	STable* global = createTable(GLOBAL,NULL,0,0);

	STable* mainVarsTypeSmv = createTable(SMV_V_MAIN,NULL,0,0);  	
	STable* portsTypeSmv = createTable(SMV_PORTS,NULL,0,0);
	STable* writeSmvTypeTable[] = {mainVarsTypeSmv,portsTypeSmv}; 
	//pré processamento 
	preProcessSmv(smvP,controller,writeSmvTypeTable);
	setUpMainSmvTable(controller,writeSmvTypeTable,global);
  
  	printf("--------------------------------- EVAL ---------------------------------------------\n");
  	printf("\n");
  	printf("\n");
  	
	//pos processamento
	eval(root,global,writeSmvTypeTable,controller);

	printf("\n");
	printf("------------------------------------------------------------------------------\n");

 	printf("\n");	
 	printf("\n");
 	printf("\n");
 	printf("\n");

 	printf("--------------------------------- TABLES ---------------------------------------------\n");

	printTable(global);
    printf("\n");
	printTable(writeSmvTypeTable[0]);
	printf("\n");
    printTable(writeSmvTypeTable[1]);
	printf("\n");

	//letGoHeadersStruct(headers,5);


	printf("\n");	
 	printf("\n");
 	printf("\n");
 	printf("\n");

 	printf("--------------------------------- HEADERS ---------------------------------------------\n");	


	printHeader(controller->headers[0]);
	//printHeader(controller->headers[1]);
	//printHeader(controller->headers[2]);
	//printHeader(controller->headers[3]);
	//printHeader(controller->headers[4]);

	fclose(smvP);
	
	letgoNode(root);

//	letGoHeaderControl(controller); // BUG NO FREE DA ESTRUTURA QUE CONTROLA OS HEADERS SMV (quebra no letgoHeader(hs[i]), possívelmente memory leak de string)

  	



//TESTE PORTS REFS (aparentemente tudo funcionando)

/*
  	printf("TESTE PORT REFS !!! \n\n\n");
  	char* string1 = "	((cs = q0 & ports.a[time] = NULL & ports.b[time] = NULL & ports.d[time] = NULL & ports.c[time] = 0 & FALSE) -> next(cs) = p0) &";
  	char* nova =clearOldPortsRefs(string1);
  	printf("ANTES:  %s \n\n\n",string1);
    	printf("DEPOIS:  %s \n\n\n",nova);	
    	free(nova);

  	char* string2 = "	((cs = q0 & ports.a[time] = NULL & ports.d[time] = NULL & ports.c[time] != NULL & ports.b[time] = ports.c[time] & FALSE) -> next(cs) = q0);";
  	nova =clearOldPortsRefs(string2);
  	printf("ANTES:  %s \n\n\n",string2);
    	printf("DEPOIS:  %s \n\n\n",nova);	
    	free(nova); 

  	char* string3 = "	((cs = q0p0) -> ((next(cs) != q0p1))) &";
  	nova =  clearOldPortsRefs(string3);
  	printf("ANTES:  %s \n\n\n",string3);
    	printf("DEPOIS:  %s \n\n\n",nova);	
    	free(nova);    
*/

}

