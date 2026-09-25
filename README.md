# Projeto_arduino
Jogo: “Será que você consegue desarmar esta bomba?!”

Resumo
O projeto consiste em um jogo de perguntas no estilo "verdadeiro ou falso". O jogador começa com três vidas e perde uma a cada erro. Para vencer é necessário acertar cinco perguntas, entretanto, há o desafio do jogador ter apenas quinze segundos para responder cada pergunta, enquanto o sinal sonoro de bipe vai aumentando sua intensidade conforme o tempo passa, simulando a tensão de uma bomba prestes a explodir.

Componentes
1 buzzer 5V - Utilizado como aviso sonoro para o temporizador de cada pergunta
1 placa Arduino Uno - Cérebro do projeto
1 LED RGB - Representa as vidas do jogador, iniciando em azul e mudando de cor a cada erro
1 par de botões (push button) - Interface do usuário para responder "verdadeiro" ou "falso"
1 modulo MP3 DFPlayer Mini - Reproduz os áudios do jogo a partir de um cartão microSD
1 mini alto-falante 0.5W 8 - Responsável pela reprodução dos áudios

Desafios
- Organização e execução dos áudios
Um dos principais desafios foi compreender como o DFPlayer Mini organiza os arquivos de áudio. Percebemos que ele não necessariamente segue a ordem alfabética ou numérica dos nomes, mas sim a ordem de gravação dos arquivos no cartão SD.
- Aleatoriedade das perguntas
Para evitar que o jogo se tornasse previsível, implementamos um sistema de embaralhamento das perguntas. A lógica do embaralhamento exigiu um pouco de pesquisa, mas achamos necessário para uma experiência diferente a cada jogo.

considerações finais

O desenvolvimento deste projeto, assim como o aprendizado prático ao longo do curso de Arduino, proporcionou uma experiência extremamente rica e gratificante. Unir a lógica de programação à montagem física do circuito permitiu compreender de forma prática a interação entre hardware e software.

A superação dos desafios técnicos — como a gestão do tempo no temporizador, a manipulação de áudio com o DFPlayer e o controle das perguntas — consolidou fundamentos essenciais de eletrônica e programação com microcontroladores, despertando um interesse genuíno pela exploração contínua da cultura maker e dos sistemas embarcados.
