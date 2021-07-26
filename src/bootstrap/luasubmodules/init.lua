print("bootstrap server inited")
--int sock, const char * uIp, uint16_t uPort, char* buf
local opcodes={}
opcodes["test"] = function (sock,ip,port,buf) 
	print("Test opcode") 
	return "Test opcode"
end 
function undefined_event(sock, ip, port, buf, opcode)
	print("Opcode: ", opcode)
        local ropcode = string.sub(opcode, 1, 4)

	if  opcodes[ropcode] == nil then
		return nil
	end

	return opcodes[ropcode](sock,ip,port,buf)
--	print( opcodes["test"] )
--	print("Opcode: ", ropcode, " Not found")
--	return nil
end
function event1(sock, ip, port, buf)
	print("Message: ", buf, " from ", ip, " on port: ", port)
	keys = ed25519rsa.generateKeysRSA(nil, 2048, 3)
	pubkey = ed25519rsa.getaPubKey(keys)
	lnet.send(sock,"Test?")
	while 1 == 1 do
		socklen, rbytes, msg = lnet.recv(sock,1024)
		if msg == nil then break end
		if string.len(msg) > 1 then
			break
		end
	end
	print("Replay: ", msg)
	return pubkey
end
