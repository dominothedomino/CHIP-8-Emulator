#include <cstdint>
#include <chrono>
#include <random>


const unsigned int STARTADDR = 0x200;
const unsigned int FONTADDR = 0x050;
const unsigned int VIDEOWIDTH = 64;
const unsigned int VIDEOHEIGHT = 32;

const unsigned int FONTSET_SIZE = 80;
extern uint8_t fontset[FONTSET_SIZE];

class Chip8
{
public:
    uint8_t MEMORY[4096]{};
    uint8_t Registers[16]{};
    uint16_t index{};
    uint16_t pc{};
    uint16_t stack[16]{};
    uint8_t sp{};
    uint8_t delaytimer{};
    uint8_t soundtimer{};
    uint8_t keypad[16]{};
    uint32_t video[64 * 32]{};
    uint16_t opcode;

    std::default_random_engine randGen;
    std::uniform_int_distribution<uint16_t> randByte;

    typedef void (Chip8::*Chip8Func)();
    Chip8Func table[0xF + 1];
    Chip8Func table0[0xE + 1];
    Chip8Func table8[0xE + 1];
    Chip8Func tableE[0xE + 1];
    Chip8Func tableF[0x65 + 1];

    void LoadRom(char const*);
    void Cycle();
    Chip8():randGen(std::chrono::system_clock::now().time_since_epoch().count()){
        randByte = std::uniform_int_distribution<uint16_t>(0, 255U);
        
        pc = STARTADDR;

        for(long i = 0; i < FONTSET_SIZE; i++){
            MEMORY[FONTADDR + i] = fontset[i];
        }

        table[0] = &Chip8::Table0;
        table[1] = &Chip8::OP_1nnn;
        table[2] = &Chip8::OP_2nnn;
        table[3] = &Chip8::OP_3xkk;
        table[4] = &Chip8::OP_4xkk;
        table[5] = &Chip8::OP_5xy0;
        table[6] = &Chip8::OP_6xkk;
        table[7] = &Chip8::OP_7xkk;
        table[8] = &Chip8::Table8;
        table[9] = &Chip8::OP_9xy0;
        table[10] = &Chip8::OP_Annn;
        table[11] = &Chip8::OP_Bnnn;
        table[12] = &Chip8::OP_Cxkk;
        table[13] = &Chip8::OP_Dxyn;
        table[14] = &Chip8::TableE;
        table[15] = &Chip8::TableF;

        for(int i = 0; i <= 0xE; i++){
            table0[i] = &Chip8::NullOP;
            table8[i] = &Chip8::NullOP;
            tableE[i] = &Chip8::NullOP;
        }

        table0[0] = &Chip8::OP_00E0;
        table0[0xE] = &Chip8::OP_00EE;

        table8[0x0] = &Chip8::OP_8xy0;
        table8[0x1] = &Chip8::OP_8xy1;
        table8[0x2] = &Chip8::OP_8xy2;
        table8[0x3] = &Chip8::OP_8xy3;
        table8[0x4] = &Chip8::OP_8xy4;
        table8[0x5] = &Chip8::OP_8xy5;
        table8[0x6] = &Chip8::OP_8xy6;
        table8[0x7] = &Chip8::OP_8xy7;
        table8[0xE] = &Chip8::OP_8xyE;

        tableE[0x1] = &Chip8::OP_ExA1;
        tableE[0xE] = &Chip8::OP_Ex9E;

        for(int i = 0; i <= 0x65; i++){
            tableF[i] = &Chip8::NullOP;
        }

        tableF[0x07] = &Chip8::OP_Fx07;
        tableF[0x0A] = &Chip8::OP_Fx0A;
        tableF[0x15] = &Chip8::OP_Fx15;
        tableF[0x18] = &Chip8::OP_Fx18;
        tableF[0x1E] = &Chip8::OP_Fx1E;
        tableF[0x29] = &Chip8::OP_Fx29;
        tableF[0x33] = &Chip8::OP_Fx33;
        tableF[0x55] = &Chip8::OP_Fx55;
        tableF[0x65] = &Chip8::OP_Fx65;



    }

    void Table0();
    void Table8();
    void TableE();
    void TableF();
    void NullOP();

        //*****************OPCODE FUNCTIONALITY*********************

    //00E0 (CLS): clear screen
    void OP_00E0();
    //00EE (RET): return from subroutine
    void OP_00EE();
    //1nnn (JMP): jump to address nnn
    void OP_1nnn();
    //2nnn (CALL): call subroutine (like JSR)
    void OP_2nnn();
    //3xkk (SE Vx): skip next instruction if Rx = kk
    void OP_3xkk();
    //4xkk (SNE Vx): skip next instruction if Rx != kk
    void OP_4xkk();
    //5xy0 (SE Vx, Vy): skip next instruction if Rx = Ry
    void OP_5xy0();
    //6xkk (LD): set Rx = kk
    void OP_6xkk();
    //7xkk (ADD): Rx + kk = Rx
    void OP_7xkk();
    //8xy0 (MOV): Rx = Ry
    void OP_8xy0();
    //8xy1 (OR): Rx = Rx OR Ry
    void OP_8xy1();
    //8xy2 (AND): Rx = Rx AND Ry
    void OP_8xy2();
    //8xy3 (XOR): Rx = Rx XOR Ry
    void OP_8xy3();
    //8xy4 (ADD with OF flag): Rx = Rx + Ry, Rf = 1 if overflow
    void OP_8xy4();
    //8xy5 (SUB): Rx = Rx - Ry, Rf = 1 if Rx > Ry, 0 otherwise
    void OP_8xy5();
    //8xy6 (SHR Vx): Rx is shifted right 1 bit (/2), LSB is in Rf
    void OP_8xy6();
    //8xy7 (SUBN): Rx = Ry - Rx, Rf = 1 if Ry > Rx, 0 otherwise
    void OP_8xy7();
    //8xyE (SHL): Rx is shifted left 1 bit (*2), MSB is in Rf
    void OP_8xyE();
    //9xy0 (SNE): skip next instruction if Rx != Ry
    void OP_9xy0();
    //Annn: set Index to nnn
    void OP_Annn();
    //Bnnn: jump to nnn + R0
    void OP_Bnnn();
    //Cxkk: set Rx to RandByte AND kk
    void OP_Cxkk();
    //Dxyn: display n-byte sprite at mem location index = (Rx, Ry), setting Vf if there is a collision
    void OP_Dxyn();
    //Ex9E: skip next instruction if key with value Rx is pressed
    void OP_Ex9E();
    //ExA1: skip next instruction if key with value Rx is NOT pressed
    void OP_ExA1();
    //Fx07: set Rx to the delay timer value
    void OP_Fx07();
    //Fx0A: wait for keypress and then store value of key in Rx
    void OP_Fx0A();
    //Fx15: set delay timer = Rx
    void OP_Fx15();
    //Fx18: set sound timer = Rx
    void OP_Fx18();
    //Fx1E: I += Rx
    void OP_Fx1E();
    //Fx29: set I = location of sprite for digit Rx
    void OP_Fx29();
    //Fx33: Rx = decimal; hundreds in I, tens in I+1, ones in I+1
    void OP_Fx33();
    //Fx55: store R0-Rx in memory addresses starting at I
    void OP_Fx55();
    //Fx65: load R0-Rx from memory addresses starting at I
    void OP_Fx65();



};