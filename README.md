# 👾 Emulador CHIP-8 em C++

Um emulador completo da máquina virtual CHIP-8 construído do zero em **C++** moderno, utilizando a biblioteca gráfica **SFML** para renderização de vídeo e captura de input. 

Este projeto foi desenvolvido para aprofundar conhecimentos em arquitetura de computadores, manipulação de memória, operações bitwise (bitwise operations) e o ciclo fundamental de uma CPU (Fetch-Decode-Execute).

---

### 🚀 Funcionalidades

*   **Emulação de CPU Completa:** Suporte a todos os 35 opcodes originais do CHIP-8.
*   **Renderização Gráfica:** Display monocromático nativo de 64x32 escalado em tempo real via SFML.
*   **Sistema de Colisão:** Lógica fiel de detecção de colisão baseada em operações `XOR`.
*   **Timers Independentes:** Emulação precisa do Delay Timer e Sound Timer a 60Hz.
*   **Input Hexadecimal:** Mapeamento do teclado original de 16 teclas (0-F) para o padrão QWERTY moderno.

### 🛠️ Tecnologias Utilizadas

*   **C++ (Padrão Moderno)**
*   **SFML (Simple and Fast Multimedia Library)** para I/O e Gráficos.
*   **Visual Studio (MSVC)** para compilação e gerenciamento de dependências.

---

### 🧠 Sob o Capô (Como funciona)

O CHIP-8 não é um hardware físico, mas sim uma máquina virtual concebida nos anos 70. Este emulador replica suas entranhas via software:

1.  **Memória:** Um array simples de 4096 bytes (4KB), onde a ROM é carregada a partir do endereço `0x200`.
2.  **Registradores:** 16 registradores de 8 bits (`V0`-`VF`) e 1 registrador de índice de 16 bits (`I`).
3.  **Ciclo de Instrução:** A cada iteração do game loop, a máquina virtual:
    *   **Busca (Fetch):** Lê 2 bytes sequenciais da memória guiados pelo *Program Counter* (`PC`).
    *   **Decodifica (Decode):** Aplica máscaras de bits (bitmasks) e shifts para identificar o opcode.
    *   **Executa (Execute):** Altera os estados dos registradores, memória ou atualiza o buffer de vídeo.

---

### 🎮 Mapeamento de Controles

O teclado original do CHIP-8 foi mapeado para a metade esquerda do teclado moderno:

| CHIP-8 | Teclado PC |
| :---: | :---: |
| 1 2 3 C | 1 2 3 4 |
| 4 5 6 D | Q W E R |
| 7 8 9 E | A S D F |
| A 0 B F | Z X C V |

*(Nota: Alguns jogos, como Pong, exigem testes para encontrar as teclas certas, geralmente `1`/`Q` para o jogador 1).*

---

### ⚙️ Como rodar localmente

1. Clone este repositório.
2. Abra a solução `.slnx` ou `.sln` no Visual Studio.
3. Certifique-se de ter as bibliotecas e DLLs da **SFML 2.6+ (x64)** corretamente linkadas (include/lib).
4. Adicione uma ROM do CHIP-8 (ex: `pong.rom`) na raiz do diretório do executável.
5. Modifique a string de carregamento na `main.cpp` (`chip8.loadROM("sua_rom.ch8");`).
6. Compile e jogue!