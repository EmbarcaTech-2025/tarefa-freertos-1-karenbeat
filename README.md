# Tarefa: Roteiro de FreeRTOS #1 - EmbarcaTech 2025

Autor: **Karen Beatrice Souza Gonçalves**

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Brasília, junho de 2025

---

<!-- README principal -->

## 🎯 Objetivo do Repositório

Desenvolver um sistema multitarefa embarcado utilizando FreeRTOS e Pico SDK na placa BitDogLab (RP2040), controlando um LED RGB, um buzzer e botões de forma concorrente.


## 💡 Visão Geral do Projeto

Este repositório contém o projeto **FreeRTOSpico**, que implementa:

* **LED RGB**: alterna ciclicamente entre vermelho, verde e azul a cada 500 ms
* **Buzzer**: emite um beep de 100 ms a cada 1 segundo
* **Botão A**: suspende/retoma a tarefa do LED RGB
* **Botão B**: suspende/retoma a tarefa do buzzer

## 📁 Estrutura de Pastas

```plaintext
FreeRTOSpico/
├── CMakeLists.txt            # Configuração do build e CMake
├── include/
│   └── FreeRTOSConfig.h      # Configurações do FreeRTOS
├── src/
│   └── main.c                # Código principal com tarefas
└── FreeRTOS-Kernel-main/     # Source do kernel FreeRTOS
    ├── include/              # Headers do FreeRTOS
    ├── croutine.c
    └── portable/...          # Port para RP2040 e heap_4.c
```

## 🔧 Componentes Utilizados

* **Placa**: BitDogLab (Raspberry Pi Pico W)
* **FreeRTOS**: Kernel do FreeRTOS (Development branch)
* **Pico SDK**: Raspberry Pi Pico SDK v2.1.0
* **Toolchain**: ARM GCC (arm-none-eabi-gcc)
* **Ambiente**: VSCode + CMake + Ninja

## ⚙️ Como Compilar e Executar

1. No terminal, execute:

   ```bash
   rm -rf build && mkdir build && cd build
   cmake ..
   make -j4
   ```
2. Conecte a placa em modo bootloader (pressione BOOTSEL ao conectar USB).
3. Copie `build/FreeRTOSpico.uf2` para o drive que aparecer.
4. A placa reiniciará com o firmware carregado.

## ⚡ Pinagem dos Dispositivos

| Periférico   | GPIO | Função                   |
| ------------ | ---- | ------------------------ |
| LED Vermelho | 13   | Output, anodo vermelho   |
| LED Verde    | 11   | Output, anodo verde      |
| LED Azul     | 12   | Output, anodo azul       |
| Buzzer       | 10   | Output digital           |
| Botão A      | 5    | Input pull-up, ativo LOW |
| Botão B      | 6    | Input pull-up, ativo LOW |

## 🖼 Exemplos de Funcionamento

<!-- Insira capturas de tela ou fotos do projeto em execução, se disponível -->

## 📈 Resultados Esperados

* LED RGB alternando sem falhas
* Buzzer emitindo beeps periódicos
* Botões habilitando/desabilitando tarefas conforme pressionados

---

## 📜 Licença

GNU GPL-3.0

---

> Veja também o [Manual de Boas Práticas no GitHub](docs/1_3_Manual_de_Boas_Práticas_no_GitHub.pdf)
