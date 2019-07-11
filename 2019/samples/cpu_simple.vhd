library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity cpu_simple is
	port(
		clk: in std_logic;
		reset: in std_logic;
		inst_addr: out std_logic_vector(31 downto 0);  -- 指令地址
		inst: in std_logic_vector(31 downto 0);	
		inst_read: out std_logic;
		data_addr: out std_logic_vector(31 downto 0);  -- 数据地址
		data: inout std_logic_vector(31 downto 0);
		data_read: out std_logic;
		data_write: out std_logic
	    );
end entity;

architecture cpu_simple_behav of cpu_simple is
	-- utype instructions, using opcode
	constant rtype_lui: std_logic_vector(6 downto 0) := B"0110111";
	constant rtype_auipc: std_logic_vector(6 downto 0) := B"0010111";
	
	-- rtype alu operations, using opcode, funct3, funct7
	constant rtype_alu: std_logic_vector(6 downto 0) := B"0110011";
	constant rtype_addsub: std_logic_vector(2 downto 0) := B"000";
	constant rtype_add: std_logic_vector(6 downto 0) := B"0000000";
	constant rtype_sub: std_logic_vector(6 downto 0) := B"0100000";
	constant rtype_sll: std_logic_vector(2 downto 0) := B"001";
	constant rtype_slt: std_logic_vector(2 downto 0) := B"010";
	constant rtype_sltu: std_logic_vector(2 downto 0) := B"011";
	constant rtype_xor: std_logic_vector(2 downto 0) := B"100";
	constant rtype_srlsra: std_logic_vector(2 downto 0) := B"101";
	constant rtype_srl: std_logic_vector(6 downto 0) := B"0000000";
	constant rtype_sra: std_logic_vector(6 downto 0) := B"0100000";
	constant rtype_or: std_logic_vector(2 downto 0) := B"110";
	constant rtype_and: std_logic_vector(2 downto 0) := B"111";

	-- btype branches, using opcode, funct3
	constant btype_branch: std_logic_vector(6 downto 0) := B"1100011";
	constant btype_beq: std_logic_vector(2 downto 0) := B"000";
	constant btype_bne: std_logic_vector(2 downto 0) := B"001";
	constant btype_blt: std_logic_vector(2 downto 0) := B"100";
	constant btype_bge: std_logic_vector(2 downto 0) := B"101";
	constant btype_bltu: std_logic_vector(2 downto 0) := B"110";
	constant btype_bgeu: std_logic_vector(2 downto 0) := B"111";

	type regfile is array(natural range<>) of std_logic_vector(31 downto 0);
	signal regs: regfile(31 downto 0);

	signal rd_write: std_logic;
	signal rd_data: std_logic_vector(31 downto 0);

	signal opcode: std_logic_vector(6 downto 0);

	signal rd: std_logic_vector(4 downto 0);
	signal rs1: std_logic_vector(4 downto 0);
	signal rs2: std_logic_vector(4 downto 0);

	signal funct3: std_logic_vector(2 downto 0);
	signal funct7: std_logic_vector(6 downto 0);

	signal utype_imm31_12: std_logic_vector(31 downto 12);

	signal btype_imm12_1: std_logic_vector(12 downto 1);

	signal rtype_alu_result: std_logic_vector(31 downto 0);

	signal pc: std_logic_vector(31 downto 0);
	signal ir: std_logic_vector(31 downto 0);

	signal next_pc: std_logic_vector(31 downto 0);

	signal branch_target: std_logic_vector(31 downto 0);
	signal branch_taken: boolean;

	function bool2logic32(b: boolean) return std_logic_vector(31 downto 0) is
	begin
		if b then
			return X"00000001";
		else
			return X"00000000";
		end if;
	end;


begin
	-- 组合逻辑部分
	-- instruction fetch
	inst_addr <= pc;  -- 取指地址
	inst_read <= '1' when reset = '0' else '0';  -- 当reset无效时发出指令读取信号;
	ir <= inst;  -- 当前指令

	-- decode
	opcode <= ir(6 downto 0);
	rd <= ir(11 downto 7);
	rs1 <= ir(19 downto 15);
	rs2 <= ir(24 downto 20);

	funct3 <= ir(14 downto 12);
	funct7 <= ir(31 downto 25);

	utype_imm31_12 <= ir(31 downto 12);

	btype_imm12_1 <= ir(31) & ir(7) & ir(30 downto 25) & ir(11 downto 8);
	
	-- ......

	-- R-type ALU operations
	rtype_alu_result <= std_logic_vector(signed(rs1) + signed(rs2)) when funct3 = rtype_addsub and funct7 = rtype_add else
			    std_logic_vector(signed(rs1) - signed(rs2)) when funct3 = rtype_addsub and funct7 = rtype_sub else
			    rs1 sll to_integer(unsigned(rs2)) when funct3 = rtype_sll else
			    bool2logic32(signed(rs1) < signed(rs2)) when funct3 = rtype_slt else
			    bool2logic32(unsigned(rs1) < unsigned(rs2)) when funct3 = rtype_sltu else
			    rs1 xor rs2 when funct3 = rtype_xor else
			    rs1 srl to_integer(unsigned(rs2)) when funct3 = rtype_srlsra and funct7 = rtype_srl else
			    rs1 sra to_integer(unsigned(rs2)) when funct3 = rtype_srlsra and funct7 = rtype_sra else
			    rs1 or rs2 when funct3 = rtype_or else
			    rs1 and rs2 when funct3 = rtype_and else
			    X"00000000";  -- default ALU result

	rd_data <= rtype_alu_result when opcode = rtype_alu else
			      utype_imm31_12 & X"000" when opcode = utype_lui else
			      std_logic_vector(unsigned(utype_imm31_12 & X"000") + unsigned(pc)) when opcode=utype_auipc else
		       -- ......
		       X"00000000";  -- default rd data

	rd_write <= opcode=rtype_alu or opcode=utype_lui or opcode=utype_auipc;

	-- 分支指令
	branch_target(13 downto 0) <= btype_imm12_1 & '0' & '0';
	branch_target(31 downto 14) <= ( others => btype_imm12_1(12) );

	branch_taken <= rs1 = rs2 when funct3 = btype_beq else
			rs1 /= rs2 when funct3 = btype_bne else
			signed(rs1) < signed(rs2) when funct3 = btype_blt else
			signed(rs1) >= signed(rs2) when funct3 = btype_bge else
			unsigned(rs1) < unsigned(rs2) when funct3 = btype_bltu else
			unsigned(rs1) >= unsigned(rs2) when funct3 = btype_bgeu else
			false;


	-- 下一条指令地址
	next_pc <= branch_target when opcode = btype_branch and branch_taken else 
		   std_logic_vector(unsigned(pc) + 4); -- 需补充其它情况


	-- ...... (其它组合逻辑)


	-- 时序逻辑部分
	-- pc
	pc_update: process(clk)
	begin
		if(rising_edge(clk)) then
			if(reset='1') then
				pc <= X"00000000";  -- 当reset信号有效时，pc被重置为0
			else
				pc <= next_pc;
			end if;
		end if;
	end process pc_update;

	-- regs
	reg_update: process(clk)
		variable i: integer;
		variable k: integer;
	begin
		i := to_integer(unsigned(rd));

		if(rising_edge(clk)) then
			if(reset='1') then
				-- reset all regs to 0 except reg[0]
				for k in 1 to 31 loop
					regs[k] <= X"00000000";  -- reset to 0
				end loop;	

			elsif(rd_write='1' and i /= 0) then

				regs[i] <= rd_data;

			end if;
		end if;
	end process reg_update;

-- ...... (其它时序逻辑)

end;
