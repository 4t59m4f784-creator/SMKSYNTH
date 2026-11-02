# SMKSYNTH DP2 — Oitavas e transpose persistente

Experimental 3.0.1-dp2 · SMK-37 Pro_19617. Base: DP1, commit 64cfd0f226a0365809783ad1440043552981605e. Marcus aprovou o funcionamento dos Drum Pads da DP1 no teclado; os ajustes novos da DP2 aguardam validação física.

## O que mudou

- A entrada passo a passo usa a mesma nota efetivamente roteada ao instrumento: oitava física por parte e oitava da combinação split/layer. Vale também para a substituição das notas de um step com pad pressionado e para o último acorde usado ao criar steps pelos pads. A transposição própria da track é descontada ao armazenar, para não ser aplicada duas vezes na reprodução. Notas MIDI permanecem absolutas.
- A gravação ao vivo, o início automático pelo primeiro toque e a captura do arpejador já recebiam a nota ajustada; foram incluídos na regressão com as cinco oitavas e quatro partes. A soltura mantém a nota original quando a oitava ou a parte muda durante uma pressão.
- SEQ → TRANSPOSE → K1 KEYS liga/desliga a captura. O estado permanece ao mudar página, parte, banco de pads, pattern ou transporte. Layers de comandos mantêm suas funções enquanto pressionadas; sair delas retoma a captura. Drum Pads continuam tocando bateria independentemente dessa captura.
- Na tela Performance, enquanto a captura está ligada, K4 mostra **KEY TR / ON**. Girar à esquerda desliga a captura e devolve imediatamente a função normal de K4. A ativação continua disponível na página SEQ TRANSPOSE.
- Desligar KEYS não zera o deslocamento já aplicado às sequências: ele continua latched. K3 na página TRANSPOSE zera o valor. A captura não é gravada em flash nem passa a ser restaurada ao ligar.
- Ao regravar um step explicitamente, seu atraso antigo (`nudge`) é zerado. Antes, trocar as notas podia preservar o microtiming de uma tomada anterior, fazendo o step continuar atrasado. Sequências existentes não são quantizadas automaticamente; microtiming, swing, ratchets e chances continuam preservados.

Os quatro bancos, cores, velocity e reação dos Drum Pads à sequência permanecem como na DP1. Não houve alteração de síntese, efeitos, clock, loader, protocolo de backup ou formato dos dados.

## Investigação do tempo

O clock lógico foi medido em 201 tempos (40–240 BPM), sete divisões e 128 intervalos por combinação. O erro de fase ficou abaixo de um bloco de controle: 16 amostras a 44,1 kHz, aproximadamente 0,363 ms. Não houve deriva acumulada nessa medição. Isso verifica o agendamento em tempo de áudio, não mede a carga do processador físico nem prova que toda a oscilação percebida foi solucionada.

O atraso herdado ao substituir notas foi reproduzido e corrigido. Ainda podem existir diferenças intencionais por nudge/swing, ataque do patch e uma causa física de carga ou interrupções não reproduzível no host. A página GLOBAL mantém o pico de renderização em microssegundos e o sinal `!` quando um render excede seu orçamento. Se a oscilação persistir, comparar o mesmo pattern com ataque curto, swing 50 e SYNC INTERNAL permite separar clock externo, microtiming e carga.

## Instalação e retorno

Exporte e guarde seu banco usando backup-dp2 antes de instalar. O instalador keyboard-dp2 aceita as origens 19613/19614/19615/19616/19617, verifica SHA-256/modelo/CRCs dos dois pacotes e oferece DP2 e retorno à DP1 aprovada. O banco reconhecido SMK16P1 permanece intacto pelo loader; não é uma primeira instalação de fábrica. Com RESTORE PENDING, conclua a restauração usando o mesmo arquivo antes de trocar firmware.

Backup-dp2 reconhece B1/B2/DP1/DP2; restauração integral exige B2 ou posterior. As ferramentas antigas permanecem em seus endereços. A restauração troca o banco completo; não é uma mesclagem.

## Evidências

Testes nativos: todas as partes/oitavas, acordes, steps reutilizados, pad segurado, criação pelo último acorde, REC ao vivo/autostart, arpejador, split/layer, MIDI absoluto, releases após troca de oitava/foco, persistência de captura e K4. Clock: 180096 intervalos medidos. Comparação de PCM ANALOG e DRUM-X com os hashes de referência; bancos e regressões de routing preservados.

Sanitizers: AddressSanitizer e UndefinedBehaviorSanitizer no workflow DP2, com `-fno-sanitize=shift` devido aos shifts assinados preexistentes do DSP fixo; esse teste não valida shifts. Sanitizer específico de DRUM-X permanece integral. Testes WASM exercitam os botões e teclas reais do simulador, incluindo a soltura de uma tecla capturada após desligar em K4.

Instalador: pins exatos, corrupção recusada, cinco origens, modelos estrangeiros recusados, retorno e retomada vinculada ao pacote. Cliente de backup contra protocolo C real em flash simulada: exportação, CRC/SHA, retry, restauração interrompida e retomada. DP1 ↔ DP2: banco byte a byte preservado em 41 pontos de interrupção de erase em cada sentido; journal pendente bloqueia atualização. Pacote OTA e limites de flash verificados.

Recursos: aplicativo 161692/327680 bytes; RAM estática 82896/98304; pool 325984/344064; loader 7703 bytes. Firmware, WASM e fontes correspondentes têm hashes no DP2-MANIFEST.json. Sem validação física nova de USB, CPU, interrupção ou estabilidade percebida.

## Reprodução

```sh
python3 tools/build.py --voice-hardware --board smk37 --model pro --release 3.0.1-dp2 --experimental-id 19617 --sdk /caminho/SDK
sh tools/build_voice_sim.sh
cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o build/seq-dp2 tests/seq_dp2_test.c
build/seq-dp2
node tests/seq_dp2_wasm_test.mjs
node tests/seq_dp2_installer_test.mjs
```

Créditos e licenças originais preservados: OberVave / zvenson e colaboradores, Felucca/Dinghy, port SMK37 e JIANT/DRUM-X. GPL-3.0 e demais avisos em LICENSE/LICENSING.md.
