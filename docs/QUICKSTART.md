# SMKSYNTH 1.2.0 - Guia rápido

[PDF](pdf/SMKSYNTH-1.2.0-Quick-Start-PT.pdf) · [Português](QUICKSTART.md) · [English](QUICKSTART.en.md)

Referência: firmware 1.2.0, SMK-37 Pro_19649 / SMK-37 Elite_19649.

## Índice

- Quick start / 1. Primeiros passos
- ENDLESS: gravar e tocar
- MIDI MODE: controlador independente
- Tema, osciloscópio e salvamento
- Quick start / 2. Favoritos e Split/Layer
- Quick start / 3. Patterns e bateria
- Quick start / 4. Gravar, salvar e recuperar
- MIDI OUT - USB e TRS/DIN
- Sustain e MIDI PANIC

## Desenvolvimento com IA

Desenvolvido com IA: Marcus Padrini definiu e orientou a experiência, criou os patterns e testou no instrumento. O ChatGPT (OpenAI) contribuiu com soluções técnicas e gerou e revisou código, ferramentas, documentação e gráficos. Engines originais: autores em CREDITS.md. Sem endosso da OpenAI.

## Quick start / 1. Primeiros passos

SMKSYNTH 1.2.0 - M-VAVE SMK-37 Pro / Elite. Use este guia depois de instalar. Para primeira instalação, use somente o firmware oficial V016; a instalação substitui o banco. Salve e exporte seu trabalho antes de retornar ao firmware oficial.

1. Ajuste K8/master para um volume adequado e aperte PLAY.
2. K6 seleciona A-D; K7 troca o preset da parte.
3. KNOB BANK percorre Performance > Split/Layer > Patterns; Drum Pads entra no ciclo se a parte selecionada é DRUM-X.
4. No banco Patterns, toque P1-P5 para os exemplos. P6-P16 estão livres.
5. PLAY para. Segure KNOB BANK 0,7 s para voltar a Performance/Favoritos.

![knobs](diagrams/knobs-pt.svg)

![faders](diagrams/faders-pt.svg)

K1-K4 seguem a tela. K5: tempo na Performance; LEVEL/OCT em Split/Layer; página em STEPS; pattern nas outras vistas SEQ/Mixer. FADER BANK troca apenas os faders. GLOBAL > FADER MODE: JUMP assume imediatamente; CATCH espera cruzar o valor salvo. F4 Bank B = K8/master.

Os cinco exemplos começam a 90 BPM. Há oito favoritos e nenhum preset pessoal extra. PLAY/REC e escolha de engine preservam o banco de pads; SEQ solicita explicitamente os steps.

## ENDLESS: gravar e tocar

ENDLESS faz parte da versão oficial 1.2. Funciona em SINGLE, com as quatro partes e comprimentos independentes. Split/Layer precisa estar desligado. DRUM-X mantém seu sequenciador de bateria. Abrir a página não ativa ENDLESS.

1. Selecione a parte com K6 e percorra as vistas SEQ até ENDLESS. Toque PARA para ativar; PARA novamente desativa sem apagar as notas. REC também ativa e começa a entrada de notas.
2. Com o transporte parado, toque REC e grave notas ou acordes: ao soltar todas as teclas, o próximo toque acrescenta um step. São até 256 steps por parte.
3. Durante a entrada, seta direita acrescenta pausa sem teclas ou prolonga as notas seguradas; seta esquerda remove o último step após soltar as teclas. Toque REC novamente para encerrar a entrada.
4. Com ENDLESS ativo, uma tecla dispara/transpõe a sessão das partes: C4 mantém a afinação original, sem depender da oitava de edição da parte. PLAY inicia ou para o transporte. K4 HOLD mantém a execução após soltar as teclas; desligado, a última tecla solta para a sessão.
5. K1 RATE, K2 GATE, K3 DIR (FWD/REV/RND), K4 HOLD. Salve no banco de patterns para guardar sequências, sons e ajustes.
6. Segurar REC por mais de 1,2 s apaga a sequência e automações da parte selecionada. Parar, trocar de tela ou desligar ENDLESS não apaga o que foi gravado.

ENDLESS transmite as notas por MIDI USB e TRS/DIN usando o canal de cada parte, como o sequenciador normal. Não envia MIDI Clock/Start/Stop.

## MIDI MODE: controlador independente

No MIDI MODE o sintetizador fica silencioso e os controles enviam MIDI por USB e TRS/DIN. Não inclui Bluetooth. A entrada e a saída encerram as notas e limpam modulação temporária para preservar o timbre ao retornar ao synth.

1. No synth, percorra GLOBE até MIDI MODE. Antes de ativar, a tela mostra somente o comando de entrada; gire K1 para a direita.
2. No MIDI MODE, GLOBE abre os ajustes: K1 à esquerda retorna ao synth; K2 escolhe MAP 1-4; K3 define o canal principal 1-16; K4 escolhe encoders ABS ou REL. O canal deste modo é independente do canal-base das partes do synth.
3. KNOB BANK e FADER BANK alternam os bancos A/B. Pressione os dois juntos para trocar o banco dos pads. OCT-/+ transpõem o teclado.
4. Toque PARA para editar. Mova o controle desejado ou use K5/OCT para selecioná-lo. K1 escolhe o tipo permitido (OFF, CC, CC TOGGLE, NOTE ou PITCH BEND), K2 o número CC/nota, K3 o canal (MAIN ou 1-16) e K4 a cor dos pads. Nem todos os tipos se aplicam a todos os controles.
5. BT salva os quatro mapas, incluindo canais, cores e comportamento. Solte teclas, pads e pedal antes de salvar. PARA volta à visão de performance. Encoders ABS aceleram conforme a velocidade; REL usa mensagens relativas.
6. A tela mantém três áreas independentes: último controle com valor e barras, pads com atividade e teclado virtual de 37 teclas com acordes. GLOBE + PLAY aciona MIDI PANIC.

O simulador demonstra a interface e preserva sua memória no navegador; não substitui um teste das portas físicas USB/TRS. O backup .smksynth guarda o banco do synth; mapas MIDI e tema ficam em memória separada e não são incluídos nesse arquivo.

## Tema, osciloscópio e salvamento

GLOBE > THEME: K1 escolhe OBERVAVE (padrão) ou ZOOM. A preferência é gravada automaticamente após 1,5 s, com transporte parado e áudio em silêncio. Não é necessário salvar um preset. A gravação do tema não salva edições pendentes de mapas MIDI.

Zoom usa nomes e valores maiores. A cor da parte identifica a linha do cabeçalho, o box da parte (letra branca) e o número do preset. Textos, valores e grade preservam a paleta do instrumento. O número do pattern continua visível na performance.

Performance com valores é a tela padrão. PATCH curto abre a tela de logo, osciloscópio da saída geral e vozes/peças de bateria; outro toque entra no ciclo do Mixer. PATCH longo volta aos valores. O osciloscópio mantém suas cores e não mostra pattern. Edições aparecem por cerca de um segundo em um aviso Zoom e depois somem.

| Fluxo | Telas e comandos |
| --- | --- |
| Save rápido | BT curto salva o som na posição atual, mantendo o nome. |
| Salvar como / renomear | BT longo abre edição. K1 move cursor; K2 escolhe caractere (até 10); K7 escolhe posição 01-99. A tela mostra o programa já presente no destino. |
| Revisar substituição | BT abre a revisão do nome e destino, com o programa que será substituído. Solte e pressione BT novamente para gravar. |
| Voltar / cancelar | PATCH na revisão volta à edição; PATCH na edição cancela sem escrever. |

## Quick start / 2. Favoritos e Split/Layer

### Banco 1 - Performance/Favoritos

![performance](diagrams/performance-pt.svg)

1-4: parte A-D. 5/6: send delay/reverb on/off. 7/8: preset anterior/próximo. 9-16: oito favoritos. Toque para recuperar na parte selecionada; segure 0,7 s para capturar. Pare e solte notas/sustain; aguarde FAVOURITES SAVED.

### Banco 2 - Split/Layer

![split](diagrams/split-pt.svg)

1. Na mesma fileira, segure uma parte A-D e toque outra para formar o par. Use duas partes ANALOG.
2. Escolha SINGLE, SPLIT ou LAYER. K4 ajusta ponto de split.
3. K5 alterna K2/K3 entre níveis e oitavas das duas partes.
4. Pad 8 ou 16 seleciona a linha tocada (branco forte). Editar a outra linha não a ativa.
5. Salve o pattern para guardar ambas as configurações e a linha ativa.

K6 escolhe a parte gravada. BT salva preset; não substitui o salvamento de pattern/cena.

## Quick start / 3. Patterns e bateria

### Banco 3 - Patterns/Cenas

![patterns](diagrams/patterns-pt.svg)

Toque carrega. Durante PLAY, a troca é agendada para o próximo compasso. Segure 0,7 s para salvar o pattern atual nessa posição. Se estiver ocupada, toque no mesmo pad em até 4 s para confirmar OVERWRITE; outro pad/PATCH cancela. Pare e aguarde a conclusão da escrita.

Vermelho: ocupado; vermelho muito fraco: vazio; azul: selecionado; azul piscando: próximo; vermelho piscando: confirmar. O pattern guarda sequência, quatro sons, mix e Split/Layer. Não há save automático ao trocar.

### Banco 4 - Drum Pads

![drums](diagrams/drums-pt.svg)

1. K6 escolhe a parte. GLOBE até PART SETUP; K1 = DRUM-X e K2 = kit.
2. KNOB BANK até Drum Pads. A escolha de kit não ativa esse banco automaticamente.
3. Pads 1-8 tocam com velocity; 9-16 são reservados. Violeta em repouso, branco no toque e nos hits da sequência.
4. PLAY, depois REC: grave ao vivo pelos pads. REC encerra a gravação; PLAY para.

Em ANALOG, um banco Drum Pads já selecionado fica inativo. Volte a DRUM-X ou use KNOB BANK.

## Quick start / 4. Gravar, salvar e recuperar

| Botão | Uso imediato |
| --- | --- |
| PATCH | Toque: Performance; na Performance abre o osciloscópio; outro toque abre Mixer; no Mixer percorre as sete linhas. Segure 0,7 s: volta à Performance. |
| PARA | Toque: páginas de síntese; em STEPS limpa o step e avança (DRUM: limpa o hit da lane). Segure: edição/erase. |
| CHORD | ANALOG: FILTER ENV / AMP ENV. DRUM-X: DRUM SOUND. |
| NOTE REPEAT | ANALOG: LFO / LFO DEST / LFO +. DRUM-X redireciona para DRUM SOUND. |
| ARP | Abre ARP. Novo toque nessa página alterna HOLD da parte. |
| SCALE | Segure em STEPS ANALOG: STEP+. Na página GLOBAL, toque alterna SYNC AUTO / INTERNAL. |
| GLOBE | Toque: GLOBAL > POLYPHONY > OUTPUT > PART SETUP > CONTROLS > THEME > MIDI MODE. Segure: teclas para start/stop, mute, solo e tap tempo. |
| FX | Toque: CHORUS > DELAY > REVERB > DRIVE. Segure: K1/K2/K3 retornos chorus/delay/reverb; K4 tempo do delay. |
| SEQ | Toque: vistas do sequenciador e posse explícita dos pads de steps. Segure: teclas brancas viram steps. |
| REC | Toque: arma parado; em execução liga/desliga gravação. Segure mais de 1,2 s: apaga a track selecionada. |

1. Gravação ao vivo: selecione K6, PLAY e REC; toque; REC encerra.
2. Passo a passo ANALOG: PLAY parado, REC apagado, SEQ > STEPS. Toque nota/acorde e solte para avançar. K1 move o step; deixe vazio para pausa.
3. Se LENGTH = OFF, use TRACKS K3 para ligar a track.
4. Salvar som: BT toque. Save As: segure BT, escolha K7, confirme BT. Salvar sequência/cena: segure pad do banco Patterns.
5. Para backup, salve no instrumento e espere a conclusão. Pare PLAY/REC e solte notas/sustain. Exporte na ferramenta web.
6. Restauração substitui todo o banco: exporte o atual, escolha o .smksynth e confirme. RESTORE PENDING exige o mesmo arquivo.

REC segurado por mais de 1,2 s apaga a track selecionada. Para transposição persistente: SEQ > TRANSPOSE K1 liga a captura; K2 ajusta, K3 zera. Performance K4 KEY TR desliga captura, sem zerar o valor.

## MIDI OUT - USB e TRS/DIN

A saída MIDI transmite as quatro partes, independentemente da parte selecionada no painel. GLOBAL > MIDI CH define o canal-base de A (1-13). B, C e D usam base +1, +2 e +3. OMNI na recepção corresponde a canais de saída 1-4.

| Parte | Canal de saída | Exemplo: base 3 |
| --- | --- | --- |
| A / 1 | BASE | 3 |
| B / 2 | BASE + 1 | 4 |
| C / 3 | BASE + 2 | 5 |
| D / 4 | BASE + 3 | 6 |

USB e TRS recebem as mesmas mensagens musicais. Teclas, pads, Split/Layer, arpejos e sequenciadores transmitem notas. DRUM-X usa o canal da sua parte, sem canal 10 obrigatório. Split/Layer transmite as notas reais de cada parte, com suas oitavas. Cada Note Off conserva o destino do Note On original.

Conecte MIDI OUT de 3,5 mm ao MIDI IN do destino com o adaptador TRS-DIN adequado ao teclado. A saída de áudio é outro conector. TRS funciona com bateria e sem computador; USB requer host MIDI.

Pitch Bend é enviado em 14 bits (centro 8192); mod wheel usa CC1 (0-127). Ambos acompanham somente a parte selecionada, inclusive em Split/Layer, como a engine local. Trocar a parte neutraliza o canal anterior. O estado das rodas é reenviado após Panic.

Não há transmissão de MIDI Clock/Start/Stop nem envio automático de Program Change ao trocar um preset interno. Notas MIDI recebidas não fazem Thru simples; os arpejos gerados podem transmitir. A recepção de bend é global no canal-base, herdada da engine; isto é diferente do envio por parte.

## Sustain e MIDI PANIC

### Pedal de sustain

1. Conecte o pedal antes de ligar e mantenha-o solto durante a inicialização.
2. A polaridade de um pedal de chave é detectada ao ligar. Não há meio pedal; reinicie após trocar o pedal.
3. Pressione o pedal, toque e solte um acorde: ele continua até soltar o pedal. O sustain atende as quatro partes melódicas, inclusive Split/Layer e ARP. DRUM-X mantém seu disparo único interno.
4. CC64 é enviado aos canais das quatro partes e às notas ainda mantidas em canais anteriores. Ao soltar, esses canais recebem pedal off. O sustain físico e o recebido por MIDI têm controle independente.

### Panic: GLOBE + PLAY

Pressione GLOBE e PLAY juntos, em qualquer ordem. O comando para PLAY/REC, desliga HOLD/sustain, limpa notas pendentes e corta as vozes internas. USB e TRS enviam CC64=0, CC123=0 e CC120=0 aos 16 canais. Solte o pedal antes de usar sustain novamente. As caudas dos efeitos podem terminar normalmente.

A 1.2.0 também corrige vozes presas ao mudar POLY/DUAL para MONO/UNISON com teclas mantidas. O Note Off já chegava, mas vozes do modo anterior podiam não ser liberadas. Glide não foi alterado; o caso de mudar somente Glide não reproduziu a falha nos testes.

## Links

- [Site](https://4t59m4f784-creator.github.io/SMKSYNTH/)
- [Instalador](https://4t59m4f784-creator.github.io/SMKSYNTH/install/)
- [Simulador](https://4t59m4f784-creator.github.io/SMKSYNTH/simulator/)
- [Backup](https://4t59m4f784-creator.github.io/SMKSYNTH/backup/)

[GitHub](https://github.com/4t59m4f784-creator/SMKSYNTH)
