/*#ifndef __clang__
module;
#endif*/

#include "encdec/base64.h"
#include "encdec/rsa_ed25519.h"

#include <memory>
#include <string>

//export module rsa_ed25519;
namespace bacteria {
  using std::string;
  namespace rsa_ed25519 {
  typedef std::basic_string<unsigned char> ustring;
  constexpr size_t len_key = 32;
  using keypair_t = std::pair<ustring, size_t>;
  using signedDataT = keypair_t;
static  auto initSigPtr = [](size_t keySize) {
    return std::unique_ptr<uint8_t>{new uint8_t(keySize)};
  };

  }; // namespace rsa_ed25519
  using namespace rsa_ed25519;
  class basic_verify {
  private:
  protected:
    void freeMKey(void);

  protected:
    struct aKeyPair mKeyPair;
    aTypes mType;

  public:
    void generateKeys(std::string filepath = "", unsigned int bits = 8192,
                      unsigned int primes = 4) ;
    basic_verify(aTypes type, std::string filepath = "",
                 unsigned int bits = 8192, unsigned int primes = 4);
    basic_verify &operator=(basic_verify &&b);

    ~basic_verify(void);
    virtual bool verify(ustring data, ustring pubkey, size_t keySize = len_key,
                        const EVP_MD *mdalgo = EVP_sha512());

    virtual keypair_t
    sign(string data, size_t retSize = 65,
         const EVP_MD *mdalgo = EVP_sha512());

    signedDataT getPubKey(void);

    signedDataT getPrivKey(void) ;
};

  class rsa : public basic_verify {
    rsa(std::string filepath = "", unsigned int bits = 8192,
        unsigned int primes = 4);

  };
  class ed25519 : public basic_verify {
    ed25519(std::string filepath = "");
  };
};
