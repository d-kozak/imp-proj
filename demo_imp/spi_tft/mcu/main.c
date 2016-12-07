#include <fitkitlib.h>

// SPI port signals
#define M_STE 0x01
#define M_SIMO 0x02
#define M_SOMI 0x04
#define M_CLK 0x08

void TFT_Init(void); /* Initialization of TFT Display */
void TFT_SendREG(unsigned char reg_h, unsigned char reg_l); /* Send command to TFT display */
void TFT_SendDAT(unsigned char dat_h, unsigned char dat_l); /* Send data to TFT display */
void TFT_FillDisplay(unsigned char color_h, unsigned char color_l); /* Fill display with select color */


// ****************************************************************************
// *                        MSP430 USCI-B0-SPI routines                       *
// ****************************************************************************

// Init SPI
void MSPI_Init(unsigned char smclkdiv)
{
#if defined MSP_16X
    // ****************************
    // * FITkit 1.x not supported *
    // ****************************
#elif defined MSP_261X
    UCB0CTL1 = UCSWRST; // reset UCB0
    UCB0CTL1 |= UCSSEL_2;   // SMCLK clock

    // SPI mode: CLK phase 0, idle polarity 1, MSB first, 8-bit,
    // master, 3-pin and synchronous mode
    UCB0CTL0 = UCCKPL | UCMSB | UCMST | UCMODE_0 | UCSYNC;

    UCB0BR0 = smclkdiv;     // speed SMCLK/smclkdiv = 14/smclkdiv MHz
    UCB0BR1 = 0;

    // SPI I/O signals setup
    P3DIR |= (M_STE | M_SIMO | M_CLK);  // outputs
    P3DIR &= ~M_SOMI;   // input
    P3OUT |= M_STE;     // CS inactive -> 1
    P3SEL |= (M_SIMO | M_SOMI | M_CLK); // set UCB0 module function

    UCB0CTL1 &= ~UCSWRST; // release UCB0 reset
#else
    #error "Can not initialize the SPI bus"
#endif
}

// Disconnect SPI
void MSPI_Close(void)
{
#if defined MSP_16X
    // ****************************
    // * FITkit 1.x not supported *
    // ****************************
#elif defined MSP_261X
    UCB0CTL1 |= UCSWRST;  // reset UCB0

    // disable I/O
    P3DIR &= ~(M_STE | M_SIMO | M_SOMI | M_CLK);  // set all as inputs
    P3SEL &= ~(M_STE | M_SIMO | M_SOMI | M_CLK);  // disconnect UCB0 pins
#endif
}

// Transfer a byte via SPI
unsigned char MSPI_byte_trans(unsigned char data)
{
#if defined MSP_16X
    // **************************************
    return 0;   // FITkit 1.x not supported *
    // **************************************
#elif defined MSP_261X
    UCB0TXBUF = data;
    while (UCB0STAT & UCBUSY) { }
    
    return UCB0RXBUF;
#endif
}


// ****************************************************************************
//                             FITkit mandatory code                          *
// ****************************************************************************

int main(void)
{
    initialize_hardware();
    WDG_stop();

    MSPI_Init(1); 
    TFT_Init();
    for(;;)
    {
        TFT_FillDisplay(0xF8, 0x00);
        TFT_FillDisplay(0x07, 0xE0);
        TFT_FillDisplay(0x00, 0x1F);
        TFT_FillDisplay(0xCF, 0x2D);
    }
    
    return 0;
}

void print_user_help(void) { }

unsigned char decode_user_cmd(char *cmd_ucase, char *cmd)
{
    return (CMD_UNKNOWN);
}

void fpga_initialized() { }

// ****************************************************************************
//                         TFT operations in the SPI mode                     *
// ****************************************************************************

void TFT_Init(void)
{
  //************* Start Initial Sequence **********//
  TFT_SendREG(0x00, 0xE5); TFT_SendDAT(0x80, 0x00); // Set the internal vcore voltage 
  TFT_SendREG(0x00, 0x00); TFT_SendDAT(0x00, 0x01); // Start internal OSC.
  TFT_SendREG(0x00, 0x01); TFT_SendDAT(0x01, 0x00); // Set SS and SM bit
  TFT_SendREG(0x00, 0x02); TFT_SendDAT(0x07, 0x00); // Set 1 line inversion
  TFT_SendREG(0x00, 0x03); TFT_SendDAT(0x10, 0xB0);//TFT_SendDAT(0x10, 0x30); // Set GRAM write direction and BGR=1.
  TFT_SendREG(0x00, 0x04); TFT_SendDAT(0x00, 0x00); // Resize register
  TFT_SendREG(0x00, 0x08); TFT_SendDAT(0x02, 0x02); // Set the back porch and front porch
  TFT_SendREG(0x00, 0x09); TFT_SendDAT(0x00, 0x00); // Set non-display area refresh cycle ISC[3:0]
  TFT_SendREG(0x00, 0x0A); TFT_SendDAT(0x00, 0x00); // FMARK function
  TFT_SendREG(0x00, 0x0C); TFT_SendDAT(0x00, 0x00); // RGB interface setting
  TFT_SendREG(0x00, 0x0D); TFT_SendDAT(0x00, 0x00); // Frame marker Position
  TFT_SendREG(0x00, 0x0F); TFT_SendDAT(0x00, 0x00); // RGB interface polarity

  //************* Power On sequence ****************//
  TFT_SendREG(0x00, 0x10); TFT_SendDAT(0x00, 0x00); // SAP); TFT_SendDAT(BT[3:0]); TFT_SendDAT(AP); TFT_SendDAT(DSTB); TFT_SendDAT(SLP); TFT_SendDAT(STB
  TFT_SendREG(0x00, 0x11); TFT_SendDAT(0x00, 0x07); // DC1[2:0]); TFT_SendDAT(DC0[2:0]); TFT_SendDAT(VC[2:0]
  TFT_SendREG(0x00, 0x12); TFT_SendDAT(0x00, 0x00); // VREG1OUT voltage
  TFT_SendREG(0x00, 0x13); TFT_SendDAT(0x00, 0x00); // VDV[4:0] for VCOM amplitude
  delay_ms(200); // Dis-charge capacitor power voltage
  
  TFT_SendREG(0x00, 0x10); TFT_SendDAT(0x17, 0xB0); // SAP); TFT_SendDAT(BT[3:0]); TFT_SendDAT(AP); TFT_SendDAT(DSTB); TFT_SendDAT(SLP); TFT_SendDAT(STB
  TFT_SendREG(0x00, 0x11); TFT_SendDAT(0x00, 0x37); // DC1[2:0]); TFT_SendDAT(DC0[2:0]); TFT_SendDAT(VC[2:0]
  delay_ms(50); // Delay 50ms
  
  TFT_SendREG(0x00, 0x12); TFT_SendDAT(0x01, 0x3A); // VREG1OUT voltage
  delay_ms(50); // Delay 50ms
  
  TFT_SendREG(0x00, 0x13); TFT_SendDAT(0x16, 0x00); // VDV[4:0] for VCOM amplitude
  TFT_SendREG(0x00, 0x29); TFT_SendDAT(0x00, 0x0C); // VCM[4:0] for VCOMH
  delay_ms(50);
  
  //************* Adjust the Gamma Curve ****************//
  TFT_SendREG(0x00, 0x30); TFT_SendDAT(0x05, 0x04);
  TFT_SendREG(0x00, 0x31); TFT_SendDAT(0x07, 0x03);
  TFT_SendREG(0x00, 0x32); TFT_SendDAT(0x07, 0x02);
  TFT_SendREG(0x00, 0x35); TFT_SendDAT(0x01, 0x01);
  TFT_SendREG(0x00, 0x36); TFT_SendDAT(0x0A, 0x1F);
  TFT_SendREG(0x00, 0x37); TFT_SendDAT(0x05, 0x04);
  TFT_SendREG(0x00, 0x38); TFT_SendDAT(0x00, 0x03);
  TFT_SendREG(0x00, 0x39); TFT_SendDAT(0x07, 0x06);
  TFT_SendREG(0x00, 0x3C); TFT_SendDAT(0x07, 0x07);
  TFT_SendREG(0x00, 0x3D); TFT_SendDAT(0x09, 0x1F);
  
  //************* Set GRAM area ****************//
  TFT_SendREG(0x00, 0x50); TFT_SendDAT(0x00, 0x00); // Horizontal GRAM Start Address
  TFT_SendREG(0x00, 0x51); TFT_SendDAT(0x00, 0xEF); // Horizontal GRAM End Address
  TFT_SendREG(0x00, 0x52); TFT_SendDAT(0x00, 0x00); // Vertical GRAM Start Address
  TFT_SendREG(0x00, 0x53); TFT_SendDAT(0x01, 0x3F); // Vertical GRAM Start Address
  TFT_SendREG(0x00, 0x60); TFT_SendDAT(0x27, 0x00); // Gate Scan Line
  TFT_SendREG(0x00, 0x61); TFT_SendDAT(0x00, 0x01); // NDL,VLE); TFT_SendDAT(REV
  TFT_SendREG(0x00, 0x6A); TFT_SendDAT(0x00, 0x00); // set scrolling line

  //************* Partial Display Control ****************//
  TFT_SendREG(0x00, 0x80); TFT_SendDAT(0x00, 0x00);
  TFT_SendREG(0x00, 0x81); TFT_SendDAT(0x00, 0x00);
  TFT_SendREG(0x00, 0x82); TFT_SendDAT(0x00, 0x00);
  TFT_SendREG(0x00, 0x83); TFT_SendDAT(0x00, 0x00);
  TFT_SendREG(0x00, 0x84); TFT_SendDAT(0x00, 0x00);
  TFT_SendREG(0x00, 0x85); TFT_SendDAT(0x00, 0x00);

  //************* Panel Control ****************//
  TFT_SendREG(0x00, 0x90); TFT_SendDAT(0x00, 0x10);
  TFT_SendREG(0x00, 0x92); TFT_SendDAT(0x00, 0x00);
  TFT_SendREG(0x00, 0x93); TFT_SendDAT(0x00, 0x03);
  TFT_SendREG(0x00, 0x95); TFT_SendDAT(0x01, 0x10);
  TFT_SendREG(0x00, 0x97); TFT_SendDAT(0x00, 0x00);
  TFT_SendREG(0x00, 0x98); TFT_SendDAT(0x00, 0x00);
  
  TFT_SendREG(0x00, 0x20); TFT_SendDAT(0x00, 0x00); // GRAM horizontal Address
  TFT_SendREG(0x00, 0x21); TFT_SendDAT(0x00, 0x00); // GRAM Vertical Address
  TFT_SendREG(0x00, 0x07); TFT_SendDAT(0x01, 0x73); // 262K color and display ON
  
  TFT_SendREG(0x00, 0x22);
}

void TFT_SendREG(unsigned char reg_h, unsigned char reg_l)
{
    P3OUT &= ~M_STE;  // activate CS (Slave Trans Enable)
    MSPI_byte_trans(0x70);
    MSPI_byte_trans(reg_h);
    MSPI_byte_trans(reg_l);
    P3OUT |= M_STE;  // deactivate CS
}

void TFT_SendDAT(unsigned char dat_h, unsigned char dat_l)
{
    P3OUT &= ~M_STE;  // activate CS (Slave Trans Enable)
    MSPI_byte_trans(0x72);
    MSPI_byte_trans(dat_h);
    MSPI_byte_trans(dat_l);
    P3OUT |= M_STE;  // deactivate CS
}

void TFT_FillDisplay(unsigned char color_h, unsigned char color_l)
{
    int i,j;
  
    for(i=0;i<320;i++)
    {
        for(j=0;j<240;j++)
        {
            TFT_SendDAT(color_h, color_l);  
        }
    }
}

