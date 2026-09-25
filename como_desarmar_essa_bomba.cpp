#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
SoftwareSerial mySerial(10, 11); // RX (Arduino 10), TX (Arduino 11) para comunicação com DFPlayer Mini
DFRobotDFPlayerMini player;
const int PINO_BOTAO_SIM = 2;
const int PINO_BOTAO_NAO = 3;
const int PINO_BUZZER = 4;
const int PINO_LED_VERMELHO = 5;
const int PINO_LED_VERDE = 6;
const int PINO_LED_AZUL = 9;
int vidasRestantes = 3;
int acertosConcecutivos = 0; //acertos totais
const int ACERTOS_PARA_VITORIA = 5; // Quantidade de acertos para ganhar o jogo
int indicePerguntaAtual = -1; // -1 indica que nenhuma pergunta foi carregada
const unsigned long TEMPO_MAXIMO_RESPOSTA = 15000; // 15 segundos para responder
const unsigned long TEMPO_INICIO_BIP = 5000; // Bip começa após 5 segundos de espera pela resposta
// Array com o gabarito do jogo (0 = Falso, 1 = Verdadeiro)
int gabaritoRespostas[10] = {
1, // Pergunta 1
0, // Pergunta 2
0, // Pergunta 3
0, // Pergunta 4
1, // Pergunta 5
1, // Pergunta 6
1, // Pergunta 7
0, // Pergunta 8
1, // Pergunta 9
0 // Pergunta 10
};
// Array para gerenciar perguntas já feitas
int perguntasDisponiveis[10];
int totalPerguntasFeitas = 0;
const int NUM_PERGUNTAS = 10;
void setup() {
pinMode(PINO_BOTAO_SIM, INPUT_PULLUP);
pinMode(PINO_BOTAO_NAO, INPUT_PULLUP);
pinMode(PINO_BUZZER, OUTPUT);
pinMode(PINO_LED_VERMELHO, OUTPUT);
pinMode(PINO_LED_VERDE, OUTPUT);
pinMode(PINO_LED_AZUL, OUTPUT);
Serial.begin(9600);
mySerial.begin(9600);
Serial.println("--- Jogo de Perguntas e Respostas ---");
// Tenta iniciar o DFPlayer Mini. Se a inicialização falhar, o programa trava aqui para indicar o erro.
if (!player.begin(mySerial)) {
Serial.println("ERRO: DFPlayer não iniciado! Verifique as conexões e o cartão SD.");
Serial.println("Certifique-se de que a pasta 'mp3' está na raiz do cartão SD.");
while (true); // Trava o programa
}
Serial.println("DFPlayer iniciado com sucesso.");
player.volume(25); // Define o volume do DFPlayer (valor de 0 a 30)
randomSeed(analogRead(A0)); // Inicializa o gerador de números aleatórios com uma leitura analógica.
// Preenche e embaralha o array de perguntas disponíveis para a primeira rodada.
resetarPerguntasDisponiveis();
player.play(0); // tca intro
delay(40000); // audio possui 38 seg
atualizarLED(); // inicia o led pra marcar vida
}
void loop() {
//
if (vidasRestantes > 0 && acertosConcecutivos < ACERTOS_PARA_VITORIA) {
if (totalPerguntasFeitas >= NUM_PERGUNTAS) {
Serial.println("\nTodas as perguntas foram feitas! Reiniciando a lista de perguntas.");
resetarPerguntasDisponiveis(); // caso necessario jogo continua com perguntas repetidas
}
prepararNovaPergunta();
aguardarRespostaDoJogador();
} else if (acertosConcecutivos >= ACERTOS_PARA_VITORIA) {
finalizarJogoVitoria();
} else {
finalizarJogoDerrota();
}
}
// função para preparar pergunta aleatoria e tocar o audio correspondente
void prepararNovaPergunta() {
indicePerguntaAtual = perguntasDisponiveis[totalPerguntasFeitas];
totalPerguntasFeitas++; //incrementa o contador
// indice +1 por que as perguntas vao de 001 a 010
int numeroAudioParaTocar = indicePerguntaAtual + 1;
if (numeroAudioParaTocar == 10) {
numeroAudioParaTocar = 11; // Mude para o índice 11, onde ela realmente está no SD
Serial.println("ATENÇÃO: Tocando 0010.mp3 do índice 11 no SD card.");
}
Serial.print("\n--- Nova Pergunta ---\n");
Serial.print("Pergunta: "); Serial.print(totalPerguntasFeitas); // Número da pergunta na rodada atual
Serial.print(" (Corresponde ao arquivo 00");
if (indicePerguntaAtual + 1 < 10) Serial.print("0");
Serial.print(indicePerguntaAtual ); Serial.print(".mp3) - Índice do DFPlayer: "); Serial.println(numeroAudioParaTocar);
player.play(numeroAudioParaTocar); // Toca o áudio da pergunta
delay(5000); // Aguarda 5 segundos para o áudio da pergunta terminar.
}
// interação do jogador
void aguardarRespostaDoJogador() {
Serial.println("Aguardando resposta...");
int respostaRecebida = -1; // variavel pra receber a resposta do jogador
bool respondeu = false; // Flag de resposta
unsigned long tempoInicioEspera = millis(); // Marca o início do timer
unsigned long tempoUltimoBip = 0; // Marca o último bip
int intervaloBip = 1000; // Intervalo inicial entre bips
// Loop enquanto o jogador nao responde
while (!respondeu && (millis() - tempoInicioEspera < TEMPO_MAXIMO_RESPOSTA)) {
// Verifica o estado dos botões.
if (digitalRead(PINO_BOTAO_SIM) == LOW) {
Serial.println("Botão SIM pressionado.");
respostaRecebida = 1;
respondeu = true;
} else if (digitalRead(PINO_BOTAO_NAO) == LOW) {
Serial.println("Botão NÃO pressionado.");
respostaRecebida = 0;
respondeu = true;
}
// bips decrescentes enquanto jogador nao responde
if (millis() - tempoInicioEspera >= TEMPO_INICIO_BIP) {
if (millis() - tempoUltimoBip >= intervaloBip) {
Serial.print("Bip! Intervalo atual: "); Serial.print(intervaloBip); Serial.println("ms");
tone(PINO_BUZZER, 1500, 100);
tempoUltimoBip = millis(); // Atualiza o tempo do último bip.
intervaloBip = max(100, intervaloBip - 100); // reduz o intervalo dos bips
}
}
delay(10);
}
//loop que verifica se jogador respondeu
if (respondeu) {
verificarResposta(respostaRecebida);
} else {
// caso tempo acabe perde vida
Serial.println("Tempo esgotado! Resposta incorreta.");
vidasRestantes--;
Serial.print("Vidas restantes: "); Serial.println(vidasRestantes);
Serial.println("Tocando áudio de resposta incorreta (arquivo 0012.mp3).");
player.play(16); // Toca o arquivo 0015.mp3
delay(6000); //tempo do audio
atualizarLED(); // Atualiza a cor do LED.
}
}
// Compara a resposta do jogador com a resposta correta do gabarito.
void verificarResposta(int respostaJogador) {
int respostaCorretaDaPergunta = gabaritoRespostas[indicePerguntaAtual];
Serial.print("Resposta Correta da Pergunta: "); Serial.println(respostaCorretaDaPergunta);
Serial.print("Resposta do Jogador: "); Serial.println(respostaJogador);
if (respostaJogador == respostaCorretaDaPergunta) {
Serial.println("Resposta correta! Tocando som de acerto.");
Serial.println("Tocando áudio de resposta correta (arquivo 0011.mp3).");
player.play(12);
delay(6000); //aguarda audio
acertosConcecutivos++; // Incrementa o contador de acertos
Serial.print("Acertos totais: "); Serial.println(acertosConcecutivos);
} else {
vidasRestantes--;
Serial.print("Resposta incorreta. Vidas restantes: "); Serial.println(vidasRestantes);
Serial.println("Tocando áudio de resposta incorreta (arquivo 0012.mp3).");
player.play(13); // Toca o arquivo 0012.mp3
delay(6000);
atualizarLED();
}
}
// led dependendo da quantidade de vidas
void atualizarLED() {
Serial.print("Atualizando LED RGB - Vidas: "); Serial.println(vidasRestantes);
if (vidasRestantes == 3) {
setRGB(255, 0, 0); // Azul para 3 vidas
Serial.println("Cor: Azul");
} else if (vidasRestantes == 2) {
setRGB(0, 255, 0); // Verde para 2 vidas
Serial.println("Cor: Verde");
} else if (vidasRestantes == 1) {
setRGB(0, 0, 255); // Vermelho para 1 vida
Serial.println("Cor: Vermelho");
} else {
setRGB(0, 0, 0); // Apaga o LED quando as vidas acabam
Serial.println("Cor: Apagado");
}
}
void setRGB(int r, int g, int b) {
analogWrite(PINO_LED_VERMELHO, r);
analogWrite(PINO_LED_VERDE, g);
analogWrite(PINO_LED_AZUL, b);
}
// caso derrota
void finalizarJogoDerrota() {
Serial.println("\n--- FIM DE JOGO (DERROTA) ---");
Serial.println("Tocando áudio de Game Over.");
player.play(14); // arquivo 0013.mp3
delay(5000);
setRGB(0, 0, 0);
Serial.println("Jogo encerrado.");
while (true);
}
// vitoria
void finalizarJogoVitoria() {
Serial.println("\n--- PARABÉNS! VOCÊ VENCEU! ---");
Serial.println("Tocando áudio de vitória.");
player.play(15); // Toca o arquivo 0014.mp3
delay(10000);
setRGB(0, 255, 255);
Serial.println("Jogo encerrado.");
while (true);
}
// preenche o array com indice de 0 até 9
void resetarPerguntasDisponiveis() {
for (int i = 0; i < NUM_PERGUNTAS; i++) {
perguntasDisponiveis[i] = i;
}
embaralharArray(perguntasDisponiveis, NUM_PERGUNTAS);
totalPerguntasFeitas = 0;
Serial.println("Lista de perguntas embaralhada e resetada.");
}
// algoritimo de embaralhamento
void embaralharArray(int arr[], int n) {
for (int i = n - 1; i > 0; i--) {
int j = random(0, i + 1); // Gera um índice aleatório
// Troca os elementos de posição.
int temp = arr[i];
arr[i] = arr[j];
arr[j] = temp;
}
}