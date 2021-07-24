#include "encdec/x25519.h"
#include <openssl/pem.h>

static size_t len_key = LENKEY;
// TODO:

#define ADDPREFIX(what, to) what##to

struct x25519_keysPair x25519_createKeyPair(const uint8_t *priv,
                                            const uint8_t *pub) {
  struct x25519_keysPair ret;
  bzero(ret.pubKey, sizeof(ret.pubKey));
  EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(NID_X25519, NULL);
  if (!ctx) {
    fprintf(stderr, "Cant create x25519 pair\n");
    return ret;
  }
  EVP_PKEY *privKey =
      EVP_PKEY_new_raw_private_key(EVP_PKEY_X25519, NULL, priv, len_key);
  x25519_getRawPubKey(privKey, ret.pubKey);
  // EVP_PKEY_get_raw_public_key(privKey, ret.pubKey, &len_key);
  ret.privKey = privKey;
  // ret.pubKey = pubKey;
  EVP_PKEY_CTX_free(ctx);
  ctx = EVP_PKEY_CTX_new(privKey, NULL);
  ret.pKeyCtx = ctx;
  return ret;
}
#define BUFSIZE 256
struct x25519_keysPair x25519_initKeyPairFromFile(const char *filepath) {
  struct x25519_keysPair rt;
  FILE *keyfile = fopen(filepath, "rb");
  size_t fs;
  fseek(keyfile, SEEK_END, 0);
  fs = ftell(keyfile);
  fseek(keyfile, SEEK_SET, 0);
  if (fs == 0) {
    fclose(keyfile);
    return rt;
  }
  char buf[BUFSIZE];
  fread(buf, BUFSIZE, 1, keyfile);
  fseek(keyfile, SEEK_SET, 0);
  if (strstr(buf, "-----BEGIN PRIVATE KEY-----") == NULL) {
    // puts("Not found");
    fclose(keyfile);
    return rt;
  }

  if (keyfile == NULL)
    return rt;
  // EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(NID_X25519, NULL);
  EVP_PKEY *pkey = NULL;
  pkey = PEM_read_PrivateKey(keyfile, NULL, NULL, NULL);
  unsigned char pubKey[LENKEY + 1];
  unsigned char privKey[LENKEY + 1];
  pubKey[LENKEY] = 0;
  privKey[LENKEY] = 0;
  x25519_getRawPubKey(pkey, pubKey);
  x25519_getRawPrivKey(pkey, privKey);
  // EVP_PKEY_CTX_free(ctx);
  EVP_PKEY_free(pkey);
  return x25519_createKeyPair(privKey, pubKey);
}

int x25519_savePrivKey(const char *filepath, struct x25519_keysPair *p) {
  FILE *exitFile;
  if (p->privKey == NULL || filepath == NULL ||
      (exitFile = fopen(filepath, "w")) == NULL)
    return -1;
  PEM_write_PrivateKey(exitFile, p->privKey, NULL, NULL, 0, NULL, NULL);
  fclose(exitFile);
  return 0;
}

void x25519_getRawPrivKey(EVP_PKEY *privKey, uint8_t *priv) {
  EVP_PKEY_get_raw_private_key(privKey, priv, &len_key);
}
void x25519_getRawPubKey(EVP_PKEY *privKey, uint8_t *pub) {
  EVP_PKEY_get_raw_public_key(privKey, pub, &len_key);
}

void x25519_freeKeyPair(struct x25519_keysPair *pair) {
  if (pair->pKeyCtx != NULL)
    EVP_PKEY_CTX_free(pair->pKeyCtx);
  if (pair->privKey != NULL)
    EVP_PKEY_free(pair->privKey);
  //	EVP_PKEY_free(pair->pubKey);
}
void x25519_freeSharedKey(uint8_t *w) {
  if (w != NULL)
    OPENSSL_free(w);
}

void x25519_freeSharedKeys(uint8_t *w, ...) {
  va_list ap;
  va_start(ap, w);
  while (*w) {
    x25519_freeSharedKey(w);
  }
  va_end(ap);
}

struct x25519_keysPair x25519_generateKeyPair(void) {
  struct x25519_keysPair ret;
  EVP_PKEY_CTX *ctx;
  EVP_PKEY *pkey = NULL;
  ctx = EVP_PKEY_CTX_new_id(NID_X25519, NULL);
  if (!ctx) {
    fprintf(stderr, "Cant create x25519 pair\n");
    return ret;
  }
  if (EVP_PKEY_keygen_init(ctx) <= 0) {
    fprintf(stderr, "can't keygen x25519 pair\n");
    EVP_PKEY_CTX_free(ctx);
    return ret;
  }
  EVP_PKEY_keygen(ctx, &pkey);
  EVP_PKEY_CTX_free(ctx);
  ctx = EVP_PKEY_CTX_new(pkey, NULL);
  bzero(ret.pubKey, sizeof(ret.pubKey));
  EVP_PKEY_get_raw_public_key(pkey, ret.pubKey, &len_key);
  // EVP_PKEY_CTX_free (ctx);

  ret.privKey = pkey;
  // ret.pubKey=pubKey;
  ret.pKeyCtx = ctx;
  return ret;
}

uint8_t *x25519_getSharedKey(struct x25519_keysPair *pair,
                             const uint8_t *pubPeer, size_t *skeylen) {
  if (!pubPeer || (pubPeer[31] & 0x80)) {
    fprintf(stderr, "Is not pubkey!\n");
    return NULL;
  } // not x25519 key

  if (EVP_PKEY_derive_init(pair->pKeyCtx) <= 0) {
    fprintf(stderr, "derive init error\n");
    return NULL;
  }

  EVP_PKEY *pkey =
      EVP_PKEY_new_raw_public_key(NID_X25519, NULL, pubPeer, len_key);

  if (EVP_PKEY_derive_set_peer(pair->pKeyCtx, pkey) <= 0) {
    fprintf(stderr, "Set peer key error\n");
    EVP_PKEY_free(pkey);
    return NULL;
  }
  if (EVP_PKEY_derive(pair->pKeyCtx, NULL, skeylen) <= 0) {
    fprintf(stderr, "buffer length derive err\n");
    EVP_PKEY_free(pkey);
    return NULL;
  }
  uint8_t *ret = OPENSSL_malloc(*skeylen);
  if (ret == 0) {
    fprintf(stderr, "OPENSSL malloc err\n");
    EVP_PKEY_free(pkey);
    return NULL;
  }
  if (EVP_PKEY_derive(pair->pKeyCtx, ret, skeylen) <= 0) {
    fprintf(stderr, "shared key write err\n");
    EVP_PKEY_free(pkey);
    return NULL;
  }
  EVP_PKEY_free(pkey);
  return ret;
}

static int x25519_bacteria_test(void) {
  struct x25519_keysPair pair = x25519_generateKeyPair();
  struct x25519_keysPair pair1 = x25519_generateKeyPair();
  if (pair.pKeyCtx == NULL || pair1.pKeyCtx == NULL) {
    return fprintf(stderr, "can't CTX init\n");
  }
  printf("pubKey: %s\nLen(strlen): %d\n", pair.pubKey,
         (int)strlen(pair.pubKey));
  printf("pubKey1: %s\nLen(strlen): %d\n", pair1.pubKey,
         (int)strlen(pair1.pubKey));
  puts("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  size_t skeylen, skeylen1;

  // bzero(shared0,sizeof(shared0));
  // bzero(shared1,sizeof(shared0));
  uint8_t *shared0 = x25519_getSharedKey(&pair, pair1.pubKey, &skeylen);
  uint8_t *shared1 = x25519_getSharedKey(&pair1, pair.pubKey, &skeylen1);
  printf("shared0: %s \nshared1: %s \n", shared0, shared1);
  x25519_freeSharedKeys(shared0, shared1, NULL);
  x25519_freeKeyPair(&pair);
  x25519_freeKeyPair(&pair1);
}
