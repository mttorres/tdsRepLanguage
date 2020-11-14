#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../headers/PostProcess.h"

                                                // ex: 1 + 1
char* SmvConversions[] = {"%s", "{%s};",  "%s, ",  "%s %s %s ", "redef%d%s", "%s \n", "init(%s)", "next(%s)",
                          "%s:= %s; ",  "%s:= %s; \n",  "case \n\t\t%s \n\tesac; \n",
                          "init(%s) = %s : %s; \n ", "next(%s) = %s : %s; \n", "TRUE : NULL; \n", "%s = %s : %s; \n" };

int  ALOC_SIZE_LINE = 300;

void updateType(char* varName ,HeaderSmv* header, STable* writeSmvTypeTable, const char* newValue, int type,int minmax)
{
    // começando com numérico x..y;
    // criar enum mapeador ao decorrer...
    if(type == 1)
    {
        if(!minmax)
        {
                   printf("soon...");
        }
        else{
            printf("ainda...");
        }
    }
}


void createType(char* varName ,HeaderSmv* header, STable* writeSmvTypeTable, const char* newValue, int type)
{

}


void updateAssign(char* varName ,HeaderSmv* header, STable* writeSmvTypeTable, const char* newValue, const char* condition, int type ,int typeExpr, int minmax)
{
    // checa o typeSet dessa variável para o atualizar!
    updateType(varName,header,writeSmvTypeTable,newValue,type,minmax);

    char *updated = NULL; // ponteiro para referenciar a string que vamos tratar
    char *upVar = NULL; // ponteiro para localizar o assign na tabela
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
    updateSubStringInterval(newValue, updated, sizeNew, pointIni, pointEnd, &size, &newPointInit, &newPointEnd);



    // atualizar range de interesse e tamanho da string na tabela!
    // fazer duas chamadas por enquanto
    size = -1*((newPointEnd-newPointInit+1) - sizeNew) + size;
    void* vpSize[] = {&size};  // evita ter que "reescrever" o vetor inteiro separando em varios vps
    // é como se a gente tivesse atualizando cada indice dos vetores menos o (0)
    updateValue(upVar,vpSize,WRITE_SMV_INFO,1,1,-1,writeSmvTypeTable);
    void* vpIni[] = {&newPointInit};
    updateValue(upVar,vpIni,WRITE_SMV_INFO,1,2,-1,writeSmvTypeTable);
    void* vpInEnd[] = {&newPointEnd};
    updateValue(upVar,vpInEnd,WRITE_SMV_INFO,1,3,-1,writeSmvTypeTable);
}


/*
void createAssign(char* varName ,HeaderSmv* header, STable* writeSmvTypeTable, const char* newValue, const char* condition, int redef, int typeExpr ,int type)
{
    char* exprResultString = malloc(sizeof(char)*ALOC_SIZE_LINE);
    char exprInterL[300];
    char exprInterR[300];
    char interRedef[300];

    if(!redef)
    {
        sprintf(exprInterL, SmvConversions[typeExpr], varName);
    }
    else{
        sprintf(interRedef,SmvConversions[5],redef,varName);
        sprintf(exprInterL, SmvConversions[typeExpr], interRedef);
    }

    // atualiza o typeSet (salvando ponto de interesse para mudança, e)
    //TODO

    int size;
    int pointIni;
    int pointEnd;


    if(condition)
    {
        printf("ainda...");
    }
    else{

        // atualiza o init/next dessa função
        // criar a expressão definitiva com sprintf?
        // pros : é facil
        // cons: não tenho controle sobre qual parte da string minha expressão começa ou termina...
        // NA VERDADE EU TENHO, posIni:  até o = (posso salvar em um array mapeador também)
        //  posEnd = posIni + tamanhoString(expressão)

        sprintf(exprInterR,"%s",newValue);
        sprintf(exprResultString,SmvConversions[8],exprInterL,exprInterR);

        size = strlen(exprResultString)-1;
        pointIni = (strlen(exprInterL) - 1) + 4; //  %s:= %s (+2 para pular ele mesmo e o : e depois = e espaço)
        pointEnd = pointIni + strlen(exprInterR)-1;

    }


    // escreve atribuição no buffer
    // encapsular em método(!)
    header->assignBuffer[header->ASSIGN_POINTER] = exprResultString;
    header->ASSIGN_POINTER += 1;
    int pos = header->ASSIGN_POINTER;
    // caso de next
    if(typeExpr)
    {
        // atualiza tabela de simbolos dessa operação
        void* po[] = {&pos, &size,&pointIni,&pointEnd};
        addValue(exprInterL, po, WRITE_SMV_INFO, 4, 0, writeSmvTypeTable);
    }
}
*/


void updateTime(HeaderSmv* main , STable * writeSmvTypeTable, const char* newValue, int type)
{
    int pos;
    int size;
    TableEntry* entryPosSizeInterval;
    TableEntry* entryPosSizeAssing;

    char* originalInterval;
    char* originalAssign;
    char delim;

    entryPosSizeInterval =  lookup(writeSmvTypeTable,"time");
    pos = *(int*) entryPosSizeInterval->val->values[0];
    size = *(int*) entryPosSizeInterval->val->values[1];
    originalInterval = main->varBuffer[pos];

    // init
    if(type)
    {
        delim = ' ';
        // mudança da declaração (inicio)
        //loop de 3 iterações no maximo (mas é melhor do que deixar hardcoded)
        while(*originalInterval != delim)
        {
            originalInterval++;
        }
        originalInterval++;
        *originalInterval = newValue[0];

        entryPosSizeAssing =  lookup(writeSmvTypeTable,"init(time)");
        // mudança init
        pos = *(int*) entryPosSizeAssing->val->values[0];
        size = *(int*) entryPosSizeAssing->val->values[1];
        main->assignBuffer[pos][size-2] = newValue[0];

    }
    // final
    else{
        delim = '<';
        // mudança da declaração (final)
        size = *(int*) entryPosSizeInterval->val->values[1];
        originalInterval[size-2] = newValue[0];

        entryPosSizeAssing =  lookup(writeSmvTypeTable,"next(time)");
        //mudança next
        pos = *(int*) entryPosSizeAssing->val->values[0];

        originalAssign = main->assignBuffer[pos];
        while(*originalAssign != delim)
        {
            originalAssign++;
        }
        originalAssign++;
        originalAssign++;
        *originalAssign = newValue[0];
    }
}