#pragma once
#include <cstdint> // Necessário para usar uint8_t, uint16_t, etc.
#include <random>  // Para gerar números aleatórios

class Chip8 {
public:
    // Construtor: Inicializa a máquina
    Chip8();

    // Carrega um jogo (ROM) na memória
    void loadROM(const char* filename);

    // Executa um ciclo de CPU (Fetch, Decode, Execute)
    void cycle();

    // Teclado (Keypad): 16 teclas. true = pressionada, false = solta
    uint8_t keypad[16];

    // Display: 64 * 32 pixels. 
    // uint32_t para facilitar o uso com bibliotecas gráficas (0 ou 1)
    uint32_t video[64 * 32];

    uint8_t memory[4096];

    uint16_t pc;

    uint16_t index;

    uint16_t opcode;

private:
    // --- O CORAÇÃO DA MÁQUINA ---

    // Memória: 4KB (4096 bytes)
    

    // Registradores: 16 registradores de 8 bits (V0 até VF)
    uint8_t registers[16];

    // Index Register (I): Aponta para locais na memória. 16 bits.
    

    // Program Counter (PC): Aponta para a instrução atual. 16 bits.
    

    // Pilha (Stack): Lembra endereços de retorno (até 16 níveis de profundidade)
    uint16_t stack[16];

    // Stack Pointer (SP): Aponta para o topo da pilha
    uint8_t sp;

    // Timers: O CHIP-8 tem dois cronômetros que diminuem a 60Hz
    uint8_t delayTimer;
    uint8_t soundTimer;

    // Opcode atual: A instrução que estamos processando (2 bytes / 16 bits)
    
};