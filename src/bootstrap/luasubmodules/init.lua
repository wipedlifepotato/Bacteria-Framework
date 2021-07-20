print("bootstrap server inited")
--int sock, const char * uIp, uint16_t uPort, char* buf
function event1(sock, ip, port, buf)
	print("Message: ", buf, " from ", ip, " on port: ", port)
	keys = ed25519rsa.generateKeysRSA(nil, 2048, 3)
	pubkey = ed25519rsa.getaPubKey(keys)
	lnet.send(sock,"Test?")
	while 1 == 1 do
		socklen, rbytes, msg = lnet.recv(sock,1024)
		if string.len(msg) > 1 then
			break
		end
	end
	print("Replay: ", msg)
	return pubkey
end
