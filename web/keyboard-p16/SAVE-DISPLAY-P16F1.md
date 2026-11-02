# SMKSYNTH P16.1 — salvamento estável na tela e nome do destino

Base: P16, fonte 3f0cdb77af1ff319582982a273750dfb005f62f1, recuperada integralmente do pacote P16 preservado. Branch smksynth-p16f1. Build 3.0.1-p16f1, identidade SMK-37 Pro_19612. A AD1 oficial e as páginas U2 permanecem intactas.

## Incluído

- 16 patterns completos: linha superior pads1–8, inferior9–16. Toque carrega; segure0,7s salva. Slot ocupado exige novo toque no mesmo pad em4s; o pad continua piscando vermelho até confirmar/cancelar.
- Avisos de salvar presets e patterns deixam de apagar/desenhar a tela inteira a cada100ms. Agora desenham faixas opacas prontas e ficam parados enquanto seu conteúdo não muda. A contagem regressiva atualiza somente uma linha de240×20 pixels, uma vez por segundo. Não há framebuffer adicional nem mudança de resolução.
- BT longo abre SAVE PRESET AS. K7 escolhe destino: tela mostra número do programa, parte e **nome do preset existente**, com origem **USER** ou **FACTORY**. Posições de fábrica INIT aparecem como **FACTORY: INIT**. Consultar um destino não carrega nem sobrescreve seu som. BT confirma; PATCH cancela.
- BT curto continua salvando o preset atual no mesmo local, com os avisos de espera/conclusão. Presets continuam99; favoritos continuam8.

Não altera DSP, engines, vozes, sequências, cenas, transpose, formato dos dados, mapas de flash, regras de SAVE ou tempos de confirmação. Mantém duas configurações split/layer por pattern e foco da parte selecionada.

## Instalação e dados

**Instalar ou reinstalar ainda reinicia o banco experimental de presets, patterns e favoritos do teclado.** Esta correção não implementa exportação física nem modifica o loader para preservar esses saves. O pacote anterior preservado é backup de código/firmware, não dos sons dentro do SMK. Ler PATTERNS16-P16.md e STORAGE-COMPATIBILITY.md.

Instalador P16 atualizado: https://obervave-voice-lab.padrini.chatgpt.site/keyboard-p16/

Simulador P16 atualizado: https://obervave-voice-lab.padrini.chatgpt.site/smksynth-p16/

Identidade final esperada: SMK-37 Pro_19612. Usar Chrome/Edge, cabo USB de dados, fechar DAWs e aguardar primeiro boot. Esta página também retorna à U2/AD1. Recuperação: ligar segurando OCT− e consultar pela mesma página; validação USB física pendente.

Sessões do simulador continuam usando o mesmo armazenamento P16 de19 blocos; não são apagadas por esta atualização de interface. A importação aceita P16 e builds anteriores compatíveis, com migração dos11 blocos antigos. Exportação de sessão do navegador não lê saves do teclado.

## Verificação

Testes nativos instrumentam chamadas LCD: zero clears nos avisos, zero transferências ao aguardar em SAVE AS, mudança de destino sem carregar o programa, nomes USER/FACTORY/INIT, contagem só4800 pixels/segundo e fechamento após4s. Capturas reais do framebuffer240×240 incluídas. Cenários também exercitados no loop real do firmware em WASM, além das regressões de16 slots, split/layer, áudio, navegação e salvamentos.

Linker: imagem155148/327680B, RAM estática82480/98304B, pool325984/344064B. Comparada à P16: +732B de imagem, +32B de RAM estática; pool de áudio inalterado. Sem alteração no mapa SMK16P1.

**Validação física pendente:** a causa de repaint foi eliminada e verificada por contadores de transferência. Isso não comprova ausência de flicker no TFT real; testar no SMK37 as telas de preset e confirmação de pattern, com transporte tocando/parado, destino USER/FACTORY/INIT e timeout. Nenhuma escrita USB no instrumento foi feita nesta etapa.

## Builds

Mesmos toolchain/SDK da P16. Configurar JIELI_TOOLCHAIN/AC79_SDK e ZIG/WASM_OPT conforme README e PATTERNS16-P16.md.

```sh
python tools/build.py --voice-hardware --board smk37 --model pro --release 3.0.1-p16f1 --experimental-id 19612
sh tools/build_voice_sim.sh
sh tests/run_p16_tests.sh
sh tests/run_sl1_tests.sh
```
