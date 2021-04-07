//
// Created by mateus on 02/04/2021.
//

#ifndef TDSREPLANGUAGE_TYPEMINMAX_H
#define TDSREPLANGUAGE_TYPEMINMAX_H

typedef struct type_min_max
{
    //int pos;  // talvez seja desnecessário
    //int size; // só precisamos da referencia (mesmo para type-set) em que iremos usar os demais valores para auxiliar o que devemos jogar no nuXmv
    int min;
    int max;
} TypeMinMax;

/**
 * para todos os casos devemos tratar caso min = max (e colocar 1) senão o nuXmv acha que é UMA CONSTANTE
 */

/**
 * Trata inicializações de type-min-max de valores unicos considerando que não devemos montar "constantes".
 * Esse método pode ser melhorado no futuro
 * @param valSin o valor unico sintetizado junto de um objeto
 * @return um type-min-max montado com um valor único
 */
TypeMinMax* createMinMaxByOneValue(int valSin);

/**
 * Cria o type-min max associado a um valor/variável/expressão. Realizando o tratamento necessário
 * @param min o valor máximo
 * @param max o valor mínimo
 */
TypeMinMax* createMinMax(int min, int max);


/**
 * Cria um novo min-max que representa a soma de outros dois tipos inteiros.
 *
 * A representação segue as seguintes regras:
 *
 *  tmm1:   min: + ,  max : +
 *                              - resulta em -> tmmR:   min: min1 + min2 ,  max: max1+max2
 *  tmm2:   min: + ,  max : +      (0,10) ,  (2,5) ->  (2,15)
 *
 *  tmm1:   min: + ,  max : +
 *                              - resulta em -> tmmR:   min: min1 + min2 ,  max: max1+max2
 *  tmm2:   min: - ,  max : +      (-3,10) ,  (2,5) ->  (-1,15)
 *
 *  tmm1:  min : -, max : -
 *                           -  resulta em ->  min: min1 - min 2 , max> max1 - max2
 *                              (-3,-6) , (-4,-1) ->   -7 , -7
 *  tmm2:  min: -,  max : -
 *
 *  generalizando.... o nuXmv faz assim (como escrevi na subtração que é um pouco diferente):
 *
 *   (0,10) ,  (2,5) ->  (2,15)
 *   0  +  (2~5)   ->  2~5
 *   1  +  (2~5)   ->  3~6
 *   2  +  (2~5)   ->  4~7
 *   3  +  (2~5)   ->  5~8
 *   4  +  (2~5)   ->  6~9
 *   5  +  (2~5)   ->  7~10
 *   6  +  (2~5)   ->  8~11
 *   7  +  (2~5)   ->  9~12
 *   8  +  (2~5)   ->  10~13
 *   9  +  (2~5)   ->  11~14
 *   10  +  (2~5)  ->  12~15
 *
 *   No caso da soma é "trivial"
 *
 * @param typeRefMinMax1 o type-min-max do primeiro operando
 * @param typeRefMinMax2 o type-min-max do segundo operando
 * @return um type-min-max novo que segue as regras especificadas.
 */
TypeMinMax* computeMinMaxSum(TypeMinMax* typeRefMinMax1, TypeMinMax* typeRefMinMax2);

/**
 * Cria um novo min-max que representa a subtração de outros dois tipos inteiros.
 *
 * A representação segue as seguintes regras:
 *
 *  tmm1:   min: + ,  max : +
 *                              - resulta em -> tmmR:   min: min1 - min2 ,  max: max1-max2
 *  tmm2:   min: + ,  max : +      (0,10) ,  (2,5) ->  (-2,5)
 *
 *  tmm1:   min: + ,  max : +
 *                              - resulta em -> tmmR:   min: min1 - min2 ,  max: max1-max2
 *  tmm2:   min: - ,  max : +
 *
 *  tmm1:  min : +, max : +
 *                           -  resulta em ->  min: min1 - min 2 , max: max1 - max2
 *                              (-4,1), (0,6) ->   -4, -5  ? (ERRADO) -> na verdade o que ele faz é
 *  tmm2:  min: -,  max : -
 *                              -4 - (0~6)    -> (-4,-10)
 *                              -3 - (0~6)
 *                              -2 - (0~6)
 *                              -1 - (0~6)
 *                              0 -  (0~6)
 *                              1 - (0~6)    ->  (1,-5)
 *
 *                              O certo é:
 *                              min:  min1 - max2 ,  max: max1 - min2
 *                              -4-6  e   1-0    (-10,1)
 *                              A lógica é... o minimo deve ter O MAIOR VALOR POSSÍVEL RETIRADO (ou seja o max do operando 2)
 *                              E o max deve ter o menos retirado o possível
 *
 * @param typeRefMinMax1 o type-min-max do primeiro operando
 * @param typeRefMinMax2 o type-min-max do segundo operando
 * @return um type-min-max novo que segue as regras especificadas.
 */
TypeMinMax* computeMinMaxSub(TypeMinMax* typeRefMinMax1, TypeMinMax* typeRefMinMax2);

/**
 * Realiza a operação de negativo de um valor único de type-min-max. Seguindo a regra:
 *
 * Se o número é positivo:
 * min:  -max1
 * max:  -min1
 *
 * Se o número é negativo:
 *
 * min: -max1
 * max: -min1
 *
 * A ideia é tratar a distancia em módulo de valores
 * @param typeRefMinMax1
 * @return o novo type-set com o min max que segue a regra
 */
TypeMinMax* computeMinMaxNeg(TypeMinMax* typeRefMinMax1);

/**
 * Cria um novo min-max que representa a multiplicação de outros dois tipos inteiros seguindo a seguinte regra:
 *
 *   min:
 *
 *      Se min1 e min2 forem positivos  >= 0. min1*min2
 *      Se min1 ou min2 forem negativos:  negativo1*max2  ou negativo2*max1 (vai resultar em uma multiplicação bem negativa)
 *
 *  max:
 *
 *      max1*max2 se ambos positivos
 *      se algum deles forem negativos (ou seja o max =< 0 deve ser  max1*maiorNegativo ou o contrário)
 *      Da a mesma coisa no final das contas...
 *
 * @param typeRefMinMax1 o type-min-max do primeiro operando
 * @param typeRefMinMax2 o type-min-max do segundo operando
 * @return um type-min-max novo que segue as regras especificadas
 */
TypeMinMax* computeMinMaxMul(TypeMinMax* typeRefMinMax1, TypeMinMax* typeRefMinMax2);

/**
 * Cria um novo min-max que representa a divisão de outros dois tipos inteiros seguindo a seguinte regra:
 *
 *  min: min1/max2  ou min1/min2 (caso min1 seja negativo, deve ser o min2 > 0 de forma a gerar o negativo com maior distancia a zero),
 *
 *  max: max1/min2 tal que min seja > 0, senão ele pega o menor valor positivo possível (caso exista)
 *  Se não existir esse menor (max) positivo (número negativo completo),
 *  (vamos estar gerando uma operação com resultado com o maior número possível, sendo na verdade o menor..)
 *  Enão fazer max: max1/max2  (de forma que ainda vai gerar um negativo porém "maior" isso é mais perto de zero).
 *
 * @param typeRefMinMax1 o type-min-max do primeiro operando
 * @param typeRefMinMax2 o type-min-max do segundo operando
 * @return retorna um novo type-min-max que resolva o intervalo
 */
TypeMinMax* computeMinMaxDiv(TypeMinMax* typeRefMinMax1, TypeMinMax* typeRefMinMax2);

/**
 *
 *  Cria um novo min-max que representa a operação de modlulo entre dois tipos inteiros seguindo a seguinte regra:
 *
 *   operando2 não pode ser igual a zero (mas a linguagem já trata)
      um módulo de  x mod y  pode ter os seguintes valores:
      0 divisão exata  (a operação mod não tem negativos)
      ~
      x y é muito maior que x
      Como o operando1 pode ter um intervalo variado. Devemos estender entre o seu min e max caso seja um número
      que varia entre positivos e negativos. Caso contrário, será do 0 até o seu max.
      E o nuXmv trata diferente da regra:
            a/b = q with remainder r
            Such that: b*q + r = a and 0 <= r < b
      Assim como em C e C++
 *
 * @param typeRefMinMax1 o type-min-max do primeiro operando
 * @param typeRefMinMax2 o type-min-max do segundo operando
 * @return retorna um novo type-min-max que resolva o intervalo
 */
TypeMinMax* computeMinMaxMod(TypeMinMax* typeRefMinMax1);

/**
**
 * Cria um novo objeto type-min-max
 * @return uma copia do type-min-max original
 */
TypeMinMax* copyTypeMinMax(TypeMinMax* original);

/**
 * Decide o min-max de um type-min-max baseado em uma nova informação sintetizada.
 * Após realizar a decisão retorna um código que irá sinalizar qual parte da declaração deve ser alterada na string.
 * @param original o type-min-max que já está contido na declaração
 * @param newInfo a nova informação de type-min-max sintetizada
 * @return -1 caso nenhum tenha sido alterado, 0 se o minimo necessita de alteração, 1, se o maximo necessita, 2 se ambos necessitam
 */
int mergeTypeMinMax(TypeMinMax* original, TypeMinMax* newInfo);

int changeMinMax(TypeMinMax* tmm, int value);

void letGoTypeMinMax(TypeMinMax* tmm);

#endif //TDSREPLANGUAGE_TYPEMINMAX_H