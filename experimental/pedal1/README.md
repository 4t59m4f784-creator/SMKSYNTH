# SMKSYNTH PED1 — Sustain / MIDI Panic

## Português

Protótipo **1.0.3-ped1 / SMK-37 Pro_19630**, baseado na versão pública 1.0.3 (main `1dc005e`). Não promovido à estável. Formatos de banco, favoritos, presets e patterns preservados.

### Auditoria da versão pública

| Ponto | Evidência | Limite |
|---|---|---|
| USB-MIDI OUT | Teclas centrais e extremas usam `panel_midi_send`; `platform.c:midi_send` enfileira pacotes em `usb.c`, drenados em EP MIDI. Há logs e testes de host. | Não foi localizado teste físico documentado da transmissão de notas nesta release. Reconhecimento USB ou backup funcionando não comprova Note On/Off transmitido. |
| TRS/DIN OUT | Não existe caminho de transmissão UART implementado. `FELUCCA_UART` é desabilitado em SMK37; `midi_uart.c` é recepção FM1. | Não anunciar MIDI OUT TRS/DIN como funcional. |
| Panic | CC120/123 recebido já pede `ctl_panic`. | Não havia atalho físico; a rotina antiga não limpa todo o estado de entrada e apenas libera gates. Um relato de nota presa não estabelece sua causa. |
| Pedal físico | `smk_input.h` coleta `fm1_in.pedal`, mux C endereço 6. | Não existia consumidor dessa leitura na aplicação; sustain CC64 funciona por uma via diferente. |

### Alterações

- Pedal de chave com polaridade detectada na inicialização, histerese e debounce de 12 ms. Conecte antes de ligar e mantenha solto; não é meio pedal. Os limiares ADC de 256/768 e a polaridade precisam de validação física. Troca de pedal exige reinício.
- Sustain físico e recebido por MIDI têm posse independente: soltar um não cancela o outro. O pedal continua global para as partes melódicas, inclusive Split/Layer e ARP; DRUM-X mantém seu disparo único.
- Pedal transmite CC64 nos 16 canais. Panic **GLOBE + PLAY**, em qualquer ordem, para PLAY/REC, limpa HOLD, sustain, notas e roteamentos pendentes, corta as vozes e transmite CC64=0, CC123=0 e CC120=0 nos 16 canais. Não altera patches ou patterns. Caudas de efeitos terminam normalmente. O pedal precisa ser solto para rearmar após Panic.
- Os CCs são enviados gradualmente, respeitando espaço na fila USB; não são descartados quando ela está cheia.
- A rotina de Panic não identifica nem corrige uma causa ainda desconhecida do relato de notas presas.

### Validação

`tests/sustain_panic_test.c`: ambas as polaridades, debounce por tempo, Layer, coexistência com CC64, limpeza de onset enfileirado, transporte/HOLD/REC, recuperação, 16 canais, fila cheia e consumo do atalho. ASan/UBSan passaram com `-fno-sanitize=shift`: o DSP herdado faz shift de inteiro negativo em `obv_dsp.h:130`, fora desta alteração.

A suíte `tests/run_physical_controls_tests.sh` passou (controles, sequenciador, oitavas, Split/Layer, saves, 16 patterns, drums, cenas e boot). Build JieLi concluído: app 165444 B, RAM estática 86672 B, pool 325984 B; loader 7703 B. Isso não mede CPU nem comprova funcionamento no hardware.

Use `index.html` em HTTPS ou localhost. A página só atualiza SMK-37 Pro com identidade 19618/19630; inclui retorno preservando banco para 1.0.3. Não use o instalador público para sair de PED1: ele ainda não aceita a identidade 19630. Ambos os pacotes são conferidos por SHA-256, identidade e CRC antes da operação. Caso uma transferência seja interrompida, retome na mesma página e navegador. Exportar backup antes do teste é necessário; a ferramenta pública pode recusar PED1 até receber a atualização de compatibilidade deste ramo. Nesse caso, retorne primeiro à 1.0.3 nesta página.

Para teste físico, siga a lista em `index.html`: pedal em SINGLE/SPLIT/LAYER/ARP, Panic durante HOLD/sustain/sequência/REC, retomada das notas e monitor MIDI USB com áudio USB ativo. Registrar modelo/polaridade do pedal e os resultados; ainda não há aprovação física de PED1.

## English

Experimental **1.0.3-ped1 / SMK-37 Pro_19630**, based on public 1.0.3. Saved-bank layouts are unchanged. Not promoted to stable and not hardware validated.

Public sources implement **USB-MIDI output**, with host tests but no located physical output test. They implement **no TRS/DIN transmit path**. The physical pedal ADC was sampled without an application consumer. Received CC64 and CC120/123 already had separate handlers.

Connect a switch pedal before power-on, released. PED1 detects rest polarity at startup and uses hysteresis plus 12 ms debounce; ADC thresholds require physical testing. No half-pedal support. Restart after changing pedals. Physical and received MIDI sustain are combined independently. Physical CC64 is transmitted on all 16 channels.

**GLOBE + PLAY** stops transport/recording, clears HOLD/sustain and queued/live notes, kills voices and queues sustain-off / all-notes-off / all-sound-off on all channels. USB queue backpressure is respected. Effect tails may finish. Release the pedal before using sustain again. This is recovery functionality, not proof of the unidentified stuck-note cause.

Host regression and focused sanitizer tests passed; negative signed shifts in inherited DSP require disabling only the shift sanitizer. The JieLi package builds within flash/RAM limits. Real CPU load, pedal voltages and physical MIDI output remain unverified.

Serve this folder over HTTPS or localhost. The installer accepts only SMK-37 Pro 19618 or 19630 and includes a bank-preserving return to public 1.0.3. Both packages are pinned and validated before transfer; resume interrupted transfers in the same page/browser. Export a backup beforehand. Until public backup compatibility is updated, return to 1.0.3 through this page before exporting again. Follow the physical checks listed in the Portuguese installer and record pedal model/polarity and results.
