----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 02/11/2020 01:31:45 PM
-- Design Name: 
-- Module Name: assign4 - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use std.textio.all;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity assign4 is
Port (
    clk: in std_logic;
    seven_seg: in std_logic;
    a: out std_logic;
    b: out std_logic;
    c: out std_logic;
    d: out std_logic;
    e: out std_logic;
    f: out std_logic;
    g: out std_logic;
    an1: out std_logic;
    an2: out std_logic;
    an3: out std_logic;
    an4: out std_logic
 );
end assign4;

architecture Behavioral of assign4 is

type reg_value is array(31 downto 0) of integer;
    signal reg_array : reg_value := (0 => 2 , 1 => 1 , 29 => 4095 , others => 0);

signal pc: integer:=1;
signal clock_cycle_count: integer:=0;
signal segmentout: integer:=0;
signal an : std_logic_vector(0 to 3):="1111";
signal dummy_out : std_logic_vector(15 downto 0);
signal display1 :integer:= 0;
signal display2 :integer:= 0;
signal display3 :integer:= 0;
signal display4 :integer:= 0;
signal start :std_logic:='0';

type state is (idle, load , store ,store_help ,store_help1, active);

signal st : state := active;

signal ena : STD_LOGIC:='1';
signal wea : STD_LOGIC_VECTOR(0 DOWNTO 0):="0";
signal addra : STD_LOGIC_VECTOR(11 DOWNTO 0) := "000000000000";
signal dina : STD_LOGIC_VECTOR(31 DOWNTO 0);
signal douta : STD_LOGIC_VECTOR(31 DOWNTO 0);
signal clk_1k : std_logic;

component blk_mem_gen_0
port(
    clka : IN STD_LOGIC;
    ena : IN STD_LOGIC;
    wea : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    addra : IN STD_LOGIC_VECTOR(11 DOWNTO 0);
    dina : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    douta : OUT STD_LOGIC_VECTOR(31 DOWNTO 0) 
);
end component;

begin

an1 <= an(0);
an2 <= an(1);
an3 <= an(2);
an4 <= an(3);

br: blk_mem_gen_0 port map(
clka=> clk, ena => ena, wea => wea, addra => addra, dina=> dina, douta => douta
);

process(clk)
variable cnt1: integer := 0;
       variable t: std_logic := '0';     
       begin
           if(clk = '1' and clk'event) then
               if(cnt1 = 50000) then cnt1 := 0; t := not t;
               else cnt1 := cnt1 + 1;
               end if;
            end if;
       clk_1k <= t;
   end process;

process(clk_1k)
variable dummy: std_logic_vector(31 downto 0);
variable intv: integer;
variable intv1: integer;
variable stdv1: std_logic_vector(31 downto 0);
variable stdv2: std_logic_vector(31 downto 0);

begin
if(clk_1k = '1' and clk_1k'event) then
if not (st=idle) then
clock_cycle_count <= clock_cycle_count + 1;
end if;
case st is
when active=>
dummy := douta;
addra <= std_logic_vector(to_unsigned(pc,12));
if(dummy = "00000000000000000000000000000000" and not (pc = 0)) then
    st<= idle;
-- add
elsif(dummy(31 downto 26) = "000000" and dummy(5 downto 0) = "100000") then
    intv:= reg_array(to_integer(unsigned(dummy(20 downto 16)))) + reg_array(to_integer(unsigned(dummy(25 downto 21))));
    reg_array(to_integer(unsigned(dummy(15 downto 11)))) <= intv;
	 stdv1:= std_logic_vector(to_signed(intv, 32));
	 dummy_out <= stdv1(15 downto 0);
	 pc <= pc+1;
       
-- sub
elsif(dummy(31 downto 26) = "000000" and dummy(5 downto 0) = "100010") then
    intv:= reg_array(to_integer(unsigned(dummy(25 downto 21)))) - reg_array(to_integer(unsigned(dummy(20 downto 16))));
    reg_array(to_integer(unsigned(dummy(15 downto 11)))) <= intv;
	 stdv1:= std_logic_vector(to_signed(intv, 32));
	 dummy_out <= stdv1(15 downto 0);
	 pc <= pc+1;

--lw
elsif(dummy(31 downto 26) = "100011") then
    intv:= to_integer(unsigned(dummy(15 downto 0))) + reg_array(to_integer(unsigned(dummy(25 downto 21))));
    addra <= std_logic_vector(to_unsigned(intv,12));
    st <= load;

--sw
elsif(dummy(31 downto 26) = "101011") then
    intv:= to_integer(unsigned(dummy(15 downto 0))) + reg_array(to_integer(unsigned(dummy(25 downto 21))));
    addra <= std_logic_vector(to_unsigned(intv,12));
    wea <= "1";
    stdv1:=std_logic_vector(to_signed(reg_array(to_integer(unsigned(dummy(20 downto 16)))), 32));
    dummy_out <= stdv1(15 downto 0);
    dina <= stdv1;
    st <= store_help; 
    
-- sll
elsif(dummy(31 downto 26) = "000000" and dummy(5 downto 0) = "000000") then
    stdv1:= std_logic_vector(to_unsigned(reg_array(to_integer(unsigned(dummy(20 downto 16)))), 32));
    intv:=to_integer(unsigned(dummy(10 downto 6)));
    stdv2:= std_logic_vector(shift_left(unsigned(stdv1) , intv));
    intv1:=to_integer(unsigned(stdv2));
    reg_array(to_integer(unsigned(dummy(15 downto 11)))) <= intv1;
	dummy_out <= stdv2(15 downto 0);
	pc <= pc+1;
     
-- srl
elsif(dummy(31 downto 26) = "000000" and dummy(5 downto 0) = "000010") then
    stdv1:= std_logic_vector(to_unsigned(reg_array(to_integer(unsigned(dummy(20 downto 16)))), 32));
    intv:=to_integer(unsigned(dummy(10 downto 6)));
    stdv2:= std_logic_vector(shift_right( unsigned(stdv1) , intv));
    intv1:=to_integer(unsigned(stdv2));
    reg_array(to_integer(unsigned(dummy(15 downto 11)))) <= intv1;
	 dummy_out <= stdv2(15 downto 0);
	 pc <= pc+1;
	 
--bne
elsif(dummy(31 downto 26) = "000101") then
    intv:= reg_array(to_integer(unsigned(dummy(25 downto 21))));
    intv1:= reg_array(to_integer(unsigned(dummy(20 downto 16))));
    
    if(not (intv = intv1)) then
        --addra <= std_logic_vector(shift_left(unsigned(dummy(15 downto 0)) , 2));
        addra <= std_logic_vector(to_unsigned(to_integer(unsigned(addra)) + to_integer(unsigned(dummy(11 downto 0))), 12));
        pc <= pc + to_integer(unsigned(dummy(11 downto 0)));
    else
        pc <= pc+1;
    end if;
    
--beq
elsif(dummy(31 downto 26) = "000100") then
    intv:= reg_array(to_integer(unsigned(dummy(25 downto 21))));
    intv1:= reg_array(to_integer(unsigned(dummy(20 downto 16))));
    if(intv = intv1) then
        --addra <= std_logic_vector(shift_left(unsigned(dummy(15 downto 0)) , 2));
        addra <= std_logic_vector(to_unsigned(to_integer(unsigned(addra)) + to_integer(unsigned(dummy(11 downto 0))), 12));
        pc <= pc + to_integer(unsigned(dummy(11 downto 0)));
    else
        pc <= pc+1;
    end if;
    
--blez
elsif(dummy(31 downto 26) = "000110") then
    intv:= reg_array(to_integer(unsigned(dummy(25 downto 21))));
    if(intv <= 0) then
        --addra <= std_logic_vector(shift_left(unsigned(dummy(15 downto 0)) , 2));
        addra <= std_logic_vector(to_unsigned(to_integer(unsigned(addra)) + to_integer(unsigned(dummy(11 downto 0))), 12));
        pc <= pc + to_integer(unsigned(dummy(11 downto 0)));
    else
        pc <= pc+1;
    end if;
    
--bgtz
elsif(dummy(31 downto 26) = "000111") then
    intv:= reg_array(to_integer(unsigned(dummy(25 downto 21))));
    if(intv > 0) then
        --addra <= std_logic_vector(shift_left(unsigned(dummy(15 downto 0)) , 2));
       addra <= std_logic_vector(to_unsigned(to_integer(unsigned(addra)) + to_integer(unsigned(dummy(11 downto 0))), 12));
       pc <= pc + to_integer(unsigned(dummy(11 downto 0)));
    else
        pc <= pc+1;
    end if;
    
--j
elsif(dummy(31 downto 26) = "000010") then
   -- addra <= std_logic_vector(shift_left(unsigned(stdv1) , 2));
    addra <= dummy(11 downto 0);
    pc <= to_integer(unsigned(dummy(11 downto 0))) + 1;
    
--jal :stores the return address in $31
elsif(dummy(31 downto 26) = "000011") then
    reg_array(31) <= pc;
    addra <= dummy(11 downto 0);
    pc <= to_integer(unsigned(dummy(11 downto 0))) + 1;
    
--jr $s: jump to address in register s
elsif(dummy(31 downto 26) = "000000" and dummy(5 downto 0) = "001000") then
    intv:= reg_array(to_integer(unsigned(dummy(25 downto 21))));
    pc <= intv + 1;
    addra <= std_logic_vector(to_unsigned(intv , 12));

   
end if;

when load =>    
    stdv1:= douta;
    intv1:=to_integer(signed(stdv1));
    reg_array(to_integer(unsigned(dummy(20 downto 16)))) <= intv1;
    dummy_out <= std_logic_vector(to_unsigned(intv1,16));
    addra <= std_logic_vector(to_unsigned(pc,12));
    pc <= pc+1;
    st <= active;

when store_help =>
    wea <= "0";
    addra <= std_logic_vector(to_unsigned(pc,12));
    pc <= pc+1;
    st <= active;
    
when others=>
    st <= st;
end case;
end if;

end process;

--seven segment display
 process(segmentOut)

  begin
    if(segmentOut=1 or segmentOut=4 or segmentOut=11 or segmentOut=13) then a<='1';
     else a<='0';
     end if;
    if(segmentOut=5 or segmentOut=6 or segmentOut=11 or segmentOut=12 or segmentOut=14 or segmentOut=15) then b<='1';
     else b<='0';
     end if;
    if(segmentOut=2 or segmentOut=12 or segmentOut=14 or segmentOut=15) then c<='1';
     else c<='0';
     end if;
    if(segmentOut=1 or segmentOut=4 or segmentOut=7 or  segmentOut=10 or segmentOut=15) then d<='1';
     else d<='0';
     end if;
    if(segmentOut=1 or segmentOut=3 or segmentOut=4 or segmentOut=5 or segmentOut=7 or segmentOut=9 ) then e<='1';
     else e<='0';
     end if;
    if(segmentOut=1 or segmentOut=2 or segmentOut=3 or segmentOut=7 or segmentOut=13) then f<='1';
     else f<='0';
     end if;
    if(segmentOut=0 or segmentOut=1 or segmentOut=7 or segmentOut=12) then g<='1';
     else g<='0';
     end if;                                                                                                                                          

  end process;
  
process(clk_1k)
      variable count:integer:=0;
      begin
      if(clk_1k='1' and clk_1k'event) then
      case count is
          when 0 =>
              an <= "0111";
              segmentOut <= display4;
              count := 1;
          when 1 =>
              an <= "1011";
              segmentOut <= display3;
              count := 2;
          when 2 =>
              an <= "1101";
              segmentOut <= display2;
              count := 3;
          when others =>
              an <= "1110";
              segmentOut <= display1;
              count := 0;
      end case;
      end if;
  end process;
  
process(clk)
begin
if(clk = '1' and clk'event) then
if(seven_seg = '1') then
    display4 <= (clock_cycle_count) mod 16;
    display3 <= ((clock_cycle_count)/16) mod 16;
    display2 <= ((clock_cycle_count)/256) mod 16;
    display1 <= ((clock_cycle_count)/4096) mod 16;
    
else
    display1 <= to_integer(unsigned(dummy_out(15 downto 12)));
    display2 <= to_integer(unsigned(dummy_out(11 downto 8)));
    display3 <= to_integer(unsigned(dummy_out(7 downto 4)));
    display4 <= to_integer(unsigned(dummy_out(3 downto 0)));
    
   
end if;
end if;
end process;
end Behavioral;
