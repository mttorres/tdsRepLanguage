#ifndef TM_H

#define TM_H


#define  ALOC_SIZE_LINE 1000

/**
 *  Concatena strings, porém sempre retornando o final da string como ponteiro, servindo para evitar percorrer a mesma
 *  diversas vezes ao se concatenar várias componentes (usada ao adicionar parâmetros por exemplo). Por outro lado,
 *  deve ser de total responsabilidade do método que chamou esse percorrer de volta para o endereço de inicio da string original
 *
 *  @param dest a string alocada de destino
 *  @param src a string origem
 *  @param toIgnore o caracter o qual deve ser ignorado ou onde deve pararm a concatenação
 *  @param ignoreMode 0 caso deve copiar tudo antes do que deve ser ignorado,
 *                    1 ignorar tudo antes do que deve ser ignorado incluindo ele mesmo,
 *
 *  @sideEffects :  Atualiza dest, indo para a última posição disponível em memória, ou seja, com '\0'
 *  @return A nova um ponteiro para o fim da string destino já preenchida
 * */
char * customCat(char* dest, char* src, char toIgnore, int ignoreMode);



/**
 *  Adiciona um parâmetro a uma declaração. Ex:  MODULE portsModule  ->  MODULE portsModule(time),
 *  ou  MODULE portsModule(time) -> MODULE portsModule(time,param2), ou ainda {0,1,NULL} -> {0,1,NULL, OCUPADO}.
 *  Ou seja, adiciona a qualquer delcaração com ou sem delimitadores (, {, }, ), ... um parâmetro.
 *
 *  Caso a delcaração não possua delimitadores, esses serão adicionados ao redor do parâmetro,
 *  caso essa já possua delimitador, o mais a direita (ex: }, ), ...  ) será removido até se preencher o parâmetro.
 *
 *
 *  @param original um ponteiro para a string original
 *  @param param a componente a ser adicionada
 *  @param
 *
 *  @sideEffects :  Atualiza o updated, seja lá de onde que essa string veio em primeiro lugar
 *
 *  @return A nova string alocada em outro ponteiro (a responsabilidade de free ou realloc fica fora dessa função)
 * */
char *addParams(char *original, char *param, char *delim1, char *delim2, int useRealloc);



void clearOldPortsRefs(char* oldConstraint, char* toCopyResult);

/**
 *  Genralização, escreve no meio do intervalo  intervalo início,fim de uma String qualquer.
 *  Após isso atualiza os novos intervalos para uma próxima alteração
 *
 *  @param newValue o que será escrito
 *  @param updated o ponteiro para a string que será alterada
 *  @param sizeNew o tamanho do valor a ser escrito (subString)
 *  @param pointIni o início do intervalo
 *  @param paramEnd o fim do intervalo .
 *  @param size o tamanho da string total original
 *  @param newPointInit o ponteiro do novo ponto de partida do intervalo
 *  @param newPointEnd o ponteiro do novo fim do intervalo
 *  @param variantPointIni uma flag utilizada para decidir se existe referência a string escrita anteriormente
 *                          (usado para updateAssign)
 *  @sideEffects :  Atualiza o updated, seja lá de onde que essa string veio em primeiro lugar
 *  (a responsabilidade de free ou realloc fica fora dessa função)
 *  Caso o tamanho da string venha a VIOLAR o tamanho padrão de texto, a string passada é realocada!
 *  @returns um ponteiro da string atualizada (justamente para o caso de realocação)
 * */
char* updateSubStringInterval(const char *newValue, char *updated, int sizeNew, int pointIni, int pointEnd, int size,
                             int *newPointInit, int *newPointEnd, int variantPointIni);

/**
 *  Retorna uma declaração de módulo/função sem nenhum dos parâmetros originais e com um novo parâmetro.
 *  Ex: module(time) ->  module(ports)
 *
 *  @param moduleName a declaração original do módulo (ou função)
 *  @param param o novo parâmetro que irá sobrescrever todos os demais
 *
 *  @returns a nova declaração.
 *  @SideEffects :  Aloca string como retorno
 *  (a responsabilidade de free ou realloc fica fora dessa função)
 * */
char* overwriteParam(char* moduleName, char* param);

/**
 * Recebendo uma String dest como parâmetro, para evitar a responsabiliddade de retornar uma alocada (podemos inclusive mudar isso nos demais métodos)
 * copia dados de src para ela até um determinado caracter de parada
 * @param dest a string destino alocada ou não fora desse método (tenha em mente que a string NÃO PODE SER READ ONLY)
 * @param src a string a qual iremos copiar
 * @param stop o caracter onde pararemos a copia.
 * @SideEffects: Copia caracteres para dest, se é alocado ou não é responsabilidade de quem chamar a função
 */
void removeAfter(char* dest, char* src, char stop);

/**
 * Converte um número ou diretiva para uma string da mesma
 * @param num o número ou diretiva passada
 * @return uma string alocada com o valor
 * @SideEffects: Aloca uma string com o valor do número que deve ser liberado pelo chamador
 */
char* formatNumeric(int ctime);

/**
 * Remove todas as ocorrencias de uma substring
 * @param string a string original
 * @param toRemove a substring que deve ser removida
 * @return uma nova string igual a antiga porém sem ocorrencias de toRemove
 * @SideEffects: Aloca uma string no formato descrito e deve ser liberada pelo chamador
 */
char* removeAllSubStrings(char* string, char* toRemove);

#endif
