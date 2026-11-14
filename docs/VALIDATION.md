# Validação pública 1.2.0

- Base funcional MO1a /19648 aprovada por Marcus no Pro, incluindo ENDLESS, MIDI MODE, Zoom, nomes de presets e cores. O fluxo Elite foi validado anteriormente por um usuário real. A promoção altera identidade/versão, sem modificar o comportamento aprovado.
- Identidades estáveis: SMK-37 Pro_19649 e SMK-37 Elite_19649. Aplicação Pro 224880 bytes; RAM 95392/98304; pool 328368/344064; loader 7703 bytes. Essas medidas são limites de build, não uma medição da carga de áudio em tempo real.
- Os quatro pacotes finais foram verificados por SHA-256, CRCs, modelo, identidades e loader. Primeira instalação contém o banco de fábrica; Update preserva os bytes do banco. Atualização 1.1.0 → 1.2.0 passou por 56 interrupções/retomadas em flash simulada por modelo, incluindo áreas protegidas e journal.
- Regressões nativas cobrem MIDI por parte, ENDLESS, sustain/Panic, transições do controlador, backpressure, release longo SUPER com limite de cinco vozes, cores, presets, sequenciador, pads, Split/Layer e controles físicos.
- O WASM público executa o loop real para ENDLESS (comprimentos 3/5/7, C4 fixo, HOLD, drums e salvamento), teclas/pads/controles independentes no MIDI MODE, retorno ao synth, tema persistente e salvamento com confirmação distinta. O banco demo mantém cinco patterns, oito favoritos, 90 BPM e nenhum preset pessoal extra.
- Site, instalador e simulador preservam PT/EN; links, quatro pins, fonte correspondente e quatro PDFs versionados são verificados. O hero é uma captura do renderer de splash do firmware, sem redesenho da marca.
- Não houve nova instalação física de cada um dos quatro pacotes 1.2.0. Testes de host/flash simulada não substituem teste de CPU e áudio no hardware. Mantido o problema conhecido de cliques/cortes em vídeo USB no iPhone.

Hashes: [RELEASE.json](../RELEASE.json). Downloads: `releases/1.2.0/SHA256SUMS.txt`.
