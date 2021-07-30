baes = {}

function string.fromhex(str)
    return (str:gsub('..', function (cc)
        return string.char(tonumber(cc, 16))
    end))
end

function string.tohex(str)
    return (str:gsub('.', function (c)
        return string.format('%02X', string.byte(c))
    end))
end

function baes:newKeyPair(keyPair) -- x25519
	local obj = {}
		obj.keyPair=keyPair or {}
	function obj:initKeyPair(pub,priv)
	 	--print("init")
		if not obj.keyPair == {} then return false end 
		keyPair = {}
		p = priv or nil
		pb = pub or nil
		if p == nil or pb == nil then
		--	print("geKeyPair()")
			keyPair = encdec.getKeyPair();
		else
		--	print("p != {} and pb != {}")
		--	print("Pub: ", pub, "priv: ",priv)
			keyPair = encdec.createKeyPair(pub,priv)
		end 
		obj.keyPair = keyPair
		return true;
	end
	function obj:getKeyPair()
		if obj.keyPair == {} then return nil end 
		return obj.keyPair
	end
	function obj:getPubKey()
		if obj.keyPair == nil then
			 print("PubKey is nil")
			 return nil 
		end 
		--print("ret pub key",encdec.getPubKey(obj.keyPair))
		return encdec.getPubKey(obj.keyPair)
	end
	function obj:getPrivKey()
		if obj.keyPair == nil then
			 print("PubKey is nil")
			 return nil 
		end 
		--print("ret pub key",encdec.getPubKey(obj.keyPair))
		return encdec.getPrivKey(obj.keyPair)
	end
	function obj:getSharedKey(key)
		--print("getSharedKey and pub is: ", key) 
		if obj.keyPair == {} then return nil end
		if key == nil then error("PubKey not will be nil") end
		return encdec.getSharedKey(obj.keyPair, key)
	end
	function obj:clear()
		if not obj.keyPair == nil then
			encdec.freeKeyPair(obj.keyPair)
			obj.keyPair = nil
		end
	end
        function obj:initKeyPairFromFile(filepath)
		tmp = encdec.initKeyPairFromFile(filepath)
		if tmp == nil then error("Can't init keypair from file") end
		obj.keyPair = tmp;
	end
	function obj:saveKeyPairToFile(filepath)
		if obj.keyPair == nil then
			error("Can't save. not inited key pair")
		end
		r = encdec.saveKeyPairToFile(obj.keyPair, filepath)
		if r == nil then error("Can't save to file") end
	end
	return obj
end
function baes:new(key,iv)
	local obj = {}
		obj.key=key or encdec.genRandBytes(32)
		obj.iv=iv   or encdec.genRandBytes(16)
		obj.aesdata_enc={}
		obj.aesdata_dec={}
		while string.len(obj.key) ~= 32 do
			if string.len(obj.key) > 32 then
				obj.key=string.sub(obj.key,0,32)
			else
				obj.key=key.."0"
			end
		end

		function obj:checkSizes()	
			if not string.len(obj.iv) == 16 then error("IV size have to be 128 bit(16 byte/16 len)") end
			if not string.len(obj.key) == 32 then error("KEY size have to be 256 bit(32 byte/32 len)") end
		end

		function obj:getKey()
			return obj.key
		end

		function obj:getIV()
			return obj.iv
		end
		function obj:getAESData_rawEnc()
			return obj.aesdata_enc
		end
		function obj:getAESData_rawDec()
			return obj.aesdata_dec
		end
		function obj:getAESData_enc()
			return encdec.getAESData(obj.aesdata_enc),encdec.getAESData_len(obj.aesdata_enc)
		end

		function obj:getAESData_dec()
			return encdec.getAESData(obj.aesdata_dec),encdec.getAESData_len(obj.aesdata_dec)
		end

		function obj:setAESData_enc(msg,size)
			msg = msg or ""
			size = size or string.len(msg)
			obj.aesdata_enc = encdec.createAESData(msg,size)
		end
		function obj:encrypt(msg,t)
			obj:checkSizes()
			t = t or AESENCType['t_chacha20']
			obj.aesdata_enc=encdec.AESenc(obj.key,obj.iv,msg,t)
		end
		function obj:decrypt(aesdata,t)
			obj:checkSizes()
			t = t or AESENCType['t_chacha20']
			obj.aesdata_dec=encdec.AESdec(obj.key,obj.iv,aesdata,t)
		end

		function obj:clear()
				if not obj.aesdata_enc == nil then 
					encdec.freeAESData(obj.aesdata_enc) 
					obj.aesdata_enc = nil
				end
				if not obj.aesdata_dec == nil then 
					encdec.freeAESData(obj.aesdata_dec) 
					obj.aesdata_dec = nil
				end
		end
		return obj
end


--checkAllTypes("")
--checkAllTypes("is example message")
--checkAllTypes("a")
--checkAllTypes("ab")
--checkAllTypes("abc")
--checkAllTypes("abcd")
-- end test stuff



return baes
