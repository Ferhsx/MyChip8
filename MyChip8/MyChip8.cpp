#include <iostream>
#include "Chip8.h"
#include <SFML/Graphics.hpp> // Biblioteca Gráfica

int main() {
    // Configura uma janela de 640x320 pixels
    // (O Chip8 original é 64x32, então multiplicamos por 10 para ver melhor)
    sf::RenderWindow window(sf::VideoMode(640, 320), "Emulador Chip-8");

    Chip8 chip8;

    // Loop do Jogo (Game Loop)
    while (window.isOpen()) {
        // 1. Verifica eventos (como clicar no X para fechar)
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // 2. Atualiza a CPU (Emulação)
        chip8.cycle();

        // 3. Desenha na tela (Renderização)
        window.clear(sf::Color::Black); // Limpa com preto

        // (Aqui depois vamos desenhar os pixels do Chip8)

        // Exemplo: Desenhando um quadrado branco só para testar
        sf::RectangleShape rect(sf::Vector2f(50, 50));
        rect.setFillColor(sf::Color::White);
        rect.setPosition(100, 100);
        window.draw(rect);

        window.display(); // Mostra o frame
    }

    return 0;
}