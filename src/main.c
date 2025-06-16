#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

// Definição dos pinos do hardware (BitDogLab Pico W)
#define LED_R_PIN    13    // LED RGB - componente Vermelho (GPIO13):contentReference[oaicite:1]{index=1}
#define LED_G_PIN    11    // LED RGB - componente Verde (GPIO11):contentReference[oaicite:2]{index=2}
#define LED_B_PIN    12    // LED RGB - componente Azul  (GPIO12):contentReference[oaicite:3]{index=3}
#define BUZZER_PIN   10    // Buzzer (GPIO10, conforme design da BitDogLab)
#define BUTTON_A_PIN 5     // Botão A (GPIO5):contentReference[oaicite:4]{index=4}
#define BUTTON_B_PIN 6     // Botão B (GPIO6):contentReference[oaicite:5]{index=5}

// Constantes de temporização (em milissegundos)
#define LED_DELAY_MS       500   // intervalo de troca de cor do LED RGB (500ms)
#define BUZZER_ON_MS       100   // duração do beep do buzzer (100ms ligado)
#define BUZZER_OFF_MS      900   // intervalo silencioso do buzzer (900ms desligado, total 1s)
#define BUTTON_POLL_DELAY  100   // intervalo de polling dos botões (100ms)

// Handles (referências) das tarefas para controle de suspensão/retomada
static TaskHandle_t ledTaskHandle   = NULL;
static TaskHandle_t buzzerTaskHandle = NULL;

// Declaração das funções de tarefa (protótipos)
void led_task(void *pvParameters);
void buzzer_task(void *pvParameters);
void button_task(void *pvParameters);

int main() {
    // Inicialização básica do Pico (UART/USB stdout, se necessário)
    stdio_init_all();

    // Criação das tarefas FreeRTOS:
    // Tarefa LED RGB (alterna cores a cada 500ms), prioridade baixa (1)
    xTaskCreate(led_task, "LED_Task", 256, NULL, 1, &ledTaskHandle);
    // Tarefa Buzzer (beep periódico 1s, com 100ms ligado), prioridade baixa (1)
    xTaskCreate(buzzer_task, "Buzzer_Task", 256, NULL, 1, &buzzerTaskHandle);
    // Tarefa Botões (monitora botões a cada 100ms), prioridade mais alta (2) para melhor responsividade
    xTaskCreate(button_task, "Button_Task", 256, NULL, 2, NULL);

    // Inicia o agendador do FreeRTOS (não retorna se tudo estiver correto)
    vTaskStartScheduler();

    // Se o scheduler for finalizado (por falta de memória, por exemplo), entra em loop infinito
    while (true) {
        // Opcional: poderia colocar um log de erro aqui
    }

    return 0;  // (Nunca deve chegar aqui)
}

// Tarefa: LED RGB piscando ciclicamente entre vermelho, verde e azul
void led_task(void *pvParameters) {
    (void) pvParameters;  // evita warning "unused parameter"
    // Configura os pinos do LED RGB como saída e inicializa desligados
    gpio_init(LED_R_PIN);
    gpio_init(LED_G_PIN);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);
    gpio_put(LED_R_PIN, 0);
    gpio_put(LED_G_PIN, 0);
    gpio_put(LED_B_PIN, 0);

    for (;;) {
        // Acende LED Vermelho, apaga os outros
        gpio_put(LED_R_PIN, 1);
        gpio_put(LED_G_PIN, 0);
        gpio_put(LED_B_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(LED_DELAY_MS));  // espera 500 ms

        // Acende LED Verde, apaga os outros
        gpio_put(LED_R_PIN, 0);
        gpio_put(LED_G_PIN, 1);
        gpio_put(LED_B_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(LED_DELAY_MS));  // espera 500 ms

        // Acende LED Azul, apaga os outros
        gpio_put(LED_R_PIN, 0);
        gpio_put(LED_G_PIN, 0);
        gpio_put(LED_B_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(LED_DELAY_MS));  // espera 500 ms

        // (Loop volta ao início alternando novamente as cores)
    }
}

// Tarefa: Buzzer emitindo "beeps" periódicos (100ms ligado, 900ms desligado)
void buzzer_task(void *pvParameters) {
    (void) pvParameters;  // evita warning "unused parameter"
    // Configura o pino do buzzer como saída e garante que inicia desligado
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_put(BUZZER_PIN, 0);

    for (;;) {
        // Liga o buzzer por 100ms (beep)
        gpio_put(BUZZER_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(BUZZER_ON_MS));   // espera 100 ms

        // Desliga o buzzer por 900ms (silêncio)
        gpio_put(BUZZER_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(BUZZER_OFF_MS));  // espera 900 ms

        // (Loop repete a cada 1 segundo no total)
    }
}

// Tarefa: Botões de controle (A para LED, B para buzzer)
// Suspende ou retoma as tarefas do LED e do buzzer conforme o estado dos botões
void button_task(void *pvParameters) {
    (void) pvParameters;  // evita warning "unused parameter"
    // Configura os pinos dos botões como entrada com pull-up interno
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    // Variáveis para armazenar o último estado lido dos botões (0 = pressionado, 1 = solto)
    bool lastA = false;
    bool lastB = false;

    for (;;) {
        // Leitura dos botões (ativo em nível baixo: 0 quando pressionado)
        bool pressedA = (gpio_get(BUTTON_A_PIN) == 0);
        bool pressedB = (gpio_get(BUTTON_B_PIN) == 0);

        // Botão A: se foi pressionado agora e antes estava solto, suspende tarefa LED
        if (pressedA && !lastA) {
            vTaskSuspend(ledTaskHandle);
        }
        // Se foi solto agora e antes estava pressionado, retoma tarefa LED
        if (!pressedA && lastA) {
            vTaskResume(ledTaskHandle);
        }

        // Botão B: se foi pressionado agora e antes estava solto, suspende tarefa Buzzer
        if (pressedB && !lastB) {
            vTaskSuspend(buzzerTaskHandle);
        }
        // Se foi solto agora e antes estava pressionado, retoma tarefa Buzzer
        if (!pressedB && lastB) {
            vTaskResume(buzzerTaskHandle);
        }

        // Atualiza os estados anteriores para a próxima iteração
        lastA = pressedA;
        lastB = pressedB;

        // Aguarda 100ms antes da próxima leitura (polling)
        vTaskDelay(pdMS_TO_TICKS(BUTTON_POLL_DELAY));
    }
}
