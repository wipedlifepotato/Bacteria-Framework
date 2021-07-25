#include "ed25519rsa.h"
namespace bacteria {
using namespace rsa_ed25519;
void basic_verify::freeMKey(void) {
  if (mKeyPair.pkey != nullptr)
    EVP_PKEY_free(mKeyPair.pkey);
  if (mKeyPair.privKey != nullptr)
    free(mKeyPair.privKey);
  if (mKeyPair.pubKey != nullptr)
    free(mKeyPair.pubKey);
} //
void basic_verify::generateKeys(std::string filepath, unsigned int bits,
                                unsigned int primes) {
  freeMKey();
  FILE *exitFile = nullptr;
  if (filepath != "") {
    exitFile = fopen(filepath.c_str(), "wb");
    if (exitFile == nullptr)
      throw std::runtime_error("Can't save rsa/ed25519 key to: " + filepath);
  }
  if (mType == aTypes::ed25519) {
    mKeyPair = generateKeysEd25519(exitFile);
  } else {
    mKeyPair = generateKeysRSA(bits, primes, exitFile);
  }
  if (exitFile != nullptr)
    fclose(exitFile);
} //

basic_verify::basic_verify(aTypes type, std::string filepath, unsigned int bits,
                           unsigned int primes) {
  mType = type;
  if (access(filepath.c_str(), F_OK) == 0) {
    mKeyPair = ed25519rsa_initPrivKey(filepath.c_str(), mType);
    if (mKeyPair.pkey == NULL)
      throw std::runtime_error("cant parse key from file: " + filepath);
  } else
    generateKeys(filepath, bits, primes);
}

basic_verify &basic_verify::operator=(basic_verify &&b) {
  freeMKey();
  this->mKeyPair = b.mKeyPair;
  this->mType = b.mType;
  return *this;
}
basic_verify::~basic_verify(void) { freeMKey(); }

bool basic_verify::verify(ustring data, ustring pubkey, size_t keySize,
                          const EVP_MD *mdalgo) {
  auto signature = initSigPtr(keySize + 1);
  auto rt = ed25519rsa_verifyIt(
      signature.get(), keySize, const_cast<uint8_t *>(data.c_str()),
      data.size(), const_cast<uint8_t *>(pubkey.c_str()), pubkey.size(), mdalgo,
      mType);
  if (rt > 0)
    return true;
  return false;
}
keypair_t basic_verify::sign(string data, size_t retSize,
                             const EVP_MD *mdalgo) { // sha512 512/8 = 64
  auto ret = initSigPtr(retSize);
  //				size_t ed25519rsa_signIt(const char
  //*plaintext, size_t plaintext_size, uint8_t *sigret,
  // EVP_PKEY *key,const EVP_MD * mdalgo,enum aTypes type);
  size_t keySize = ed25519rsa_signIt(data.c_str(), data.size(), ret.get(),
                                     mKeyPair.pkey, mdalgo, mType);
  return {ret.get(), keySize};
}
signedDataT basic_verify::getPubKey(void) {
  return {reinterpret_cast<unsigned char *>(mKeyPair.pubKey),
          mKeyPair.pubKeyLen};
}
signedDataT basic_verify::getPrivKey(void) {
  return {reinterpret_cast<unsigned char *>(mKeyPair.privKey),
          mKeyPair.privKeyLen};
}
/*
RSA
*/
rsa::rsa(std::string filepath, unsigned int bits, unsigned int primes)
    : basic_verify(aTypes::aRSA, filepath, bits, primes) {}
/*
ED25519
*/

ed25519::ed25519(std::string filepath)
    : basic_verify(aTypes::ed25519, filepath, 0, 0) {}

}; // namespace bacteria
