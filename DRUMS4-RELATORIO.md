# Obervave Voice + Drums — Checkpoint 4: sequenciador de oito lanes

Base recuperada integralmente do Checkpoint 3: commit
`f55e9f3e96049ef3d2973176e1a43654826f4131`. Branch `obervave-voice`.
Snapshots independentes: `pf12-preserved`, `voice2-preserved`, `drums3-preserved`.
Instaladores e versão estável não foram alterados. Sem firmware instalável novo.

## Resultado e uso

Cada parte DRUMS tem uma grade de oito instrumentos × até 256 steps. Todos os
oito hits podem ser agendados no mesmo step; o choke closed/open hi-hat permanece
ativo, portanto os dois hats não sustentam juntos. O transporte, divisão, swing,
length, pattern queue e mixer das quatro partes continuam compartilhados.
ANALOG e VOICE mantêm seu sequenciador melódico original de quatro notas por step.

A seleção de engine continua discreta em PART SETUP, ao final do ciclo GLOBE.
Com DRUMS selecionada, toque SEQ: a tela mostra oito lanes e 16 steps por página.
O desenho usa as fontes, cores e canvas existentes. A grade é desenhada em dois
blocos de quatro lanes e um rodapé, sem aumentar o buffer gráfico.

| Contexto | K1 | K2 | K3 | K4 |
| --- | --- | --- | --- | --- |
| DRUM | instrumento/lane | cursor do step | velocity do step | ratchet 1–4 |
| DRUM+ | instrumento/lane | chance 0–100% | nudge 0–127 | on/off do step |
| TRACK | length até 256 | divisão | swing | transpose legado, ignorado por DRUMS |
| TRACKS / PATTERNS | funções já existentes | funções já existentes | funções já existentes | funções já existentes |

SEQ percorre DRUM → DRUM+ → TRACK → TRACKS → PATTERNS → DRUM. SCALE mantido
na grade também mostra os controles DRUM+. K5 ou setas percorrem páginas de 16
steps nas duas telas DRUM; K6 continua selecionando a parte. Nos outros contextos,
K5 mantém a navegação anterior. K7 continua programa/destino, K8 master.

Na grade, os 16 pads alternam o hit da lane selecionada e escolhem o step. Um hit
novo pode ser ouvido como preview quando parado. PARA apaga somente o hit dessa
lane no cursor, com undo; não apaga os demais instrumentos do mesmo step.

REC muda os oito pads superiores para os oito instrumentos; os inferiores ficam
reservados nessa tela. A primeira nota inicia a gravação armada. REC encerra a
gravação e PLAY para o transporte. Na página DRUM SOUND, os oito hits e favoritos
continuam como no Checkpoint 3. PATCH sai do sequenciador e recupera a performance.
O banco manual de steps e a layer SEQ também editam a lane selecionada de DRUMS.

Velocity, ratchet, chance, nudge e on/off são **do step**, compartilhados pelos
hits simultâneos. Não são oito conjuntos independentes desses parâmetros. Essa
semântica conserva o sequenciador original e sua representação de timing. Na
gravação de vários hits no mesmo step, a última velocity recebida vale para o step.
Volume, decay e pitch por instrumento continuam na edição do kit.

## Persistência e compatibilidade

O prefixo de 13.416 bytes de SEQ4 continua exatamente no mesmo lugar. Foi
acrescentada a extensão versionada DRS1:

- magic de 4 bytes;
- quatro flags de ativação, uma por parte;
- quatro vetores de 256 máscaras de 8 bits, uma máscara por step.

Novo pattern em RAM: **14.448 bytes**, aumento de **1.032 bytes**. Com a cena de
380 bytes, o conteúdo do slot soma **14.828 bytes**, dentro dos 16.368 disponíveis
após o cabeçalho. Endereços, setores e a proteção CRC do armazenamento não mudam.
Há static asserts para o offset legado e o limite do slot.

O carregador tenta o formato estendido e, em seguida, o formato legado com seu
tamanho original. A cena é lida no offset correspondente a cada tamanho. Patterns
legados de uma versão anterior à SEQ4 continuam na rota de migração existente.
Não há gravação automática ao carregar ou converter dados em RAM.

A grade inicial de DRUMS pode ler as notas GM/aliases de um pattern antigo. No
primeiro **edit/REC explícito** de DRUMS, as máscaras da parte são inicializadas
em RAM. As notas melódicas originais ficam intactas em outro campo. Trocar engine
não reescreve essas notas. Ao voltar a ANALOG/VOICE, elas continuam disponíveis.
Metadados de step (velocity/timing/etc.) continuam compartilhados e suas edições
são deliberadas; não são dois patterns inteiramente independentes na mesma parte.

Um pattern sem máscaras ativadas é salvo com o tamanho legado, conservando sua
representação anterior. Patterns com máscaras usam o tamanho novo e exigem este
ramo experimental para ler a grade. Não importe uma sessão nova na versão estável
esperando suporte ao formato DRS1. O simulador usa espaço local v4 separado e
aceita importar sessões v3 e anteriores; a sessão de origem não é sobrescrita.

Undo, cópia, troca de pattern na barra, shift e length ×2 levam as máscaras
junto. A remoção por nota reconhece o instrumento no mapa de DRUMS. Transposição
melódica não muda o instrumento de bateria; a operação de transpose da layer EDIT
indica mapa fixo para DRUMS.

## Testes e evidências

- Teste nativo: oito eventos de hit em um step, zero cards melódicos por DRUMS,
  chance zero, step off, nudge e três ratchets no caminho das máscaras.
- Pads: oito lanes sobre o mesmo step, remoção isolada, segunda página, undo,
  shift e length ×2; REC com oito pads e liberação de posse das notas.
- Armazenamento real simulado: slot legado e cena no offset original; nenhum
  writeback automático. Novo slot/máscaras/cena recuperados sem diferenças.
- Todos os 1.024 bytes de máscaras (quatro partes × 256 steps) sobrevivem ao
  roundtrip, incluindo o step 256.
- Troca agendada na barra com partes ANALOG + VOICE + DRUMS: máscaras e cena
  acompanham a troca; os quatro playheads permanecem sincronizados.
- WebAssembly: edição pelos controles nativos, áudio real não nulo da sequência,
  save manual, export/reboot, cópia de pattern com kit, REC de oito pads e foco
  entre parte DRUMS e parte melódica.
- Regressões de performance, knobs, faders, wheels, favoritos, scenes e programas
  passaram. A matriz de encoders compara o prefixo legado de SEQ4; a extensão é
  verificada pelos testes dedicados acima.
- ANALOG: PCM idêntico à PF12 preservada, com assert de áudio não nulo.
  SHA256 `4ae6e1508a661e6e362824033f358902a80a81fe527cbcf52b36117c5ed42932`.
- Mix ANALOG + VOICE: PCM idêntico ao checkpoint 2.
  SHA256 `08d7ce3bc6886ae0c264b54359eaee90bdb2ffa7a214300b0be44c6221873781`.
- Groove legado DRUMS: PCM idêntico ao checkpoint 3, 442.624 frames stereo.
  SHA256 `6f8c3e333588f4334f3abdb7e7c1ca389a25622575bdc3ebe5461da3befd6a3e`.
- DSPs isolados VOICE e DRUM-X passaram novamente nos sanitizers. A pendência de
  shift negativo herdado no DSP analógico não foi apagada nem declarada resolvida.
- Navegador: conferidos grade completa, edição de kick/snare por lane, modos de
  pads e REC. O navegador local usou fallback de interface sem áudio. Áudio foi
  exercitado nos testes C/WASM e no process() do AudioWorklet sob host Node.

Logs ficam em validation/. Comandos: `sh tests/run_voice_foundation_tests.sh`,
`sh tests/run_drums_tests.sh` e `sh tests/run_drums_grid_tests.sh`.
Build WASM: `ZIG=... WASM_OPT=... sh tools/build_voice_sim.sh`.

## Recursos, limitações e próximo passo

As quatro instâncias de pattern em RAM (atual, próximo, anterior e undo) somam
**4.128 bytes adicionais** pelos sizeof medidos no host. O pool DRUMS continua
com 2.688 bytes. Não houve aumento do canvas gráfico; ele continua sendo reusado
em tiras. Isso não substitui o mapa de memória e as medições do processador JieLi.
O hash e o tamanho exatos do WASM estão em web/voice-lab/build.json; WASM não é
medida de flash do teclado.

Não houve teste físico, medição de CPU/flash/RAM final, latência real ou longa
duração no SMK37. O empacotamento instalável continua bloqueado. Não se promete
polifonia segura sob carga real apenas porque o simulador e os testes passaram.
A próxima etapa é preparar o build de destino, backup/recuperação, medições e
validação no hardware; ajustes de workflow/sonoridade podem ser feitos antes.

## Checkpoint e recuperação

O pacote contém source.zip, histórico completo em obervave-voice.bundle, este
relatório, uma imagem gerada diretamente pelo framebuffer C e um groove WAV
produzido pelo novo sequenciador. O WAV não contém samples externos, não foi
normalizado e recebeu apenas um fade de 80 ms no fim. Reproduza o render com:
`cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o demo tests/drums_grid_demo.c`,
`./demo groove.raw` (int32 stereo 44,1 kHz).

Clone do histórico: `git clone -b obervave-voice obervave-voice.bundle obervave-voice`.
As bases preservadas ficam como origin/pf12-preserved, origin/voice2-preserved e
origin/drums3-preserved após um clone. Exporte sua sessão do simulador antes de
importar outra; aguarde os saves manuais e caudas terminarem antes de reiniciar.

Simulador privado: https://obervave-voice-lab.padrini.chatgpt.site/
GPL-3.0-only; créditos e referências JIANT permanecem em LICENSING.md e nos ports.
