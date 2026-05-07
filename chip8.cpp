#include <fstream>
#include "chip8.h"

uint8_t fontset[FONTSET_SIZE] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void Chip8::Cycle(){
    opcode = ((MEMORY[pc] << 8) | MEMORY[pc+1]);
    pc += 2;

    ((*this).*(table[(opcode & 0xF000) >> 12]))();
    
    if(delaytimer) delaytimer--;
    if(soundtimer) soundtimer--;
}

void Chip8::LoadRom(char const* filename){
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if(file.is_open()){
        std::streampos size = file.tellg();
        char* buffer = new char[size];

        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        for(long i = 0; i < size; i++){
            MEMORY[STARTADDR + i] = buffer[i];
        }

        delete []buffer;
    }
}

void Chip8::OP_00E0(){
    memset(video, 0, sizeof(video));
}

void Chip8::OP_00EE(){
    sp--;
    pc = stack[sp];
}

void Chip8::OP_1nnn(){
    pc = opcode & 0x0FFF;
}

void Chip8::OP_2nnn(){
    stack[sp] = pc;
    sp++;
    pc = opcode & 0x0FFF;
}

void Chip8::OP_3xkk(){
    uint8_t currReg = (opcode & 0x0F00) >> 8;
    if(Registers[currReg] == (opcode & 0x00FF)) pc += 2;
}

void Chip8::OP_4xkk(){
    uint8_t currReg = (opcode & 0x0F00) >> 8;
    if(Registers[currReg] != (opcode & 0x00FF)) pc += 2;
}

void Chip8::OP_5xy0(){
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t reg2 = (opcode & 0x00F0) >> 4;
    if(Registers[reg1] == Registers[reg2]) pc += 2;
}

void Chip8::OP_6xkk(){
    uint8_t currReg = (opcode & 0x0F00) >> 8;
    Registers[currReg] = opcode & 0x00FF;
}

void Chip8::OP_7xkk(){
    uint8_t currReg = (opcode & 0x0F00) >> 8;
    Registers[currReg] += opcode & 0x00FF;
}

void Chip8::OP_8xy0(){
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t reg2 = (opcode & 0x00F0) >> 4;
    Registers[reg1] = Registers[reg2];
}

void Chip8::OP_8xy1(){
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t reg2 = (opcode & 0x00F0) >> 4;
    Registers[reg1] = Registers[reg1] | Registers[reg2];   
}

void Chip8::OP_8xy2(){
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t reg2 = (opcode & 0x00F0) >> 4;
    Registers[reg1] = Registers[reg1] & Registers[reg2];  
}

void Chip8::OP_8xy3(){
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t reg2 = (opcode & 0x00F0) >> 4;
    Registers[reg1] = Registers[reg1] ^ Registers[reg2];  
}

void Chip8::OP_8xy4(){
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t reg2 = (opcode & 0x00F0) >> 4;
    uint16_t sum = Registers[reg1] + Registers[reg2];
    if(sum > 255) Registers[0xF] = 1;
    else Registers[0xF] = 0; 
    Registers[reg1] = sum & 0xFF; 
}

void Chip8::OP_8xy5(){
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t reg2 = (opcode & 0x00F0) >> 4;
    if(Registers[reg1] > Registers[reg2]) Registers[0xF] = 1;
    else Registers[0xF] = 0;  
    Registers[reg1] = Registers[reg1] - Registers[reg2];
}

void Chip8::OP_8xy6(){
    uint8_t currReg = (opcode & 0x0F00) >> 8;
    Registers[0xF] = Registers[currReg] & 0x01;
    Registers[currReg] = Registers[currReg] >> 1;
}

void Chip8::OP_8xy7(){
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t reg2 = (opcode & 0x00F0) >> 4;
    Registers[reg1] = Registers[reg2] - Registers[reg1];
    if(Registers[reg1] < Registers[reg2]) Registers[0xF] = 1;
    else Registers[0xF] = 0;   
}

void Chip8::OP_8xyE(){
    uint8_t currReg = (opcode & 0x0F00) >> 8;
    Registers[0xF] = (Registers[currReg] & 0x90) >> 7;
    Registers[currReg] = Registers[currReg] << 1;
}

void Chip8::OP_9xy0(){
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t reg2 = (opcode & 0x00F0) >> 4;
    if(Registers[reg1] != Registers[reg2]) pc += 2;
}

void Chip8::OP_Annn(){
    index = opcode & 0x0FFF;
}

void Chip8::OP_Bnnn(){
    pc = (opcode & 0x0FFF) + Registers[0];
}

void Chip8::OP_Cxkk(){
    uint8_t currReg = (opcode & 0x0F00) >> 8;
    uint8_t randNum = static_cast<uint8_t>(randByte(randGen));
    Registers[currReg] = (opcode & 0x00FF) & randNum;
}

void Chip8::OP_Dxyn(){
    uint8_t byteSize = opcode & 0x000F;
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t reg2 = (opcode & 0x00F0) >> 4;
    unsigned int xcoord = Registers[reg1] % 64; unsigned int ycoord = Registers[reg2] % 32;
    Registers[0xF] = 0;

    for(int row = 0; row < byteSize; row++){
        uint8_t spriteRow = MEMORY[index + row];
        for(int col = 0; col  < 8; col++){
            uint8_t spriteBit = spriteRow & (0x80 >> col);
            uint32_t* screenBit = &video[(ycoord + row) * VIDEOWIDTH + (xcoord + col)];

            if(spriteBit){
                if(*screenBit == 0xFFFFFFFF){
                    Registers[0xF] = 1;
                }
                *screenBit ^= 0xFFFFFFFF;
            }
        }


    }


}

void Chip8::OP_Ex9E(){
    uint8_t key = Registers[(opcode & 0x0F00) >> 8];
    if(keypad[key]){
        pc += 2;
    }
}

void Chip8::OP_ExA1(){
    uint8_t key = Registers[(opcode & 0x0F00) >> 8];
    if(!keypad[key]){
        pc += 2;
    }
}

void Chip8::OP_Fx07(){
    uint8_t currReg = (opcode & 0x0F00) >> 8;
    Registers[currReg] = delaytimer;
}

void Chip8::OP_Fx0A(){
    uint8_t currReg = (opcode & 0x0F00) >> 8;
    if(keypad[0]){
        Registers[currReg] = 0;
    }
    else if(keypad[1]){
        Registers[currReg] = 1;
    }
    else if(keypad[2]){
        Registers[currReg] = 2;
    }
    else if(keypad[3]){
        Registers[currReg] = 3;
    }
    else if(keypad[4]){
        Registers[currReg] = 4;
    }
    else if(keypad[5]){
        Registers[currReg] = 5;
    }
    else if(keypad[6]){
        Registers[currReg] = 6;
    }
    else if(keypad[7]){
        Registers[currReg] = 7;
    }
    else if(keypad[8]){
        Registers[currReg] = 8;
    }
    else if(keypad[9]){
        Registers[currReg] = 9;
    }
    else if(keypad[10]){
        Registers[currReg] = 10;
    }
    else if(keypad[11]){
        Registers[currReg] = 11;
    }
    else if(keypad[12]){
        Registers[currReg] = 12;
    }
    else if(keypad[13]){
        Registers[currReg] = 13;
    }
    else if(keypad[14]){
        Registers[currReg] = 14;
    }
    else if(keypad[15]){
        Registers[currReg] = 15;
    }
    else{
        pc -= 2;
    }
}

void Chip8::OP_Fx15(){
    delaytimer = Registers[(opcode & 0x0F00) >> 8];
}

void Chip8::OP_Fx18(){
    soundtimer = Registers[(opcode & 0x0F00) >> 8];
}

void Chip8::OP_Fx1E(){
    uint8_t currReg = (opcode & 0x0F00) >> 8;
    index += Registers[currReg];
}

void Chip8::OP_Fx29(){
    uint8_t currReg = (opcode & 0x0F00) >> 8;
    uint8_t letter = Registers[currReg];
    index = FONTADDR + (5 * letter);
}

void Chip8::OP_Fx33(){
    uint8_t currReg = (opcode & 0x0F00) >> 8;
    uint8_t ones, tens, hundreds;
    uint8_t val = Registers[currReg];
    hundreds = (val / 100)%10;
    tens = (val%100) / 10;
    ones = (val%10);
    MEMORY[index] = hundreds;
    MEMORY[index + 1] = tens;
    MEMORY[index + 2] = ones;
}

void Chip8::OP_Fx55(){
    uint8_t currReg = (opcode & 0x0F00) >> 8;
    for(int i = 0; i <= currReg; i++){
        MEMORY[index + i] = Registers[i];
    }
}

void Chip8::OP_Fx65(){
    uint8_t currReg = (opcode & 0x0F00) >> 8;
    for(int i = 0; i <= currReg; i++){
        Registers[i] = MEMORY[index + i];
    }
}

void Chip8::Table0(){
    ((*this).*(table0[opcode & 0x000F]))();
}

void Chip8::Table8(){
    ((*this).*(table8[opcode & 0x000F]))();
}

void Chip8::TableE(){
    ((*this).*(tableE[opcode & 0x000F]))();
}

void Chip8::TableF(){
    ((*this).*(tableF[opcode & 0x00FF]))();
}

void Chip8::NullOP(){}