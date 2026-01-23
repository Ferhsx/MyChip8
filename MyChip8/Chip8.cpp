#include "Chip8.h"
#include <fstream>
#include <iostream>
#include <cstring>

const unsigned int START_ADDRESS = 0x200;

Chip8::Chip8() {

	pc = START_ADDRESS;

	memset(memory, 0, sizeof(memory));
	memset(registers, 0, sizeof(registers));
	memset(video, 0, sizeof(video));
	memset(stack, 0, sizeof(stack));
	memset(keypad, 0, sizeof(keypad));

	index = 0;
	sp = 0;
	delayTimer = 0;
	soundTimer = 0;
	opcode = 0;
}

void Chip8::loadROM(const char* filename) {
	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	if (file.is_open()) {
		std::streampos size = file.tellg();
		char* buffer = new char[size];
		file.seekg(0, std::ios:: beg);

		file.read(buffer, size);
		file.close();

		for (long i = 0; i < size; ++i) {
			memory[START_ADDRESS + i] = buffer[i];
		}
		delete[] buffer;
	}
}

void Chip8::cycle() {
    // --- 1. FETCH (BUSCAR) ---
    // Pega o byte no PC e o próximo byte, junta os dois para formar o Opcode de 16 bits.
    opcode = (memory[pc] << 8) | memory[pc + 1];

    // Incrementa o PC para a próxima instrução (preparando para o próximo ciclo)
    pc += 2;

    // --- 2. DECODE & EXECUTE (DECODIFICAR E EXECUTAR) ---
    // O CHIP-8 tem 35 opcodes. Vamos identificar qual é usando Switch/Case.
    // Opcodes são identificados pelos seus primeiros 4 bits (Nibble).
    // Exemplo: 0xA2F0. Queremos isolar o 'A'.
    // Fazemos isso com um AND lógico (&) com 0xF000.

    switch (opcode & 0xF000) {

    case 0x0000:
        switch (opcode & 0x000F) {
        case 0x0000: // 00E0: CLS (Clear Screen)
            // Código para limpar a tela vai aqui
            // memset(video, 0, sizeof(video));
            break;

        case 0x000E: // 00EE: RET (Return from subroutine)
            // Código para voltar da subrotina vai aqui
            // pc = stack[--sp];
            break;
        }
        break;

    case 0x1000: // 1NNN: JP addr (Jump to address NNN)
        // O código é 1NNN. Queremos pegar o NNN.
        // Ex: 1200 -> Pula para 200.
        // Usamos uma máscara 0x0FFF para pegar os últimos 12 bits.
        pc = opcode & 0x0FFF;
        break;

    case 0x6000: // 6XNN: LD Vx, byte (Set Register Vx = NN)
        // Ex: 62FF -> Coloca o valor FF no registrador V2.
        // Precisamos descobrir quem é X (o registrador) e quem é NN (o valor).

        // (opcode & 0x0F00) >> 8  -> Pega o segundo nibble e joga pra direita.
        // Ex: 62FF & 0F00 = 0200. >> 8 = 2. Achamos o índice do registrador!
        registers[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
        break;

    case 0x7000: // 7XNN: ADD Vx, byte (Set Vx = Vx + NN)
        // Ex: 7210 -> Soma 10 ao valor que já está em V2.
        registers[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
        break;

    case 0xA000: // ANNN: LD I, addr (Set Index Register I = NNN)
        index = opcode & 0x0FFF;
        break;

        // ... existem mais opcodes, vamos implementar aos poucos ...

    default:
        std::cout << "Opcode desconhecido: 0x" << std::hex << opcode << std::endl;
    }

    // --- 3. ATUALIZAR TIMERS ---
    if (delayTimer > 0) {
        --delayTimer;
    }

    if (soundTimer > 0) {
        if (soundTimer == 1) {
            std::cout << "BEEP!" << std::endl; // Som simples
        }
        --soundTimer;
    }
}