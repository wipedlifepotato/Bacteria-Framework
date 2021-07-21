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
//#include"encdec/AES.h"
#include"encdec/base64.h"
//#include"encdec/hashes.h"
#include"encdec/rsa_ed25519.h"
//#include"encdec/x25519.h"
};
#include<string>
#include<memory>

export module rsa_ed25519;
export namespace bacteria{
	using std::string;
	namespace rsa_ed25519{
		typedef std::basic_string<unsigned char> ustring;
		constexpr size_t len_key = 32;
		using keypair_t = std::pair<ustring, size_t>;
		using signedDataT = keypair_t;
		auto initSigPtr = [](size_t keySize){
			return std::unique_ptr<uint8_t>{new uint8_t(keySize)};
		};

	};
	using namespace rsa_ed25519;
	class basic_verify{
		private:
		protected:
			void freeMKey(void){
 			  if(mKeyPair.pkey != nullptr) EVP_PKEY_free(mKeyPair.pkey);
			  if(mKeyPair.privKey != nullptr) free(mKeyPair.privKey);
			  if(mKeyPair.pubKey != nullptr) free(mKeyPair.pubKey);
			}
		protected:
			struct aKeyPair mKeyPair;
			aTypes mType;
		public:
			void generateKeys(std::string filepath="", unsigned int bits=8192, unsigned int primes=4){
				freeMKey();
				FILE * exitFile = nullptr;
				if(filepath != ""){
					exitFile = fopen(filepath.c_str(), "wb");
					if(exitFile == nullptr) throw std::runtime_error("Can't save rsa/ed25519 key to: "+filepath);
				}
				if(mType == ed25519){
					mKeyPair = generateKeysEd25519(exitFile);
				}
				else{
					mKeyPair = generateKeysRSA(bits,primes,exitFile);
				}
				if(exitFile != nullptr) fclose(exitFile);
			}
			basic_verify(aTypes type,std::string filepath="", unsigned int bits=8192, unsigned int primes=4){
				mType = type;
				generateKeys(filepath, bits, primes);
			}
			basic_verify & operator=(basic_verify && b ){
				freeMKey();
				this->mKeyPair = b.mKeyPair;
				this->mType = b.mType;
				return *this;
			}
			~basic_verify(void){
			 freeMKey();
			}
			virtual bool verify(ustring data, ustring pubkey, size_t keySize = len_key, const EVP_MD * mdalgo = EVP_sha512()){
		      		auto signature = initSigPtr(keySize+1);
		      		auto rt= 
					ed25519rsa_verifyIt(signature.get(), keySize, const_cast<uint8_t*>(data.c_str()), data.size(),
	                		const_cast<uint8_t*>(pubkey.c_str()), pubkey.size(), mdalgo,mType);
				if( rt > 0 ) return true;
				return false;
			}
			virtual keypair_t sign(string data, size_t retSize = 65, const EVP_MD * mdalgo = EVP_sha512()){ // sha512 512/8 = 64
				auto ret = initSigPtr(retSize);
//				size_t ed25519rsa_signIt(const char *plaintext, size_t plaintext_size, uint8_t *sigret,
//			              EVP_PKEY *key,const EVP_MD * mdalgo,enum aTypes type);
				size_t keySize = ed25519rsa_signIt(data.c_str(), data.size(), ret.get(),
					mKeyPair.pkey, mdalgo, mType);
				return { ret.get(), keySize };

			}
			signedDataT getPubKey(void){
				return {reinterpret_cast<unsigned char*>(mKeyPair.pubKey), mKeyPair.pubKeyLen };
			}
			signedDataT getPrivKey(void){
				return {reinterpret_cast<unsigned char*>(mKeyPair.privKey), mKeyPair.privKeyLen };
			}		
	};
	
	class rsa : public basic_verify{
		rsa(std::string filepath="", unsigned int bits=8192, unsigned int primes=4):
		basic_verify(aTypes::aRSA,filepath,bits,primes)
		{}
	};
	class ed25519 : public basic_verify{
		ed25519(std::string filepath=""):
		basic_verify(aTypes::ed25519,filepath,0,0)
		{}

	};

};



