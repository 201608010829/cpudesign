library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

-- use work.regsprober.all;

entity cpu_singlecycle is
	port(
		clk: in std_logic;
		reset: in std_logic;
		inst_addr: out std_logic_vector(31 downto 0);
		inst: in std_logic_vector(31 downto 0);
		data_addr: out std_logic_vector(31 downto 0);
		data_in: in std_logic_vector(31 downto 0);
		data_out: out std_logic_vector(31 downto 0);
		data_read: out std_logic;
		data_write: out std_logic
	);
end entity cpu_singlecycle;

architecture behav of cpu_singlecycle is
		signal ir: std_logic_vector(31 downto 0);
		signal pc: std_logic_vector(31 downto 0);

		signal next_pc: std_logic_vector(31 downto 0);

		-- Fields in instruction
		signal opcode: std_logic_vector(6 downto 0);
		signal rd: std_logic_vector(4 downto 0);
		signal funct3: std_logic_vector(2 downto 0);
		signal rs1: std_logic_vector(4 downto 0);
		signal rs2: std_logic_vector(4 downto 0);
		signal funct7: std_logic_vector(6 downto 0);
		signal shamt: std_logic_vector(4 downto 0);
    	
    	        signal Imm31_12U : std_logic_vector(31 downto 0);
    	        signal Imm11_0I : std_logic_vector(31 downto 0);
    	        signal Imm20_1J : std_logic_vector(31 downto 0);
    	        signal Imm12_1B : std_logic_vector(31 downto 0);
    	        signal Imm11_0S : std_logic_vector(31 downto 0);

		signal src1: std_logic_vector(31 downto 0);
		signal src2: std_logic_vector(31 downto 0);
        
        signal sb_a1: std_logic_vector(31 downto 0);
		signal sb_d1: std_logic_vector(31 downto 0);
		
		
		signal LUIresult: std_logic_vector(31 downto 0);
		signal AUIPCresult: std_logic_vector(31 downto 0);
		signal ADDIresult: std_logic_vector(31 downto 0);
		signal ANDIresult: std_logic_vector(31 downto 0);
		signal ORIresult: std_logic_vector(31 downto 0);
		signal XORIresult: std_logic_vector(31 downto 0);
		
		signal LBUresult: std_logic_vector(31 downto 0);
		signal SLTIresult: std_logic_vector(31 downto 0);
		signal SRAIresult: std_logic_vector(31 downto 0);
		signal JALresult: std_logic_vector(31 downto 0);
		
		signal SBresult: std_logic_vector(31 downto 0);
		signal ANDresult: std_logic_vector(31 downto 0);
		signal ORresult: std_logic_vector(31 downto 0);
		signal XORresult: std_logic_vector(31 downto 0);
		signal ADDresult: std_logic_vector(31 downto 0);
		signal SUBresult: std_logic_vector(31 downto 0);

		type regfile is array(natural range<>) of std_logic_vector(31 downto 0);
		signal regs: regfile(31 downto 0);
		signal reg_write: std_logic;
		signal reg_write_id: std_logic_vector(4 downto 0);
		signal reg_write_data: std_logic_vector(31 downto 0);
begin
		-- register file prober
		--gen: for i in 31 downto 0 generate
		--	regsview(i) <= regs(i);
		--end generate gen;

		-- Instruction Fetch
		inst_addr <= pc;
		ir <= inst;

		-- Decode
		-- Not finished
		opcode <= ir(6 downto 0);
		rd <= ir(11 downto 7);
		funct3 <= ir(14 downto 12);
		rs1 <= ir(19 downto 15);
		rs2 <= ir(24 downto 20);
		funct7 <= ir(31 downto 25);
		shamt <= rs2;
		
		
		Imm31_12U <= ir(31 downto 12) & "000000000000";

		Imm11_0I <= "11111111111111111111" & ir(31 downto 20) when ir(31)='1' else
									"00000000000000000000" & ir(31 downto 20);
    	Imm20_1J <= "111111111111" & ir(31) & ir(19 downto 12) & ir(20) & ir(30 downto 21) when ir(31)='1' else
									"000000000000" & ir(31) & ir(19 downto 12) & ir(20) & ir(30 downto 21);
    	Imm11_0S <= "11111111111111111111" & ir(31 downto 25) & ir(11 downto 7) when ir(31)='1' else
									"00000000000000000000" & ir(31 downto 25) & ir(11 downto 7);
		Imm12_1B <= "11111111111111111111" & ir(31) & ir(7) & ir(30 downto 25) & ir(11 downto 8) when ir(31)='1' else
									"00000000000000000000" & ir(31) & ir(7) & ir(30 downto 25) & ir(11 downto 8);

		-- Read operands from register file
		src1 <= regs(TO_INTEGER(UNSIGNED(rs1)));
		src2 <= regs(TO_INTEGER(UNSIGNED(rs2)));
		
		sb_d1 <=  src2 and "11111111";
		sb_a1 <= STD_LOGIC_VECTOR (SIGNED(src1) + SIGNED(Imm11_0S));

		-- Prepare index and data to write into register file
		reg_write_id <= rd;

	
		ADDIresult <= STD_LOGIC_VECTOR(SIGNED(src1) + SIGNED(Imm11_0I));
		SLTIresult <= "00000000000000000000000000000001" when TO_INTEGER(UNSIGNED(src1)) < TO_INTEGER(UNSIGNED(Imm11_0I)) else
						"00000000000000000000000000000000";
		ANDIresult <= src1 and Imm11_0I;
		ORIresult <= src1 or Imm11_0I;
		XORIresult <= src1 xor Imm11_0I;
		LUIresult <= Imm31_12U;
		AUIPCresult <= STD_LOGIC_VECTOR(SIGNED(pc) + SIGNED(Imm31_12U));
		
		ADDresult <= STD_LOGIC_VECTOR(SIGNED(src1) + SIGNED(src2));
		SUBresult <= STD_LOGIC_VECTOR(SIGNED(src1) - SIGNED(src2));
		ANDresult <= src1 and src2;
		ORresult <= src1 or src2;
		XORresult <= src1 xor src2;
		
		SRAIresult <= to_stdlogicvector( to_bitvector(src1) SRA to_integer(unsigned(shamt)) ) ;
		
		LBUresult <= "000000000000000000000000" & data_in(7 downto 0);
		JALresult <= STD_LOGIC_VECTOR(UNSIGNED(pc)+4);    

		-- more
		-- ......

		reg_write_data <= ADDIresult when opcode = "0010011" and funct3 = "000" else
						  SLTIresult when opcode = "0010011" and funct3 = "010" else
						  XORIresult when opcode = "0010011" and funct3 = "100" else
						  ORIresult when opcode = "0010011" and funct3 = "110" else
						  ANDIresult when opcode = "0010011" and funct3 = "111" else
						  ANDresult when opcode = "0110011" and funct3 = "111" else
						  ORresult when opcode = "0110011" and funct3 = "110" else
						  XORresult when opcode = "0110011" and funct3 = "100" else
						  ADDresult when opcode = "0110011" and funct7 = "0000000" else
						  SUBresult when opcode = "0110011" and funct7 = "0100000" else
						  SRAIresult when opcode = "0010011" and funct7 = "0100000" else
						  LBUresult when opcode = "0000011" and funct3 = "100" else
						  JALresult when opcode = "1101111" else
					     "00000000000000000000000000000000";
		data_addr <= sb_a1 when opcode = "0100011" and funct3 = "000";
		data_out <= sb_d1 when	opcode = "0100011" and funct3 = "000";	    
		next_pc <= STD_LOGIC_VECTOR(UNSIGNED(pc) + UNSIGNED(Imm20_1J)) when opcode = "1101111" else
			       STD_LOGIC_VECTOR(UNSIGNED(pc) + UNSIGNED(Imm12_1B))  when opcode = "1100011" and funct3 = "110" and (SIGNED(src1) < SIGNED(src2)) else
			       STD_LOGIC_VECTOR(UNSIGNED(pc)+4);
						  -- more 
						  -- ......
						  -- At last, set a default value
						  --"00000000000000000000000000000000";

		-- Execute
		-- Not finished

		-- Update pc and register file at rising edge of clk
		process(clk)
		begin
			if(rising_edge(clk)) then
				if (reset='1') then
					pc <= "00000000000000000000000000000000";
					-- Clear register file?
				else
					pc <= next_pc;

					if (reg_write = '1') then
						regs(TO_INTEGER(UNSIGNED(reg_write_id))) <= reg_write_data;
					end if; -- reg_write = '1'
				end if; -- reset = '1'
			end if; -- rising_edge(clk)
		end process; -- clk

end architecture behav;