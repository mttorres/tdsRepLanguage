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
    //printf("EITA");
    FILE *smvP; // .smv file;
    //printf("EITA");
    smvP = fopen("sample/novo/merger-fifo/nuxmv.smv","r+");
    long * partesArquivoSmv = (long*) malloc(sizeof(long)*3); // MAIN, PORTS MODULE, AUTOMATO 
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
