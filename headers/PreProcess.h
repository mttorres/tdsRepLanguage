#ifndef PREP_H

#define PREP_H

#include "HeaderSmv.h"




/* 
	salva o headerSMV do módulo lido anteriormente. 
	se for um automato o ponteiro de transição não é vázio.
	efeito colateral: * ao criar ao salvar o header incrementa o tamanho de headers do HeaderController
					  *	cria um header (necessita de free depois) 
*/

void initPreProcessHeader(smvtype type, char* moduleName, HeaderController* Hcontrol);




/*
	escolhe um buffer do header atual para salvar a linha (alocando sempre uma string para essa linha, que depois deve ser liberada).
	efeitos colaterais:  * ao ter readVarPortsModule como true, ele salva as variáveis do portsModule em uma tabela de portas
						 * ao ter controlRename como true, ele remove todas as ocorrências de determinados caracteres (no caso [])

*/
void selectBuffer(headerpart part, char* line, HeaderSmv* header, int controlRename);


/*
	prepara para chamar a função de escolha de buffer, t
	ratando a posição (pos) que representa o tamanho do vetor de HEADERS do controller (LEN-1)
						 
*/
void saveLineOnBuffer(smvtype currentHeader, headerpart part, char *line, HeaderController *Hcontrol, int controlRename);


/* fases: criação, var, assign(pode não existir), trans(pode não existir) (as partes de interesse)
 	as partes de interesse servem como delimitadores,  quebras de linha servem como delimitadores dos módulos
 	stages são os módulos 0(main), automato(2), ports(3)
*/
void processPhase(smvtype stage, headerpart part, HeaderController *Hcontrol, char *line, int controlRename);

/**
 * Salva o arquivo SMV original em headers, separados por categorias e partes de um arquivo SMV.
 * Durante esse pré processamento, são usadas variáveis strings auxiliares para reconhecer pontos de interesse no arquivo.
 * A cada ponto de interesse identificado, um buffer do header é escolhido, e esse atualizado.
 * Ao se identificar o alguma referência a portsModule essa tem o parâmetro time adicionado, e uma referência é salva no header do automato,
 * para renomações futuras. Além disso são criadas entradas na tabelas de simbolos auxiliares (SMV_INFO) usadas pelo main e ports, respectivamente,
 * a variável time e as portas que vierem do módulo original.
 * */
void preProcessSmv(FILE *smvP, HeaderController *Hcontrol);

/**
 *  Configura a tabela de controle SMV info da main para a variável time a incializando
 *  com uma tupla de inteiros (que não possui typeset, por so poder ser X..Y). Além disso,
 *  inicializa as diretivas temporais no escopo global (main).
 *
 *  @param Hcontrol o controller de headers e informações SMV
 *  @param global a tabela de simbolos global do "programa" .tds
 *
 *  @SideEffects:  Chama o addValue da tabela de simbolos para os casos citados, alocando memória para as Entry de
 *  cada tupla.
 * */
void setUpMainSmvTable(HeaderController *Hcontrol, STable *global);

#endif

