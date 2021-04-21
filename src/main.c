  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
  #include "../headers/STable.h"
  #include "../headers/Node.h"
  #include "../headers/PreProcess.h"
  #include "../headers/Interpreter.h"


  extern int yylex(void);
  extern int yyparse();
  extern FILE *yyin;
  FILE *astout;
  extern Node* root;
  extern void yyerror(const char *s);

int main(int argc, char* argv[]) {

    int interactive = 0;
    int interactiveParamPassed = 0;
	FILE *fp = NULL; // .tds file
	int sizeFpName;
	FILE *smvP = NULL; // .smv file
	int sizeSmvName;
	if(argv[1]){
	    int size = strlen(argv[1]);
	    if(size == 4){
	        interactive = 1;
	        interactiveParamPassed = 1;
	    }
	    else{
            fp = fopen(argv[1], "r");
            sizeFpName = size;
            if(argv[1][sizeFpName-1] != 's' || argv[1][sizeFpName-2] != 'd' || argv[1][sizeFpName-3] != 't'){
                fprintf(stderr,"Expected .tds input file \n");
                exit(-1);
            }
	    }
	}
	else{
	    // não tem input tds nem -int
	    fprintf(stderr,"No .tds input file found!\n");
	    exit(-1);
	}

	if(argv[2]){
	    if(interactiveParamPassed){
            fp = fopen(argv[2], "r");
            sizeFpName = strlen(argv[2]);
            if(argv[2][sizeFpName-1] != 's' || argv[2][sizeFpName-2] != 'd' || argv[2][sizeFpName-3] != 't'){
                fprintf(stderr,"Expected .tds input file \n");
                exit(-1);
            }
	    }
	    else{
            smvP = fopen(argv[2], "r+");
            sizeSmvName = strlen(argv[2]);
            if(argv[2][sizeSmvName-1] != 'v' || argv[2][sizeSmvName-2] != 'm' || argv[2][sizeSmvName-3] != 's'){
                fprintf(stderr,"Expected .smv input file \n");
                exit(-1);
            }
	    }
	}
	else{
	    if(interactiveParamPassed){
            fprintf(stderr,"No .tds input file found!\n");
            exit(-1);
	    }
	}
	if(interactiveParamPassed){
	    if(argv[3]){
            smvP = fopen(argv[3], "r+");
            sizeSmvName = strlen(argv[3]);
            if(argv[3][sizeSmvName-1] != 'v' || argv[3][sizeSmvName-2] != 'm' || argv[3][sizeSmvName-3] != 's'){
                fprintf(stderr,"Expected .smv input file \n");
                exit(-1);
            }
	    }
	}


    if(!fp){
        fprintf(stderr,".tds input file path not found!\n");
        return -1;
    }
    astout = fopen("results/astOutput", "w");
	yyin = fp;
	yyparse();
	fprintf(astout,"--------------TREE--------------------\n");
	filePrintNode(root,astout);
	fclose(astout);
	fclose(fp);

	// tabelas e componentes globais
    EnvController* controller = createController(interactive);
	STable* global = createTable(GLOBAL, NULL, 0, 0, -1);

	//pré processamento
	printf("\n\n");
	if(smvP){
	    printf("Start PreProcess\n\n");
        preProcessSmv(smvP,controller);
	}
	else{
        printf("No .smv file detected. Using default model\n\n");
	    setDefaultSmv(controller);
	}
	setUpMainSmvTable(controller,global);

  	//printf("--------------------------------- EVAL ---------------------------------------------\n");
  	printf("\n");
  	printf("\n");

	//interpretação e pos processamento
    startInterpreter(root,global,controller);

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

 	//printf("--------------------------------- HEADERS ---------------------------------------------\n");
    //printAllHeaders(controller);
    validateAfterInterPost(controller);
 	writeResultantHeaders(controller,"results/newSmvfile.smv");
 	if(smvP) {
 	    fclose(smvP);
    }
	letGoHeaderControl(controller);
}