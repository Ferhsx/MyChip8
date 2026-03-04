#include "Chip8.h"
#include <SFML/Graphics.hpp>
#include <iostream>
// Configurações da Tela
const int VIDEO_WIDTH = 64;
const int VIDEO_HEIGHT = 32;
const int VIDEO_SCALE = 15; // Zoom de 15x (Janela será 960x480)

int main() {
    // 1. Cria a janela
    sf::RenderWindow window(sf::VideoMode(VIDEO_WIDTH * VIDEO_SCALE, VIDEO_HEIGHT * VIDEO_SCALE), "Emulador CHIP-8");

        Chip8 chip8;

    chip8.memory[0x200] = 0x00;
    chip8.memory[0x201] = 0xE0;

    // 2. Prepara a Textura (O "Canvas" da GPU)
    sf::Texture texture;
    texture.create(VIDEO_WIDTH, VIDEO_HEIGHT); // Cria uma textura de 64x32

    // 3. Prepara o Sprite (O "Quadro" que segura o Canvas)
    sf::Sprite sprite(texture);
    sprite.setScale(VIDEO_SCALE, VIDEO_SCALE); // Estica para caber na janela

    // INJEÇÃO DE TESTE: Vamos sujar a memória de vídeo manualmente para ver se funciona
    chip8.memory[0x400] = 0xF0;
    chip8.memory[0x401] = 0x80;
    chip8.memory[0x402] = 0xF0;
    chip8.memory[0x403] = 0x80;
    chip8.memory[0x404] = 0x80;


    //configura os registradores
    //A000: I = 400 (aponta para onde guardamos os sprite)

    chip8.memory[0x200] = 0xA4;
    chip8.memory[0x201] = 0x00;
    
    //6000: V0 = 10 (posição X)
    chip8.memory[0x202] = 0x60;
    chip8.memory[0x203] = 0x0A;

    //6100: V1 = 10 (posição Y)
    chip8.memory[0x204] = 0x61;
    chip8.memory[0x205] = 0x0A;

    //D015: DRW V0, V1, 5 (Desenha na pos X=V0, Y=V1, altura=5 bytes)

    chip8.memory[0x206] = 0xD0;
    chip8.memory[0x207] = 0x15;

    // opcional mas é legal, um loop infinito no final para não crashar lendo lixo
    //1208: JP 208
    chip8.memory[0x208] = 0x12;
    chip8.memory[0x209] = 0x08;


    // Loop Principal
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        //colocando teclados eehhhhh
        //mapeando os qwerty para o hex keypa d do chip-8

        //1 2 3 4 -> 1 2 3 C

        chip8.keypad[0x1] = sf::Keyboard::isKeyPressed(sf::Keyboard::Num1);
        chip8.keypad[0x2] = sf::Keyboard::isKeyPressed(sf::Keyboard::Num2);
        chip8.keypad[0x3] = sf::Keyboard::isKeyPressed(sf::Keyboard::Num3);
        chip8.keypad[0xC] = sf::Keyboard::isKeyPressed(sf::Keyboard::Num4);

        // Q W E R -> 4 5 6 D
        
        chip8.keypad[0x4] = sf::Keyboard::isKeyPressed(sf::Keyboard::Q);
        chip8.keypad[0x5] = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
        chip8.keypad[0x6] = sf::Keyboard::isKeyPressed(sf::Keyboard::E);
        chip8.keypad[0xD] = sf::Keyboard::isKeyPressed(sf::Keyboard::R);

        //A S D F -> 7 8 9 E

        chip8.keypad[0x7] = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
        chip8.keypad[0x8] = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
        chip8.keypad[0x9] = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
        chip8.keypad[0xE] = sf::Keyboard::isKeyPressed(sf::Keyboard::F);

        //Z X C V -> A 0 B F
        chip8.keypad[0xA] = sf::Keyboard::isKeyPressed(sf::Keyboard::Z);
        chip8.keypad[0x0] = sf::Keyboard::isKeyPressed(sf::Keyboard::X);
        chip8.keypad[0xB] = sf::Keyboard::isKeyPressed(sf::Keyboard::C);
        chip8.keypad[0xF] = sf::Keyboard::isKeyPressed(sf::Keyboard::V);


        // --- A MÁGICA ACONTECE AQUI ---

        // 1. Roda um ciclo da CPU (Fetch/Decode/Execute)
        chip8.cycle();

        // 2. Atualiza a Textura da SFML com os dados do CHIP-8
        // O método .update espera um array de pixels (uint8* ou uint32*)
        texture.update((uint8_t*)chip8.video);

        // 3. Desenha
        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.display();
    }

    return 0;
}