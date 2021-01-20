#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../headers/textManager.h"

// nota: ignore mode = 0 só o caracter especifico e tudo DEPOIS
//  =  1 tudo ANTES DO CARACTER e o caracter especifico
char * customCat(char* dest, char* src, char toIgnore, int ignoreMode) {

    int ignoredExtra = 0;
    //printf("\t[customCat] source:  %s \n\n\n",src);
    
	while(*dest){
		//printf("\t[customCat] %s \n",dest);				
		dest++; // vai para a "proxima posição disponivel" (no caso de strings usadas anteriormente por essa função só roda "uma vez")
	}
	while(*src){
		
		//printf("\t[customCat] source atual: %s \n",src);
		if(!ignoreMode){
		    *dest = *src;
		    //printf("\t[customCat] nova string atual:  %s \n",dest);
		    dest++;
		}
		else {
		   ignoredExtra++; 
		}

		src++;
		if(*src == toIgnore){
			//printf("\t[customCat] delimiter detected\n");			
			break;
		}
	}
	
	if(!ignoreMode){
	  *dest = '\0';
	  --dest;   
	  //printf("\t[customCat - ignoreMode = 0] ponteiro fim : %s \n",dest);	  
	}
	else {
	        *dest = ignoredExtra;
	        //printf("\t[customCat - ignoreMode = 1] caracteres ignorados : %d \n",ignoredExtra);
	       	//printf("\t[customCat - ignoreMode = 1] equivalencia ascii: %c \n",ignoredExtra);
	        // salva quantos foram ignorados!    
	}
	//printf("\t[customCat] ponteiro 'final' string: %s \n\n\n\n",dest);		
	return dest;

}

void clearOldPortsRefs(char* oldConstraint, char* toCopyResult) {
    
    int removedCharacters = 0;
    int removing = 0;

    char* oldConstraintRef = oldConstraint;
    char* startResultRef = toCopyResult;
    while(*oldConstraintRef){
        

        if(*oldConstraintRef == '['){
            removing = 1;
        }

        if(!removing){
            *startResultRef = *oldConstraintRef; 
            startResultRef++;
        }

        if(removing){
            removedCharacters++;
        }

        if(*oldConstraintRef == ']'){
            removing = 0;
        }

        oldConstraintRef++;
        //printf("[clearOldPortsRefs] copiando e filtrando... %c \n",*oldConstraintRef);
    }
}

char *addParams(char *original, char *param, char* delim1, char* delim2) {
 
	char *newString;
	int breakline=0;
	int statementEnd = 0;
	if(original[strlen(original)-1] == '\n'){
	  breakline = 1;
	}
    //printf("aaaa: %c \n",original[strlen(original)-2]);
	if(original[strlen(original)-2]== ';'){
        statementEnd = 1;
    }

	if(statementEnd || original[strlen(original)-1] == delim2[0] || original[strlen(original)-2] == delim2[0]){
	    newString = (char*) malloc(sizeof(char)*(strlen(original)+2+strlen(param)+1)); // original + param + ',' + ' ' +  '\0'
		newString = customCat(newString,original,delim2[0],0);
		//printf("caso 1... %s \n",newString);
		newString = customCat(newString,", ",0,0);
        //printf("caso 1... %s \n",newString);
		newString = customCat(newString,param,0,0);
        //printf("caso 1... %s \n",newString);
		newString = customCat(newString,delim2,0,0);
        //printf("caso 1... %s \n",newString);
	}
	else {
		//printf("%ld %ld \n",strlen(original),strlen(param));	   	
		newString = (char*) malloc(sizeof(char)*(strlen(original)+1+strlen(param)+1+1)); // ( , ) e  \0	   	
		newString = customCat(newString,original,'\n',0);
		newString = customCat(newString,delim1,0,0);
		newString = customCat(newString,param,0,0);
		newString = customCat(newString,delim2,0,0);
	}
    if(statementEnd){
        newString = customCat(newString,";",0,0);
    }

	if(breakline){
	  newString = customCat(newString,"\n",0,0);
	}
	newString = newString - ((strlen(original)+strlen(param)+2)-1);	

	return newString;
}

void updateSubStringInterval(const char *newValue,  char *updated, int sizeNew, int pointIni, int pointEnd, int size, int *newPointInit, int *newPointEnd)
{
    char aux[size - (pointEnd+1)]; // pointEnd+1 (é o indice(tamanho) sem ser 0-index) (+1 é para estarmos fora da zona da sobrescrita)
    int i;
    // deve-se copiar os caracteres que vem após a zona de sobrescrita
    //deve-se liberar sizeNew espaços empurrando os caras que vão ser salvos em aux
    for (i = pointEnd+1; i < size; i++) {
        //printf("copiando... %c \n",updated[i]);
        aux[i-(pointEnd+1)] = updated[i];
    }
    // atualiza o tamanho
    int oldSize = size;
    size = -1*((pointEnd-pointIni+1) - sizeNew) + size;

    // a zona de "sobrescrita" não aumentou
    if(sizeNew <= (pointEnd - pointIni)+1)
    {
        // deve-se "destruir" o resto da string partindo de pointIni
        updated[pointIni] = '\0';
        updated[oldSize-1] = '\0';
    }
    // após isso, tanto para o caso de ter aumentado ou não deve-se recuperar a substring salva no
    // buffer auxiliar (delimitador e tudo o que vem depois), e escrever logo após escrever a nova string.
    for(i = pointIni; i < size; i++)
    {
        // i em indice medição sizeNew como tamanho (tirar -1)
        if(i >= pointIni+sizeNew)
        {
            // ponto de sobrescrita do delimitador
            updated[i] = aux[(i-(pointIni))-(sizeNew)];
        }
            // escrita do newValue
        else{
            updated[i] = newValue[i-(pointIni)];
            // devemos salvar o novo intervalo de interesse
            if(i+1 == pointIni+sizeNew)
            {
                //printf("salvando novo indice fim... \n");
                (*newPointEnd) = i;
            }
        }
    }
    updated[size] = '\0';
    (*newPointInit) = pointIni; // invariante (manter por enquanto)

}

