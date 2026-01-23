#include "Chip8.h"
#include <iostream>
#include <thread> // Para sleep (opcional, para não rodar rápido demais)
#include <chrono>

int main() {
    Chip8 chip8;

    // GAMBIARRA EDUCACIONAL: Vamos injetar código na memória manualmente
    // em vez de carregar um arquivo, só para testar o ciclo.

    // Instrução 1: 0xA2F0 -> Define o Index (I) para 0x2F0
    // Lembre-se: Memória começa em 0x200
    // chip8.memory é privado, então por enquanto vamos ter que confiar na lógica
    // ou tornar memory publico temporariamente para testar.

    // Nota: Para este teste, vá no Chip8.h e mova 'memory' para 'public' rapidinho.

    chip8.memory[0x200] = 0xA2;
    chip8.memory[0x201] = 0xF0;

    // Instrução 2: 0x1200 -> Jump para 0x200 (Loop infinito)
    chip8.memory[0x202] = 0x12;
    chip8.memory[0x203] = 0x00;

    // Loop principal de emulação
    for (int i = 0; i < 5; ++i) { // Rodar 10 ciclos
        chip8.cycle();

        std::cout << "Ciclo: " << i
            << " | PC: 0x" << std::hex << chip8.pc
            << " | Opcode Lido: 0x" << std::hex << chip8.opcode
            << " | Reg I: 0x" << std::hex << chip8.index
            << std::endl;
    }

    // Segura a tela aberta
    std::cin.get();
    return 0;
}