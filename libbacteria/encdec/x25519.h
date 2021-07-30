#pragma once
#include<openssl/crypto.h>
#include<openssl/evp.h>
#include<stdio.h>
#include<string.h>
#define X25519_LENKEY 32
struct x25519_keysPair{
//	const size_t pubKeyLen = X25519_LENKEY;
	EVP_PKEY * privKey;	//, *pubKey ;
	unsigned char pubKey[X25519_LENKEY+1];
	EVP_PKEY_CTX * pKeyCtx;//just ctx

};

struct x25519_keysPair x25519_createKeyPair(const uint8_t * priv, const uint8_t * pub);
struct x25519_keysPair x25519_initKeyPairFromFile(const char * filepath);
struct x25519_keysPair x25519_generateKeyPair(void);
int x25519_savePrivKey(const char * filepath, struct x25519_keysPair * p);

uint8_t * x25519_getSharedKey( struct x25519_keysPair * pair, const uint8_t * pubPeer, size_t * skeylen );

void x25519_getRawPubKey(EVP_PKEY * privKey, uint8_t * pub);
void x25519_getRawPrivKey(EVP_PKEY * privKey, uint8_t * priv);

void x25519_freeKeyPair(struct x25519_keysPair * pair);
void x25519_freeSharedKeys(uint8_t * w, ...);
void x25519_freeSharedKey(uint8_t * w);
