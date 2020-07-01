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

