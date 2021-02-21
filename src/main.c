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
    HeaderController* controller = createController();
	STable* global = createTable(GLOBAL, NULL, 0, 0, -1);

	//pré processamento
	preProcessSmv(smvP,controller);
	setUpMainSmvTable(controller,global);
  
  	printf("--------------------------------- EVAL ---------------------------------------------\n");
  	printf("\n");
  	printf("\n");
  	
	//pos processamento
    eval(root, global, controller);

	printf("\n");
	printf("------------------------------------------------------------------------------\n");

 	printf("\n");	
 	printf("\n");
 	printf("\n");
 	printf("\n");

 	printf("--------------------------------- PROGRAM TABLES ---------------------------------------------\n");

	printTable(global);
    printf("\n");

    printf("--------------------------------- smv-INFO TABLES ---------------------------------------------\n");

    printTable(controller->mainInfo);
    printf("\n");
    printf("\n");
    printf("\n");
    printTable(controller->originalPorts);
    printf("\n");
 	printf("\n");
 	printf("\n");

    letgoTable(global);

 	printf("--------------------------------- HEADERS ---------------------------------------------\n");


    printAllHeaders(controller);

 	if(controller->declaredPortsNumber != controller->expectedPorts){
 	    printf("\n");
 	    if(!controller->declaredPortsNumber){
            fprintf(stderr, "[WARNING] THE MODEL GENERATION WAS SUCCESSFUL, HOWEVER, NO VALID TDS DEFINITION WAS FOUND \n IT IS RECOMMENDED THAT YOU REVIEW YOUR .tds FILE \n");
 	    }
 	    else{
 	        if(controller->validPorts != controller->expectedPorts){
 	            if(controller->validPorts){
                    fprintf(stderr, "[WARNING] THE MODEL GENERATION WAS SUCCESSFUL, HOWEVER, OF THE %d PORTS DECLARED ONLY %d of %d PORTS WERE COMPLIANT TO THE ORIGINAL MODEL. \n IT IS RECOMMENDED THAT YOU REVIEW YOUR .tds FILE \n ",
                            controller->declaredPortsNumber, controller->validPorts,controller->expectedPorts);
                }
 	            else{
                    fprintf(stderr, "[WARNING] THE MODEL GENERATION WAS SUCCESSFUL, HOWEVER, OF THE %d PORTS DECLARED NONE WERE COMPLIANT TO THE ORIGINAL MODEL. \n IT IS RECOMMENDED THAT YOU REVIEW YOUR .tds FILE \n ",
                            controller->declaredPortsNumber);
 	            }
 	        }
 	    }
 	}
 	writeResultantHeaders(controller,"results/newSmvfile.smv");
	fclose(smvP);
	letgoNode(root);



//  char* temp = controller->PORTS_RELATED[0]->varBuffer[1];
//  char* result = addParams(temp,"BATATA","{","}");
//  printf("%s\n",result);
//  char* testAgain = addParams(result,"JUDGEMENT AHS COME TO YOU","{","}");
//  printf("%s\n",testAgain);

	letGoHeaderControl(controller);



}

