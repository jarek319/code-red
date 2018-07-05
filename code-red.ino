#include <SPI.h>
#define LATCH 15

//595: !TEXT4CS !TEXT3CS !TEXT2CS !TEXT1CS !D11 D10 D9 D8 D7 D6 D5 D4 D3 D2 D1 D0
#define SR_A0  ( 1 << 7 )
#define SR_A1  ( 1 << 8 )
#define SR_W   ( 1 << 9 )
#define SR_CU  ( 1 << 10 )
#define SR_CLN ( 1 << 11 ) // LOW is on
#define SR_CE1 ( 1 << 12 )
#define SR_CE2 ( 1 << 13 )
#define SR_CE3 ( 1 << 14 )
#define SR_CE4 ( 1 << 15 )
#define ICM_A0 ( 1 << 8 )
#define ICM_A1 ( 1 << 9 )
#define ICM_A2 ( 1 << 10 )
#define ICM_MODE ( 1 << 7 )
#define ICM_W ( 1 << 6 )

//DL1416T: X X X X X !CU !W A1 A0 D6 D5 D4 D3 D2 D1 D0

//ICM7244: X X X X X A2 A1 A0 MODE !WR D5 D4 D2 D2 D1 D0
//COLONS: !D11

//AS1116: X R/W 0 REG5 REG4 REG3 REG2 REG1 REG0 D7 D6 D5 D4 D3 D2 D1 D0
#define AS_NOP    0x0000
#define AS_DIGIT0 0x0100
#define AS_DIGIT1 0x0200
#define AS_DIGIT2 0x0300
#define AS_DIGIT3 0x0400
#define AS_DIGIT4 0x0500
#define AS_DIGIT5 0x0600
#define AS_DIGIT6 0x0700
#define AS_DIGIT7 0x0800
#define AS_DECODE 0x0900
#define AS_GINTEN 0x0A00
#define AS_SCNLIM 0X0B00
#define AS_SHTDWN 0x0C00
#define AS_FEATUR 0x0E00
#define AS_TSTMDE 0x0F00
#define AS_D01ITY 0x1000
#define AS_D23ITY 0x1100
#define AS_D45ITY 0x1200
#define AS_D67ITY 0x1300
// First three displays are DL1416T and 4th is ICM7244
// ICM7244 controls the top left 4 digit 16 seg display
// These four are accessed using two shift registers
// The 595 shift registers map to the chip inputs as per above
// Four AS1116 displays follow the shift registers
// AS1116-4 is the dot matrix
// AS1116-3 is the rightmost red 7seg cluster
// AS1116-2 is the middle red 7seg cluster and right two digits next to colon
// AS1116-1 is the left red 7seg cluster and left two digits next to colon
// So the order should be
// AS1116-4 AS1116-3 AS1116-2 AS1116-1 595-2 595-1


uint16_t sr = 0x0000;
uint16_t as1 = 0x0C01;
uint16_t as2 = 0x0C01;
uint16_t as3 = 0x0C01;
uint16_t as4 = 0x0C01;

uint8_t Bit_Reverse( uint8_t x ) 
{ 
   x = ((x >> 1) & 0x55) | ((x << 1) & 0xaa); 
   x = ((x >> 2) & 0x33) | ((x << 2) & 0xcc); 
   x = ((x >> 4) & 0x0f) | ((x << 4) & 0xf0); 
   return x;    
}

void updateDisplays() {
  digitalWrite( LATCH, LOW );
  //as4 = ( Bit_Reverse( as4 & 0xFF ) ) | ( Bit_Reverse(( as4 >> 4 ) & 0xFF ));
  SPI.transfer16( as4 );
  SPI.transfer16( as3 );
  SPI.transfer16( as2 );
  SPI.transfer16( as1 );
  SPI.transfer16( sr );
  digitalWrite( LATCH, HIGH );
  digitalWrite( LATCH, LOW );
  SPI.transfer16( as4 );
  SPI.transfer16( as3 );
  SPI.transfer16( as2 );
  SPI.transfer16( as1 );
  SPI.transfer16(( sr | SR_CE1 | SR_CE2 | SR_CE3 | ICM_W ) & ~(SR_CE4) );
  digitalWrite( LATCH, HIGH );
}

void setup() {
  Serial.begin( 115200 );
  Serial.println( "hi" );
  SPI.begin();
  pinMode( LATCH, OUTPUT );
  digitalWrite( LATCH, LOW );
  //AS1116 send 0x01 to AS_SHTDWN
  as4 = AS_SHTDWN | 0x01;
  as3 = AS_SHTDWN | 0x01;
  as2 = AS_SHTDWN | 0x01;
  as1 = AS_SHTDWN | 0x01;
  updateDisplays();
  as4 = AS_GINTEN | 0xFF;
  as3 = AS_GINTEN | 0xFF;
  as2 = AS_GINTEN | 0xFF;
  as1 = AS_GINTEN | 0xFF;
  updateDisplays();
  as4 = AS_SCNLIM | 0x07;
  as3 = AS_SCNLIM | 0x07;
  as2 = AS_SCNLIM | 0x07;
  as1 = AS_SCNLIM | 0x07;
  updateDisplays();
}

void loop() {
  for ( uint16_t i = 0; i < 0xFF; i++ ) {
    Serial.println( i, HEX );
    as1 = AS_DIGIT0 | i;
    as2 = AS_DIGIT2 | i;
    as3 = AS_DIGIT5 | i;
    as4 = AS_DIGIT7 | i;
    
    sr = SR_CE1 | SR_CE2 | SR_CU | 'L' ;
    updateDisplays();
    sr = SR_CE1 | SR_CE2 | SR_CU | SR_A0 | 'I' ;
    updateDisplays();
    sr = SR_CE1 | SR_CE2 | SR_CU | SR_A1 | 'A' ;
    updateDisplays();
    sr = SR_CE1 | SR_CE2 | SR_CU | SR_A0 | SR_A1 | 'M' ;
    updateDisplays();
    sr = SR_CE1 | SR_CE3 | SR_CU | 'W' ;
    updateDisplays();
    sr = SR_CE1 | SR_CE3 | SR_CU | SR_A0 | 'O' ;
    updateDisplays();
    sr = SR_CE1 | SR_CE3 | SR_CU | SR_A1 | 'L' ;
    updateDisplays();
    sr = SR_CE1 | SR_CE3 | SR_CU | SR_A0 | SR_A1 | ( i & 0x3F ) ;
    updateDisplays();
    sr = SR_CE2 | SR_CE3 | SR_CU | 'H' ;
    updateDisplays();
    sr = SR_CE2 | SR_CE3 | SR_CU | SR_A0 | 'G' ;
    updateDisplays();
    sr = SR_CE2 | SR_CE3 | SR_CU | SR_A1 | 'I' ;
    updateDisplays();
    sr = SR_CE2 | SR_CE3 | SR_CU | SR_A0 | SR_A1 | 'H' ;
    updateDisplays();
    sr = ( SR_CE1 | SR_CE2 | SR_CE3 | SR_CE4 | 'T' ) & ~( ICM_W );
    updateDisplays();
    sr = ( SR_CE1 | SR_CE2 | SR_CE3 | SR_CE4 | ICM_A0 | 'I' ) & ~( ICM_W );
    updateDisplays();
    sr = ( SR_CE1 | SR_CE2 | SR_CE3 | SR_CE4 | ICM_A1 | 'M' ) & ~( ICM_W );
    updateDisplays();
    sr = ( SR_CE1 | SR_CE2 | SR_CE3 | SR_CE4 | ICM_A0 | ICM_A1 | 'E' ) & ~( ICM_W );
    updateDisplays();
    delay( 25 );
  }
}
