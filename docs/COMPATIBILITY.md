# Compatibility

| Item | Supported / meaning |
| --- | --- |
| Physical target verified in this project | M-VAVE SMK-37 Pro |
| B2 update origins | 19613 / 19614 / 19615 only |
| B2 destination | SMK-37 Pro_19615 |
| Data layout | SMK16P1 |
| Authoring backup | smksynth-authoring-backup, version 1 |
| Accepted backup files | B1 and B2 complete `.smksynth` files |
| Simulator | Approved P16.2 musical base; its local sessions are separate from hardware banks |
| Stock firmware / another fork | First install and recovery not yet validated for this public package |
| FM-1 / SMK-37 Elite / MKE-P37 | Not offered by this package |
| DAW controller mode | Future idea, not implemented |
| Selective import / combining banks | Not implemented; restore is complete-bank only |
| Public factory collection | Awaiting Marcus's selection |

Saved-data compatibility and update-loader preservation are distinct. An old installer can overwrite storage even if the new program understands the data. Use only the documented version path and keep backups.


## Endless MIDI 1.2.1

Edição oficial: Endless MIDI, identidade 19653 para Pro e Elite. Inclui ENDLESS com MIDI USB/TRS por parte, seleção e layers de canais do teclado pelo banco B do MODO MIDI, rodas e sustain, bateria e identificação da versão. Pads A, encoders e faders mantêm os canais de seus mapas. Consulte o [manual atual](MANUAL.md) e as [notas da versão](RELEASE-1.2.1.md) para o fluxo completo. ML1 /19650, ML1a /19651 e Endless MIDI A /19652 atualizam diretamente, preservando os dados salvos.
