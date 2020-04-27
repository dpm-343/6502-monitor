/* 

   Original File by Ben Eater http://eater.net
   
   Released under Creative Commons Attribution: https://creativecommons.org/licenses/by/4.0/
   
   For use with Arduino Mega 2560
   
   Added opcode decoding based on 6502 CPU SYNC flag. Requires additional connection to the SYNC pin (pin 7)

*/
const char ADDR[] = {22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52};
const char DATA[] = {39, 41, 43, 45, 47, 49, 51, 53};

#define CLOCK 2
#define READ_WRITE 3
#define SYNC 4        // SYNC (6502 pin 7) indicates when CPU is performing OPCODE fetch

const char opcodeMatrix[256][5] = {\
             "BRK", "ORA", ""   , "", "TSB", "ORA", "ASL", "RMB0", "PHP", "ORA", "ASL", ""   , "TSB", "ORA", "ASL", "BBR0",\
             "BPL", "ORA", "ORA", "", "TRB", "ORA", "ASL", "RMB1", "CLC", "ORA", "INC", ""   , "TRB", "ORA", "ASL", "BBR1",\
             "JSR", "AND", ""   , "", "BIT", "AND", "ROL", "RMB2", "PLP", "AND", "ROL", ""   , "BIT", "AND", "ROL", "BBR2",\
             "BMI", "AND", "AND", "", "BIT", "AND", "ROL", "RMB3", "SEC", "AND", "DEC", ""   , "BIT", "AND", "ROL", "BBR3",\
             "RTI", "EOR", ""   , "", ""   , "EOR", "LSR", "RMB4", "PHA", "EOR", "LSR", ""   , "JMP", "EOR", "LSR", "BBR4",\
             "BVC", "EOR", "EOR", "", ""   , "EOR", "LSR", "RMB5", "CLI", "EOR", "PHY", ""   , ""   , "EOR", "LSR", "BBR5",\
             "RTS", "ADC", ""   , "", "STZ", "ADC", "ROR", "RMB6", "PLA", "ADC", "ROR", ""   , "JMP", "ADC", "ROR", "BBR6",\
             "BVS", "ADC", "ADC", "", "STZ", "ADC", "ROR", "RMB7", "SEI", "ADC", "PLY", ""   , "JMP", "ADC", "ROR", "BBR7",\
             "BRA", "STA", ""   , "", "STY", "STA", "STX", "SMB0", "DEY", "BIT", "TXA", ""   , "STY", "STA", "STX", "BBS0",\
             "BCC", "STA", "STA", "", "STY", "STA", "STX", "SMB1", "TYA", "STA", "TXS", ""   , "STZ", "STA", "STZ", "BBS1",\
             "LDY", "LDA", "LDX", "", "LDY", "LDA", "LDX", "SMB2", "TAY", "LDA", "TAX", ""   , "LDY", "LDA", "LDX", "BBS2",\
             "BCS", "LDA", "LDA", "", "LDY", "LDA", "LDX", "SMB3", "CLV", "LDA", "TSX", ""   , "LDY", "LDA", "LDX", "BBS3",\
             "CPY", "CMP", ""   , "", "CPY", "CMP", "DEC", "SMB4", "INY", "CMP", "DEX", "WAI", "CPY", "CMP", "DEC", "BBS4",\
             "BNE", "CMP", "CMP", "", ""   , "CMP", "DEC", "SMB5", "CLD", "CMP", "PHX", "STP", ""   , "CMP", "DEC", "BBS5",\
             "CPX", "SBC", ""   , "", "CPX", "SBC", "INC", "SMB6", "INX", "SBC", "NOP", ""   , "CPX", "SBC", "INC", "BBS6",\
             "BEQ", "SBC", "SBC", "", ""   , "SBC", "INC", "SMB7", "SED", "SBC", "PLX", ""   , ""   , "SBC", "INC", "BBS7"};
                 

void setup() {
  for (int n = 0; n < 16; n += 1) {
    pinMode(ADDR[n], INPUT);
  }
  for (int n = 0; n < 8; n += 1) {
    pinMode(DATA[n], INPUT);
  }
  pinMode(CLOCK, INPUT);
  pinMode(READ_WRITE, INPUT);
  pinMode(SYNC, INPUT);

  attachInterrupt(digitalPinToInterrupt(CLOCK), onClock, RISING);

  
  Serial.begin(57600);
}

void onClock() {
  char output[24];

  unsigned int address = 0;
  for (int n = 0; n < 16; n += 1) {
    int bit = digitalRead(ADDR[n]) ? 1 : 0;
    Serial.print(bit);
    address = (address << 1) + bit;
  }
  
  Serial.print("   ");
  
  unsigned int data = 0;
  for (int n = 0; n < 8; n += 1) {
    int bit = digitalRead(DATA[n]) ? 1 : 0;
    Serial.print(bit);
    data = (data << 1) + bit;
  }

  if(digitalRead(SYNC) && data <= 0xff) {
    // Add OPCODE decode when SYNC line is high (CPU is decoding instruction)
    sprintf(output, "   %04x  %c %02x : %4s", address, digitalRead(READ_WRITE) ? 'r' : 'W', data, opcodeMatrix[data]);
  } else {
    // Suppress OPCODE decode
    sprintf(output, "   %04x  %c %02x", address, digitalRead(READ_WRITE) ? 'r' : 'W', data);
  }

  Serial.println(output);  
}

void loop() {
}
