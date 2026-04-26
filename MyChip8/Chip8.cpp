#include "Chip8.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <random>

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_START_ADDRESS = 0x50;

// O desenho dos números de 0-9 e A-F na memória (5 bytes por letra)
uint8_t fontset[80] = {
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

Chip8::Chip8() {
    pc = START_ADDRESS;
    memset(memory, 0, sizeof(memory));
    memset(registers, 0, sizeof(registers));
    memset(video, 0, sizeof(video));
    memset(stack, 0, sizeof(stack));
    memset(keypad, 0, sizeof(keypad));
    index = 0; sp = 0; delayTimer = 0; soundTimer = 0; opcode = 0;

    // Carrega a fonte na memória (antes do espaço do jogo)
    for (unsigned int i = 0; i < 80; ++i) {
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }
}

void Chip8::loadROM(const char* filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        std::streampos sizePos = file.tellg();
        long size = (long)sizePos;
        char* buffer = new char[size];
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        for (long i = 0; i < size; ++i) {
            if ((START_ADDRESS + i) < 4096) {
                memory[START_ADDRESS + i] = buffer[i];
            }
        }
        delete[] buffer;
    }
}

void Chip8::cycle() {
    opcode = (memory[pc] << 8) | memory[pc + 1];
    pc += 2;

    // Extrai partes do Opcode para facilitar
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    uint8_t NN = opcode & 0x00FF;
    uint16_t NNN = opcode & 0x0FFF;

    switch (opcode & 0xF000) {
    case 0x0000:
        switch (opcode & 0x000F) {
        case 0x0000: memset(video, 0, sizeof(video)); break; // CLS
        case 0x000E: --sp; pc = stack[sp]; break;            // RET (Volta da Pilha)
        }
        break;

    case 0x1000: pc = NNN; break; // JP (Pulo)

    case 0x2000: // CALL (Pula e salva o endereço na Pilha)
        stack[sp] = pc;
        ++sp;
        pc = NNN;
        break;

    case 0x3000: if (registers[Vx] == NN) pc += 2; break; // Skip If
    case 0x4000: if (registers[Vx] != NN) pc += 2; break; // Skip If Not
    case 0x5000: if (registers[Vx] == registers[Vy]) pc += 2; break;

    case 0x6000: registers[Vx] = NN; break; // Set
    case 0x7000: registers[Vx] += NN; break; // Add

    case 0x8000: // Bloco de Matemática e Lógica
    {
        switch (opcode & 0x000F) {
        case 0x0: registers[Vx] = registers[Vy]; break;
        case 0x1: registers[Vx] |= registers[Vy]; break;
        case 0x2: registers[Vx] &= registers[Vy]; break;
        case 0x3: registers[Vx] ^= registers[Vy]; break;

        case 0x4: // ADD com CARRY (Se transbordar o balde)
        {
            uint16_t sum = registers[Vx] + registers[Vy];
            registers[0xF] = (sum > 255U) ? 1 : 0;
            registers[Vx] = sum & 0xFF;
        } break;

        case 0x5: // SUB (Se precisar pedir emprestado)
        {
            registers[0xF] = (registers[Vx] > registers[Vy]) ? 1 : 0;
            registers[Vx] -= registers[Vy];
        } break;

        case 0x6: // SHIFT RIGHT
        {
            registers[0xF] = registers[Vx] & 0x1;
            registers[Vx] >>= 1;
        } break;

        case 0x7: // SUBN
        {
            registers[0xF] = (registers[Vy] > registers[Vx]) ? 1 : 0;
            registers[Vx] = registers[Vy] - registers[Vx];
        } break;

        case 0xE: // SHIFT LEFT
        {
            registers[0xF] = (registers[Vx] & 0x80) >> 7;
            registers[Vx] <<= 1;
        } break;
        }
    } break;

    case 0x9000: if (registers[Vx] != registers[Vy]) pc += 2; break;

    case 0xA000: index = NNN; break; // Set Index

    case 0xB000: pc = NNN + registers[0]; break;

    case 0xC000: registers[Vx] = (rand() % 256) & NN; break; // Random

    case 0xD000: // DRW (Desenhar o Sprite)
    {
        uint8_t xPos = registers[Vx] % 64;
        uint8_t yPos = registers[Vy] % 32;
        uint8_t height = opcode & 0x000F;

        registers[0xF] = 0; // Reseta colisão

        for (unsigned int row = 0; row < height; ++row) {
            uint8_t spriteByte = memory[index + row];

            for (unsigned int col = 0; col < 8; ++col) {
                uint8_t spritePixel = spriteByte & (0x80 >> col);

                if (spritePixel != 0) {
                    int screenIndex = ((xPos + col) % 64) + (((yPos + row) % 32) * 64);
                    if (video[screenIndex] == 0xFFFFFFFF) {
                        registers[0xF] = 1; // Bateu em algo!
                    }
                    video[screenIndex] ^= 0xFFFFFFFF; // Inverte o pixel
                }
            }
        }
    } break;

    // Opcodes de Teclado
    case 0xE000:
        if ((opcode & 0x00FF) == 0x009E) {
            if (keypad[registers[Vx]]) pc += 2; // Pula se tecla pressionada
        }
        else if ((opcode & 0x00FF) == 0x00A1) {
            if (!keypad[registers[Vx]]) pc += 2; // Pula se tecla SOLTA
        }
        break;

    case 0xF000:
        switch (opcode & 0x00FF) {
        case 0x0007: registers[Vx] = delayTimer; break;
        case 0x0015: delayTimer = registers[Vx]; break;
        case 0x0018: soundTimer = registers[Vx]; break;
        case 0x001E: index += registers[Vx]; break;
        case 0x0029: index = FONTSET_START_ADDRESS + (5 * registers[Vx]); break; // Pega o desenho da letra

        case 0x0033: // BCD: Pega um número (ex: 156) e guarda 1, 5 e 6 na memória
            memory[index] = registers[Vx] / 100;
            memory[index + 1] = (registers[Vx] / 10) % 10;
            memory[index + 2] = registers[Vx] % 10;
            break;

        case 0x0055: // Salva os registradores na memória
            for (uint8_t i = 0; i <= Vx; ++i) memory[index + i] = registers[i];
            break;

        case 0x0065: // Carrega os registradores da memória
            for (uint8_t i = 0; i <= Vx; ++i) registers[i] = memory[index + i];
            break;
        }
        break;
    }
}

// --- TIMERS ---
void Chip8::updateTimers() {
    if (delayTimer > 0) --delayTimer;
    if (soundTimer > 0) --soundTimer;
}