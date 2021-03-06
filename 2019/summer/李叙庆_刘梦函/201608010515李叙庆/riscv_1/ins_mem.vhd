library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity ins_mem is
		port(
			addrbus: in std_logic_vector(31 downto 0);
			databus: inout std_logic_vector(31 downto 0)
			--read: in std_logic
			--write: in std_logic
			);
end entity;

architecture mem_behav of ins_mem is
		type memtype is array(natural range<>) of std_logic_vector(7 downto 0);
		signal memdata: memtype(1023 downto 0) := (
			0 => X"b7",   
			1 => X"10",
			2 => X"00",
			3 => X"00", --00000000 00000000 0001 00001 0110111  LUI
			
			4 => X"08",
			5 => X"00",
			6 => X"00",
			7 => X"00", --
			
			8 => X"00",
			9 => X"00",
			10=> X"00",
			11=> X"00",
			others => X"11"
		);

begin
		do_read: process(addrbus)
				variable i: integer;
		begin
			i := to_integer(unsigned(addrbus));
			--if (read='1') then
				-- assume little-endian
				databus <= memdata(i+3) & memdata(i+2) & memdata(i+1) & memdata(i);
			--else
				--databus <= "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ";
			--end if;
		end process do_read;
end;