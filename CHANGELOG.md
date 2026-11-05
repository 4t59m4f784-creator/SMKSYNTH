# Changelog

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
