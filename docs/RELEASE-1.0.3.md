# SMKSYNTH 1.0.3 — SUPER

## Português

SUPER chega aos dois osciladores da engine ANALOG: sete serras por oscilador, com DETUNE e MIX independentes. Na página OSC, PW passa a ser DETUNE ao escolher SUPER. PARA também acessa a página SUPER: K1 DET1, K2 MIX1, K3 DET2 e K4 MIX2.

A origem da supersaw é o [Clipper, de 3dSam](https://github.com/isod89/clipper-fm1), sobre a plataforma Felucca. O Clipper referencia a análise do JP-8000 de Adam Szabo. O SMKSYNTH adapta sua curva e offsets de desafinação, acrescentando MIX independente, normalização e otimizações próprias. Os créditos e avisos GPL-3.0-only são preservados em [CREDITS](https://github.com/4t59m4f784-creator/SMKSYNTH/blob/main/CREDITS.md) e [LICENSING](https://github.com/4t59m4f784-creator/SMKSYNTH/blob/main/LICENSING.md).

A versão pública promove a SS3, aprovada por Marcus no SMK-37 Pro após os testes de acordes com SUPER. As otimizações mantêm os resultados numéricos do DSP de referência. A polifonia continua sendo de oito vozes ANALOG compartilhadas; UNISON empilha vozes completas. SYNC e X-MOD são ignorados quando algum OSC usa SUPER. MIX é salvo em presets, favoritos, cenas e backups, sem CC/automação dedicados. DETUNE reaproveita PW e sua automação. Não é uma emulação exata do JP-8000/JP-8080.

No instalador, **Atualizar** preserva o banco de SMKSYNTH 1.0.x ou SUPER SS1/SS2/SS3. Exporte um backup antes. **Instalar** inicializa o banco e exige o firmware oficial SMK-37 Pro V016. A identidade pública permanece `SMK-37 Pro_19618`. Se uma instalação antiga estiver interrompida, retome primeiro pelo instalador daquela versão. Firmware anterior à SUPER não interpreta seus novos sons; guarde um backup anterior para eventual retorno.

Áudio USB da 1.0.2 mantido: estéreo, 16 bits, 44,1/48 kHz, testado no Mac e no iPhone com AudioShare. Problema conhecido: vídeos gravados com áudio USB no iPhone podem apresentar cliques e cortes.

## English

SUPER adds seven saws to each ANALOG oscillator, with independent DETUNE and MIX. Selecting SUPER turns PW into DETUNE. PARA opens the SUPER page: K1 DET1, K2 MIX1, K3 DET2, K4 MIX2.

The supersaw originates in [Clipper by 3dSam](https://github.com/isod89/clipper-fm1), built on Felucca. Clipper cites Adam Szabo’s JP-8000 analysis. SMKSYNTH adapts its detune curve and offsets, adding independent MIX, normalization and its own optimizations. GPL-3.0-only notices and attribution are retained in CREDITS and LICENSING.

This release promotes the SS3 implementation approved by Marcus on his SMK-37 Pro. DSP optimizations preserve numerical reference results. Eight ANALOG voices remain shared; UNISON stacks whole voices. SYNC and X-MOD are bypassed when either OSC uses SUPER. MIX is saved in presets, favourites, scenes and backups, with no dedicated CC/automation. DETUNE reuses PW and its automation. This is not an exact JP-8000/JP-8080 emulation.

**Update** preserves the saved bank from SMKSYNTH 1.0.x or SUPER SS1/SS2/SS3; export a backup first. **Install** initializes the bank and requires official SMK-37 Pro V016. Public identity remains `SMK-37 Pro_19618`. Resume an interrupted older installation using its original installer first. Firmware predating SUPER cannot interpret its new sounds; retain a pre-SUPER backup for downgrades.

USB audio from 1.0.2 is retained: stereo 16-bit, 44.1/48 kHz, tested on Mac and on iPhone with AudioShare. Known issue: iPhone video recordings using USB audio may contain clicks and dropouts.
