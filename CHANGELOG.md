# Changelog

## 1.1.0 - 2026-11-11


## Português

A 1.1.0 promove a MIDI M1.2 validada por Marcus no SMK-37 Pro e integra Pro e Elite no mesmo instalador. O caminho de instalação Elite também foi validado por um usuário real. A identificação automática escolhe um pacote específico para cada modelo; não é preciso selecionar ou baixar firmware manualmente.

- **MIDI OUT USB e TRS/DIN:** teclas, pads, Split/Layer, arpejadores e sequenciadores transmitem as notas reais das quatro partes. GLOBAL MIDI CH define o canal-base de A (1-13); B-D usam os três seguintes. OMNI transmite em 1-4. DRUM-X usa o canal da própria parte. Cada Note Off conserva canal/nota de origem.
- **Pitch bend e mod wheel:** enviados no canal da parte selecionada, inclusive em Split/Layer. Trocar de parte neutraliza o canal anterior; o estado físico é reenviado após Panic ou recuperação de fila. USB e TRS seguem o mesmo roteamento.
- **Sustain físico:** conecte antes de ligar e mantenha solto na inicialização. Polaridade automática para pedal de chave, sem meio pedal. O sustain atende as quatro partes melódicas, com CC64 nos canais apropriados e liberação de canais antigos. Estados pendentes por canal evitam sustain preso durante soltura/reacionamento rápidos.
- **MIDI PANIC:** GLOBE + PLAY para PLAY/REC, desliga HOLD/sustain, limpa notas pendentes e corta as vozes. USB/TRS enviam CC64=0, CC123=0 e CC120=0 aos 16 canais. Solte o pedal antes de reutilizá-lo; caudas dos efeitos podem terminar.
- **Correção de notas presas:** vozes POLY/DUAL são liberadas corretamente após mudar para MONO/UNISON com teclas pressionadas. O Note Off já chegava; um retorno antecipado na engine impedia parte da liberação. Glide não mudou e sua alteração isolada não reproduziu a falha nos testes.
- **Instalador único Pro / Elite:** primeira instalação exige V016 oficial do modelo conectado e inicializa o banco. Atualização preserva programas, favoritos, patterns e configurações salvas, inclusive das builds MIDI M1-M1.2 e de testes compatíveis. Exporte um backup antes. Retomadas antigas continuam nos instaladores preservados.

Identidade de destino: `SMK-37 Pro_19634` ou `SMK-37 Elite_19634`. Cinco patterns e oito favoritos de fábrica mantidos. A ENDLESS permanece em ramo separado; seus recursos não estão nesta release. Atualizar preserva o banco salvo, mas para desfazer alterações feitas em outra linha é necessário restaurar o backup anterior.

As sequências enviam notas, mas não há MIDI Clock/Start/Stop de saída nem Program Change automático ao trocar presets internos. A recepção de bend continua global no canal-base, herdada da engine; a saída de bend é por parte. Áudio USB 44,1/48 kHz mantido; cliques/cortes ao gravar vídeo no iPhone com áudio USB continuam como problema conhecido.

Créditos: OberVave / zvenson e colaboradores; DRUM-X / JIANT; SUPER / Clipper / 3dSam; Felucca, Dinghy, SLOOP e port SMK-37. Driver TRS adaptado de M. Geisler (genkigenki/makkuro), via SLOOP / Tonitor, sob GPL-3.0-only. Veja CREDITS.md e LICENSING.md.

## English

1.1.0 promotes MIDI M1.2, hardware-validated by Marcus on SMK-37 Pro, and combines Pro and Elite in one installer. The Elite installation path was also validated by a real user. Automatic identification selects a model-specific package; no manual firmware selection is required.

- USB and TRS/DIN output transmit all four parts: keys, pads, Split/Layer, ARP and sequencers. A uses GLOBAL MIDI CH base 1-13; B-D use the next channels. OMNI transmits 1-4. DRUM-X uses its part channel. Note Off retains the original channel and pitch.
- Physical Pitch Bend and CC1 follow the selected part, including Split/Layer. Old channels are neutralized on focus changes; wheel state is resent after Panic/queue recovery.
- Switch-pedal sustain detects polarity at startup. Connect before power-on, released. No half-pedal support. Sustain covers all melodic parts and releases previous channels correctly; pending CC64 values are independent per channel.
- GLOBE + PLAY stops transport/REC, clears HOLD/sustain and pending notes, kills voices and sends CC64/123/120 off on all 16 channels. Release the pedal before reusing sustain. Effects tails may finish.
- Fixed old POLY/DUAL voices remaining gated after a live change to MONO/UNISON. Glide is unchanged; Glide-only edits did not reproduce the bug in tests.
- One installer identifies Pro or Elite and chooses first installation from model-specific stock V016, or a bank-preserving SMKSYNTH update. Export a backup first. Old interrupted installers are preserved.

Target identities are `SMK-37 Pro_19634` and `SMK-37 Elite_19634`. Factory patterns/favourites are retained. ENDLESS remains separate. Sequence notes transmit; MIDI Clock/Start/Stop output and automatic preset Program Changes are not included. Incoming bend remains global on the base channel. USB audio remains supported; iPhone USB video clicks/dropouts remain a known issue. Original credits and GPL-3.0-only notices are preserved.


## 1.0.3 — 2026-11-05

SUPER chega aos dois osciladores da engine ANALOG: sete serras por oscilador, com DETUNE e MIX independentes. Na página OSC, PW passa a ser DETUNE ao escolher SUPER. PARA também acessa a página SUPER: K1 DET1, K2 MIX1, K3 DET2 e K4 MIX2.

A origem da supersaw é o [Clipper, de 3dSam](https://github.com/isod89/clipper-fm1), sobre a plataforma Felucca. O Clipper referencia a análise do JP-8000 de Adam Szabo. O SMKSYNTH adapta sua curva e offsets de desafinação, acrescentando MIX independente, normalização e otimizações próprias. Os créditos e avisos GPL-3.0-only são preservados em [CREDITS](https://github.com/4t59m4f784-creator/SMKSYNTH/blob/main/CREDITS.md) e [LICENSING](https://github.com/4t59m4f784-creator/SMKSYNTH/blob/main/LICENSING.md).

A versão pública promove a SS3, aprovada por Marcus no SMK-37 Pro após os testes de acordes com SUPER. As otimizações mantêm os resultados numéricos do DSP de referência. A polifonia continua sendo de oito vozes ANALOG compartilhadas; UNISON empilha vozes completas. SYNC e X-MOD são ignorados quando algum OSC usa SUPER. MIX é salvo em presets, favoritos, cenas e backups, sem CC/automação dedicados. DETUNE reaproveita PW e sua automação. Não é uma emulação exata do JP-8000/JP-8080.

No instalador, **Atualizar** preserva o banco de SMKSYNTH 1.0.x ou SUPER SS1/SS2/SS3. Exporte um backup antes. **Instalar** inicializa o banco e exige o firmware oficial SMK-37 Pro V016. A identidade pública permanece `SMK-37 Pro_19618`. Se uma instalação antiga estiver interrompida, retome primeiro pelo instalador daquela versão. Firmware anterior à SUPER não interpreta seus novos sons; guarde um backup anterior para eventual retorno.

Áudio USB da 1.0.2 mantido: estéreo, 16 bits, 44,1/48 kHz, testado no Mac e no iPhone com AudioShare. Problema conhecido: vídeos gravados com áudio USB no iPhone podem apresentar cliques e cortes.

### English

SUPER adds seven saws to each ANALOG oscillator, with independent DETUNE and MIX. Selecting SUPER turns PW into DETUNE. PARA opens the SUPER page: K1 DET1, K2 MIX1, K3 DET2, K4 MIX2.

The supersaw originates in [Clipper by 3dSam](https://github.com/isod89/clipper-fm1), built on Felucca. Clipper cites Adam Szabo’s JP-8000 analysis. SMKSYNTH adapts its detune curve and offsets, adding independent MIX, normalization and its own optimizations. GPL-3.0-only notices and attribution are retained in CREDITS and LICENSING.

This release promotes the SS3 implementation approved by Marcus on his SMK-37 Pro. DSP optimizations preserve numerical reference results. Eight ANALOG voices remain shared; UNISON stacks whole voices. SYNC and X-MOD are bypassed when either OSC uses SUPER. MIX is saved in presets, favourites, scenes and backups, with no dedicated CC/automation. DETUNE reuses PW and its automation. This is not an exact JP-8000/JP-8080 emulation.

**Update** preserves the saved bank from SMKSYNTH 1.0.x or SUPER SS1/SS2/SS3; export a backup first. **Install** initializes the bank and requires official SMK-37 Pro V016. Public identity remains `SMK-37 Pro_19618`. Resume an interrupted older installation using its original installer first. Firmware predating SUPER cannot interpret its new sounds; retain a pre-SUPER backup for downgrades.

USB audio from 1.0.2 is retained: stereo 16-bit, 44.1/48 kHz, tested on Mac and on iPhone with AudioShare. Known issue: iPhone video recordings using USB audio may contain clicks and dropouts.


## 1.0.2 — 2026-11-04

- Áudio USB estéreo 16 bits em 44,1/48 kHz, com a implementação validada na UA1. Testado por Marcus no Mac e no iPhone com AudioShare. JUMP/CATCH preservado.
- Problema conhecido: a gravação de vídeo com áudio USB no iPhone pode apresentar cliques e pequenos cortes durante a reprodução, independentemente das trocas de patterns. Em investigação.
- Stereo 16-bit USB audio at 44.1/48 kHz, using the UA1 implementation. Tested by Marcus on Mac and on iPhone with AudioShare. JUMP/CATCH retained.
- Known issue: iPhone video recording using USB audio may contain clicks and brief dropouts during playback, independently of pattern changes. Under investigation.


## 1.0.1 — 2026-11-04

- GLOBAL: FADER MODE JUMP/CATCH. Em CATCH, o fader assume o parâmetro ao cruzar seu valor; a proteção é reativada ao mudar de contexto.
- Encoders relativos com filtragem de transições inválidas e proteção contra deltas pendentes após mudanças de contexto.
- Atualização direta do SMKSYNTH com preservação do banco.
- USB dedicado a MIDI, backup/restauração e atualizações; áudio pela saída física. Diagnósticos de desenvolvimento removidos.

### English

- GLOBAL: FADER MODE JUMP/CATCH. CATCH engages when the fader crosses the current value and rearms when its context changes.
- Relative encoders reject invalid transitions and stale deltas across context changes.
- Direct SMKSYNTH update preserves the saved bank.
- USB provides MIDI, backup/restore and firmware updates; record audio from the physical output. Development diagnostics removed.

## 1.0.0 — preparação do lançamento · 2026-11-02

- Base DP2 testada e aprovada no SMK‑37 Pro.
- Cinco patterns com cenas completas e oito favoritos de Marcus Padrini.
- Banco da base preservado; zero presets de usuário extras.
- Instalação inicial a partir da M‑VAVE V016; identidade estrita e retomada vinculada ao pacote.
- Simulador com o mesmo conteúdo; backup reconhece 19618.
- README, créditos, guias, fontes, hashes e evidências.

O pacote inicial foi validado em simulação. Primeira instalação física por USB ainda não confirmada. Checkpoints técnicos de origem permanecem documentados no repositório.
