# Indicador de bateria reutilizado do SMKTRON

Adaptação da implementação publicada em `4t59m4f784-creator/SMKTRON`, commit `ba5d498716e67a8ccb0a5c12f08df608819f2fb2`, `experiments/appearance/engine/firmware/src/mel.c` e `smktron_play.h`. Código GPL-3.0-only. Nenhum sample, efeito ou alteração do SMKTRON é incorporado.

- Quatro faixas aproximadas: ADC abaixo de 530, 530–559, 560–601 e 602–1022. Não são percentuais de carga nem uma previsão de autonomia.
- Mesma média IIR e intervalo de um segundo; uma faixa diferente precisa permanecer por três amostras filtradas. Primeira indicação válida aparece após cerca de três segundos.
- Uma barra vermelha na faixa baixa. Leitura ausente (0) ou saturada (1023+) deixa apenas o contorno cinza, sem afirmar bateria cheia ou baixa.
- Ícone discreto no cabeçalho comum, abaixo do logo, sem cobrir nome, favorito, parte ou tags. Visível em Performance, edição, Global e FX. O sequenciador em tela cheia mantém seu desenho atual; consulte a bateria voltando à Performance com PATCH.
- Usa apenas `fm1_in.batt`, já lido pelo driver do SMK-37. Não acrescenta leitura ADC bloqueante, processamento de áudio, escrita em flash, campos em presets, favoritos, patterns ou configurações.
- Não informa carregamento, tensão ou porcentagem. Não desliga o instrumento nem bloqueia controles.

Testes automatizados cobrem limites das quatro faixas, temporização, filtragem, leituras inválidas, volta do contador de tempo, limites gráficos e preservação dos dados salvos. A verificação visual usa telas renderizadas pela própria engine. A leitura e a posição no SMKSYNTH ainda precisam de teste no teclado.

## English

Reuses the published SMKTRON implementation at the commit and paths above, under GPL-3.0-only. No samples, effects or unrelated SMKTRON changes are included.

Four approximate ADC bands (530/560/602 boundaries), one-second IIR filtering and three consecutive filtered samples before changing the display. One red bar means the lowest band; an empty grey outline means unavailable/saturated ADC. This is not charge percentage, voltage, remaining time or charging detection.

The icon appears below the logo in the common Performance/edit/Global/FX header. Full-screen sequencer layouts remain unchanged; press PATCH to consult it. It uses the existing cached SMK-37 ADC value and adds no audio processing, flash writes or saved-record changes. Automated boundary, timing, transient, invalid-reading, timer-wrap, rendering and data-preservation checks pass. SMKSYNTH hardware validation remains pending.
