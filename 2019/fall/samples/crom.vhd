﻿library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity crom is
		port(
			inputs: in std_logic_vector(10 downto 0);
			outputs: out std_logic_vector(14 downto 0);
			enable: in std_logic
			);
end entity;

architecture crom_behav of crom is
		type cromtype is array(natural range<>) of std_logic_vector(14 downto 0);
		signal cromdata: cromtype(2047 downto 0) := (
			48 => B"00000_00000_00101",  -- for add assuming 000 for alusel
			others => B"00000_00000_00000"
		);

begin
		do_read: process(inputs, enable)
				variable i: integer;
		begin
			i := to_integer(unsigned(inputs));
			if (enable='1') then
				outputs <= cromdata(i);
			else
				outputs <= "ZZZZZZZZZZZZZZZ";
			end if;
		end process do_read;
end;
