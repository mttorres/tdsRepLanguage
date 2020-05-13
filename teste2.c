#include <stdio.h>
#include <string.h>
#include <stdlib.h>



void preProcessSmv(FILE* smvP, long* ds);

void postProcessSmv(FILE* smvP, int* ds);

void preProcessSmv(FILE* smvP, long* ds) {
	
	char *buffer;
    	size_t bufsize = 300;
	ds[0] = 0L;
	ds[1] = 0L;
	ds[2] = 0L;	
	long prevFcursor = 0L;

	char* portsModuleString = "MODULE portsModule";
	char* automataString = "MODULE finalAutomata(time)";

	int limpaPortsModule = 0;	
	
	buffer = (char *) malloc(bufsize * sizeof(char));
   	while ((fgets(buffer,bufsize,smvP))) {
		if(strstr(buffer,automataString)){
			printf("%s \n",buffer);
			ds[1] = ftell(smvP);		
		} 		
		if(strstr(buffer,portsModuleString)){
			printf("%s \n",buffer);			
			ds[2] = ftell(smvP);
					
		}
		
	}
	free(buffer);	
   

}


int main()
{
    //TESTE PARMETROS MODULOS

    printf("-----------EITA----------------------\n");
    char* automataString = "MODULE finalAutomata(time)";
    char *lastParStr = (char*)malloc(sizeof(char)*(strlen(automataString)+1));
    strncpy(lastParStr, automataString,strlen(automataString));
    printf("lets head back... %s \n",lastParStr);
    printf("-----------EITA----------------------\n");
    free(lastParStr);
    printf("\n");

//TESTE PARMETROS MODULOS

    //printf("EITA");
    FILE *smvP; // .smv file;
    //printf("EITA");
    smvP = fopen("sample/novo/merger-fifo/nuxmv.smv","r+");
    long * partesArquivoSmv = (long*) malloc(sizeof(long)*3); // MAIN, AUTOMATO, PORTS MODULE
    //long * partesArquivoSmv = (long*) malloc(sizeof(long)*2); // VAR, assign

    // "outra struct"  ---> localização dos principais "cabeçalhos" (3 exatamente)  e um offset que deve ser verificado...

    // onde salvar? IDEIA: MAIN (ESCOPO-MAIN), AUTOMATO (outra struct), PORTS MODULE (outra struct tmb "header") 
    //				 --> cada variavel : entrada e "pos" no arquivo			--> cada tds: entrada e "pos" no arquivo
    // fazer em "ordem" ou atualizar as entradas depois? (mesmo em ordem pode necessitar atualizar depois?)   
    preProcessSmv(smvP,partesArquivoSmv); 

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
    fclose(smvP);
    free(buffer);	
    return 0;
}
