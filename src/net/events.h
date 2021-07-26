#pragma once
#ifndef EVENTS_H
#define EVENTS_H
#include"lua/lua.h"
#include"net.h"
#include<stdint.h>
#include<array>

namespace events{
union sock_data{
	private:
		bool mIsUDP;
	public:
	bool isUDP(void){ return mIsUDP; }
	int mSock;
	std::pair<int, struct sockaddr_in> mPair;
	sock_data(int & sock):mSock(sock){ mIsUDP = false;}
	sock_data(int & sock, struct sockaddr_in & sockaddr) : mPair{sock, sockaddr} { mIsUDP = true ;}
};
//is idea to disable lua for opcodes as wrapper if is need.
#define EVENTDEF lua_State * L, int sock, const char * uIp, uint16_t uPort, char* buf
	using fun =  void(*)(EVENTDEF, ...);

	class event{
			protected:
				fun m_fun;
			public:
				event(fun f): m_fun(f)
				{}
template<typename ...args>
void run(EVENTDEF, args ... values){ 
	m_fun(L, sock, (const char*)uIp, uPort, buf, values...); 
};

	};

};
#endif
