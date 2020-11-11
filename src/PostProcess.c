#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../headers/PostProcess.h"

int  ALOC_SIZE_LINE = 300;


                                                // ex: 1 + 1
char* SmvConversions[] = {"%s", "{%s};" , "%s, ", "%s %s %s ", "redef%d%s", "%s \n", "init(%s)", "next(%s)",
                          "init(%s) := %s; \n", "next(%s) := %s; \n" , "case \n\t\t%s \n\tesac; \n",
                          "init(%s) = %s : %s; \n ", "next(%s) = %s : %s; \n", "TRUE : NULL; \n", "%s = %s : %s; \n" };


void updateTypeSet(char* varName ,HeaderSmv* header, STable* writeSmvTypeTable, const char* newValue, int type)
{

}


/**
 *  Atualiza uma String do tipo init/next(varName) = ... ;
 *  É usado para atualizar as atribuições de diretivas temporais, e atribuições case.
 *
 *  @param varName o nome da variável
 *  @param header a estrutura auxiliar associada com a localidade (main ou ports module) para a escrita em arquivo .smv
 *  @param writeSmvTypeTable a tabela de simbolos auxiliar para escrita em arquivo .smv
 *  @param newValue o valor que irá substituir ou ser adicionado a uma atribuição simples ou condicional
 *  @param condition a o cubo de condições associadas, caso exista.
 *  @param type o tipo que será usado na tabela de simbolos typeSet
 *  @param typeExpr o tipo se é init ou next
 *
 *  @sideEffects :  Atualiza o header->assignBuffer[pos] recuperado da tabela de simbolos writeSmvTYpeTable,
 *  além disso, atualiza na entrada da tabela da simbolos o tipo (typeSet) associada a varName,
 *  se o tipo mudar (caso de next após mudança de contexto).
 * */
void updateAssign(char* varName ,HeaderSmv* header, STable* writeSmvTypeTable, const char* newValue, const char* condition, int type ,int typeExpr)
{
    // checa o typeSet dessa variável para o atualizar ! (chamar aqui ou no interpretador ?)
    updateTypeSet(varName,header,writeSmvTypeTable,newValue,type);

    char *updated = NULL; // ponteiro para referenciar a string que vamos tratar
    char *upVar = NULL; // ponteiro para localizar o assign na tabela
    sprintf(upVar,SmvConversions[typeExpr],varName); // recupera string default init/next
    int sizeNew = strlen(newValue);
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

    // verifica o tamanho possível após a mudança (o +1 é justamente porque se o intervalo é x-y = 0, o tamanh é 1,
    // ou seja torna como size based ao inves de index based)
    if((-1*((pointEnd-pointIni+1) - sizeNew) + size)  >= ALOC_SIZE_LINE)
    {
        char* newStrSize = realloc(header->assignBuffer[pos],ALOC_SIZE_LINE*2);
        ALOC_SIZE_LINE = ALOC_SIZE_LINE*2;
        if(newStrSize == NULL)
        {
            fprintf(stderr, "FAIL IN REALOC HEADER SIZE FOR %s !",upVar);
            exit(-1);
        }
        header->assignBuffer[pos] = newStrSize;
    }
    updated = header->assignBuffer[pos];

    // é um simples next(x)/init(x):= yyyyyyy;  -> substituir y's por expressão nova em newValue antes do delmitador (;)
    // os delmitadores podem variar, ex: em caso de next(time) o delimitador é (:)
    if(!condition)
    {
        char aux[size-(pointEnd+1)]; // pointEnd+1 (é o indice(tamanho) sem ser 0-index) (+1 é para estarmos fora da zona da sobrescrita)
        int i;
        // deve-se copiar os caracteres que vem após a zona de sobrescrita
        //deve-se liberar sizeNew espaços empurrando os caras que vão ser salvos em aux
        for (i = pointEnd+1; i < size; i++) {
            //printf("copiando... %c \n",updated[i]);
            aux[i-(pointEnd+1)] = updated[i];
        }
        // atualiza o tamanho
        size = -1*((pointEnd-pointIni+1) - sizeNew) + size;

        // a zona de "sobrescrita" não aumentou
        if(sizeNew <= (pointEnd - pointIni)+1)
        {
            // deve-se "destruir" o resto da string partindo de pointIni
            updated[pointIni] = '\0';
        }
        // após isso, tanto para o caso de ter aumentado ou não deve-se recuperar a substring salva no
        // buffer auxiliar (delimitador e tudo o que vem depois), e escrever logo após escrever a nova string.
        for(i = pointIni; i < size; i++)
        {
            if(i >= pointIni+sizeNew) // i em indice medição sizeNew como tamanho (tirar -1)
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
                    newPointEnd = i;
                }
            }
        }
        newPointInit = pointIni;
    }
    else{
        // tratamento de init/next(varName):= case ... TRUE : x; esac; , geralmente TRUE: NULL ou outra condição parecida
        // é sempre o "delmitador final"
    }
    // atualizar range de interesse e tamanho da string na tabela!
    // fazer duas chamadas por enquanto
    void* vpSize[] = {&size};  // evita ter que "reescrever" o vetor inteiro separando em varios vps
    // é como se a gente tivesse atualizando cada indice dos vetores menos o (0)
    updateValue(upVar,vpSize,WRITE_SMV_INFO,1,1,-1,writeSmvTypeTable);
    void* vpIni[] = {&newPointInit};
    updateValue(upVar,vpIni,WRITE_SMV_INFO,1,2,-1,writeSmvTypeTable);
    void* vpInEnd[] = {&newPointEnd};
    updateValue(upVar,vpInEnd,WRITE_SMV_INFO,1,3,-1,writeSmvTypeTable);
}

/**
 *  Cria uma declaração do tipo init(varName) := newValue ; ou  init(varName) := case condition : newValue esac;
 *  Onde varName pode ser varName ou redef{redef}VarName.
 *
 *  @param varName o nome da nova variável
 *  @param condition a o cubo de condições associadas, caso exista.
 *  @param header a estrutura auxiliar associada com a localidade (main ou ports module) para a escrita em arquivo .smv
 *  @param writeSmvTypeTable a tabela de simbolos auxiliar para escrita em arquivo .smv
 *  @param newValue o valor que irá substituir ou ser adicionado a uma atribuição simples ou condicional
 *  @param condition o cubo de condições associadas, caso exista.
 *  @param redef se a variável é uma redefinição (é o numero de redefinições)
 *  @param typeExpr o tipo se é init ou next
 *  @param type o tipo da nova variável
 *
 *  @sideEffects: Adiciona uma nova string ao buffer do header->assignBuffer[posNova], e consequnetemente atualiza o POS desse header.
 *  além disso, cria uma na entrada da tabela de simbolos desse init e cria uma associada ao varName com o tipo (typeSet) associado.
 * */

void createAssign(char* varName ,HeaderSmv* header, STable* writeSmvTypeTable, const char* newValue, const char* condition, int redef, int typeExpr ,int type)
{
    char* exprResultString = malloc(sizeof(char)*ALOC_SIZE_LINE);
    char interRedef[300];

    if(!redef)
    {
        sprintf(exprResultString,SmvConversions[typeExpr],varName);
    }
    else{
        sprintf(interRedef,SmvConversions[5],redef,varName);
        sprintf(exprResultString,SmvConversions[typeExpr],interRedef);
    }

    // atualiza o typeSet
    // atualiza o init dessa função (init precisa de tabela de simbolos?) não é que nem um next !



}



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