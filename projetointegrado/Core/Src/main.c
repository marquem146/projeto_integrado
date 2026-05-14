/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "st7735\st7735.h"
#include <stdio.h>
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RED   0xF800
#define GREEN 0x07E0
#define BLUE  0x001F
//definições dos botoes:PA9,PA10,PA11 e PA12
#define iniciaraula HAL_GPIO_ReadPin(iniciar_aula_GPIO_Port, iniciar_aula_Pin)
#define quanti HAL_GPIO_ReadPin(quanti_GPIO_Port, quanti_Pin)
#define BA HAL_GPIO_ReadPin(banheiro_agua_GPIO_Port, banheiro_agua_Pin)
#define final HAL_GPIO_ReadPin(final_GPIO_Port, final_Pin)
//definições dos leds:PB3,PB4,PB5 e PB6
#define lediniciar(state) HAL_GPIO_WritePin(led_iniciar_GPIO_Port, led_iniciar_Pin, state)
#define ledquantid(state) HAL_GPIO_WritePin(led_quanti_GPIO_Port,  led_quanti_Pin,  state)
#define ledBA(state)      HAL_GPIO_WritePin(led_BA_GPIO_Port,      led_BA_Pin,      state)
#define ledfinal(state)   HAL_GPIO_WritePin(led_final_GPIO_Port,   led_final_Pin,   state)
#define maxTentativas 3
#define bloqueabase 5000
#define maxalunos 40
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */
uint8_t saida_BA = 0;
uint8_t nao_retornarem = 0;
uint32_t tick_inicio = 0;
uint8_t  digito_atual = 0;
uint8_t  senha[4];
uint8_t  indice       = 0;
uint8_t  senha_correta[4];
uint8_t  qtd_alunos1  = 15;
uint8_t  qtd_alunos2  = 20;
uint8_t  qtdmaxalunos  = 40;
char     buffer_tela[20];
uint32_t timeout      = 500;
uint8_t tentativas = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */
void tela_login(void);
void gerar_senha(void);
void atualiza_senha_display(void);
bool verifica_senha(void);
void inserir_digito(uint8_t d);
void turma(void);
void total_alunos(void);
void encerrar_aula(uint8_t alunos_sala, uint8_t alunos_fora, uint8_t total_saidas);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint32_t last_time = 0;

void tela_login(void)
{
    ST7735_FillScreen(0x0000);
    ST7735_WriteString(10, 20, "Digite a senha:", Font_7x10,  GREEN, 0x0000);
    ST7735_WriteString(20, 50, "_ _ _ _",         Font_11x18, BLUE,  0x0000);
}

void gerar_senha(void)
{

    ST7735_FillScreen(0x0000);
    ST7735_WriteString(10, 10, "Senha Gerada:", Font_7x10, GREEN, 0x0000);

    char temp[2];
    for (int i = 0; i < 4; i++) {
        senha_correta[i] = rand() % 10;
        sprintf(temp, "%d", senha_correta[i]);
        ST7735_WriteString(10 + (i * 15), 30, temp, Font_11x18, WHITE, 0x0000);
    }

    ST7735_WriteString(10, 60, "Decore a senha!", Font_7x10, RED, 0x0000);
    HAL_Delay(3000);
}
void total_alunos(void)
{
    #define maxFora 3

    uint8_t alunos_sala  = 0;
    uint8_t alunos_fora  = 0;
    uint8_t total_saidas = 0;
    char linha1[20], linha2[20];

    tick_inicio = HAL_GetTick();

    ST7735_FillScreen(0x0000);
    ST7735_WriteString(10,  5, "AULA",           Font_7x10, GREEN, 0x0000);
    ST7735_WriteString(10, 20, "Sala:  0",       Font_7x10, WHITE, 0x0000);
    ST7735_WriteString(10, 35, "Fora:  0",       Font_7x10, WHITE, 0x0000);
    ST7735_WriteString(10, 80, "iniciar: +aluno",Font_7x10, BLUE,  0x0000);
    ST7735_WriteString(10, 95, "BA    : sair",   Font_7x10, BLUE,  0x0000);
    ST7735_WriteString(10,110, "final : voltar", Font_7x10, BLUE,  0x0000);
    ST7735_WriteString(10,125, "ini+fin:  fim",  Font_7x10, BLUE,  0x0000);

    while (1)
    {
        /* --- detecção do combo de encerramento --- */
        if (iniciaraula == GPIO_PIN_RESET && final == GPIO_PIN_RESET)
        {
            HAL_Delay(50);
            if (iniciaraula == GPIO_PIN_RESET && final == GPIO_PIN_RESET)
            {
                encerrar_aula(alunos_sala, alunos_fora, total_saidas);
                return;
            }
        }

        /* --- entrada de aluno --- */
        if (iniciaraula == GPIO_PIN_RESET)
        {
            alunos_sala++;
            HAL_Delay(300);
            sprintf(linha1, "Sala: %2d  ", alunos_sala);
            ST7735_WriteString(10, 20, linha1, Font_7x10, WHITE, 0x0000);
        }

        /* --- saída para BA --- */
        if (BA == GPIO_PIN_RESET)
        {
            if (final != GPIO_PIN_RESET)
            {
                if (alunos_fora < maxFora && alunos_sala > 0)
                {
                    alunos_fora++;
                    alunos_sala--;
                    total_saidas++;
                    HAL_Delay(300);
                    sprintf(linha1, "Sala: %2d  ", alunos_sala);
                    sprintf(linha2, "Fora: %2d  ", alunos_fora);
                    ST7735_WriteString(10, 20, linha1, Font_7x10, WHITE, 0x0000);
                    ST7735_WriteString(10, 35, linha2, Font_7x10, BLUE,  0x0000);
                }
                else if (alunos_fora >= maxFora)
                {
                    ST7735_WriteString(10, 55, "Limite atingido!", Font_7x10, RED, 0x0000);
                    HAL_Delay(1500);
                    ST7735_WriteString(10, 55, "                ", Font_7x10, 0x0000, 0x0000);
                }
            }
        }

        /* --- retorno do BA --- */
        if (final == GPIO_PIN_RESET)
        {
            if (iniciaraula != GPIO_PIN_RESET)
            {
                if (alunos_fora > 0)
                {
                    alunos_fora--;
                    alunos_sala++;
                    HAL_Delay(300);
                    sprintf(linha1, "Sala: %2d  ", alunos_sala);
                    sprintf(linha2, "Fora: %2d  ", alunos_fora);
                    ST7735_WriteString(10, 20, linha1, Font_7x10, WHITE, 0x0000);
                    ST7735_WriteString(10, 35, linha2, Font_7x10, BLUE,  0x0000);
                }
            }
        }
    }
}

void atualiza_senha_display(void)
{
    char display[9] = "_ _ _ _";

    for (int i = 0; i < indice; i++) {
        display[i * 2] = '0' + senha[i];
    }

    if (indice < 4) {
        display[indice * 2] = '0' + digito_atual;
    }

    ST7735_WriteString(20, 50, display, Font_11x18, RED, 0x0000);
}

bool verifica_senha(void)
{
    if (indice < 4) return false;

    int ok = 1;
    for (int i = 0; i < 4; i++) {
        if (senha[i] != senha_correta[i]) {
            ok = 0;
            break;
        }
    }

    if (ok) {
        ST7735_FillScreen(GREEN);
        ST7735_WriteString(20, 60, "ACESSO OK", Font_11x18, 0x0000, GREEN);
        HAL_Delay(2000);
        return true;
    } else {
        ST7735_FillScreen(RED);
        ST7735_WriteString(40, 60, "ERRO", Font_11x18, 0x0000, RED);
        HAL_Delay(2000);
        indice       = 0;
        digito_atual = 0;
        tela_login();
        return false;
    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_SPI1_Init();

  /* USER CODE BEGIN 2 */
  ST7735_Init();
  gerar_senha();
  tela_login();

  indice       = 0;
  digito_atual = 0;

  // Loop de login: incrementa com iniciaraula, confirma com quanti
  while (1)
  {
      // Botão INCREMENTAR (iniciaraula) — aumenta o dígito de 0 a 9, depois volta a 0
      if (iniciaraula == GPIO_PIN_RESET)
      {
          digito_atual++;
          if (digito_atual > 9) digito_atual = 0;

          atualiza_senha_display();
          HAL_Delay(300);
      }

      // Botão CONFIRMAR (quanti) — salva o dígito e passa para o próximo
      if (quanti == GPIO_PIN_RESET)
      {
          senha[indice] = digito_atual;
          indice++;
          digito_atual = 0;
          HAL_Delay(300);

          if (indice == 4)
          {
              if (verifica_senha()) {
                  break;
              }
          }
          else
          {
              atualiza_senha_display();
          }
      }
  }

  while(1)
  {
      total_alunos();
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  }
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) Error_Handler();

  RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                   |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) Error_Handler();
}

/**
  * @brief SPI1 Initialization Function
  * @retval None
  */
static void MX_SPI1_Init(void)
{
  hspi1.Instance               = SPI1;
  hspi1.Init.Mode              = SPI_MODE_MASTER;
  hspi1.Init.Direction         = SPI_DIRECTION_1LINE;
  hspi1.Init.DataSize          = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity       = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase          = SPI_PHASE_1EDGE;
  hspi1.Init.NSS               = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode            = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial     = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK) Error_Handler();
}

/**
  * @brief GPIO Initialization Function
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, ST7735_DC_Pin|ST7735_RES_Pin|led_iniciar_Pin|led_quanti_Pin
                          |led_BA_Pin|led_final_Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin   = ST7735_CS_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ST7735_CS_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin   = ST7735_DC_Pin|ST7735_RES_Pin|led_iniciar_Pin|led_quanti_Pin
                         |led_BA_Pin|led_final_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // Botões com PULLUP — lê GPIO_PIN_RESET quando pressionado
  GPIO_InitStruct.Pin  = final_Pin|banheiro_agua_Pin|quanti_Pin|iniciar_aula_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */
void encerrar_aula(uint8_t alunos_sala, uint8_t alunos_fora, uint8_t total_saidas)
{
    char linha[24];

    ST7735_FillScreen(0x0000);
    ST7735_WriteString(10,  5, " RELATORIO ", Font_7x10, GREEN,  0x0000);

    sprintf(linha, "Presentes: %2d",  alunos_sala);
    ST7735_WriteString(10, 25, linha, Font_7x10, WHITE, 0x0000);

    sprintf(linha, "Saidas BA: %2d",  total_saidas);
    ST7735_WriteString(10, 40, linha, Font_7x10, WHITE, 0x0000);

    sprintf(linha, "Fora agora:%2d",  alunos_fora);
    ST7735_WriteString(10, 55, linha, Font_7x10, alunos_fora > 0 ? RED : WHITE, 0x0000);

    uint32_t duracao_s = (HAL_GetTick() - tick_inicio) / 1000;
    sprintf(linha, "Duracao:%3lu min", duracao_s / 60);
    ST7735_WriteString(10, 70, linha, Font_7x10, BLUE,  0x0000);

    ST7735_WriteString(10, 100, "Aula encerrada.", Font_7x10, GREEN, 0x0000);

    // Trava aqui — relatório permanece até reset
    while (1) { HAL_Delay(1000); }
}

void inserir_digito(uint8_t d)
{
    if (indice < 4) {
        senha[indice] = d;
        indice++;
        atualiza_senha_display();
    }
}

void turma(void)
{
    uint8_t confirmacao = 0;
    char texto_qtd[40];

    while (confirmacao == 0)
    {
        if (iniciaraula == 0) {
            qtd_alunos1 = 15;
            sprintf(texto_qtd, "qtd: %d", qtd_alunos1);
            ST7735_WriteString(30, 60, texto_qtd, Font_11x18, BLUE, 0x0000);
            HAL_Delay(250);
        } else if (BA == 0) {
            qtd_alunos2 = 20;
            sprintf(texto_qtd, "qtd: %d", qtd_alunos2);
            ST7735_WriteString(30, 60, texto_qtd, Font_11x18, BLUE, 0x0000);
            HAL_Delay(500);
        }

        if (final == 0) {
            confirmacao = 1;
            HAL_Delay(250);
        }
    }

    ST7735_FillScreen(GREEN);
    ST7735_WriteString(20, 60, "Turma OK!", Font_11x18, 0x0000, GREEN);
    HAL_Delay(1000);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1) {}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
