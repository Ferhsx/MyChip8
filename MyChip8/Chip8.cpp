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
		std::streampos sizePos = file.tellg();
        long size = (long)sizePos;

		char* buffer = new char[size];
		file.seekg(0, std::ios:: beg);

		file.read(buffer, size);
		file.close();

		for (long i = 0; i < size; ++i) {
            if((START_ADDRESS + i) < 4096){
                memory[START_ADDRESS + i] = buffer[i];
            }
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
            memset(video, 0, sizeof(video));
            break;

        case 0x000E: // 00EE: RET (Return from subroutine)
            // Código para voltar da subrotina vai aqui
            if (sp > 0) {
                sp--;
                pc = stack[sp];
            }
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

    case 0xD000: { //DXYN: Draw sprite at (Vx, Vy) with height N

        //Pegamos as coordenadas X e Y dos registradores
        uint8_t Vx = (opcode & 0x0F00) >> 8;
        uint8_t Vy = (opcode & 0x00F0) >> 4;

        uint8_t xPos = registers[Vx];
        uint8_t yPos = registers[Vy];

        uint8_t height = opcode & 0x000F;

        // resetar o registrador de colisão (VF) para 0

        registers[0xF] = 0;

        // loop para cada linha do sprite (altura)

        for (unsigned int row = 0; row < height; ++row) {
            uint8_t spriteByte = memory[index + row];

            //loop para cada coluna (pixel) do sprite (sempre 8 bits)

            for (unsigned int col = 0; col < 8; ++col) {
                uint8_t spritePixel = spriteByte & (0x80 >> col);

                //se o pixel  do prite for 1 (aceso), precisamos desenhar
                if (spritePixel != 0) {
                    //calcula a posição exata no array de video 1d

                    //(y * largura) + x
                    //o %64 e %32 fazem o sprite "dar a volta" na tela se sair da borda (Wrapping)
                    int screenIndex = ((xPos + col) % 64) + (((yPos + row) % 32) * 64);

                    //5 verifica colisão 
                    //se o pixel na tela já é branco (0xFFFFFFFF), vamos apagá-lo.

                    if (video[screenIndex] == 0xFFFFFFFF) {
                        registers[0xF] = 1; //dectação de colisão
                    }

                    //Desenha agora usando XOR (^)
                    // se era preto(0) ^ Branco(1) -> branco(1)
                    //se era branco(1) ^ branco(1) -> preto(0)

                    video[screenIndex] ^= 0xFFFFFFFF;

                }
            }
        }
    }

    break;

     

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