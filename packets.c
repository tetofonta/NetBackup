/**
 * \file packets.c
 * \version 1.0
 * \author Stefano
 * \date 31-10-2017
 * \brief Metodi di compilazione strutture
 */

#include "Headers.h"

const char id_h[] = "id";
const char pk_h[] = "pk";
const char answ_h[] = "as";
const char new_h[] = "ba";

void make(char * dest, const char * in){
#pragma unroll
    for(int i = 0; i < 2; i++) dest[i] = in[i];
}

identify_h buildIdentify(){
    identify_h foo;
    make(foo.mw, id_h);
    return foo;
}

identify_answ_h buildIdentifyAnsw(){
    identify_answ_h foo;
    make(foo.mw, answ_h);
    return foo;
}

askPkey_h buildPkeyPacket(){
    askPkey_h foo;
    make(foo.mw, pk_h);
    return foo;
}

newBak_h buildBakPacket(){
    newBak_h foo;
    make(foo.mw, new_h);
    return foo;
}