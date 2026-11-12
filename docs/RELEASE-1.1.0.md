# SMKSYNTH 1.1.0 - MIDI, sustain e Pro / Elite

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
