# SMKSYNTH ML1a /19651 — MIDI Layers

Experimental sobre a oficial 1.2.0, para Pro e Elite. Banco B: pads 1–16 selecionam os canais do teclado. Segure um e acrescente outros para formar um layer; soltar mantém a seleção; novo gesto com um único pad substitui o layer. Troca de bancos mantém a seleção. Entrada no MIDI MODE e troca de mapa voltam ao canal principal; a seleção de canais é temporária.

Teclado, rodas e sustain usam todos os canais selecionados, por USB e TRS/DIN. Encoders, faders e banco A mantêm os canais configurados. Os antigos mapeamentos do banco B são preservados no armazenamento para retornar à oficial, mas ficam inativos e não editáveis nesta experimental. Rodas mostram KEYS no campo de canal; seu tipo e número CC continuam editáveis.

Notas já iniciadas conservam seus destinos e afinação para o Note Off, inclusive após trocar canais/oitava. Sustained notes recebem pedal-off nos canais anteriores. Novos destinos recebem o estado atual das rodas antes das novas notas. Repetições, colisões de notas e saída bloqueada usam envio limitado por ciclo; até 37 x 16 notas não ocupam a fila de eventos dos outros controles.

O banco do synth, seus programas, ENDLESS, tema e registros MIDI existentes são preservados pelo pacote de atualização. A 1.2.0 continua oficial. Há retorno a ela neste instalador, sem inicializar o banco. Um backup .smksynth não contém tema ou mapas MIDI.

## Verificação

Testes nativos com AddressSanitizer/UndefinedBehaviorSanitizer: seleção, ausência de notas/CCs mapeados no B, 37 teclas x 16 canais, trocas com notas mantidas, mesmo pitch por duas teclas, sustain em canais antigos/novos, bend de 14 bits, CC1, fila bloqueada, retrigger, Panic e retorno ao synth. Testes WASM exercitam os controles reais e geram as imagens da interface. Os testes automatizados não medem o timing real da saída TRS; teste no hardware antes de promover.

Build: `python3 tools/build.py --board smk37 --model pro --release 1.2.0-ml1a --experimental-id 19651 --voice-hardware --sdk /path/to/sdk`. Elite: `--model elite`. Não use --first-install. Simulador: `SIM_VERSION=1.2.0-ml1a sh tools/build_controls_sim.sh`.

## Correção ML1a /19651

Reproduzido em teste nativo com as filas reais: um host USB configurado que não lê MIDI pode encher a fila e bloquear também a TRS, inclusive em um só canal. A TRS agora depende somente da disponibilidade de sua UART. Se o USB exceder sua fila, ao retomar a leitura descarta eventos antigos e envia sustain off, all notes off e all sound off nos 16 canais antes de aceitar novos gestos. Notas mantidas durante essa retomada precisam ser tocadas novamente no receptor USB; a TRS continua normalmente. Não altera os mapas, programas ou a versão pública 1.2.0.

Validação de hardware pendente: os testes automatizados cobrem a falha e a recuperação, mas não substituem o teste físico do cabo.
