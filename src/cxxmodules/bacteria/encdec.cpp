#ifndef __clang__
module;
#endif
extern "C" {
//#include "lua/luaserv.h"
//#include <lauxlib.h>
//#include <lua.h>
//#include <lualib.h>
//#include"cryptocoins.h"
//#include"json_rpc.h"
#include"encdec/AES.h"
//#include"encdec/base64.h"
#include"encdec/hashes.h"
//#include"encdec/rsa_ed25519.h"
#include"encdec/x25519.h"
};
#include<string>
#include<memory>

export module encdec;
constexpr const auto x25519_lenKey = 32;

export namespace bacteria{
namespace encdec{
		typedef std::basic_string<unsigned char> ustring;

		using ubytes = unsigned char;
		enum encryptor_type{
			CBC=t_cbc, ECB=t_ecb, ChaCha20=t_chacha20
		};
		std::unique_ptr<ubytes> getRandBytes(size_t len = 32);
		namespace hashes{
		std::string SHA512(std::string &);
		std::string SHA256(std::string &);
		std::string SHA512(const char *);
		std::string SHA256(const char*);
		};
		class sync{
			private:
				encryptor_type mType;
				ustring mKey,mIV;
			public:
				explicit sync(ustring key, ustring IV,encryptor_type isType);
				ustring encrypt(ustring & data);
				ustring decrypt(ustring & data);
				ustring encrypt(const char *);
				ustring decrypt(const char *);				
				void setkey(ustring key);
				void setiv(ustring iv);
				ustring & getKey(void);
				ustring & getIV(void);
		};
		class x25519{

			using RawKeyT = std::unique_ptr<uint8_t>;
			private:
				struct x25519_keysPair mKeysPair;
			public:
			explicit x25519(const uint8_t * priv, const uint8_t * pub) noexcept;
			x25519(std::string filepath);
			explicit x25519(void) noexcept;
			~x25519(void) noexcept ;
			RawKeyT getRawPubKey(void);
			RawKeyT getRawPrivKey(void);
			RawKeyT getSharedKey(const uint8_t * key);
			void saveKeyPair(std::string & filepath);
			void saveKeyPair(const char * filepath);
			
		};
};
}

using namespace bacteria;
using namespace encdec;
using bacteria::encdec::ubytes;
using bacteria::encdec::ustring;
/*hashes*/
std::string SHA512(std::string &data){
	  char outputstring[SHA512_OUTPUTSTRING_SIZE];
	  toSHA512(reinterpret_cast<uint8_t*>(const_cast<char*>(data.c_str())), outputstring);
	  return std::string{outputstring};
}
std::string SHA256(std::string &data){
	  char outputstring[SHA256_OUTPUTSTRING_SIZE];
	  toSHA256(reinterpret_cast<uint8_t*>(const_cast<char*>(data.c_str())), outputstring);
	  return std::string{outputstring};

}
std::string SHA512(const char *data){
	std::string t{data};
	return SHA512(t);
}
std::string SHA256(const char*data){
	std::string t{data};
	return SHA256(t);
}

/*x25519*/
x25519::x25519(const uint8_t * priv, const uint8_t *pub) noexcept 
:mKeysPair{x25519_createKeyPair(priv,pub)}
{}
x25519::x25519(std::string filepath)
:mKeysPair{x25519_initKeyPairFromFile(filepath.c_str())}
{
	if( mKeysPair.privKey == nullptr || mKeysPair.pKeyCtx == nullptr ) 
		throw std::runtime_error("Cant init x25519 keys from file: "+filepath);
}
x25519::x25519(void) noexcept
: mKeysPair{ x25519_generateKeyPair() }
{}
x25519::~x25519(void) noexcept{
	x25519_freeKeyPair(&mKeysPair);
}

x25519::RawKeyT x25519::getRawPubKey(void){
	x25519::RawKeyT rawKey{ new uint8_t(x25519_lenKey+1) };
	x25519_getRawPubKey(mKeysPair.privKey, rawKey.get());
	return rawKey;
}
x25519::RawKeyT x25519::getRawPrivKey(void){
	x25519::RawKeyT rawKey{ new uint8_t(x25519_lenKey+1) };
	x25519_getRawPrivKey(mKeysPair.privKey, rawKey.get());
	return rawKey;
}
x25519::RawKeyT x25519::getSharedKey(const uint8_t * key){
	size_t keyLine=x25519_lenKey;
	auto pKey = x25519_getSharedKey( &mKeysPair, key, &keyLine );
	return x25519::RawKeyT{ pKey };
}

void x25519::saveKeyPair(std::string & filepath){
	x25519_savePrivKey(filepath.c_str(), &mKeysPair);
}
void x25519::saveKeyPair(const char * filepath){
	std::string f{filepath};
	saveKeyPair(f);
}

/*encdec*/
std::unique_ptr<ubytes> encdec::getRandBytes(size_t len){
	  ubytes * pBytes = new ubytes[len];
	  auto rt = std::unique_ptr<ubytes>{pBytes};
	  generate_rand_bytes(len, rt.get());
	  pBytes[len] = '0';
	  return rt;
}

encdec::sync::sync(ustring key, ustring IV,encryptor_type isType):mType(isType),mKey(key),mIV(IV){}
void encdec::sync::setkey(ustring key){
	mKey = key;
}
void encdec::sync::setiv(ustring iv){
	mIV = iv;
}
ustring & encdec::sync::getKey(void){
	return mKey;
}
ustring & encdec::sync::getIV(void){
	return mIV;
}
ustring encdec::sync::encrypt(ustring &data){
	std::unique_ptr<ubytes> pTmpData;
	size_t chipher_len;
	unsigned char * ptrData = const_cast<ubytes*>(data.c_str());
	unsigned char * ptrKey = const_cast<ubytes*>(mKey.c_str());
	unsigned char * ptrIV = const_cast<ubytes*>(mIV.c_str());
	auto setTmpData=[&pTmpData](size_t len, size_t adv=17){
			pTmpData = std::unique_ptr<ubytes>(new ubytes(len + adv));
	};
	switch(mType){
		case encryptor_type::CBC:
			setTmpData(data.size());
		 	chipher_len = AES_256_cbc_encrypt(ptrData, data.size(), ptrKey, ptrIV, pTmpData.get());    

		case encryptor_type::ECB:
			setTmpData(data.size());
		 	chipher_len = AES_256_ecb_encrypt(ptrData, data.size(), ptrKey, ptrIV, pTmpData.get());    
		case encryptor_type::ChaCha20:
			setTmpData(data.size(),1);
		 	chipher_len = chacha20_poly1305_encrypt(ptrData, data.size(), ptrKey, ptrIV, pTmpData.get());    
		default:
			throw std::runtime_error("Undefined encryptor_type");
		break;
	}
	if(chipher_len<=0) throw std::runtime_error("Can't encrypt data");
	pTmpData.get()[chipher_len]=0;
	return {pTmpData.get()};
}

ustring encdec::sync::decrypt(ustring &data){
	std::unique_ptr<ubytes> pTmpData;
	size_t decrypt_len;
	unsigned char * ptrData = const_cast<ubytes*>(data.c_str());
	unsigned char * ptrKey = const_cast<ubytes*>(mKey.c_str());
	unsigned char * ptrIV = const_cast<ubytes*>(mIV.c_str());
	auto setTmpData=[&pTmpData](size_t len, size_t adv=17){
			pTmpData = std::unique_ptr<ubytes>(new ubytes(len + adv));
	};
	switch(mType){
		case encryptor_type::CBC:
			setTmpData(data.size());
		 	decrypt_len = AES_256_cbc_decrypt(pTmpData.get(), data.size(), ptrKey, ptrIV, ptrData);    
		case encryptor_type::ECB:
			setTmpData(data.size());
		 	decrypt_len = AES_256_cbc_decrypt(pTmpData.get(), data.size(), ptrKey, ptrIV, ptrData);    
		case encryptor_type::ChaCha20:
			setTmpData(data.size(),1);
		 	decrypt_len = AES_256_cbc_decrypt(pTmpData.get(), data.size(), ptrKey, ptrIV, ptrData);    
		default:
			throw std::runtime_error("Undefined encryptor_type");
		break;
	}
	if(decrypt_len<=0) throw std::runtime_error("Can't decrypt data");
	pTmpData.get()[decrypt_len]=0;
	return {pTmpData.get()};
}
ustring encdec::sync::encrypt(const char * data){
	ustring rt{(unsigned char*)(data)};
	return encdec::sync::encrypt(rt);
}
ustring encdec::sync::decrypt(const char * data){
	ustring rt{(unsigned char*)(data)};
	return encdec::sync::decrypt(rt);
}



