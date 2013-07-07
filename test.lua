local ffi = require 'ffi'
loadelf = require 'luaelf/loadelf'


local elf = loadelf.init()

local mem = elf.load(arg[1])

function dump_mem(mem)
   for k, v in pairs(mem) do
      if k % 32 == 0 then
	 io.write("\n")
      end
      io.write(string.format("%x:%x ", k, v))
   end
end

function dump_mem_scns(mem)
   print(#(mem.scns), "\nsections")
   for i, s in ipairs(mem.scns) do
      print("\nSection: "..ffi.string(s.name))
      local base = tonumber(s.sh_addr)
      local limit = base + tonumber(s.sh_size)
      for a=base, limit-1 do
	 if a % 16 == 0 then
	    io.write(string.format("\n%x: ", a))
	 end
	 io.write(string.format("%2x ", mem[a]))
      end
   end
end

--dump_mem_scns(mem)
