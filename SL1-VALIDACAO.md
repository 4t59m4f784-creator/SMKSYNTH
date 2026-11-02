# SL1 — arquitetura e validação

## Isolamento e backup

Branch `smksynth-split-layer`, worktree independente da AD1 oficial `a0cbe7e348384cd00af40b2c2f117b083899a07f`. Antes das alterações foram copiados o fonte e os arquivos publicados da AD1: instalador, simulador, firmware e pacotes de retorno. Firmware AD1 preservado: SHA-256 `7d91de2be983bb3805e350e34f0bfbd78278fa01e833ffa499cceb86cefe784a`.

Não foi alterado o projeto do Obervave Player estável, o instalador público oficial ou o simulador oficial AD1. A entrega experimental usa novos caminhos no laboratório privado.

## Arquitetura

- `obv_keyboard_config.h`: modo, par de partes, split, oitavas e ganhos; 8 B de parâmetros mais 4 B de identificação KBD1.
- `obervave.c`: mapa de rotas por tecla e contagem de referências por parte/nota. Note-off usa a rota capturada no note-on, mesmo com mudança de foco/oitava/modo. Octavas fora da faixa são descartadas. Eventos de uma tecla dupla são enfileirados na mesma seção crítica.
- `obv_engine.h`: a origem teclado combinado acompanha o evento, vozes, held notes/arps e eventos diferidos durante troca de engine. Só essa origem recebe o ganho de teclado. Sequência, patch volume, mixer e sends permanecem independentes. Vozes de sequência e de teclado na mesma nota têm releases separados. O overflow de releases usa máscara drenada pela ISR para não deixar notas presas.
- O alocador e o número de vozes não foram ampliados. Não foram alterados os algoritmos de oscilador, filtro, envelope, DRUM-X ou efeitos. O ponto de controle de amplitude aplica somente o ganho adicional solicitado para o teclado combinado.
- `obv_seq.h`: extensão append-only do pattern; leitura dos tamanhos antigos. A configuração e seus ganhos são trocados na virada sincronizada existente. O SEQ TRANSPOSE continua separado.
- O recorder mantém gates por parte e um identificador da geração de pattern para impedir que o release de uma tecla do pattern anterior escreva ties no pattern seguinte.
- O formato de cenas `psnd_t` continua com 380 B. A configuração não é duplicada nele ou em favoritos.
- Banco 4 e página KEYBOARD acrescentados aos controles existentes. Páginas/IDs e funções dos bancos anteriores não foram renumerados.

## Recursos do build

Os números finais medidos e hashes estão em `web/voice-lab/build.json` e `validation/sl1-hardware-build.log`. As regiões de RAM são distintas; não representam uma medição de pico de stack/heap nem carga física de CPU. AD1: flash 170.544 B, RAM .data+.bss 73.808 B e pool 325.900 B.

| Região | AD1 | SL1 | Limite |
|---|---:|---:|---:|
| Imagem flash | 170.544 B | 175.360 B | 327.680 B |
| RAM .data + .bss | 73.808 B | 78.976 B | 98.304 B |
| Pool | 325.900 B | 325.936 B | 344.064 B |

## Testes

A suite vigente é `tests/run_sl1_tests.sh`, seguida dos testes WASM e de instalador citados abaixo. Os logs acompanham a entrega.

- PCM ANALOG em SINGLE: byte a byte igual à AD1, SHA-256 `4ae6e1508a661e6e362824033f358902a80a81fe527cbcf52b36117c5ed42932`.
- Groove DRUM-X: byte a byte igual à AD1, SHA-256 `5ac7a82852462679e74d0976eefb6d5183bf4aba9619079ce2b50ea84f837dfa`.
- Gesto com dois pads, escolha e rejeição de partes, SINGLE/SPLIT/LAYER, limite do split, oitavas, liberação após mudança de foco/configuração, sustain e mono/unison.
- Nível de camada zerado enquanto uma sequência da mesma parte continua audível; patch/mixer preservados. Teclado/seq na mesma nota, nos dois sentidos de release. SEQ TRANSPOSE só retuna vozes sequenciadas.
- REC somente na parte selecionada, com a oitava audível, sem escrever na segunda; release após troca de foco. Held REC na troca de pattern sem ties vazando para o próximo.
- Slots SEQ4/DRS1/KBD1, cópia e roundtrip dos prefixos e cenas; pattern antigo em SINGLE. Configuração recuperada no swap sincronizado; nenhuma gravação automática.
- Fila saturada e todos os releases; referências de teclas após sobreposição de oitavas; metadados diferidos na troca de engine, inclusive teclado e sequência na mesma altura; duas partes com arp/sustain.
- Carga musical no host: duas sequências ANALOG, DRUM-X com até oito lanes, teclado LAYER, mudanças de SEQ TRANSPOSE e todos os sends. Saída não silenciosa; encerramento sem gates presos. Esse teste não mede o processador físico.
- Regressões de grade, gravação por pads, efeitos, favoritos, programas, scenes e SEQ TRANSPOSE da AD1. DRUM-X isolada com AddressSanitizer/UBSan: 640 cantos de parâmetros, 10.240.000 amostras, sem erros; leak detection desligada.
- WASM `split_layer_wasm_test.mjs`: banco 4, gesto real de pads, duas partes soando, divisão, oitavas, níveis exclusivos, REC selecionado, SAVE manual e reboot, SINGLE.
- WASM regressões de DRUM-X e SEQ TRANSPOSE; código real do AudioWorklet em harness. Não equivale a teste de saída de áudio do navegador ou do SMK37.
- Instalador SL1: hashes/identidades/CRCs, corrupção/truncamento, bloqueios de modelo, confirmação/cancelamento, retorno AD1/PF12/PF9 e interrupção/retomada simulada. Nenhum acesso USB físico executado pelo agente.

## Reproduzir

```sh
JIELI_TOOLCHAIN=/caminho/toolchain AC79_SDK=/caminho/sdk \
 python3 tools/build.py --voice-hardware --board smk37 --model pro \
 --release 3.0.1-sl1 --experimental-id 19606
ZIG=/caminho/zig WASM_OPT=/caminho/wasm-opt sh tools/build_voice_sim.sh
sh tests/run_sl1_tests.sh
node tests/split_layer_wasm_test.mjs
node tests/ad1_transpose_wasm_test.mjs
node tests/drums_grid_wasm_test.mjs
node tests/ad1_worklet_test.mjs
node tests/sl1_installer_test.mjs
node tests/sl1_controller_test.mjs
```

O nome histórico `--voice-hardware` seleciona o perfil isolado de armazenamento e loader; VOICE continua removida. Para executar testes de instalador no fonte extraído, copie os quatro .fwsc da pasta de instalador do pacote para `web/keyboard-sl1/firmware/`.

A toolchain JieLi e os arquivos de SDK são os mesmos usados na AD1; Zig 0.13.0 e Binaryen constroem o WASM. Licenças, créditos, fonte e instruções de build correspondentes acompanham o pacote. DRUM-X permanece sem samples.
