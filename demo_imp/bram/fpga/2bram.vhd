library IEEE;
use IEEE.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

architecture arch_beh of tlv_bare_ifc is

   signal bram1_addr     : std_logic_vector(9 downto 0);
   signal bram1_data_out : std_logic_vector(15 downto 0);
   signal bram1_data_in  : std_logic_vector(15 downto 0);
   signal bram1_we       : std_logic;

   signal bram2_addr     : std_logic_vector(9 downto 0);
   signal bram2_data_out : std_logic_vector(15 downto 0);
   signal bram2_data_in  : std_logic_vector(15 downto 0);
   signal bram2_we       : std_logic;

   -- pouzite komponenty
   component SPI_adc
      generic (
         ADDR_WIDTH : integer;
         DATA_WIDTH : integer;
         ADDR_OUT_WIDTH : integer;
         BASE_ADDR  : integer
      );
      port (
         CLK      : in  std_logic;

         CS       : in  std_logic;
         DO       : in  std_logic;
         DO_VLD   : in  std_logic;
         DI       : out std_logic;
         DI_REQ   : in  std_logic;

         ADDR     : out std_logic_vector (ADDR_OUT_WIDTH-1 downto 0);
         DATA_OUT : out std_logic_vector (DATA_WIDTH-1 downto 0);
         DATA_IN  : in  std_logic_vector (DATA_WIDTH-1 downto 0);

         WRITE_EN : out std_logic;
         READ_EN  : out std_logic
      );
   end component;

   component RAMB16_S18
      port (
         DO   : out std_logic_vector(15 downto 0);
         DOP  : out std_logic_vector(1 downto 0);
         ADDR : in std_logic_vector(9 downto 0);
         CLK  : in std_ulogic;
         DI   : in std_logic_vector(15 downto 0);
         DIP  : in std_logic_vector(1 downto 0);
         EN   : in std_ulogic;
         SSR  : in std_ulogic;
         WE   : in std_ulogic
      );
   end component;

begin
   spi_adc_bram1: SPI_adc
      generic map (
         ADDR_WIDTH => 16,      -- sirka adresy 16 bitu
         DATA_WIDTH => 16,      -- sirka dat 16 bitu
         ADDR_OUT_WIDTH => 10,  -- sirka adresy k adresaci BRAM 10 bitu
         BASE_ADDR  => 16#0000# -- adresovy prostor od 0x0000 - 0x03FF
      )
      port map (
         CLK      => CLK,
         CS       => SPI_CS,

         DO       => SPI_DO,
         DO_VLD   => SPI_DO_VLD,
         DI       => SPI_DI,
         DI_REQ   => SPI_DI_REQ,

         ADDR     => bram1_addr,
         DATA_OUT => bram1_data_in,
         DATA_IN  => bram1_data_out,
         WRITE_EN => bram1_we,
         READ_EN  => open
      );

   spi_adc_bram2: SPI_adc
      generic map (
         ADDR_WIDTH => 16,      -- sirka adresy 16 bitu
         DATA_WIDTH => 16,      -- sirka dat 16 bitu
         ADDR_OUT_WIDTH => 10,  -- sirka adresy k adresaci BRAM 10 bitu
         BASE_ADDR  => 16#0400# -- adresovy prostor od 0x0400 - 0x07FF
      )
      port map (
         CLK      => CLK,
         CS       => SPI_CS,

         DO       => SPI_DO,
         DO_VLD   => SPI_DO_VLD,
         DI       => SPI_DI,
         DI_REQ   => SPI_DI_REQ,

         ADDR     => bram2_addr,
         DATA_OUT => bram2_data_in,
         DATA_IN  => bram2_data_out,
         WRITE_EN => bram2_we,
         READ_EN  => open
      );

   bram1: RAMB16_S18
      port map (
         CLK  => CLK,
         DO   => bram1_data_out,
         DI   => bram1_data_in,
         ADDR => bram1_addr,
         EN   => '1',
         SSR  => '0',
         WE   => bram1_we,
         DOP  => open,
         DIP  => "00"
      );

   bram2: RAMB16_S18
      port map (
         CLK  => CLK,
         DO   => bram2_data_out,
         DI   => bram2_data_in,
         ADDR => bram2_addr,
         EN   => '1',
         SSR  => '0',
         WE   => bram2_we,
         DOP  => open,
         DIP  => "00"
      );

end arch_beh;
