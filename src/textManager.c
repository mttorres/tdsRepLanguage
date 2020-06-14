#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../headers/textManager.h"




// nota: ignore mode = 0 só o caracter especifico e tudo DEPOIS
//  =  1 tudo ANTES DO CARACTER e o caracter especifico
char * customCat(char* dest, char* src, char toIgnore, int ignoreMode) {

    int ignoredExtra = 0;
    
	while(*dest){
		//printf("%s \n",dest);				
		dest++; // vai para a "proxima posição disponivel" (no caso de strings usadas anteriormente por essa função só roda "uma vez")
	}
	while(*src){
		
		if(!ignoreMode){
		    *dest = *src;
		    dest++;
		}
		else {
		   ignoredExtra++; 
		}
		//printf("d : %s \n",dest);
		//printf("s : %s \n",src);				
		src++;
		if(*src == toIgnore){
			//printf("break detected\n");			
			break;
		}
	}
	
	if(!ignoreMode){
	  --dest;   
	}
	else {
	        *dest = ignoredExtra;
	        // salva quantos foram ignorados!    
	}
	return dest;

}

