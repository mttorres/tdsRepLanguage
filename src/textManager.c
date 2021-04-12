#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../headers/textManager.h"

int multiFactor = 1;

char * customCat(char* dest, char* src, char toIgnore, int ignoreMode) {

    //printf("\t[customCat] source:  %s \n\n\n",src);

    while(*dest){
        //printf("\t[customCat] %s \n",dest);
        //*dest = '\0';
        dest++; // vai para a "proxima posição disponivel" (no caso de strings usadas anteriormente por essa função só roda "uma vez")
    }
    while(*src){
        //printf("\t[customCat] source atual: %s \n",src);
        if(!ignoreMode){
            *dest = *src;
            //printf("\t[customCat] nova string atual:  %s \n",dest);
            dest++;
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
    return dest;

}

void removeAfter(char* dest, char* src, char stop){
    customCat(dest,src,stop,0);
}

char* formatNumeric(int ctime){
    char* directiveValueBind = malloc(sizeof(char)*ALOC_SIZE_LINE/10);
    sprintf(directiveValueBind, "%d", ctime);
    return directiveValueBind;
}

void clearOldPortsRefs(char* oldConstraint, char* toCopyResult) {

    int removedCharacters = 0;
    int removing = 0;
    char* resultProperty = ".value";
    char* refProp = resultProperty;

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
            while(*refProp){
                *startResultRef = *refProp;
                refProp++;
                startResultRef++;
            }
        }

        if(*oldConstraintRef == ']' && !*refProp){
            removing = 0;
            refProp = resultProperty;
        }
        oldConstraintRef++;
        //printf("[clearOldPortsRefs] copiando e filtrando... %c \n",*oldConstraintRef);
    }
}

char *addParams(char *original, char *param, char *delim1, char *delim2, int useRealloc) {

    char *newString;
    char *auxNewAdd;
    int breakline=0; // verifica se o final da string é \n
    int statementEnd = 0; // verifica se o final da string é ; (não é nome de módulo)

    int tamOriginal = strlen(original);
    if(original[tamOriginal-1] == '\n'){
        breakline = 1;
    }

    if(original[tamOriginal-2]== ';'){
        statementEnd = 1;
    }
    // qualquer string passada (original) são das seguintes formas:  xxxxxxxxx  ,  xxxxxxxxx;  , xxxxxxxxx(...)\n  , xxxxxxxxx(...);\n
    // caso possua o delmitador de fechamento ) ou qualquer outro...
    if(original[tamOriginal-3] == delim2[0]  || original[tamOriginal-2] == delim2[0]){
        int newSize = strlen(original) + strlen(param) + 2 + 1; // original + param + ',' + ' ' +  '\0'
        newString = useRealloc ? (char*) realloc(original,newSize) : (char*) calloc(newSize, sizeof(char));
        if(!newString){
            fprintf(stderr,"[addParams] Fail in realloc\n");
            exit(-1);
        }
        auxNewAdd = newString;
        if(!useRealloc){
            auxNewAdd = customCat(auxNewAdd,original,delim2[0],0);
        }
        else{
            auxNewAdd[tamOriginal-3] = '\0';
        }
        //printf("caso 1... %s \n",newString);
        auxNewAdd = customCat(auxNewAdd,", ",0,0);
        //printf("caso 1... %s \n",newString);
        auxNewAdd = customCat(auxNewAdd,param,0,0);
        //printf("caso 1... %s \n",newString);
        auxNewAdd = customCat(auxNewAdd,delim2,0,0);
        //printf("caso 1... %s \n",newString);
    }
    else {
        int newSize = strlen(original) + 1 + strlen(param) + 1 + 1;
        newString = useRealloc ? (char*) realloc(original,newSize) : (char*) calloc(newSize, sizeof(char));
        if(!newString){
            fprintf(stderr,"[addParams] Fail in realloc\n");
            exit(-1);
        }
        auxNewAdd = newString;
        if(!useRealloc){
            if(statementEnd){
                auxNewAdd = customCat(auxNewAdd,original,';',0);
            }
            else{
                auxNewAdd = customCat(auxNewAdd,original,'\n',0);
            }
        }
        else{
            auxNewAdd[tamOriginal-1] = '\0';
        }
        auxNewAdd = customCat(auxNewAdd,delim1,0,0);
        auxNewAdd = customCat(auxNewAdd,param,0,0);
        auxNewAdd = customCat(auxNewAdd,delim2,0,0);
    }
    if(statementEnd){
        auxNewAdd = customCat(auxNewAdd,";",0,0);
    }
    if(breakline){
        auxNewAdd = customCat(auxNewAdd,"\n",0,0);
    }
//	newString = newString - ((strlen(original)+strlen(param)+2)-breakline);

    return newString;
}

char* updateSubStringInterval(const char *newValue, char *updated, int sizeNew, int pointIni, int pointEnd, int size,
                             int *newPointInit, int *newPointEnd, int variantPointIni)
{

    if(size + sizeNew >= ALOC_SIZE_LINE*multiFactor)
    {
        multiFactor = multiFactor*2;
        char* newStrSize = realloc(updated,ALOC_SIZE_LINE*multiFactor);
        if(newStrSize == NULL)
        {
            fprintf(stderr, "[updateSubStringInterval] FAIL IN REALLOCATE STRING SIZE FOR %s !",updated);
            exit(-1);
        }
        updated = newStrSize;
    }

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
    (*newPointInit) = variantPointIni? (*newPointEnd) : pointIni;
    return updated;
}


char* overwriteParam(char* moduleName, char* param){
    char* refOldPt = moduleName;
    char moduleNameNoParam[strlen(moduleName)];
    char* refModuleNameNoParam = moduleNameNoParam;
    //refOldPt = strstr(moduleName,"(");
    while(*refOldPt != '('){
        *refModuleNameNoParam = *refOldPt;
        refOldPt++;
        refModuleNameNoParam++;
    }
    if(moduleName[strlen(moduleName)-2] == ';'){
        *refModuleNameNoParam = ';';
        refModuleNameNoParam++;
    }
    *refModuleNameNoParam = '\n';
    refModuleNameNoParam++;
    *refModuleNameNoParam = '\0';

    //memset(refOldPt, '\n', 1);
    //refOldPt++;
    //memset(refOldPt, '\0', strlen(refOldPt));
    return addParams(moduleNameNoParam, param, "(", ")", 0);
}

