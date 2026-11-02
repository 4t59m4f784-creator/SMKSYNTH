# SMKSYNTH SL1 — Split & Layer

Protótipo 3.0.1-sl1 para SMK-37 Pro, identidade SMK-37 Pro_19606.
Base exata: AD1 oficial aprovada, commit de fonte `a0cbe7e348384cd00af40b2c2f117b083899a07f`.
Branch isolada: `smksynth-split-layer`. A AD1 oficial e seu instalador público não foram substituídos.

## Usar

1. Saia da página SEQ/REC para retomar os bancos manuais dos pads.
2. Toque KNOB BANK até **BANK 4 SPLIT / LAYER**. Os três bancos anteriores mantêm suas funções.
3. Segure dois pads entre 1–4 (partes A–D). São aceitas somente partes ANALOG. A combinação começa em LAYER. A ordem do gesto determina primeira/segunda parte.
4. Pads **5 SINGLE**, **6 SPLIT**, **7 LAYER** mudam o modo. Pads 8–16 ficam reservados nesse banco.
5. K1 escolhe o modo; K2/K3 editam os níveis exclusivos do teclado de cada parte; K4 escolhe a nota de divisão. K5 alterna **LEVELS / OCTAVES** para K2/K3 editarem as oitavas individuais, de −2 a +2. K6 continua selecionando a parte, K7 o programa/contexto anterior e K8 o master.
6. **BT/SAVE duas vezes na tela KEYBOARD salva o pattern com a configuração.** Também é possível salvar pelo banco PATTERNS ou pela página SEQ como na AD1. Aguarde a conclusão da gravação antes de desligar.

Em SPLIT, a primeira parte recebe notas abaixo do ponto; a segunda recebe a nota do split e todas acima. O ponto padrão é C4 (MIDI 60), comparado antes das oitavas individuais. Em LAYER, ambas recebem a nota, com suas respectivas oitavas. Notas deslocadas para fora de MIDI 0–127 são ignoradas, sem acumular no limite.

SINGLE devolve o teclado à parte selecionada. Sair da tela/banco mantém o modo atual; a identificação SPLIT/LAYER e as partes aparecem na tela de performance. Notas já pressionadas conservam seus destinos até os respectivos note-offs, mesmo se a combinação, oitava ou parte selecionada mudar. Novos note-ons seguem a nova configuração. O pedal sustain continua valendo para ambas as partes.

## Ganho e gravação

Os níveis são ganhos exclusivos das vozes disparadas pelo teclado combinado (0–100%, padrão 100%). Incluem notas geradas pelo arpejador alimentado por esse teclado. Não alteram patch volume, mixer, sends, nível de sequências ou eventos MIDI endereçados diretamente às tracks. O ganho segue a rampa de amplitude já existente; não foi alterada a síntese dos osciladores, filtros, envelopes ou efeitos.

**REC grava somente a parte selecionada em K6**, mesmo quando as duas estão soando. A oitava dessa parte é considerada na nota gravada; o ganho exclusivo de monitoramento não reduz a velocity registrada. Para gravar a combinação, selecione uma de suas duas partes. Se a parte selecionada não pertence ao par, o teclado combinado não grava notas silenciosamente nessa terceira parte. A gravação normal por pads, especialmente DRUM-X, permanece com seu roteamento anterior. Entrada por steps mantém o workflow da AD1.

As sequências continuam independentes. SEQ TRANSPOSE permanece global somente para sequências ANALOG; não transpõe teclado ao vivo nem DRUM-X. DRUM-X não pode ser escolhida para a combinação. Se um timbre/cena transformar uma das partes escolhidas em DRUM-X, a combinação fica inativa e o teclado opera em SINGLE até existirem duas partes ANALOG válidas. A configuração guardada não é apagada automaticamente.

O pool continua sendo **8 vozes ANALOG compartilhadas**. Um acorde em LAYER POLY de duas partes usa duas vozes por tecla; sequências e unison usam o mesmo pool e mantêm a política de prioridade existente. MONO/UNISON continuam monofônicos por parte: sequenciador e teclado nessa mesma parte disputam seu grupo conforme o comportamento de prioridade do instrumento. DRUM-X mantém seus hits independentes.

## Patterns, cenas e SAVE

Modo, duas partes, split point, oitavas e ganhos pertencem ao pattern, em uma extensão KBD1 de **12 bytes**. Patterns antigos SEQ4 (13.416 B), AD1/DRS1 (14.448 B) e a migração anterior dos patterns 1.5 continuam sendo lidos. Os antigos começam em SINGLE. Patterns com configuração nova têm 14.460 B; o snapshot de sons/mixer continua com 380 B, sem campo duplicado de teclado.

A troca sincronizada existente carrega a configuração na mesma virada que as sequências, sons e mixer. Alterações existem só em RAM até um SAVE explícito. Não foi introduzido autosave. Favoritos/cenas alteram seus sons e estados anteriores, preservando a configuração atual do teclado. Copiar um pattern inclui sua configuração; não existe salvamento duplicado em uma cena.

## Instalação e retorno

Instale pelo instalador experimental SL1, em Chrome/Edge de computador, USB de dados conectado ao **SMK-37 Pro**. Ele confere identidade, SHA-256 e CRCs e oferece retorno à **AD1 oficial**, PF12 e PF9. Mantenha o cabo e a alimentação até aparecer SMK-37 Pro_19606. Não é build para Elite ou MKE.

**Instalar/reinstalar/retornar reinicia o banco de saves usado pela AD1/SL1.** Esse é o comportamento do perfil de atualização atual. O banco original do Player é preservado e é usado para iniciar uma nova cópia. O backup incluído contém os arquivos da AD1 oficial, não uma extração dos presets/patterns que estão na memória do seu teclado. A compatibilidade de formatos foi testada com arquivos legados; não significa que a instalação conserve o banco experimental durante o flash. Registre os ajustes importantes antes de atualizar. Saves manuais persistem normalmente entre desligar e ligar.

## Verificação e limites

Veja `SL1-VALIDACAO.md` e os logs. Foram executados testes nativos, WASM, do código do AudioWorklet e do instalador em harness. **Validação física de áudio, CPU, latência, USB, sustain e uso prolongado no SMK37 continua pendente.** A AD1 segue sendo a versão oficial; a SL1 aguarda seu teste e aprovação.
