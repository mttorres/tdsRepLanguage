//
// Created by mateus on 02/04/2021.
//

#include <stdlib.h>
#include "../headers/TypeMinMax.h"


TypeMinMax* createMinMaxByOneValue(int valSin){
    int min = 0; // init default
    int max = 1;
    if(valSin <= 0){
        min = valSin;
    }
    else{
        max = valSin;
    }
    return createMinMax(min,max);
}

TypeMinMax* createMinMax(int min, int max){
     TypeMinMax* newMinMax = malloc(sizeof(TypeMinMax));
     newMinMax->min = min;
     newMinMax->max = max;
     return newMinMax;
}

TypeMinMax* computeMinMaxSum(TypeMinMax* typeRefMinMax1, TypeMinMax* typeRefMinMax2){
    int min = typeRefMinMax1->min + typeRefMinMax2->min;
    int max = typeRefMinMax1->max + typeRefMinMax2->max;
    return createMinMax(min,max);
}

TypeMinMax* computeMinMaxSub(TypeMinMax* typeRefMinMax1, TypeMinMax* typeRefMinMax2){
    int min = typeRefMinMax1->min - typeRefMinMax2->max;
    int max = typeRefMinMax1->max - typeRefMinMax2->min;
    return createMinMax(min,max);
}

TypeMinMax* computeMinMaxNeg(TypeMinMax* typeRefMinMax1){
    int eNegativo = typeRefMinMax1->max <= 0;
    int min;
    int max;
    // maior positivo
    if(eNegativo){
        min = (-1)*typeRefMinMax1->min;
        max = (-1)*typeRefMinMax1->max;
    }
    // maior negativo
    else{
        min = (-1)*typeRefMinMax1->max;
        max = (-1)*typeRefMinMax1->min;;
    }
    return createMinMax(min,max);
}

TypeMinMax* computeMinMaxMul(TypeMinMax* typeRefMinMax1, TypeMinMax* typeRefMinMax2){
    int min;
    int max;
    if(typeRefMinMax1->min >= 0 && typeRefMinMax2->min >= 0){
        min = typeRefMinMax1->min*typeRefMinMax2->min;
    }
    else{
        int cond = typeRefMinMax1->min >= 0;
        // min1 >= 0   escolhe min2        senao escolhe min1
        int negativo = cond? typeRefMinMax2->min : typeRefMinMax1->min;
        // faz o contrário
        int positivo = cond?  typeRefMinMax1->max : typeRefMinMax2->max;
        min = negativo*positivo;
    }
    //if(typeRefMinMax1->max > 0 && typeRefMinMax2->max > 0){
    max = typeRefMinMax1->max *typeRefMinMax2->max;
    //}
    return createMinMax(min,max);
}

TypeMinMax* computeMinMaxDiv(TypeMinMax* typeRefMinMax1, TypeMinMax* typeRefMinMax2){
    int min;
    int max;
    if(typeRefMinMax1->min >= 0 && typeRefMinMax2->max > 0){
        min = typeRefMinMax1->min / typeRefMinMax2->max;
    }
    // ou o min1 é negativo ou max2 é zero/negativo (tornando minmax2 negativo)
    else{
       // min1 é negativo
       if(typeRefMinMax1->min < 0){
           if(typeRefMinMax2->min > 0){
               min = typeRefMinMax1->min / typeRefMinMax2->min; // tenta gerar o negativo com maior distancia a 0
           }
           else{
               // min2 negativo
               // o segundo é negativo (mas é o mais próximo de zero)
               if(typeRefMinMax2->max < 0){
                   min = typeRefMinMax1->min / typeRefMinMax2->max; // deve gerar o menor positivo o possível
               }
               else{
                    // o max do segundo é perto de zero e positivo (gerando valores muito grandes) ou seja o valor minimo deve ser ele mesmo
                    min = typeRefMinMax1->min;
               }
           }
       }
       // max2 é negativo e é o mais próximo de zero (um valor de módulo pequeno) (e o min2 também é negativo só que de modulo grande)
       else{
           // o min 1 é positivo ou zero (senão teriamos caido no código de antes) ou seja devemos criar um negativo mais longe de zero (ou seja de módulo alto)
           if(typeRefMinMax1->min > 0 && typeRefMinMax2->max != 0){
               min = typeRefMinMax1->min / typeRefMinMax2->max; // deve gerar o negativo de maior módulo, considerando que min2 é um negativo "grande"
                // código replicado porém "separado"
           }
           // max2 é zero ou seja o menor valor de divisão é algum número entre min2 ~ -1.
           // Como min1 é positivo, a menor divisão que ele vai assumir é  ele mesmo negativo, qualquer outra daria um número mais perto de zero.
           else{
               min = typeRefMinMax1->min/(-1);
           }
       }
    }

    if(typeRefMinMax2->min > 0){
        max = typeRefMinMax1->max / typeRefMinMax2->min; // vai gerar o número de maior módulo independente do sinal
        // no caso mesmo que max1 seja negativo ainda quer dizer que ele é o mais próximo de zero comparado aos demais e seu minimo
    }
    else{
        // min2 é negativo
        if(typeRefMinMax2->min < 0){
            if(typeRefMinMax2->max > 0 ){
                // o valor máximo assumido pelo resultado é ele mesmo porque o menor positivo nesse caso é o 1
                max = typeRefMinMax1->max; // seja positivo ou negativo
            }
            // o max2 é um número menor que zero porém de módulo pequeno (comparado ao seu minimo)
            else{
                if(typeRefMinMax2->max != 0){
                    max = typeRefMinMax1->max/typeRefMinMax2->max; // max1/max2 (max2 é um número bem perto de zero e negativo, tirando o menos possivel de max1)
                }
                else{
                    // max2 é zero ou seja o número até pode ser zero, mas não vai realizar a divisão por zero.
                    // Ou seja o valor do segundo operando que menos vai tirar de max2 é -1
                    max = typeRefMinMax1->max /(-1);
                }
            }
        }
        // min2 é zero
        else{
            // ou seja max2 é positivo
            // então o que iria tirar menos é o 1 (já que max2 não pode ser zero tmb)
            max = typeRefMinMax1->max;
        }
    }
    return createMinMax(min,max);
}


TypeMinMax* computeMinMaxMod(TypeMinMax* typeRefMinMax1){
    int min = typeRefMinMax1->min < 0? typeRefMinMax1->min : 0; // o mínimo é o resto da divisão ser zero ou ser o menor número possível
    int max = typeRefMinMax1->max; // o máximo é ser o próprio número em seu valor máximo
    return createMinMax(min,max);
}

TypeMinMax* copyTypeMinMax(TypeMinMax* original){
    if (original){
        return createMinMax(original->min,original->max);
    }
    return NULL;
}

int mergeTypeMinMax(TypeMinMax* original, TypeMinMax* newInfo){
    int min = original->min;
    int max = original->max;
    int newMax = newInfo->max;
    int newMin = newInfo->min;

    int changeMin = newMin < min? 0 : -1;
    int changeMax = newMax > max? 1 : -1;

    if(changeMax != -1 && changeMin != -1){
        original->max = newMax;
        original->min = newMin;
        return 2;
    }
    if(changeMin != -1){
        original->min = newMin;
        return changeMin;
    }
    if(changeMax != -1){
        original->max = newMax;
        return changeMax;
    }
    return -1;
}

int changeMinMax(TypeMinMax* tmm, int value){
    int min = tmm->min;
    int max = tmm->max;
    if(value < min){
        tmm->min = value;
        return 0;
    }
    if(value > max){
        tmm->max = value;
        return 1;
    }
    return -1;
}

void letGoTypeMinMax(TypeMinMax* tmm){
    free(tmm);
}
