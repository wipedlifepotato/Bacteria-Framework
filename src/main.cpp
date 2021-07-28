
#include "async_serv.h"
#include "cryptocoins.h"
#include "json_rpc.h"
#include "lua/luaserv.h"
#include "signal_handler.h"

#include <thread>
#include <time.h>
#include<iostream>
#include<unistd.h>
#include<getopt.h>

// extern int start_lua(void);
// void __start(void){} #
//extern void example(void); // cryptocoins in C(deprecated, maybe)

//
static  std::string host{"127.0.0.1"};
static  uint16_t port = 3245;

static  std::string luahost{"127.0.0.1"};
static  uint16_t luaport = 6596;

static  std::string pathCryptocoinsINI{"cryptocoins.ini"};

static  std::string initLuaFile{"./luasubmodules/init.lua"};
//

static void usage(char *const program) {
  puts(PROGRAM_INFO);
  auto printoption = [&program](const char *longopt, char opt,
                                std::string desc="no description") {
    printf("--%s -%c\t=> %s\n", longopt, opt, desc.c_str());
  };
  printf("\nUSAGE: %s [options]\n", program);

  printoption("help", 'h', "help menu");
  printoption("listen host", 'l', std::string("Host for listening(def: ")+host+")");
  printoption("port", 'p', "Port for listening (def: "+std::to_string(port)+")");

  printoption("luahost", 'j', "Host of listeningluaserv (def: "+luahost+")");
  printoption("luaport", 'k', "port of listeningluaserv (def: "+std::to_string(luaport)+")");
  printoption("fcryptocoins", 'c', "path to file of cryptocoins (def: "+pathCryptocoinsINI+")");
  printoption("iluafile", 'i', "init file for lua part (def: "+initLuaFile+")");
};


int main(int argc, char **argv) {

  catch_badsignals();
  srand(time(NULL));
  char c;

  int option_index;
  static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                         {"host", required_argument, 0, 'l'},
                                         {"port", required_argument, 0, 'p'},
                                         {"luahost", required_argument, 0, 'j'},
                                         {"luaport", required_argument, 0, 'k'},
                                         {"fcryptocoins", required_argument, 0, 'c'},
                                         {"iluafile", required_argument, 0, 'i'},
                                         {0, 0, 0, 0}};
  while ((c = getopt_long(argc, argv, "hp:h:c:i:", long_options, &option_index)) !=
         -1) {
    switch (c) {
    case 0:
      if (std::string(long_options[option_index].name) ==
          std::string("usage")) {
        usage(argv[0]);
        exit(1);
      case 'j':
        luahost = optarg;
        break;
      case 'k':
        luaport = atoi(optarg);
        break;
      case 'l':
        host = optarg;
        break;
      case 'p':
        port = atoi(optarg);
        break;
      case 'c':
	pathCryptocoinsINI=optarg;
	break;
      case 'i':
	initLuaFile=optarg;
	break;
      case '?':
        std::cerr << "Undefined argument" << std::endl;
      default:
        usage(argv[0]);
        exit(1);
        break;
      }
    }
  }

//  if (argc != 3)
//    return eprintf("%s host port\n", argv[0]);

  struct cryptocoin *cryptocoins = init_cryptocoins(pathCryptocoinsINI.c_str());
  // dump_cryptocoins(cryptocoins);
  // example();

  /*
          server part
  */
  lua_State *L = start_lua(initLuaFile.c_str());
  puts("start server");
  pthread_t pthreadServ, pthreadLuaServ;
  struct serv_arguments args = {host.c_str(), port,
                                cryptocoins, L};
  auto mainserv_fun = serv_thread;
  auto luaserv_fun = luaServer;
  //  void *(*mainserv_fun)(void *) =serv_thread;
  //  void *(*luaserv_fun)(void *) = luaServer;
  std::thread threadServ(mainserv_fun, &args);


  puts("start lua server");
  // for (int i = 0; i < 10; i++)
  //  checkRetVal(L, i);

  servArgs args_luaserv = {const_cast<char*>(luahost.c_str()), luaport, L};

  std::thread threadLuaServ(luaserv_fun, &args_luaserv);

  threadLuaServ.join();
  threadServ.join();
  // pthread_join(pthreadLuaServ, NULL);
  // pthread_join(pthreadServ, NULL);

  clear_cryptocoins(cryptocoins);
  lua_close(L);
}
/*
void example(void) {
  struct bitcoin_rpc_data bdata = {"listaccounts", brpc_prepare_params(NULL)};
  brpc_get_json(&bdata);
  const char *account_name = "ANCMS_Abj1pcMsse";
  struct cryptocoin c = {
      false, "gostcoinrpc", "97WDPgQADfazR6pQRdMEjQeDeCSzTwVaMEZU1dGaTmLo",
      19376, "127.0.0.1",   NULL};
  json_t *data = brpc_json_request(&c, &bdata);
  json_t *result = json_object_get(data, "result");

  if (!json_is_object(result)) {
    puts("Result not found");
    json_decref(data);
    //    	exit(1);
  } else {
    json_t *ancms_account = json_object_get(result, account_name);
    float balance = json_number_value(ancms_account);
    printf("Account balance of %s: %f\n", account_name, balance);
  }
  json_decref(data);
}
*/
