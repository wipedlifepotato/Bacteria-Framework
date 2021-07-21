#ifndef __clang__
module;
#endif
extern "C" {
	#include"cryptocoins.h"
};

#include<memory>
#include<string>
#include<map>
// not thread safe.
export module cryptocurrency;
export namespace bacteria{
/*
struct cryptocoin{
	bool testnet;
	char * rpcuser;
	char * rpcpassword;
	uint16_t rpcport;
	char * rpchost;
	char * cryptocoin_name;
};
*/
	class cryptocoins{
		using mapOfKeys = std::map<std::string, struct cryptocoin*>;
		protected:
			struct cryptocoin * mCryptocoins;
			unsigned int mCount;
		public:	
			cryptocoins(std::string);
			~cryptocoins(void);
			struct cryptocoin & searchByName(std::string n){
 				for (unsigned char i = 0;
				mCryptocoins[i].rpchost != NULL && mCryptocoins[i].rpcport != 0 &&
				i < mCount;
				i++){ 
				  if( strcmp(mCryptocoins[i].cryptocoin_name, n.c_str()) == 0) return mCryptocoins[i];
				}
				throw std::runtime_error("Not found cryptocoin: " + n);
				
			}
			struct cryptocoin & operator[](std::string n){
				 return searchByName(n);
			}
			mapOfKeys getMap(void){
				mapOfKeys rt;
				for (unsigned char i = 0;
				mCryptocoins[i].rpchost != NULL && mCryptocoins[i].rpcport != 0 &&
				i < mCount;
				i++){ 
					rt[mCryptocoins[i].cryptocoin_name] = &mCryptocoins[i];
				}
				return rt;
			}
	};
}

using namespace bacteria;
cryptocoins::cryptocoins(std::string initFile){
	mCryptocoins = init_new_cryptocoins(initFile.c_str(), &mCount);
}
cryptocoins::~cryptocoins(void){
	uint tmp = getCountCryptocoins();
	setCountCryptocoins(mCount);
	clear_cryptocoins(mCryptocoins);
	setCountCryptocoins(tmp);
}

