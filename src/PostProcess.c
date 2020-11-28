#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../headers/PostProcess.h"

                                                // ex: 1 + 1
char* SmvConversions[] = {"%s", "{%s};",  "%s, ",  "%s %s %s ", "%s_redef%d%", "%s \n", "init(%s)", "next(%s)",
                          "%s:= %s;",  "\t%s:= %s;\n",  "case \n\t\t%s\n\t\tTRUE : %s; \n\tesac",
                          "%s : %s;" , "TRUE : NULL; \n", "%s = %s : %s; \n" };

int  ALOC_SIZE_LINE = 300;

char *createConditionCube(char *opBind1, char *opBind2, char *operation, char *evaluation)
{
        // por enquanto...
        char inter[ALOC_SIZE_LINE];

        char* cube = malloc(sizeof(strlen(opBind1)+strlen(opBind2)+strlen(operation)+1));
        if(opBind2){
            sprintf(inter,SmvConversions[3],opBind1,operation,opBind2);
        }
        else{
            sprintf(inter,SmvConversions[3],opBind1,operation,"");
        }
        if(evaluation){
            sprintf(cube,SmvConversions[11],inter,evaluation);
        }
        else{
            sprintf(cube,SmvConversions[1],inter,evaluation);
        }

        return cube;
}


void createType(char* varName ,HeaderSmv* header, STable* writeSmvTypeTable, const char* newValue, int type)
{

}


void createAssign(char *varName, HeaderSmv *header, STable *writeSmvTypeTable, const char *newValue, char *condition,
                  int redef, int typeExpr, int type, char *defaultEvalCond)
{
    // atualiza o typeSet (salvando ponto de interesse para mudança, e)
    //TODO (createType)

    char* exprResultString = malloc(sizeof(char)*ALOC_SIZE_LINE);
    char exprInterL[300];
    char exprInterR[300];
    char interRedef[300];

    if(!redef)
    {
        sprintf(exprInterL, SmvConversions[typeExpr], varName);
    }
    else{
        sprintf(interRedef,SmvConversions[4],varName,redef);
        sprintf(exprInterL, SmvConversions[typeExpr], interRedef);
    }

    // parâmetros a ser salvo na tabela auxiliar SMV
    int size;
    int pointIni;
    int pointEnd;


    if(condition)
    {
        if(typeExpr == 6){
            sprintf(exprInterR,SmvConversions[10],condition,defaultEvalCond);
        }
        else{
            sprintf(exprInterR,SmvConversions[10],condition,varName);
        }
        sprintf(exprResultString,SmvConversions[9],exprInterL,exprInterR);

        char* auxChPoint;
        auxChPoint = strstr(exprResultString,";\n");
        long dif = auxChPoint-exprResultString;
        size = strlen(exprResultString);
        pointIni = dif;
        pointEnd = pointIni;

        free(condition);
    }
    else{
        // atualiza o init/next dessa função
        // criar a expressão definitiva com sprintf?
        // pros : é facil
        // cons: não tenho controle sobre qual parte da string minha expressão começa ou termina...
        // NA VERDADE EU TENHO, posIni:  até o = (posso salvar em um array mapeador também)
        //  posEnd = posIni + tamanhoString(expressão)

        sprintf(exprInterR,SmvConversions[0],newValue);
        sprintf(exprResultString,SmvConversions[9],exprInterL,exprInterR);

        size = strlen(exprResultString);
        pointIni = (strlen(exprInterL) - 1) + 5; //  %s:= %s (+2 para pular ele mesmo e o : e depois = e espaço, e fora o \t no inicio)
        pointEnd = pointIni + strlen(exprInterR)-1;

    }
    // escreve atribuição no buffer
    // encapsular em método(!)
    header->assignBuffer[header->ASSIGN_POINTER] = exprResultString;
    int pos = header->ASSIGN_POINTER;
    header->ASSIGN_POINTER += 1;
    //atualiza tabela auxiliar para init/next(var)
    void* po[] = {&pos, &size,&pointIni,&pointEnd};
    addValue(exprInterL, po, WRITE_SMV_INFO, 4, 0, writeSmvTypeTable, 0);
}




void updateType(char* varName ,HeaderSmv* header, STable* writeSmvTypeTable, const char* newValue, int type,int minmax)
{
    // começando com numérico x..y;
    // criar enum mapeador ao decorrer...
    if(type == 0)
    {
        int pos;
        int size;
        int pointIni;
        int pointEnd;
        // recuperar da tabela
        TableEntry* entryPosType;
        entryPosType =  lookup(writeSmvTypeTable,varName);
        if(entryPosType){
            pos = *(int*) entryPosType->val->values[0];
            size = *(int*) entryPosType->val->values[1];
            pointIni = *(int*) entryPosType->val->values[2];
            pointEnd = *(int*) entryPosType->val->values[3];

            int newPointIni = 0;
            int newPointEnd = 0;
            int sizeNew = strlen(newValue);
            // min..max;
            if(minmax)
            {
                pointIni = pointEnd+3; // n..max;
                // nota! o size já está indexbased!
                pointEnd = header->varBuffer[pos][size] == '\n' ? size-2  : size-1; // max;\n (-1 do index based) - (-2 ou -1 dependendo do fim)
                size = header->varBuffer[pos][size] == '\n' ? size+1 : size;
            }
            updateSubStringInterval(newValue,header->varBuffer[pos],sizeNew,pointIni,pointEnd,size,&newPointIni,&newPointEnd);


            void* vpSize[] = {&size};
            updateValue(varName, vpSize, WRITE_SMV_INFO, 1, 1, -1, writeSmvTypeTable, 0);
            // atualizar o fim do intervalo não mudar a nossa variável pointEnd também! Só atualiza o tamanho
            if(!minmax)
            {
                void* vpInEnd[] = {&newPointEnd};
                updateValue(varName, vpInEnd, WRITE_SMV_INFO, 1, 3, -1, writeSmvTypeTable, 0);
            }
        }
        else{
            printf("[updateType] WARNING: type of %s not declared on headers \n",varName);
        }
    }
}

void updateAssign(char* varName ,HeaderSmv* header, STable* writeSmvTypeTable, const char* newValue, const char* condition, int type ,int typeExpr, int minmax)
{
    // checa o typeSet dessa variável para o atualizar!
    updateType(varName,header,writeSmvTypeTable,newValue,type,minmax);

    char *updated = NULL; // ponteiro para referenciar a string que vamos tratar
    char upVar[300]; // ponteiro para localizar o assign na tabela
    sprintf(upVar,SmvConversions[typeExpr],varName); // recupera string default init/next
    int sizeNew = strlen(newValue);

    // tratamento de init/next(varName):= case ... TRUE : x; esac; , geralmente TRUE: NULL ou outra condição parecida
    // é sempre o "delmitador final", vai ser um caso similar ao anterior porém entre ponto de interesse - condição default, já que condições não mudam!
    // o que muda é que vamos er que concatenar esse default ? (NÃO, é criado naturalmente)
    // ou seja devemos pegar o que está sendo adicionado a sizenew e jogar uma condição se (ela existir)
    if(condition)
    {
        printf("ainda...");
    }

    int pos;
    int size;
    int pointIni;
    int pointEnd;

    TableEntry* entryPosSizeAssing;
    entryPosSizeAssing =  lookup(writeSmvTypeTable,upVar);

    // recupera a posição no buffer, o tamanho da string, e o intervalo de interesse [i,f]
    pos = *(int*) entryPosSizeAssing->val->values[0];
    size = *(int*) entryPosSizeAssing->val->values[1];
    pointIni = *(int*) entryPosSizeAssing->val->values[2];
    pointEnd = *(int*) entryPosSizeAssing->val->values[3];

    int newPointInit = 0;
    int newPointEnd = 0;

    // verifica o tamanho possível após a mudança (o +1 é justamente porque se o intervalo é x-y = 0, o tamanho é 1,
    // ou seja torna como size based ao inves de index based)
    if((-1*((pointEnd-pointIni+1) - sizeNew) + size)  >= ALOC_SIZE_LINE)
    {
        char* newStrSize = realloc(header->assignBuffer[pos],ALOC_SIZE_LINE*2);
        ALOC_SIZE_LINE = ALOC_SIZE_LINE*2;
        if(newStrSize == NULL)
        {
            fprintf(stderr, "FAIL IN REALLOCATE HEADER SIZE FOR %s !",upVar);
            exit(-1);
        }
        header->assignBuffer[pos] = newStrSize;
    }
    updated = header->assignBuffer[pos];

    // é um simples next(x)/init(x):= yyyyyyy;  -> substituir y's por expressão nova em newValue antes do delmitador (;)
    // os delmitadores podem variar, ex: em caso de next(time) o delimitador é (:)
    updateSubStringInterval(newValue, updated, sizeNew, pointIni, pointEnd, size, &newPointInit, &newPointEnd);



    // atualizar range de interesse e tamanho da string na tabela!
    // fazer duas chamadas por enquanto
    size = -1*((newPointEnd-newPointInit+1) - sizeNew) + size;
    void* vpSize[] = {&size};  // evita ter que "reescrever" o vetor inteiro separando em varios vps
    // é como se a gente tivesse atualizando cada indice dos vetores menos o (0)
    updateValue(upVar, vpSize, WRITE_SMV_INFO, 1, 1, -1, writeSmvTypeTable, 0);
    void* vpIni[] = {&newPointInit};
    updateValue(upVar, vpIni, WRITE_SMV_INFO, 1, 2, -1, writeSmvTypeTable, 0);
    void* vpInEnd[] = {&newPointEnd};
    updateValue(upVar, vpInEnd, WRITE_SMV_INFO, 1, 3, -1, writeSmvTypeTable, 0);
}

void specNext(char*varName, HeaderSmv* header, STable* writeSmvTypeTable, char* newValue, char* condition,
              int redef, int type ,int typeExpr, int minmax)
{
    char* useVar = varName;
    char interRedef[ALOC_SIZE_LINE];
    if(redef){
        sprintf(interRedef,SmvConversions[4],varName,redef);
        useVar = interRedef;
    }
    if(lookup(writeSmvTypeTable,useVar)){
        updateAssign(useVar,header,writeSmvTypeTable,newValue,condition,type,typeExpr,minmax);
    }
    else{
        createAssign(varName, header, writeSmvTypeTable, newValue, condition, redef, typeExpr, type, NULL);
    }
}


void updateTime(HeaderSmv* main , STable * writeSmvTypeTable, const char* newValue, int type, int typeExpr, int minmax)
{
    updateAssign("time",main,writeSmvTypeTable,newValue,NULL,type,typeExpr,minmax);
}