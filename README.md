```                                                                                
                                                                                
                                     @###@                                      
                                    ####@##@                                    
                         .@@         @###@                                      
                       @######     .///////@     *#####@                        
                       @######////////%.,.///////###@###@                       
                         @@@@//@,,,///(@@,///@,*//&@#@@                         
                              //@&,,,,,,@,,,,&(@/@                              
                              /////,,,,@&,,,,(///@                              
                        &&##@//@./@,,,,@@@,,,(,.@/@&#@.                         
                       ####&##@/////////,%//////@###%###                        
                       @######     @///@@//&    ,######@                        
                                     %////.                                     
                                    @#%#@#%                                     
                                    @######                                     
                                                                                
```                                                                                
(Logo by 'MrB' can be broken in some html previewer, better to open as just file)[logo.png]
(thanks for logo by some guy whose dont want to say himself name)
--------------------------------------------------
!https://travis-ci.com/wipedlifepotato/bacteria.svg?branch=master!:https://travis-ci.com/wipedlifepotato/bacteria
# ¤Bacter¤ 
## Features(not only whose in the list):
*	- cryptocoins JSON-RPC support in C and Lua {in libbacteria} {{libcrypto+libjson}}
*	- ...
*	- async/sync crypto support in C and Lua (X25519+AES[ECB/CBC]+ChaCha20) {{in libbacteria} {libencdec}}
*	- ...
*	- libgd for images. gd functions in C and Lua {in libbacteria} {{libimages}}
*	- libgmp for Lua.(for mpz and mpf, primitives) {{libbacteria}} {{lgmp}}
*	- ed25519rsa for Lua (sign/verify RSA+ed25519 support) {{libbacteria}} {{ed25519rsa}} 
*	- ed25519 + RSA + base64 preinit functional for C (sign/verify) {{libbacteria}} 
*	- ....
*	- (lua server for debug and server `console` in future)
## TODO(for now):
*	- pre-init postgres database and C code for works with it, and lua module for work with it
*	- own network protocol idea.
*	- create server
*	- create lua scripts for events.
*	- continue work with cryptocoins RPC wallets in server
*	- create exchange (cryptocoin)crypto on crypto system by 2 client.
*	- create ....(have to be changed every 8 tasks)[like in stack of 8 tasks]
	
# Depencies -
## 	Debian 10:
* -		sudo apt install lua-sql-postgres-dev automake clang-7 libjansson-dev libcurl4-gnutls-dev libiniparser-dev libssl-dev libgd-dev libpng-dev libz3-dev libfreetype6-dev libluajit-5.1-dev liblua5.1-dev libgmp-dev libgtk-3-dev nlohmann-json3-dev
## 	Ubuntu(focal):
* -		sudo apt-get install -y devscripts build-essential lintian dh-make autoconf lua-sql-postgres-dev automake clang libjansson-dev libcurl4-gnutls-dev libiniparser-dev libssl-dev libgd-dev libpng-dev libz3-dev libfreetype6-dev liblua5.1-dev libluajit-5.1-dev libgmp-dev libgtk-3-dev nlohmann-json3-dev
	

# Compiling:
	```
		autoreconf - i
		./configure CC="clang" CXX="clang++"
		make
	```

# Documentation
- for now not exists. look to https://github.com/wipedlifepotato/Bacteria-Framework/blob/master/src/luasubmodules/init.lua#L1
there all things that works on Lua. its part of libbacteria. There examples. 

