#include "libencdec.h"

#define LUA_ENUM(L, val)                                                       \
  lua_pushliteral(L, #val "");                                                 \
  lua_pushnumber(L, val);                                                      \
  lua_settable(L, -3)

//#define DEBUGAES

int luaopen_encdec(lua_State *L) {
  lua_newtable(L);
  LUA_ENUM(L, t_ecb);
  LUA_ENUM(L, t_cbc);
  LUA_ENUM(L, t_chacha20);
  lua_setglobal(L, "AESENCType");
  luaL_openlib(L, "encdec", encdeclib, 0);
  return 1;
}

int lua_genRandBytes(lua_State *L) {
  int len = (int)luaL_checknumber(L, 1);
  if (len <= 0) {
    luaL_error(L, "genRandBytes. bytes will be more than 0.");
  }
  char bytes[len + 1];
  generate_rand_bytes(len+1, bytes);
  bytes[len] = '0';
  lua_pushlstring(L, bytes, len);
  return 1;
}

#define INITENCTYPE(prefix, algo)                                              \
  if ((type & t_##prefix) == t_##prefix) {                                     \
    ciphertext_len = algo##_encrypt(plaintext, size_msg, key, iv, ciphertext); \
  }
int lua_AESenc(lua_State *L) { // lua_AESenc
  unsigned char *key = (unsigned char *)luaL_checkstring(L, 1);
  unsigned char *iv = (unsigned char *)luaL_checkstring(L, 2);
  unsigned char *plaintext = (unsigned char *)luaL_checkstring(L, 3);
  int type = (int)luaL_checknumber(L, 4);

  int ciphertext_len;
  int size_msg = strlen(plaintext);
  if (size_msg == 0)
    return 0;
  unsigned char *ciphertext =
      (unsigned char *)malloc(size_msg * sizeof(char) + 16);
  if (ciphertext == NULL)
    return 0;
#ifdef DEBUGAES
  printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ENC~~~~~~~~~~~~~~~~~~~~~~~~~"
         "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
#endif

  INITENCTYPE(cbc, AES_256_cbc)
  else INITENCTYPE(ecb, AES_256_ecb) else INITENCTYPE(
      chacha20, chacha20_poly1305) else return 0;

 // if(ciphertext_len <= 0) luaL_error(L,"encrypt failed");
  if(ciphertext_len <= 0) {
	lua_pushnil(L);
	return 1;
  }
  ciphertext[ciphertext_len] = '\0';
#ifdef DEBUGAES
  printf("IV: %s\t\tKEY: %s\n", iv, key);
  for (unsigned int i = 0; i < ciphertext_len; i++) {
    printf("%d ", ciphertext[i]);
  }
  printf("\n\n\n");
  printf("\n\n\n");
  printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
         "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
#endif
  size_t nbytes = sizeof(struct lua_AESData) +
                  (ciphertext_len - 1) * sizeof(size_t) + sizeof(char *);
  struct lua_AESData *ret = (struct lua_AESData *)lua_newuserdata(L, nbytes);
  ret->size = ciphertext_len;
  ret->data = ciphertext;
  return 1;
  //      lua_pushstring(L, ciphertext);
  //	free(ciphertext);
  //	return 2;
}

#define INITDECTYPE(prefix, algo)                                              \
  if ((type & t_##prefix) == t_##prefix) {                                     \
    plaintext_len = algo##_decrypt(in->data, in->size, key, iv, plaintext);    \
  }
int lua_AESdec(lua_State *L) {

#ifdef DEBUGAES
  printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DEC~~~~~~~~~~~~~~~~~~~~~~~~~"
         "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
#endif

  unsigned char *key = (unsigned char *)luaL_checkstring(L, 1);
  unsigned char *iv = (unsigned char *)luaL_checkstring(L, 2);
  struct lua_AESData *in = (struct lua_AESData *)lua_touserdata(L, 3);
  int type = (int)luaL_checknumber(L, 4);
  
  if (in == NULL || in->data == 0 || in->size == 0)
    return 0;

  int plaintext_len;
  char *plaintext = (char *)malloc(in->size * sizeof(char));
  if (plaintext == NULL)
    return 0;

#ifdef DEBUGAES
  printf("IV: %s\t\tKEY: %s\n", iv, key);
  for (unsigned int i = 0; i < in->size; i++) {
    printf("%d ", in->data[i]);
  }
  printf("\n\n\n");
  printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
         "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
#endif

  INITDECTYPE(cbc, AES_256_cbc)
  else INITDECTYPE(ecb, AES_256_ecb) else INITDECTYPE(
      chacha20, chacha20_poly1305) else return 0;
  //if(plaintext_len <= 0) luaL_error(L,"decrypt failed");
  if(plaintext_len <= 0) {
	lua_pushnil(L);
	return 1;
  }
  plaintext[plaintext_len] = '\0';
  
  /*if(in->data!=NULL){
          free(in->data);
  }
  in->size=plaintext_len;
//	lua_settop(L,0);
  in->data=plaintext;
//        lua_pushnumber(L, plaintext_len);
  lua_pushboolean(L,1);*/
  size_t nbytes = sizeof(struct lua_AESData) +
                  (plaintext_len - 1) * sizeof(size_t) + sizeof(char *);
  struct lua_AESData *ret = (struct lua_AESData *)lua_newuserdata(L, nbytes);
  ret->size = plaintext_len;
  ret->data = plaintext;
  return 1;
  //        lua_pushstring(L, plaintext);
  //	free(plaintext);
  //	return 2;
}

INITLUAFUNC(createAESData) {
  unsigned char *data = (unsigned char *)luaL_checkstring(L, 1);
  long long size = (long long)luaL_checknumber(L, 2);
  size_t nbytes = sizeof(struct lua_AESData) + (size - 1) * sizeof(size_t) +
                  sizeof(unsigned char *);
  struct lua_AESData *ret = (struct lua_AESData *)lua_newuserdata(L, nbytes);
  ret->size = size;
  ret->data = (unsigned char *)malloc(size * sizeof(unsigned char));
  strncpy(ret->data, data, size);
  return 1;
}

INITLUAFUNC(getAESData) {
  struct lua_AESData *in = (struct lua_AESData *)lua_touserdata(L, 1);
  if (in == NULL || in->data == 0 || in->size == 0)
    return 0;
  lua_pushstring(L, in->data);
  return 1;
}

INITLUAFUNC(getAESData_len) {
  struct lua_AESData *in = (struct lua_AESData *)lua_touserdata(L, 1);
  if (in == NULL || in->data == 0 || in->size == 0)
    return 0;
  lua_pushnumber(L, in->size);
  return 1;
}

int lua_freeAESData(lua_State *L) {
  struct lua_AESData *in = (struct lua_AESData *)lua_touserdata(L, 1);
  if (in == 0)
    luaL_error(L, "aesdata is broken");

  if (in->data != 0 || in->size != 0) {
    free(in->data); // in->size=0;in->data=0;
    lua_pushboolean(L, 1);
    return 1;
  }
  lua_pushboolean(L, 0);
  return 1;
}

// x25519 and MAYBE rsa(mamonth...)aa support LATTER(not😠️need)

int lua_getKeyPair(lua_State *L) {
  /*
          EVP_PKEY * privKey;	//, *pubKey ;
          unsigned char pubKey[X25519_LENKEY+1];
          EVP_PKEY_CTX * pKeyCtx;//just ctx
  */
  size_t nbytes = sizeof(struct x25519_keysPair) + sizeof(EVP_PKEY *) +
                  sizeof(EVP_PKEY_CTX *) + sizeof(unsigned char) * (X25519_LENKEY);
  struct x25519_keysPair *ret =
      (struct x25519_keysPair *)lua_newuserdata(L, nbytes);
  struct x25519_keysPair pair = x25519_generateKeyPair();
  if (pair.pKeyCtx == NULL) {
    luaL_error(L, "Can't CTX init");
    return fprintf(stderr, "can't CTX init\n");
  }
  ret->privKey = pair.privKey;
  memcpy(ret->pubKey, pair.pubKey, X25519_LENKEY + 1);
  // ret->pubKey = pair.pubKey;
  ret->pKeyCtx = pair.pKeyCtx;
  return 1;
}

int lua_initKeyPairFromFile(lua_State *L) {
  char *filepath = (char *)luaL_checkstring(L, 1);
  struct x25519_keysPair rt = x25519_initKeyPairFromFile(filepath);
  if (rt.privKey == NULL) {
    lua_pushnil(L);
    return 1;
  }
  size_t nbytes = sizeof(struct x25519_keysPair) + sizeof(EVP_PKEY *) +
                  sizeof(EVP_PKEY_CTX *) + sizeof(unsigned char) * (X25519_LENKEY);
  struct x25519_keysPair *ret =
      (struct x25519_keysPair *)lua_newuserdata(L, nbytes);
  ret->privKey = rt.privKey;
  memcpy(ret->pubKey, rt.pubKey, X25519_LENKEY + 1);
  // ret->pubKey = pair.pubKey;
  ret->pKeyCtx = rt.pKeyCtx;
  return 1;
}

int lua_saveKeyPairToFile(lua_State *L) {
  struct x25519_keysPair *in = (struct x25519_keysPair *)lua_touserdata(L, 1);
  char *filepath = (char *)luaL_checkstring(L, 2);
  if (in == NULL)
    luaL_error(L, "x25519_keysPair broken");
  int ret = x25519_savePrivKey(filepath, in);
  if (ret == -1)
    lua_pushnil(L);
  else
    lua_pushboolean(L, 1);
  return 1;
}

int lua_freeKeyPair(lua_State *L) {
  struct x25519_keysPair *in = (struct x25519_keysPair *)lua_touserdata(L, 1);
  if (in == NULL)
    luaL_error(L, "x25519_keysPair broken");
  x25519_freeKeyPair(in);
  lua_pushboolean(L, 1);
  return 1;
}

// int lua_freeSharedKey(lua_State *L) {}

INITLUAFUNC(toSHA512) {
  uint8_t *data = (uint8_t *)luaL_checkstring(L, 1);
  char outputstring[SHA512_OUTPUTSTRING_SIZE];
  // INITHASHFUNC(SHA512, 129);
  toSHA512(data, outputstring);
  lua_pushstring(L, outputstring);
  return 1;
}
INITLUAFUNC(toSHA256) {
  uint8_t *data = (uint8_t *)luaL_checkstring(L, 1);
  char outputstring[SHA256_OUTPUTSTRING_SIZE];
  toSHA256(data, outputstring);
  // INITHASHFUNC(SHA256, 65);
  lua_pushstring(L, outputstring);
  return 1;
}

int lua_createKeyPair(lua_State *L) {
  uint8_t *pub = (uint8_t *)luaL_checkstring(L, 1);
  uint8_t *priv = (uint8_t *)luaL_checkstring(L, 2);
  struct x25519_keysPair pair = x25519_createKeyPair(priv, pub);
  size_t nbytes = sizeof(struct x25519_keysPair) + sizeof(EVP_PKEY *) +
                  sizeof(EVP_PKEY_CTX *) + sizeof(unsigned char) * (X25519_LENKEY);
  struct x25519_keysPair *ret =
      (struct x25519_keysPair *)lua_newuserdata(L, nbytes);
  if (pair.pKeyCtx == NULL) {
    luaL_error(L, "Can't CTX init");
    return fprintf(stderr, "can't CTX init\n");
  }
  ret->privKey = pair.privKey;
  memcpy(ret->pubKey, pair.pubKey, X25519_LENKEY + 1);
  // ret->pubKey = pair.pubKey;
  ret->pKeyCtx = pair.pKeyCtx;
  return 1;
}
INITLUAFUNC(getPubKey) {
  struct x25519_keysPair *in = (struct x25519_keysPair *)lua_touserdata(L, 1);
  if (in == NULL)
    luaL_error(L, "x25519_keysPair broken");
  lua_pushlstring(L, in->pubKey, X25519_LENKEY);
  return 1;
}
INITLUAFUNC(getPrivKey) {
  struct x25519_keysPair *in = (struct x25519_keysPair *)lua_touserdata(L, 1);
  if (in == NULL)
    luaL_error(L, "x25519_keysPair broken");
  uint8_t retStr[X25519_LENKEY + 1];
  retStr[X25519_LENKEY] = 0;
  x25519_getRawPrivKey(in->privKey, retStr);
  lua_pushlstring(L, retStr, X25519_LENKEY);
  return 1;
}

int lua_getSharedKey(lua_State *L) {
  size_t skeylen;
  struct x25519_keysPair *in = (struct x25519_keysPair *)lua_touserdata(L, 1);
  uint8_t *pub = (uint8_t *)luaL_checkstring(L, 2);
  if (in == NULL)
    luaL_error(L, "x25519_keysPair broken");
  unsigned char *shared0 =
      (unsigned char *)x25519_getSharedKey(in, pub, &skeylen);
  //  printf("getShared0: %s size key:%d\n", shared0, skeylen	);
  //  lua_pop(L,2);
  lua_pushlstring(L, shared0, skeylen);
  lua_pushnumber(L, skeylen);
  free(shared0);
  return 2;
}
