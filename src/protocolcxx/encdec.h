/*#ifndef __clang__
module;
#endif*/

#include "encdec/AES.h"
#include "encdec/hashes.h"
#include "encdec/x25519.h"

#include <memory>
#include <string>
#include <stdexcept>

//export module encdec;
constexpr const auto x25519_lenKey = 32;

/*export*/
namespace bacteria {
  namespace encdec {
  typedef std::basic_string<unsigned char> ustring;

  using ubytes = unsigned char;
  enum encryptor_type { CBC = t_cbc, ECB = t_ecb, ChaCha20 = t_chacha20 };
  std::unique_ptr<ubytes> getRandBytes(size_t len = 32);
  namespace hashes {
  std::string SHA512(std::string &);
  std::string SHA256(std::string &);
  std::string SHA512(const char *);
  std::string SHA256(const char *);
  }; // namespace hashes
  class sync {
  private:
    encryptor_type mType;
    ustring mKey, mIV;

  public:
    explicit sync(ustring key, ustring IV, encryptor_type isType);
    ustring encrypt(ustring &data);
    ustring decrypt(ustring &data);
    ustring encrypt(const char *);
    ustring decrypt(const char *);
    void setkey(ustring key);
    void setiv(ustring iv);
    ustring &getKey(void);
    ustring &getIV(void);
  };
  class x25519 {

    using RawKeyT = std::unique_ptr<uint8_t>;

  private:
    struct x25519_keysPair mKeysPair;

  public:
    explicit x25519(const uint8_t *priv, const uint8_t *pub) noexcept;
    x25519(std::string filepath);
    explicit x25519(void) noexcept;
    ~x25519(void) noexcept;
    RawKeyT getRawPubKey(void);
    RawKeyT getRawPrivKey(void);
    RawKeyT getSharedKey(const uint8_t *key);
    void saveKeyPair(std::string &filepath);
    void saveKeyPair(const char *filepath);
  };
  }; // namespace encdec
}

