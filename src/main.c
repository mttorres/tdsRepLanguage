  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
  #include "../headers/STable.h"
  #include "../headers/Node.h"
  #include "../headers/HeaderSmv.h"
  #include "../headers/PreProcess.h"
  #include "../headers/Interpreter.h"
  #include "../headers/PostProcess.h"

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
	eval(root,global,writeSmvTypeTable,controller,0);

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
	printf("\n");	
 	printf("\n");
 	printf("\n");
 	printf("\n");

    letgoTable(global);

 	printf("--------------------------------- HEADERS ---------------------------------------------\n");	


	//printHeader(controller->headers[0]);
	//printHeader(controller->headers[1]);
	//printHeader(controller->headers[2]);
	//printHeader(controller->headers[3]);
//	printHeader(controller->headers[4]);

 	if(!controller->declaredPorts){
 		fprintf(stderr, "[WARNING] THE MODEL GENERATION WAS SUCCESSFUL, HOWEVER NO TDS DEFINITION WAS FOUND \n IT IS RECOMENDED THAT YOU REVIEW YOUR .tds FILE \n");
 	}
 	writeResultantHeaders(controller,"results/newSmvfile.smv");
	fclose(smvP);
	letgoNode(root);

	


//    printf("teste TYPE SET !!! \n\n\n");
//    char* temp = controller->headers[4]->varBuffer[1];
//    char* result = addParams(temp,"BATATA","{","}");
//	printf("%s\n",result);
//	char* testAgain = addParams(temp,"JUDGEMENT AHS COME TO YOU","{","}");

//	printf("%s\n",testAgain);

	letGoHeaderControl(controller);



}

