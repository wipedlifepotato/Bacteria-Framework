/*#ifndef __clang__
module;
#endif*/
#include "cryptocoins.h"
#include "json_rpc.h"

#include <nlohmann/json.hpp>
#include <map>
#include <memory>
#include <string>
#include<stdexcept>

//export module cryptocurrency;

using namespace nlohmann;

namespace bacteria {
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
  namespace bjson {
		  json req(std::string userpwd, std::string url, json &data);
  } // namespace bjson
  class cryptocoins {
    using mapOfKeys = std::map<std::string, struct cryptocoin *>;

  protected:
    struct cryptocoin *mCryptocoins;
    unsigned int mCount;

  public:
    cryptocoins(std::string);
    ~cryptocoins(void);
    struct cryptocoin &searchByName(std::string n) ;
    
    struct cryptocoin &operator[](std::string n) ;
    mapOfKeys getMap(void);

    json req(std::string &name, json &data) ;

    json req(struct cryptocoin &a, json &data);
  };
}

using namespace bacteria;

