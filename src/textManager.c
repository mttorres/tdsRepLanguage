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

char* clearOldPortsRefs(char* oldConstraint) {
    
    char* nova = (char*) malloc(sizeof(oldConstraint));
    char* anterior = NULL;
    char* preProx = NULL;
    long offsetoriginal = strlen(oldConstraint);
    char* ponteiroOldOriginal = oldConstraint;
    printf("[clearOldPortsRefs]  NANI!? : %s \n\n\n",nova);
    memset(nova,0,strlen(nova));
    printf("[clearOldPortsRefs]  NARUHOTO!? : %s \n\n\n",nova);
    long offsetfinal = offsetoriginal;
    long offsetEntreDeli = 0L;
    int avaliacaoFinalString = 0;
    
    
    while(*oldConstraint){
        
        if(!preProx){
        	preProx = customCat(nova,oldConstraint,'[',0); // primeira vez que roda (não temos o ponteiro para o final da string!)
        }
        else{
        	preProx = customCat(preProx,oldConstraint,'[',0);
        }

        avaliacaoFinalString = ponteiroOldOriginal[offsetoriginal-1]==*preProx;

        //printf("[????] %c \n\n\n\n\n\n",ponteiroOldOriginal[offsetoriginal-1]);
        //printf("[????2] %d \n\n\n\n\n\n",avaliacaoFinalString);

        if(!avaliacaoFinalString) {
        	if(anterior != NULL){
            	oldConstraint += ((preProx - anterior)+2); // diferença do anterior para o ponteiro final da string (N chars) e delmitador
        	}
        	else {
            	oldConstraint += (strlen(nova)+1);
        	}
        }
        else{
		if(anterior != NULL) {        	
		oldConstraint += ((preProx - anterior)+1); // final da string (o +1 é pq preProx e anterior não consideram o caracter terminador!) 
		}
		else {
			// nao terminou e é "primeira vez que roda"
			//printf("[clearOldPortsRefs] String rejeitada \n\n");
			oldConstraint += offsetoriginal;
		}
        }

        // terminou antes da segunda rodada (evita percorrer desnecessáriamente regiões de memória)
        if(!*oldConstraint) {
            //printf("[clearOldPortsRefs]  xablau \n");
            break;
        }
        
        //printf("[clearOldPortsRefs]  RODADA I \n\n");
        //printf("[clearOldPortsRefs]  o(i): %ld \n",offsetfinal);
        //printf("[clearOldPortsRefs]  TESTE chegada(devolvido pelo customCat): %s \n",preProx);
        //printf("[clearOldPortsRefs]  TESTE (string nova): %s \n",nova);
        //printf("[clearOldPortsRefs]  TESTE (parte atual da antiga) : %s \n\n",oldConstraint);
        

        
        preProx = customCat(preProx,oldConstraint,']',1);
        
        
        oldConstraint += (preProx[0]+1); // ignorou N caracteres (variavel) e mais o delimitador
        if(offsetEntreDeli == 0){
            offsetEntreDeli = preProx[0]; // salvou para as próximas ocorrencias 
        }
        
        //printf("skiped: %d \n\n",preProx[0]+1);
        //printf("skiped: %c \n\n",preProx[0]);
        
        if(preProx[0] == offsetEntreDeli){
            offsetfinal -= (preProx[0]+2); // delimitadores mais N caracteres se for o "padrão de variavel utilizado" (pode ser necessario remover essa linha no futuro INTEGRACAO)
        }
       
        //printf("o(ii): %lu \n",offsetfinal);

        preProx[0] = '\0'; // elimina o offset para a proxima rodada
        anterior = preProx; // salva ponteiro de preProx
        
        //printf("[clearOldPortsRefs]  RODADA II \n\n");
        //printf("[clearOldPortsRefs]  o(ii): %ld \n",offsetfinal);
        //printf("[clearOldPortsRefs]  TESTE chegada(devolvido pelo customCat): %s \n",preProx); // vai ser o final da string de antes!
        //printf("[clearOldPortsRefs]  TESTE (string nova): %s \n",nova);
        //printf("[clearOldPortsRefs]  TESTE (parte atual da antiga) : %s \n\n",oldConstraint);       
    }
    nova[offsetfinal] = '\0';
    //printf("o: %ld \n",offsetfinal);
    printf("[clearOldPortsRefs]  RESULTADO : %s \n\n\n",nova);
    return nova;
    
    
    
}

char *addParams(char *original, char *param, char* delim1, char* delim2) {
 
	char *newString;
	int breakline=0;
	int statementEnd = 0;
	if(original[strlen(original)-1] == '\n'){
	  breakline = 1;
	}
    printf("aaaa: %c \n",original[strlen(original)-2]);
	if(original[strlen(original)-2]== ';'){
        statementEnd = 1;
    }

	if(statementEnd || original[strlen(original)-1] == delim2[0] || original[strlen(original)-2] == delim2[0]){
	    newString = (char*) malloc(sizeof(char)*(strlen(original)+2+strlen(param)+1)); // original + param + ',' + ' ' +  '\0'
		newString = customCat(newString,original,delim2[0],0);
		printf("caso 1... %s \n",newString);
		newString = customCat(newString,", ",0,0);
        printf("caso 1... %s \n",newString);
		newString = customCat(newString,param,0,0);
        printf("caso 1... %s \n",newString);
		newString = customCat(newString,delim2,0,0);
        printf("caso 1... %s \n",newString);
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

