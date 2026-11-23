# Validação pública 1.3.1

Runtime aprovado: PS3 /19667. As fontes do firmware são idênticas à PS3 aprovada; a promoção muda versão/identidade para 19668 em Pro e Elite. RAM estática 96.384/98.304 bytes; pool 332.180/344.064; mesma polifonia. Nenhuma alteração de síntese adicional entrou na promoção.

O usuário confirmou o primeiro bumbo na transição para o pattern 1 e grande estabilidade em trocas repetidas com BPMs diferentes. Regressões nativas PS1/PS2/PS3, sanitizers, primeiro ataque no mesmo CTL de recall, 10.128 pedidos rápidos e 253 commits, PCM de recalls sem mudança de engine idêntico à PS2, áudio/fallback dual-core, controles, MIDI e WASM foram verificados no desenvolvimento.

Os quatro pacotes públicos finais são checados por SHA-256, CRC, identidade e loader do modelo. Aplicações de instalação inicial/update são iguais; somente a primeira instalação recebe o banco de fábrica, inalterado. A atualização preserva banco, tema e mapas MIDI. A ferramenta de backup reconhece 19664–19668, além das versões anteriores compatíveis.

Testes de host não substituem medição de CPU ou a instalação de cada pacote em cada modelo. O runtime PS3 foi aprovado em hardware; os quatro pacotes finais 1.3.1 não foram instalados individualmente. Mantido o problema conhecido de cliques em gravação de vídeo USB no iPhone. O manual e os PDFs da 1.3.0 continuam válidos, pois não houve mudança de funcionalidades.

[Notas](RELEASE-1.3.1.md) · [Manifesto](../RELEASE.json)
