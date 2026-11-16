# Validação pública 1.2.1

- Base funcional MO1a /19648 aprovada por Marcus no Pro, incluindo ENDLESS, MIDI MODE, Zoom, nomes de presets e cores. O fluxo Elite foi validado anteriormente por um usuário real. A promoção altera identidade/versão, sem modificar o comportamento aprovado.
- Identidades estáveis: SMK-37 Pro_19653 e SMK-37 Elite_19653. Aplicação Pro 224880 bytes; RAM 95392/98304; pool 328368/344064; loader 7703 bytes. Essas medidas são limites de build, não uma medição da carga de áudio em tempo real.
- Os quatro pacotes finais foram verificados por SHA-256, CRCs, modelo, identidades e loader. Primeira instalação contém o banco de fábrica; Update preserva os bytes do banco. Atualização 1.1.0 → 1.2.1 passou por 56 interrupções/retomadas em flash simulada por modelo, incluindo áreas protegidas e journal.
- Regressões nativas cobrem MIDI por parte, ENDLESS, sustain/Panic, transições do controlador, backpressure, release longo SUPER com limite de cinco vozes, cores, presets, sequenciador, pads, Split/Layer e controles físicos.
- O WASM público executa o loop real para ENDLESS (comprimentos 3/5/7, C4 fixo, HOLD, drums e salvamento), teclas/pads/controles independentes no MIDI MODE, retorno ao synth, tema persistente e salvamento com confirmação distinta. O banco demo mantém cinco patterns, oito favoritos, 90 BPM e nenhum preset pessoal extra.
- Site, instalador e simulador preservam PT/EN; links, quatro pins, fonte correspondente e quatro PDFs versionados são verificados. O hero é uma captura do renderer de splash do firmware, sem redesenho da marca.
- Não houve nova instalação física de cada um dos quatro pacotes 1.2.1. Testes de host/flash simulada não substituem teste de CPU e áudio no hardware. Mantido o problema conhecido de cliques/cortes em vídeo USB no iPhone.

Hashes: [RELEASE.json](../RELEASE.json). Downloads: `releases/1.2.1/SHA256SUMS.txt`.


## Endless MIDI 1.2.1

Edição oficial: Endless MIDI, identidade 19653 para Pro e Elite. Inclui ENDLESS com MIDI USB/TRS por parte, seleção e layers de canais do teclado pelo banco B do MODO MIDI, rodas e sustain, bateria e identificação da versão. Pads A, encoders e faders mantêm os canais de seus mapas. Consulte o [manual atual](MANUAL.md) e as [notas da versão](RELEASE-1.2.1.md) para o fluxo completo. ML1 /19650, ML1a /19651 e Endless MIDI A /19652 atualizam diretamente, preservando os dados salvos.


## 1.2.2 — Endless MIDI

TF1 /19654 tuning fix validated on hardware by a user. Ten MIDI returns and ten hard PANIC resets preserve pitch with Vintage 0/32/80/127 and patch bytes. Model-specific 1.2.2 packages use identity 19655; only build version/identity changes relative to the validated fix. Factory bank and flash protection layout remain unchanged.
