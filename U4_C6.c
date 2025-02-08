#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "hardware/timer.h"

#include "hardware/pio.h"
#include "ws2812.pio.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define RED_LED 13
#define GREEN_LED 11
#define BLUE_LED 12
#define BUTTON_A 5
#define BUTTON_B 6
#define NUM_LEDS 25
#define PIN 7

static volatile uint32_t last_event = 0;

double number_0[NUM_LEDS] = 
    {0.0, 0.2, 0.2, 0.2, 0.0,
     0.0, 0.2, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.2, 0.2, 0.0};

double number_1[NUM_LEDS] = 
        
    {0.0, 0.0, 0.2, 0.0, 0.0,
     0.0, 0.0, 0.2, 0.0, 0.0,
     0.0, 0.0, 0.2, 0.0, 0.0,
     0.0, 0.2, 0.2, 0.0, 0.0,
     0.0, 0.0, 0.2, 0.0, 0.0};

double number_2[NUM_LEDS] = 
        
    {0.0, 0.2, 0.2, 0.2, 0.0,
     0.0, 0.2, 0.0, 0.0, 0.0,
     0.0, 0.2, 0.2, 0.2, 0.0,
     0.0, 0.0, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.2, 0.2, 0.0};

double number_3[NUM_LEDS] = 
        
    {0.0, 0.2, 0.2, 0.2, 0.0,
     0.0, 0.0, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.2, 0.2, 0.0,
     0.0, 0.0, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.2, 0.2, 0.0,};

double number_4[NUM_LEDS] = 
        
    {0.0, 0.2, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.2, 0.2, 0.0,
     0.0, 0.2, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.0, 0.2, 0.0};

double number_5[NUM_LEDS] = 
        
   {0.0, 0.2, 0.2, 0.2, 0.0,
    0.0, 0.0, 0.0, 0.2, 0.0,
    0.0, 0.2, 0.2, 0.2, 0.0,
    0.0, 0.2, 0.0, 0.0, 0.0,
    0.0, 0.2, 0.2, 0.2, 0.0};

double number_6[NUM_LEDS] = 
        
    {0.0, 0.2, 0.2, 0.2, 0.0,
     0.0, 0.2, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.2, 0.2, 0.0,
     0.0, 0.0, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.2, 0.2, 0.0};

double number_7[NUM_LEDS] = 
        
    {0.0, 0.2, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.0, 0.0, 0.0,
     0.0, 0.2, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.2, 0.2, 0.0};

double number_8[NUM_LEDS] = 
        
    {0.0, 0.2, 0.2, 0.2, 0.0,
     0.0, 0.2, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.2, 0.2, 0.0,
     0.0, 0.2, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.2, 0.2, 0.0};

double number_9[NUM_LEDS] = 
        
    {0.0, 0.2, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.2, 0.2, 0.0,
     0.0, 0.2, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.2, 0.2, 0.0};

double* numbers[] = {number_0, number_1, number_2, number_3, number_4, number_5, number_6, number_7, number_8, number_9};


uint32_t rgb_matrix(double r, double g, double b) { 
    unsigned char R, G, B;
    R = (unsigned char)(r * 255);
    G = (unsigned char)(g * 255);
    B = (unsigned char)(b * 255);
    
    return (G << 24) | (R << 16) | (B << 8);
}

void draw_number(double *number, uint32_t led_value, PIO pio, uint sm, double r, double g, double b) {
    for (int16_t i = 0; i < NUM_LEDS; i++) {
        led_value = rgb_matrix(number[i] * r, number[i] * g, number[i] * b);
        pio_sm_put_blocking(pio, sm, led_value);
    }
}

void button_pressed(uint gpio, uint32_t events){ //Função para tratar a interrupção dos botões
    printf("Botão pressionado: GPIO %d\n", gpio);
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    if(current_time - last_event > 300000){

        last_event = current_time;

        if(gpio == BUTTON_A){
            gpio_put(GREEN_LED, !gpio_get(GREEN_LED));
        }else if(gpio == BUTTON_B){
            gpio_put(BLUE_LED, !gpio_get(BLUE_LED));
        }
    }
}

int main()
{
  stdio_init_all();
    PIO pio = pio0;

    bool clock_config = set_sys_clock_khz(128000, false);

  // Configuração dos pinos dos LEDs como saída
    gpio_init(RED_LED);
    gpio_set_dir(RED_LED, GPIO_OUT);

    gpio_init(GREEN_LED);
    gpio_set_dir(GREEN_LED, GPIO_OUT);

    gpio_init(BLUE_LED);
    gpio_set_dir(BLUE_LED, GPIO_OUT);

    // Configuração dos botões como entrada com pull-up interno ativado
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    
    // Adiciona o programa ao pio às maquinas de estado
    uint offset = pio_add_program(pio, &ws2812_program);
    // Reivindica uma máquina de estado livre para executar o programa PIO
    uint sm = pio_claim_unused_sm(pio, true); 
    // Configura a SM para executar o programa PIO
    ws2812_program_init(pio, sm, offset, PIN, 800000, false); 

     // Configuração da interrupção com callback para botão A
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_pressed); 

    // Configuração da interrupção com callback para botão B
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &button_pressed);
  // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA); // Pull up the data line
    gpio_pull_up(I2C_SCL); // Pull up the clock line
    ssd1306_t ssd; // Inicializa a estrutura do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    bool cor = false;
    ssd1306_fill(&ssd, cor); // Limpa o display
    ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
    ssd1306_draw_string(&ssd, "Digite um char", 0, 30); // Desenha uma string     
    ssd1306_send_data(&ssd); // Atualiza o display
  while (true)
  {
    //cor = !cor;
    // Atualiza o conteúdo do display com animações
    
    if (stdio_usb_connected())
        { // Certifica-se de que o USB está conectado
            char c;
            if (scanf("%c", &c) == 1)
            { // Lê caractere da entrada padrão
                printf("Recebido: '%c'\n", c);
                ssd1306_fill(&ssd, cor);
                ssd1306_draw_char(&ssd, c, 60, 30);
                ssd1306_send_data(&ssd);
                if(isdigit(c)){
                    draw_number(numbers[c - '0'], 0, pio, sm, 0.1, 0.0, 0.3);
                }else{
                    draw_number(numbers[0], 0, pio, sm, 0.0, 0.0, 0.0);
                }
            }
        }

    sleep_ms(1000);
  }
}