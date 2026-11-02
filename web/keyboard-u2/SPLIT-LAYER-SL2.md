# SMKSYNTH SL2 — duas linhas de performance

Build experimental 3.0.1-sl2, SMK-37 Pro_19607. Base: SL1 15c3766. AD1 oficial e SL1 preservadas em branches e instaladores próprios. Backup SL1-before-SL2.bundle contém o histórico anterior; não contém a memória física do teclado.

KNOB BANK: banco 4 SPLIT / LAYER.

| Função | Linha 1 | Linha 2 |
|---|---|---|
| Escolher partes A/B/C/D | pads 1–4 | pads 9–12 |
| SINGLE | 5 | 13 |
| SPLIT | 6 | 14 |
| LAYER | 7 | 15 |
| Tocar esta linha | **8** | **16** |

O seletor da linha ativa fica **branco RGB 255/255/255**, no máximo comandado pelo firmware. O outro fica apagado/discreto. Apenas uma linha recebe o teclado. Segure duas partes ANALOG da mesma linha para formar LAYER: a primeira é a parte inferior do split; a segunda, a superior. Cada linha guarda suas partes, modo, split, oitavas e níveis. DRUM-X não participa.

Tocar controles de uma linha seleciona essa linha para edição, sem ativá-la para tocar. Pads 8/16 ativam e também selecionam a linha para edição. A tela mostra EDIT 1/2 e PLAY 1/2. K1 modo, K2/K3 níveis exclusivos do teclado ou oitavas, K4 ponto de split; K5 alterna níveis/oitavas; K6 seleciona a parte para REC; K7 preset normal; K8 master normal. SINGLE segue a parte selecionada, como na SL1.

Ganhos independentes por linha, exclusivos das notas ao vivo; não mudam mixer, patch, sends ou sequência. Notas já tocadas mantêm seu destino ao trocar linhas; note-off/sustain permanecem correspondentes. Polifonia compartilhada existente: 8 vozes ANALOG, até duas partes por nova tecla. Notas antigas ainda podem ocupar vozes até serem soltas/terminarem, sujeitas ao alocador existente.

REC grava só a track selecionada, quando recebe a nota; nunca grava as duas partes juntas. Sequências das quatro tracks, bateria, cenas/favoritos e SEQ TRANSPOSE continuam independentes do roteamento ao vivo.

BT/SAVE duas vezes na tela confirma SAVE PATTERN N; também é possível salvar pelo banco de patterns. Guarda ambas as linhas e a seleção ativa. Não há autosave. Durante reprodução o SAVE aguarda STOP e áudio quieto; aguarde SAVED IN antes de desligar. Troca de pattern recupera o conjunto na virada sincronizada existente. O formato SL2 acrescenta 16 bytes após o prefixo SL1: patterns SL1 entram com linha 1 intacta/ativa e linha 2 SINGLE. AD1/legados entram SINGLE. Cenas não duplicam esse estado.

## Instalação e recuperação

Chrome/Edge, SMK-37 Pro por USB de dados; fechar DAWs, conectar e consultar antes de confirmar SL2. Identidade final 19607. Instalador experimental oferece retorno SL1 e AD1, além das bases anteriores. Manter alimentação/cabo até concluir. Não é firmware para Elite/MKE.

**A instalação/reinstalação reinicializa o banco experimental.** O firmware copia novamente o banco original do Player, se disponível; alterações mais recentes feitas só na AD1/SL1 não são transferidas. O backup de software não inclui presets/patterns físicos. Saves da SL2 persistem normalmente ao desligar/ligar. Esta versão não altera o layout físico do banco nem implementa exportação da AD1/SL1.

## Validação

Testes nativos: duas linhas, edição inativa, branco máximo, limites de split, notas seguradas, ganhos independentes em partes compartilhadas, sustain, REC, migração SL1, cenas byte-idênticas, roundtrip, troca sincronizada e SEQ TRANSPOSE; regressões SL1/ANALOG/DRUM-X e hashes PCM exatos da AD1. Simulador WASM e firmware derivados do mesmo código. Logs e fontes no pacote.

Recursos medidos no link: flash 175816/327680 B, RAM estática 78992/98304 B, pool 325984/344064 B. DSPs de osciladores, filtros, envelopes, bateria e efeitos preservados; somente aplicação do ganho exclusivo do teclado identifica a linha de origem.

**Validação física pendente.** Os testes nativos/WASM não medem CPU do JieLi, latência física, ruído analógico, brilho percebido dos LEDs ou estabilidade prolongada do teclado. Licenças e créditos dos componentes mantidos.
