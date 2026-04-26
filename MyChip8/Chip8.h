#pragma once
#include <cstdint>

class Chip8 {
public:
    Chip8();
    void loadROM(const char* filename);
    void cycle();

    // A Tela e o Teclado são públicos para o main.cpp poder ler e escrever neles
    uint32_t video[64 * 32];
    uint8_t keypad[16];

private:
    uint8_t memory[4096];
    uint8_t registers[16]; // V0 a VF (Os "baldes")
    uint16_t index;        // O apontador de memória
    uint16_t pc;           // O "dedo" que lê o código

    uint16_t stack[16];    // A pilha de retornos
    uint8_t sp;            // O ponteiro da pilha

    uint8_t delayTimer;
    uint8_t soundTimer;

    uint16_t opcode;
};