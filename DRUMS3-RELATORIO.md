# Obervave Voice + Drums — checkpoint 3: DRUMS

Branch `obervave-voice`, base PF12 personalizada. Preservados: `pf12-preserved`
(21fddca9039eb1cdacedc0c8cc49d1ab5dd9dd23) e `voice2-preserved`
(21d25ed8b9ffbcb57a97e05ea90d26c4ed76ded4). Nenhum instalador estável foi substituído.
A PF12 ainda aguarda validação física; a última aprovação física registrada é PF9.

## O que este checkpoint entrega

ANALOG, VOICE e DRUMS selecionáveis independentemente nas quatro partes. DRUMS
usa um pool próprio de oito slots por parte (um hit por instrumento, retriggerável),
sem reservar cards melódicos. Closed/open hi-hat fazem choke; não soam juntos.
ANALOG e VOICE mantêm os oito cards compartilhados do checkpoint anterior.

Oito instrumentos: kick, snare, clap, closed/open hi-hat, tom, rimshot e bell.
Cinco kits iniciais: 808 Boom, 909 Club, 606 Mini, 707 Snap, DRUM-X. Síntese apenas;
não há samples. Os nomes indicam inspiração e pontos de partida, sem promessa de
fidelidade às máquinas originais, especialmente à TR-707 baseada em samples.
A qualidade musical ainda depende da escuta e ajuste no SMK37.

Volume, pan, mute/solo, chorus/delay/reverb e drive usam o mixer existente.
Velocity afeta a amplitude; note-off não corta hits one-shot. Panic e troca de
engine limpam o pool; a troca preserva a rampa e o mecanismo de notas pendentes.
Gravações manuais em flash aguardam também as caudas percussivas. Sem autosave.

## Seleção discreta e controles

A escolha não aparece como primeiro parâmetro ao selecionar a parte. Ela fica
em **PART SETUP**, ao final do ciclo GLOBE: GLOBAL → VOICE → OUTPUT → PART SETUP.
A página VOICE desse ciclo é o nome legado da configuração de vozes; ela não
seleciona a engine vocal. Trocar de parte na performance mantém a performance.
Os atalhos de bancada do simulador ficam dentro de uma seção recolhida de ajuda.

| Contexto DRUMS | Função |
| --- | --- |
| PART SETUP K1 / K2 | engine / kit |
| PART SETUP K3 / K4 | volume da parte / afinação global |
| PARA | alterna DRUM KIT e DRUM SOUND |
| DRUM KIT / performance / faders A | morph, tone, decay global, fold |
| DRUM SOUND K1–K4 | instrumento, volume, decay relativo, pitch ±12 semitons |
| Oito pads superiores, somente em DRUM SOUND | oito hits; o pad escolhe também o instrumento editado |
| PATCH | retorna à performance, restaurando os pads normais |
| Oito pads inferiores | favoritos mantidos |
| K5–K8 / faders B | funções e pickup da PF12 preservados |

MIDI: kick 36, snare 38, clap 39, closed hat 42, open hat 46, tom 45,
rimshot 37, bell 56. Notas 60–67 são aliases cromáticos para os oito instrumentos.
O teclado virtual inicia em MIDI 48; portanto C4–G4 (incluindo as teclas pretas)
correspondem aos oito aliases. Cada hit segue a parte de destino do MIDI.
As barras e indicadores da tela usam a identidade escura/vermelha/azul existente;
não foram importados personagens, interface ou animações do JIANT.

## Sequenciamento e persistência: limite explícito

O sequenciador nativo toca os oito instrumentos distribuídos nos steps, com
**até quatro notas simultâneas por step**. Swing, chance, ratchet, velocity e
nudge seguem o transporte existente. Transpose e tie melódicos ficam guardados
no pattern, mas não transpoem o mapa de bateria nem impedem retriggers DRUMS.
Na página DRUM SOUND com banco de performance selecionado, REC mantém os pads
como hits. Soltar o pad libera sua posse sem criar ties; notas gravadas mantêm
o mapa de instrumentos mesmo com transpose guardado. Essa rota ainda tem o limite de
quatro notas por step. Não é a grade percussiva de oito lanes da Fase 4.

O layout de programas, favoritos e patterns não mudou neste checkpoint. O patch
continua com 80 bytes; assinatura OVE versão 1 identifica engine 2 (DRUMS), e byte
73 identifica o kit. Parâmetros por instrumento reutilizam bytes interpretados
por contexto, conforme os arrays DP_LEVEL/DP_DECAY/DP_PITCH em `obv_drums.h`.
Patches antigos sem assinatura continuam ANALOG. Não se deve abrir sessões novas
na versão estável esperando que ela reconheça VOICE/DRUMS; a compatibilidade
verificada é a leitura de dados antigos pelo ramo experimental.

Próximo checkpoint: oito lanes, hits simultâneos dos oito instrumentos e edição
por steps, com representação versionada, testes de migração e roundtrip de dados
legados antes de alterar o armazenamento. Nenhum pattern antigo foi convertido
ou sobrescrito automaticamente nesta etapa.

## Origem e adaptação do DSP

JIANT: commit e924445ae0e7c64636f15bd0758418ee312ce7ee.
`firmware/src/drumx_voice.c`, blob 07b6b665a02f5f9011e9953912ac49bf18fe4084.
Mantidos créditos de Leo Kuroshita/Hügelton Instruments e Juan Ignacio Antonucci,
GPL-3.0-only. Ver `LICENSING.md` e `LICENSE`.

O port contém oscilador, FM/metal/bell, envelope de pitch, ruído filtrado,
clap, camada sintética, fold e filtro de saída. Contexto passado explicitamente
por parte. Sem buses, sequenciador, UI, palavras ou samples do JIANT.
Núcleo a 22,05 kHz, saída interpolada a 44,1 kHz, envelope adaptado do bloco
original de 32 para 16 samples. Tabela SVF própria preserva o mapeamento de cutoff
JIANT; não usa o mapeamento analógico do Obervave. Intermediários largos evitam
overflow nos extremos cobertos pelos testes. O softclip usa a mesma fórmula de
tabela tanh dos dois projetos. A engine VOICE aprovada no checkpoint 2 não mudou.

## Verificação realizada

- Suite nativa de performance, controles, favoritos, programas, scenes, wheels,
  matriz de encoders e VOICE: passou, inclusive matriz legada PF12 idêntica.
- ANALOG vs PF12: 384.000 frames stereo; 767.715 amostras não nulas; PCM idêntico.
  SHA256: 4ae6e1508a661e6e362824033f358902a80a81fe527cbcf52b36117c5ed42932.
- Mix ANALOG + VOICE vs checkpoint 2: 409.600 frames, 814.914 amostras não nulas;
  PCM idêntico. SHA256: 08d7ce3bc6886ae0c264b54359eaee90bdb2ffa7a214300b0be44c6221873781.
- Cinco kits × oito instrumentos: energia de áudio não nula; velocity, choke
  (incluindo hits no mesmo bloco), notas curtas durante troca, panic e sends.
- Workflow nativo: seleção discreta, pads contextuais, edição por instrumento,
  automação e roundtrip real do armazenamento simulado de programas/patterns/favoritos.
- Sequência nativa: todos os oito instrumentos ao longo dos steps, até quatro
  simultâneos, swing/ratchet/nudge, retrigger com tie, mapa sem transposição.
- DSP DRUM-X isolado: 640 combinações kit/instrumento/extremos, 10.240.000 samples,
  AddressSanitizer + UBSan; leak detection desligada neste ambiente.
- DSP VOICE isolado: 64 extremos e sweeps, 4.096.000 samples, mesmos sanitizers.
- WebAssembly e process() real do AudioWorklet sob host Node: cinco kits, oito
  instrumentos, saída finita/não nula, partes independentes, favorito e reboot,
  pads contextuais; nenhuma alocação melódica por DRUMS.
- Navegador local: revisão visual e navegação em fallback de interface **sem áudio**.
  Não foi validado áudio no dispositivo de saída desse navegador.

A suite completa não é declarada livre de UB: a checagem ampliada anterior
identificou um shift negativo herdado em obv_dsp.h, também reproduzido na PF12.
Os testes isolados acima instrumentam os ports novos e não apagam essa pendência.

## Recursos medidos e limites

Pool DRUMS: **2.688 bytes** por sizeof no host (32 slots, quatro partes).
Indicadores: 64 bytes de peaks + 128 bytes de timestamps + 4 bytes de seleção.
Isso não é medida de RAM/flash final do JieLi. `web/voice-lab/build.json` registra
o tamanho e hash exatos da build WASM; seu tamanho não representa flash do teclado.

Não foram medidos CPU, flash, RAM final ou latência no SMK37. Não há polifonia
melódica nova prometida, nem garantia de 28 hits simultâneos com oito vozes e todos
os efeitos no processador físico. São capacidades lógicas para teste, não capacidade
musical certificada. Nenhum teste físico ou de duração prolongada foi executado.
O empacotamento instalável continua bloqueado em tools/build.py.

## Reproduzir e recuperar

1. Extraia `source.zip` ou clone o bundle, branch `obervave-voice`.
2. Rode `sh tests/run_voice_foundation_tests.sh` e `sh tests/run_drums_tests.sh`
   (C compiler, Python3 e Node). O WASM pronto acompanha o código.
3. Para reconstruir o WASM, defina ZIG e WASM_OPT e rode `sh tools/build_voice_sim.sh`.
4. Demos: `python3 tools/render_drum_demos.py` (NumPy). São grooves gerados pelo
   sequenciador C, sem samples ou normalização; apenas fade de 80 ms no fim do arquivo.
5. O simulador usa memória local v3 separada. Importe uma sessão v2 manualmente
   pelas ferramentas de ajuda se desejar; ela é copiada para o espaço experimental.
6. Antes de reiniciar, salve explicitamente, aguarde as caudas e a conclusão da
   gravação. Exporte a sessão. A importação substitui apenas a memória experimental.
7. Não há instalação física desta build. A recuperação do teclado deve ser
   preparada e validada com os instaladores estáveis preservados antes do primeiro
   firmware experimental. O relatório não substitui um procedimento físico testado.

O simulador experimental permanece em https://obervave-voice-lab.padrini.chatgpt.site/
com acesso privado. Os instaladores e o simulador da versão original não foram alterados.
