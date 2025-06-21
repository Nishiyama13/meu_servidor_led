# Servidor Web em Modo Access Point com ESP32-C3

Este projeto transforma um microcontrolador ESP32-C3 em um ponto de acesso (Access Point) Wi-Fi. Ele hospeda um servidor web local que permite controlar o LED da placa através de uma página web com um design moderno em dark mode.

É uma solução autônoma para controle de dispositivos via Wi-Fi, ideal para cenários onde não há uma infraestrutura de rede existente.

| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- | -------- |

## Funcionalidades

* **Modo Access Point (AP):** O ESP32-C3 cria sua própria rede Wi-Fi.
* **Servidor Web HTTP:** Hospeda uma página web para interação com o usuário.
* **Controle de Hardware:** A página web possui um botão para ligar e desligar o LED conectado ao GPIO 8.
* **Interface Moderna:** A página utiliza HTML e CSS para criar uma interface dark mode agradável e responsiva.

## Como Usar

Siga os passos abaixo para testar o projeto.

### 1. Compilação e Gravação

Compile o projeto e grave-o na sua placa ESP32-C3. Depois, abra o monitor serial para ver os logs de inicialização.

```bash
idf.py -p SUA-PORTA-COM flash monitor
```
*(Substitua `SUA-PORTA-COM` pela porta serial correta, como `COM3`).*

### 2. Conectar ao Wi-Fi
…**http://192.168.4.1**

Você verá a página de controle do LED.

### 4. Controlar o LED

Clique no botão "Alternar LED" na página web. Você verá o LED da sua placa acender e apagar a cada clique. O monitor serial também exibirá o estado atual do LED.

## Saída no Monitor Serial

Abaixo está um exemplo do que você verá no monitor serial quando o sistema iniciar e um dispositivo se conectar.

```
...
W (438) WiFi-AP: Access Point iniciado. SSID:ESP32-AP senha:12345678
...
W (15388) WiFi-AP: Estacao conectada! MAC: 1a:2b:3c:4d:5e:6f, AID: 1
...
I (25498) LED: Ligado
I (28128) LED: Desligado
```

## Customização

Para modificar o projeto, você pode alterar os seguintes pontos no arquivo `main/main.c`:

* **Credenciais do Wi-Fi:** Altere as macros `SSID` e `PASSWORD` no início do arquivo.
* **Pino do LED:** O pino do LED está definido como `GPIO_NUM_8` nas funções `gpio_init()` e `led_get_handler()`.
* **Aparência da Página:** Todo o código HTML e CSS da página web está contido na variável `const char html_page[]`.
