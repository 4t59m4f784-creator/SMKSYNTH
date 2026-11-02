# Obervave Voice + Drums — checkpoint 2: VOICE

Base: nossa PF12 modificada. Branch: `obervave-voice`.
O snapshot `pf12-preserved` e o checkpoint 1 continuam no histórico Git.
Não houve publicação nem alteração dos instaladores/simuladores hospedados.

## Resultado

ANALOG e VOICE agora geram áudio e podem coexistir nas quatro tracks usando o
alocador melódico existente. DRUMS ainda não está integrada.

VOICE porta o caminho de vogais do JIANT: fonte glotal com correção BLEP,
quatro ressonadores Klatt, vogais A/E/I/O/U, morph, TALK, SHIFT, BUZZ, BREATH e Q.
Os parâmetros principais recebem suavização. ADSR, glide, pitch bend, pan,
velocity, mixer e sends vêm do Obervave. O glide é o nativo do Obervave;
não foi importado o sistema de articulação/S-curve do JIANT.

Não foram importados letras, palavras, `sing.c`, dicionários, rostos ou UI JIANT.
Nenhum sample foi incluído. Os áudios de demonstração vêm do código C do synth.

## Presets e controles

Oito pontos de partida: Choir Aah, Ooh Pad, Vox Lead, Talkbox, Whisper, Chant,
Vowel Sweep e Dark Choir. A qualidade musical ainda precisa da sua escuta e do
teste no teclado; compilação e métricas não equivalem a essa aprovação.

| Contexto | Controles |
| --- | --- |
| GLOBE | GLOBAL → VOICE (modo/unison) → OUTPUT → ENGINE |
| ENGINE | K1: ANALOG/VOICE; K2: preset vocal; K3: modo; K4: glide |
| PARA com VOICE | VOWELS: Vowel/Vowel2/Talk/Shift → TONE: Buzz/Breath/Q/Morph |
| ENV com VOICE | AMP ENV: Attack/Decay/Sustain/Release |
| Performance VOICE / Faders A | Vowel, Q, Talk, Release |
| K5/K6/K7/K8 | tempo/navegação, parte, programa/destino, master, como na PF12 |
| Faders B | sends chorus/delay/reverb por parte e master; pickup preservado |
| Mod wheel / CC1 | desloca o morph vocal em direção a U; wheel física só na parte selecionada |

Os 99 programas existentes continuam disponíveis no K7. Os oito presets vocais
são pontos de partida na página ENGINE, sem ocupar ou substituir automaticamente
programas de usuário. SAVE continua manual. A escolha ANALOG pela página ENGINE
recupera o patch analógico que estava na track antes da troca, enquanto essa
referência existir em RAM; após reiniciar sem essa referência, usa INIT ANALOG.
Ela não é um segundo patch salvo dentro do programa.

As páginas e o espectro estilizado usam a paleta e os elementos do Obervave.
O espectro representa as posições dos formants; não é um analisador FFT.
O desenho fica no loop de interface, fora do processamento de áudio.

## Persistência e compatibilidade

O patch continua com 80 bytes, os nomes no mesmo lugar e os parâmetros/CCs
existentes com os mesmos IDs. Os seis bytes reservados 74–79 recebem a assinatura
`OVE`, versão 1, engine e verificação do ID. Byte 73 identifica o preset vocal de
origem. Patches legados sem essa assinatura são ANALOG, sem migração automática.

Os parâmetros vocais usam bytes de parâmetros interpretados por contexto:
cutoff→vowel, resonance→Q, filter-env amount→TALK, oscillator-1 PW→vowel2,
oscillator-1 level→BUZZ, noise→BREATH, oscillator-2 fine→SHIFT e mod-depth→morph.
Assim, os snapshots completos já existentes transportam a configuração VOICE.
Isso não mistura síntese analógica e vocal na mesma voz.

Testados no host: gravação/leitura de programa, favorito e pattern/cena com VOICE,
além de leitura de favoritos PFV1/PFV2 herdada. O indicador de favorito agora
também compara a engine. A engine é reconhecida após recuperação do patch.
Notas novas durante a troca são enfileiradas e recuperadas; a primeira nota de
um pattern VOICE trocado no downbeat foi verificada em teste específico.
Notas com duração inteira menor que o fade de troca podem terminar antes da
ativação; essa situação limite ainda precisa da revisão de performance.

Os formats de settings/patterns/favoritos não cresceram. **Patches VOICE novos
não têm o mesmo significado sonoro no firmware estável antigo**, que ignora a
identificação da engine. Não há exportação VOICE→ANALOG. Os dados antigos não
foram alterados por migração, mas um SAVE manual sobre um slot ocupado continua
sendo uma sobrescrita, como já era na PF12.

Ainda não foi implementado backup/retorno físico completo dos dados do teclado.
O empacotador `.fwsc` permanece bloqueado e não há instalador novo nesta entrega.
O snapshot/bundle Git isola código; não cria uma área separada de dados no SMK37.

## Correção da evidência do checkpoint 1

A primeira comparação de 384.000 frames comparou silêncio: o cenário tinha
mixer/master zerados. O hash antigo `76ed5c9c…` não comprovava preservação sonora.
Esse resultado foi invalidado e o teste foi corrigido para abrir os volumes,
usar sends e **falhar se não houver áudio suficiente**.

A nova comparação contra a PF12 preservada passou, byte a byte:

- 384.000 frames estéreo, 767.715 amostras não zeradas;
- quatro tracks, sequenciador, rodas e sends;
- SHA-256 de ambos os fluxos:
  `4ae6e1508a661e6e362824033f358902a80a81fe527cbcf52b36117c5ed42932`.

Isso comprova igualdade nesse cenário, não em todas as combinações possíveis.
Os testes novos de VOICE também exigem sinal de áudio, não apenas compilação.

## Verificação executada

| Camada | Evidência | Limite |
| --- | --- | --- |
| Regressões PF12 nativas | contatos, controles, rodas, volume, LEDs, cenas, favoritos/flash guards | host, sem medidas físicas |
| Matriz de encoders | páginas antigas idênticas à PF12; páginas novas testadas separadamente | não cobre todo gesto possível |
| VOICE C | áudio, oito presets, morph/wheel, sustain/release, bend, glide, sends, quatro tracks mistas | não define polifonia segura no JieLi |
| Persistência C | programa/favorito/cena e primeira nota de troca de pattern | armazenamento simulado |
| DSP isolado instrumentado | 64 combinações extremas e sweeps; 4.096.000 samples; UBSan + AddressSanitizer | LeakSanitizer desativado no ambiente |
| WASM | boot, áudio real não zerado, engine por parte, favorito após reboot | sem temporização física |
| AudioWorklet | `process()` real executado com host de teste e saída float finita | não comprova saída de som no browser/dispositivo |
| Navegador | inicialização, carregamento de presets e páginas operados; visual inspecionado | navegador informou modo de interface sem áudio |
| SMK37 físico | não realizado | firmware instalável não liberado |

O UBSan do aplicativo completo encontra um shift assinado na `obv_dsp.h` herdada.
O mesmo erro foi reproduzido na PF12 preservada. Não foi alterado neste checkpoint
para evitar uma mudança paralela no DSP analógico. O teste instrumentado isolado
dos formants passou; não se afirma que todo o aplicativo passou no UBSan.

## Recursos

Estado dos formants: 544 bytes no host para os oito cards. Tabela de seno:
2.048 bytes de constantes. Há também estado adicional de transição, notas
pendentes e o patch analógico guardado em RAM por track. Esses números não são
um relatório de linker JieLi nem comprovam que oito vozes são seguras no hardware.

Referência de build PF12 recuperada na auditoria: imagem 151.504/619.520 B;
RAM .data+.bss 69.008/98.304 B; pool 318.708/344.064 B. Esses são valores
anteriores à integração VOICE, não devem ser usados como consumo desta build.
CPU, flash/RAM alvo, polifonia mista e comportamento prolongado sob todos os
efeitos continuam pendentes de compilação/profiling físico.

## Conteúdo do pacote e retomada

- `source.zip`: fontes atuais, testes, licenças e simulador local em `web/voice-lab/`.
- `obervave-voice.bundle`: branches/histórico, incluindo a PF12 preservada.
- `simulator/`: cópia pronta do simulador experimental local.
- `audio/`: oito presets secos, combinação ANALOG+VOICE e Choir Aah com efeitos.
- `voice-ui-preview.png`: renderização nativa das quatro páginas, não screenshot do navegador.

Os WAVs são estéreo, 44,1 kHz/16 bits, sem normalização individual. Um fade de
80 ms fecha o arquivo de audição; não é alteração na engine. O exemplo misto
usa baixo ANALOG, acorde VOICE e lead VOICE. Não há DRUMS nesse exemplo.

Para recuperar o Git: `git clone obervave-voice.bundle obervave-voice` e
`git switch obervave-voice`. A referência preservada também pode aparecer como
`origin/pf12-preserved` no clone; o script de testes reconhece ambas. Testes: `sh tests/run_voice_foundation_tests.sh`.
Requisitos dos testes: compilador C com sanitizers, Python/Pillow, Node.js e Git.
Build do simulador: `ZIG=/caminho/zig WASM_OPT=/caminho/wasm-opt sh tools/build_voice_sim.sh`.
Não há publicação automática. O simulador precisa de HTTP local/localhost para
Worker/AudioWorklet; abrir o HTML diretamente por `file://` não é suportado.

## Próximas etapas

DRUM-X por MIDI e hits independentes → sequenciador de oito lanes com formato
versionado → revisão completa dos controles/migração → build/profiling JieLi →
simulador e hardware comparados → instalação/backup/retorno testados → RC.

GPL-3.0-only e créditos preservados em `LICENSE` e `LICENSING.md`. Referência
JIANT fixada em `e924445ae0e7c64636f15bd0758418ee312ce7ee`; portados apenas os
trechos de síntese vocal e auxiliares necessários. Não é uma release final.
