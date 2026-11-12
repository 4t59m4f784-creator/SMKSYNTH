# SMKSYNTH 1.1.0 - Guia rápido

[PDF](pdf/SMKSYNTH-1.1.0-Quick-Start-PT.pdf) · [Português](QUICKSTART.md) · [English](QUICKSTART.en.md)

Referência: firmware 1.1.0, SMK-37 Pro_19634 / SMK-37 Elite_19634.

## Índice

- Quick start / 1. Primeiros passos
- Quick start / 2. Favoritos e Split/Layer
- Quick start / 3. Patterns e bateria
- Quick start / 4. Gravar, salvar e recuperar
- MIDI OUT - USB e TRS/DIN
- Sustain e MIDI PANIC

## Desenvolvimento com IA

Desenvolvido com IA: Marcus Padrini definiu e orientou a experiência, criou os patterns e testou no instrumento. O ChatGPT (OpenAI) contribuiu com soluções técnicas e gerou e revisou código, ferramentas, documentação e gráficos. Engines originais: autores em CREDITS.md. Sem endosso da OpenAI.

## Quick start / 1. Primeiros passos

SMKSYNTH 1.1.0 - M-VAVE SMK-37 Pro / Elite. Use este guia depois de instalar. Para primeira instalação, use somente o firmware oficial V016; a instalação substitui o banco. Salve e exporte seu trabalho antes de retornar ao firmware oficial.

1. Ajuste K8/master para um volume adequado e aperte PLAY.
2. K6 seleciona A-D; K7 troca o preset da parte.
3. KNOB BANK percorre Performance > Split/Layer > Patterns; Drum Pads entra no ciclo se a parte selecionada é DRUM-X.
4. No banco Patterns, toque P1-P5 para os exemplos. P6-P16 estão livres.
5. PLAY para. Segure KNOB BANK 0,7 s para voltar a Performance/Favoritos.

![knobs](diagrams/knobs-pt.svg)

![faders](diagrams/faders-pt.svg)

K1-K4 seguem a tela. K5: tempo na Performance; LEVEL/OCT em Split/Layer; página em STEPS; pattern nas outras vistas SEQ/Mixer. FADER BANK troca apenas os faders. GLOBAL > FADER MODE: JUMP assume imediatamente; CATCH espera cruzar o valor salvo. F4 Bank B = K8/master.

Os cinco exemplos começam a 90 BPM. Há oito favoritos e nenhum preset pessoal extra. PLAY/REC e escolha de engine preservam o banco de pads; SEQ solicita explicitamente os steps.

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
| PATCH | Toque: Performance; na Performance abre Mixer; no Mixer percorre as sete linhas. Segure 0,7 s: volta à Performance. |
| PARA | Toque: páginas de síntese; em STEPS limpa o step e avança (DRUM: limpa o hit da lane). Segure: edição/erase. |
| CHORD | ANALOG: FILTER ENV / AMP ENV. DRUM-X: DRUM SOUND. |
| NOTE REPEAT | ANALOG: LFO / LFO DEST / LFO +. DRUM-X redireciona para DRUM SOUND. |
| ARP | Abre ARP. Novo toque nessa página alterna HOLD da parte. |
| SCALE | Segure em STEPS ANALOG: STEP+. Na página GLOBAL, toque alterna SYNC AUTO / INTERNAL. |
| GLOBE | Toque: GLOBAL > POLYPHONY > OUTPUT > PART SETUP. Segure: teclas para start/stop, mute, solo e tap tempo. |
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

A 1.1.0 também corrige vozes presas ao mudar POLY/DUAL para MONO/UNISON com teclas mantidas. O Note Off já chegava, mas vozes do modo anterior podiam não ser liberadas. Glide não foi alterado; o caso de mudar somente Glide não reproduziu a falha nos testes.

## Links

- [Site](https://4t59m4f784-creator.github.io/SMKSYNTH/)
- [Instalador](https://4t59m4f784-creator.github.io/SMKSYNTH/install/)
- [Simulador](https://4t59m4f784-creator.github.io/SMKSYNTH/simulator/)
- [Backup](https://4t59m4f784-creator.github.io/SMKSYNTH/backup/)

[GitHub](https://github.com/4t59m4f784-creator/SMKSYNTH)
