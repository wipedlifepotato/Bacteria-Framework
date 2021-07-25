#include "encdec.h"
using namespace bacteria;
using namespace encdec;
using bacteria::encdec::ubytes;
using bacteria::encdec::ustring;
/*hashes*/
std::string SHA512(std::string &data) {
  char outputstring[SHA512_OUTPUTSTRING_SIZE];
  toSHA512(reinterpret_cast<uint8_t *>(const_cast<char *>(data.c_str())),
           outputstring);
  return std::string{outputstring};
}
std::string SHA256(std::string &data) {
  char outputstring[SHA256_OUTPUTSTRING_SIZE];
  toSHA256(reinterpret_cast<uint8_t *>(const_cast<char *>(data.c_str())),
           outputstring);
  return std::string{outputstring};
}
std::string SHA512(const char *data) {
  std::string t{data};
  return SHA512(t);
}
std::string SHA256(const char *data) {
  std::string t{data};
  return SHA256(t);
}

/*x25519*/
x25519::x25519(const uint8_t *priv, const uint8_t *pub) noexcept
    : mKeysPair{x25519_createKeyPair(priv, pub)} {}
x25519::x25519(std::string filepath)
    : mKeysPair{x25519_initKeyPairFromFile(filepath.c_str())} {
  if (mKeysPair.privKey == nullptr || mKeysPair.pKeyCtx == nullptr)
    throw std::runtime_error("Cant init x25519 keys from file: " + filepath);
}
x25519::x25519(void) noexcept : mKeysPair{x25519_generateKeyPair()} {}
x25519::~x25519(void) noexcept { x25519_freeKeyPair(&mKeysPair); }

x25519::RawKeyT x25519::getRawPubKey(void) {
  x25519::RawKeyT rawKey{new uint8_t(x25519_lenKey + 1)};
  x25519_getRawPubKey(mKeysPair.privKey, rawKey.get());
  return rawKey;
}
x25519::RawKeyT x25519::getRawPrivKey(void) {
  x25519::RawKeyT rawKey{new uint8_t(x25519_lenKey + 1)};
  x25519_getRawPrivKey(mKeysPair.privKey, rawKey.get());
  return rawKey;
}
x25519::RawKeyT x25519::getSharedKey(const uint8_t *key) {
  size_t keyLine = x25519_lenKey;
  auto pKey = x25519_getSharedKey(&mKeysPair, key, &keyLine);
  return x25519::RawKeyT{pKey};
}

void x25519::saveKeyPair(std::string &filepath) {
  x25519_savePrivKey(filepath.c_str(), &mKeysPair);
}
void x25519::saveKeyPair(const char *filepath) {
  std::string f{filepath};
  saveKeyPair(f);
}

/*encdec*/
std::unique_ptr<ubytes> encdec::getRandBytes(size_t len) {
  ubytes *pBytes = new ubytes[len];
  auto rt = std::unique_ptr<ubytes>{pBytes};
  generate_rand_bytes(len, rt.get());
  pBytes[len] = '0';
  return rt;
}

encdec::sync::sync(ustring key, ustring IV, encryptor_type isType)
    : mType(isType), mKey(key), mIV(IV) {}
void encdec::sync::setkey(ustring key) { mKey = key; }
void encdec::sync::setiv(ustring iv) { mIV = iv; }
ustring &encdec::sync::getKey(void) { return mKey; }
ustring &encdec::sync::getIV(void) { return mIV; }
ustring encdec::sync::encrypt(ustring &data) {
  std::unique_ptr<ubytes> pTmpData;
  size_t chipher_len;
  unsigned char *ptrData = const_cast<ubytes *>(data.c_str());
  unsigned char *ptrKey = const_cast<ubytes *>(mKey.c_str());
  unsigned char *ptrIV = const_cast<ubytes *>(mIV.c_str());
  auto setTmpData = [&pTmpData](size_t len, size_t adv = 17) {
    pTmpData = std::unique_ptr<ubytes>(new ubytes(len + adv));
  };
  switch (mType) {
  case encryptor_type::CBC:
    setTmpData(data.size());
    chipher_len = AES_256_cbc_encrypt(ptrData, data.size(), ptrKey, ptrIV,
                                      pTmpData.get());

  case encryptor_type::ECB:
    setTmpData(data.size());
    chipher_len = AES_256_ecb_encrypt(ptrData, data.size(), ptrKey, ptrIV,
                                      pTmpData.get());
  case encryptor_type::ChaCha20:
    setTmpData(data.size(), 1);
    chipher_len = chacha20_poly1305_encrypt(ptrData, data.size(), ptrKey, ptrIV,
                                            pTmpData.get());
  default:
    throw std::runtime_error("Undefined encryptor_type");
    break;
  }
  if (chipher_len <= 0)
    throw std::runtime_error("Can't encrypt data");
  pTmpData.get()[chipher_len] = 0;
  return {pTmpData.get()};
}

ustring encdec::sync::decrypt(ustring &data) {
  std::unique_ptr<ubytes> pTmpData;
  size_t decrypt_len;
  unsigned char *ptrData = const_cast<ubytes *>(data.c_str());
  unsigned char *ptrKey = const_cast<ubytes *>(mKey.c_str());
  unsigned char *ptrIV = const_cast<ubytes *>(mIV.c_str());
  auto setTmpData = [&pTmpData](size_t len, size_t adv = 17) {
    pTmpData = std::unique_ptr<ubytes>(new ubytes(len + adv));
  };
  switch (mType) {
  case encryptor_type::CBC:
    setTmpData(data.size());
    decrypt_len = AES_256_cbc_decrypt(pTmpData.get(), data.size(), ptrKey,
                                      ptrIV, ptrData);
  case encryptor_type::ECB:
    setTmpData(data.size());
    decrypt_len = AES_256_cbc_decrypt(pTmpData.get(), data.size(), ptrKey,
                                      ptrIV, ptrData);
  case encryptor_type::ChaCha20:
    setTmpData(data.size(), 1);
    decrypt_len = AES_256_cbc_decrypt(pTmpData.get(), data.size(), ptrKey,
                                      ptrIV, ptrData);
  default:
    throw std::runtime_error("Undefined encryptor_type");
    break;
  }
  if (decrypt_len <= 0)
    throw std::runtime_error("Can't decrypt data");
  pTmpData.get()[decrypt_len] = 0;
  return {pTmpData.get()};
}
ustring encdec::sync::encrypt(const char *data) {
  ustring rt{(unsigned char *)(data)};
  return encdec::sync::encrypt(rt);
}
ustring encdec::sync::decrypt(const char *data) {
  ustring rt{(unsigned char *)(data)};
  return encdec::sync::decrypt(rt);
}
