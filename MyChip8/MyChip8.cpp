#include "Chip8.h"
#include <SFML/Graphics.hpp>
#include <iostream>

const int VIDEO_WIDTH = 64;
const int VIDEO_HEIGHT = 32;
const int VIDEO_SCALE = 15;

int main() {
    sf::RenderWindow window(sf::VideoMode(VIDEO_WIDTH * VIDEO_SCALE, VIDEO_HEIGHT * VIDEO_SCALE), "Emulador CHIP-8");
    window.setFramerateLimit(60); // Evita que rode a 1 milhão de frames por segundo

    Chip8 chip8;

    // TODO: COLOQUE O CAMINHO DE UMA ROM AQUI (Vamos testar com PONG)
    chip8.loadROM("Space Invaders [David Winter].ch8");

    sf::Texture texture;
    texture.create(VIDEO_WIDTH, VIDEO_HEIGHT);

    sf::Sprite sprite(texture);
    sprite.setScale(VIDEO_SCALE, VIDEO_SCALE);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // MAPEAMENTO DO TECLADO
        chip8.keypad[0x1] = sf::Keyboard::isKeyPressed(sf::Keyboard::Num1);
        chip8.keypad[0x2] = sf::Keyboard::isKeyPressed(sf::Keyboard::Num2);
        chip8.keypad[0x3] = sf::Keyboard::isKeyPressed(sf::Keyboard::Num3);
        chip8.keypad[0xC] = sf::Keyboard::isKeyPressed(sf::Keyboard::Num4);

        chip8.keypad[0x4] = sf::Keyboard::isKeyPressed(sf::Keyboard::Q);
        chip8.keypad[0x5] = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
        chip8.keypad[0x6] = sf::Keyboard::isKeyPressed(sf::Keyboard::E);
        chip8.keypad[0xD] = sf::Keyboard::isKeyPressed(sf::Keyboard::R);

        chip8.keypad[0x7] = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
        chip8.keypad[0x8] = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
        chip8.keypad[0x9] = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
        chip8.keypad[0xE] = sf::Keyboard::isKeyPressed(sf::Keyboard::F);

        chip8.keypad[0xA] = sf::Keyboard::isKeyPressed(sf::Keyboard::Z);
        chip8.keypad[0x0] = sf::Keyboard::isKeyPressed(sf::Keyboard::X);
        chip8.keypad[0xB] = sf::Keyboard::isKeyPressed(sf::Keyboard::C);
        chip8.keypad[0xF] = sf::Keyboard::isKeyPressed(sf::Keyboard::V);

        // O CHIP-8 original rodava a uns 500-700 Hz. O monitor (SFML) roda a 60 Hz.
        // Portanto, executamos umas 10 instruções da CPU para cada 1 desenho da tela.
        for (int i = 0; i < 10; ++i) {
            chip8.cycle();
        }

        chip8.updateTimers();

        // Atualiza a tela
        texture.update((uint8_t*)chip8.video);
        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.display();
    }

    return 0;
}