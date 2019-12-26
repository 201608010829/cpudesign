library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
entity mem_unit is
	port(reset : in std_logic;
		 clk   : in std_logic;
		 S     : in std_logic;
		 mem_wr: in std_logic;
		 dr    : in std_logic_vector(15 downto 0);
		 sr    : in std_logic_vector(15 downto 0);
		 addr_sel: in std_logic_vector(2 downto 0);
		 sp_en : in std_logic_vector(1 downto 0);
		 jmp_relv : in std_logic;
		 data_bus : inout std_logic_vector(15 downto 0);
		 wr : out std_logic;
		 addr_bus,mem_data : out std_logic_vector(15 downto 0); 
		 pc,sp,ir : buffer std_logic_vector(15 downto 0)
		);
	end mem_unit;
	 
architecture bhv of mem_unit is
         SIGNAL OP    : STD_LOGIC_VECTOR(7 DOWNTO 0);
    	 SIGNAL offset: STD_LOGIC_VECTOR(15 DOWNTO 0);
begin
	OP <= ir(15 DOWNTO 8);
    offset <= "11111111" & ir(7 DOWNTO 0) WHEN ir(7) = '1' ELSE
              "00000000" & ir(7 DOWNTO 0);     -- ��ƫ������չΪ16λ
 
		
--ir
	process(reset,clk,S)
	VARIABLE tmp: STD_LOGIC_VECTOR(15 DOWNTO 0);
	begin
		if reset ='0' then
		tmp :="0100000000000000";
		elsif clk'event and clk = '1' and S = '1' then
			tmp := data_bus;
		end if;
		ir <= tmp;
	end process;
--sp
	process(reset,clk,S)
	VARIABLE tmp: STD_LOGIC_VECTOR(15 DOWNTO 0);
	begin
		if reset = '0' then
		tmp := x"0280";
		elsif clk'event and clk = '1' then
			if S = '0' then
				if sp_en = "01" then
				tmp :=tmp + 1;
				elsif sp_en = "10" then
				tmp := tmp - 1;
				end if;
			end if ;
		end if;
		sp <= tmp;
	end process;
				
			
  
--pc
 
    PROCESS (reset, clk)
        VARIABLE tmp: STD_LOGIC_VECTOR(15 DOWNTO 0);
	
    BEGIN
        IF reset = '0' THEN
            tmp := x"0000";
        ELSIF clk'EVENT AND clk = '1' THEN
            IF S = '1' THEN                 -- ȡָ��֮��PC��1
                tmp := tmp + 1;
            ELSIF jmp_relv = '1' THEN       -- �������ת��
                tmp := tmp + offset;
            ELSIF OP = "11000000" THEN    -- JMPAָ��
                tmp := data_bus;
            ELSIF OP = "11010000" THEN    -- MVRDָ��
                tmp := tmp + 1;
            END IF;
        END IF;
        pc <= tmp;
    END PROCESS;
--�洢����ַѡ��
	PROCESS (reset, S, addr_sel, sp, dr, sr, pc)
        VARIABLE tmp: STD_LOGIC_VECTOR(15 DOWNTO 0);
    BEGIN
        IF reset = '0' THEN
            tmp := x"0000";
        ELSIF S = '1' THEN                     -- ȡָ��
            tmp := pc;
        ELSE
            IF addr_sel = "000" THEN         -- PUSH
                tmp := sp;
            ELSIF addr_sel = "001" THEN      -- POP
                tmp := sp - 1;
            ELSIF addr_sel = "010" THEN      -- STR
                tmp := dr;
            ELSIF addr_sel = "011" THEN      -- LDR
                tmp := sr;
            ELSIF addr_sel = "100" THEN      -- ˫�ֳ�ָ��
                tmp := pc;
            ELSE
                tmp := "XXXXXXXXXXXXXXXX";
            END IF;
        END IF;
        addr_bus <= tmp;
    END PROCESS;
--�洢����д����
	-- mem_wr = ��1��: wr <= clk��  д�洢��
    -- mem_wr = ��0��: wr <= ��1��  ���洢��

    PROCESS(clk, mem_wr, data_bus, sr)
    BEGIN
        IF mem_wr = '1' THEN  -- д�洢��(STRָ��, PUSHָ��)
            data_bus <= sr;
            wr <= clk;
        ELSE                    -- ���洢��
            data_bus <= "ZZZZZZZZZZZZZZZZ";
            wr <= '1';
            mem_data <= data_bus;
        END IF;
    END PROCESS;
end bhv;
