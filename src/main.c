  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
  #include "../headers/STable.h"
  #include "../headers/constants.h"
  #include "../headers/Node.h"
  #include "../headers/Enum.h"	
  #include "../headers/HeaderSmv.h"
   	

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

	
	//pré processamento 
	HeaderSmv** headers = initHeadersStruct(5);
  	preProcessSmv(smvP,headers);
  	
  	size_t bufsize = 300;
  	char *buffer = (char *) malloc(bufsize * sizeof(char));
	
	int i;
	for(i = 0; i < 5; i++) {
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
    printHeader(headers[0]);
    printHeader(headers[1]);
    printHeader(headers[2]);
    printHeader(headers[3]);
    printHeader(headers[4]);	
    //letGoHeadersStruct(headers,5);	
  	fclose(smvP);
  	//free(buffer);
  	//letgoNode(root);
  	//smvP = fopen(argv[1], "r");
}

