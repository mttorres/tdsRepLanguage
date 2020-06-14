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
  extern Node* root;
  extern void yyerror(const char *s);

int main(int argc, char* argv[]) {
  
  
  	FILE *fp; // .tds file
  	FILE *smvP; // .smv file;
  	fp = fopen(argv[1], "r");
  	smvP = fopen(argv[2], "r+");
  	//printf("%s \n",argv[2]);

  	if(!fp){
  		printf("CADE MEU CODIGO!?");
  		return -1;
  	} 
  	yyin = fp;
 	yyparse();

  	printf("--------------TREE--------------------\n");
  	printNode(root);
  	letgoNode(root);
  	fclose(fp);
	printf("HELLO!? \n");
  	HeaderSmv** headers = initHeadersStruct(5);
  	printf("HELLO! \n");
	preProcessSmv(smvP,headers);
  	//TESTE 
  	char *buffer;
  	size_t bufsize = 300;
  	buffer = (char *) malloc(bufsize * sizeof(char));
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
    	letGoHeadersStruct(headers,5);	
  	fclose(smvP);
  	free(buffer);
  	//smvP = fopen(argv[1], "r");
}

