require 'loadelf'

local elf = loadelf.init()

local mem = elf.load(arg[1])
for k, v in pairs(mem) do
   if k % 128 == 0 then io.write(string.format('\n%x: ', k))
   io.write(string.format('%x ', v))
   end
end
