#ifdef __cplusplus
extern "C" {
#endif
#pragma once
#include<iniparser/iniparser.h>
#include"macros.h"
#include<stdint.h>
struct cryptocoin{
	bool testnet;
	char * rpcuser;
	char * rpcpassword;
	uint16_t rpcport;
	char * rpchost;
	char * cryptocoin_name;
};

struct cryptocoin * init_cryptocoins(const char *);
struct cryptocoin * init_new_cryptocoins(const char *, uint * size); // not thread safe

void dump_cryptocoins(struct cryptocoin* cryptocoins);
void clear_cryptocoins(struct cryptocoin * cryptocoins);



typedef unsigned int uint;
uint getCountCryptocoins(void);
uint setCountCryptocoins(uint);
volatile static uint count_cryptocoins=0;

#ifdef __cplusplus
}
#endif
