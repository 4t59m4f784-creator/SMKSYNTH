# VOICE JIANT — checkpoint 1 de fidelidade

## Estado

Ramo isolado `voice-jiant-fidelity`, a partir de `voice-headroom-audit` (786ef96). Referência JIANT fixada em e924445ae0e7c64636f15bd0758418ee312ce7ee. Não foi confirmado que essa seja a revisão instalada no JIANT da gravação do usuário.

Este checkpoint implementa o caminho vocal de referência e a integração necessária para avaliá-lo. Não declara equivalência de todo o instrumento JIANT, polifonia segura no hardware ou resolução auditiva confirmada do “ardido”.

## Implementação

- Núcleo vowel-only extraído do fonte fixado, com atribuições preservadas. Fonte glotal, BLEP, quatro formants, ganho/presence, BREATH, Q, SHIFT e curva de saída originais.
- Trajetória TALK original: interpola frequências/larguras entre os endpoints; deixou de percorrer automaticamente as vogais intermediárias por suavização da posição.
- Controle próprio em 32 amostras a 44,1 kHz; adaptação para duas fatias de 16 do Obervave. O bloco de 32 é calculado na primeira fatia; os eventos da segunda são refletidos no tick seguinte, até 16 amostras depois. Nenhuma redução da taxa de áudio.
- Cache exato de coeficientes, sem aproximação ou decimação adicional.
- Tabelas originais de pitch/ADSR/TALK, envelope de ataque linear, decay/release exponenciais e resposta à velocity, incluindo accent vocal acima de 110.
- Glide vocal em S, no modo RATE da referência; MONO retriggera o envelope a partir do nível atual e preserva fase/estados quando a mesma voz é reutilizada. TIME glide e matriz de modulação completa do JIANT não foram importados.
- Presets novos com os valores vocais/ADSR do JIANT para CHOIR AAH, OOH PAD, VOX LEAD, TALKBOX, WHISPER e CHANT. VOWEL SWP e DARK CHOIR continuam criações próprias. FX do JIANT não foram importados.
- Nova soma estéreo apenas para VOICE perfil 2: DC blocker, leveler, limitador e softclip padrão do JIANT. Essa soma evita uma segunda passagem pela saturação analógica na saída anterior aos efeitos do Obervave.
- ANALOG, DRUMS e VOICE legado usam a cadeia anterior. Os sends/efeitos/master opcionais do Obervave continuam compartilhados. Valores de mixer/pan/master do Obervave não têm necessariamente a mesma unidade dos controles globais do JIANT.
- A roda física continua sendo a extensão de morph por parte aprovada anteriormente, com valor mantido ao trocar de parte. LFO, pitch bend, MIDI, sequenciador, sustain, alocador de vozes e mapeamentos físicos permanecem integrados ao Obervave. Não se declara equivalência da matriz/LFO globais do JIANT.

## Compatibilidade de dados

Os patches mantêm 80 bytes. A versão do tag reservado é 2 apenas nos novos presets VOICE; patches VOICE de tag versão 1 continuam legíveis e executam o porte anterior. Não há autosave, conversão automática ou escrita na flash ao carregar/tocar.

Para avaliar o novo caminho, é necessário carregar um preset VOICE novo nesta build. Reabrir um programa antigo mantém seu perfil anterior deliberadamente. Ao escolher outro preset, faça save manual somente se desejar substituí-lo. Isso não migra parâmetros do seu programa antigo para o novo perfil.

Trocas entre os perfis VOICE usam o fade de engine, descartam as vozes do perfil anterior e conservam notas novas na fila. Há teste específico de troca nas duas direções.

Os ANALOG e DRUMS continuam com tags versão 1. As estruturas de programas, favoritos, cenas, patterns e grid DRUMS não foram ampliadas. A versão estável e seus instaladores não foram alterados.

Firmware mais antigo não reconhece patches VOICE perfil 2 como VOICE. Não transportar esses novos patches para builds antigas. A instalação/reinstalação experimental continua apagando o banco experimental e clonando os dados estáveis, conforme já documentado em VD2-INSTALACAO.md; backup/exportação completa desse banco continua pendente. Nenhum update foi instalado automaticamente.

## Testes executados

### Nativos automatizados

- 1.024.000 amostras PCM idênticas ao renderizador vowel-only fixado: vogais/endpoints mudando, TALK, BUZZ, BREATH, Q, cutoff, rampas de amplitude ascendentes/descendentes e glide nas duas direções. O fixture mantém o corpo original, com símbolos adaptados, e não usa o wrapper de saída do porte.
- 256.000 amostras estéreo idênticas ao DC/leveler/limiter/softclip padrão do JIANT, incluindo sobrecarga e recuperação em silêncio. LOWCUT, speaker/BASS+ e CLIP opcionais excluídos de ambos.
- 51.200 ticks de envelope/velocity comparados às regras originais, com duas fatias de áudio por tick. Velocities 1/64/100/127.
- 64 extremos de parâmetros e sweeps sob ASan/UBSan: 4.096.000 amostras, sem falhas detectadas. O teste cobre o perfil novo e observa a proteção de estado F4.
- Suítes existentes de controles, cenas, boot, multiengine, VOICE, DRUMS, workflow, grid e retenção da roda passaram.
- ANALOG continua com hash PCM `4ae6e1508a661e6e362824033f358902a80a81fe527cbcf52b36117c5ed42932`.
- Oito patches VOICE antigos capturados da base 786ef96 e reproduzidos com as mesmas notas: PCM idêntico entre base e este checkpoint. Hash `6d4dafaf1a72f2b56c806bd31f27f74b04bdbca0736af568236d1ca3cd886164`.

### Simulador

WASM recompilado do código novo. Testes Node/worklet passaram para VOICE audível, engines independentes, favoritos, DRUMS, grid de oito lanes, gravação por pads e persistência manual. Isso testa a execução WASM e o process() do worklet; não comprova som nos alto-falantes de um navegador nem áudio físico do teclado.

O simulador atualizado está incluído no pacote, para servir por HTTP local. Nenhum simulador hospedado ou instalador existente foi substituído neste checkpoint.

### Build para SMK37 Pro

Build experimental `3.0.1-vd3`, identidade `SMK-37 Pro_19603`:

- imagem: 180.868 / 327.680 bytes;
- RAM principal (.data + .bss): 79.600 / 98.304 bytes;
- pool: 325.900 / 344.064 bytes;
- pacote .fwsc: 651.327 bytes.

São resultados do linker/build, não medição de pico de pilha, CPU ou estabilidade no dispositivo. Não houve teste físico. Permanecem até oito slots melódicos compartilhados; isso não é promessa de oito VOICE seguras. O novo controle em 32 amostras reduz atualizações em relação ao porte anterior, mas custo real ainda precisa ser medido no teclado com morph e efeitos ativos.

## Prévia sonora

`VOICE-JIANT-Previa-Nativa.wav` é uma renderização offline estéreo a 44,1 kHz do código novo, com sends/drive/compressor desativados, sem normalização posterior. Duração aproximada: 18,29 s.

- 0–6,10 s: CHOIR AAH, acorde C4/E4/G4 e movimento da roda de morph.
- 6,10–12,19 s: VOX LEAD, notas C4/C5/G5, velocity 100.
- 12,19–18,29 s: TALKBOX, mesmas notas, TALK próprio do preset.

Pico medido na prévia: 20.161 unidades PCM16. É uma prévia para avaliação musical; não simula falta de CPU nem o DAC/entrada de gravação do SMK37.

## Reproduzir

`sh tests/run_jiant_fidelity_tests.sh` executa conformance e sanitizers. Os fontes originais fixados estão em tests/reference/jiant; o gerador de produção é tools/gen_jiant_voice.py. Os headers gerados ficam versionados, portanto builds normais não dependem de geração nem de rede.

Build do simulador: tools/build_voice_sim.sh com Zig e Binaryen. Builds físicos continuam exigindo a toolchain Jieli e o SDK/profiling experimental documentados no projeto. O pacote contém fonte completo em Git bundle, fontes de trabalho, logs, WASM e build experimental; não contém instalador web publicado novo.

## Próxima validação necessária

Ouvir a prévia e o simulador novo; comparar o sinal seco com a versão exata do JIANT usada pelo usuário; testar no teclado mono, acordes e morph sob carga; medir deadlines de áudio, CPU e comportamento com quatro partes e efeitos. Só então declarar o ruído resolvido ou determinar/restringir polifonia segura.
