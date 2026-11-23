# Validação pública 1.3.0

Runtime aprovado: SU3 /19663; arquivos do firmware idênticos. Identidade pública 19664, Pro e Elite. Mudam somente versão/identidade de build e banco de fábrica na instalação inicial. RAM nativa 96.352/98.304 bytes; pool 332.180/344.064; voz 184 bytes. Sem redução de polifonia.

Quatro pacotes verificados: SHA-256, CRC, modelo/loader, aplicações iguais entre instalação inicial e update, banco de fábrica somente na inicial. Regressões nativas SU3, sanitizers, SUB aditivo, áudio/fallback dual-core, controles, MIDI e WASM aprovadas. Backup aceita identidades públicas e experimentais, incluindo 19663 e 19664. Testes de host não substituem instalação física ou medição de CPU. O usuário aprovou a SU3; os quatro pacotes públicos finais não foram instalados individualmente em hardware. Mantido o problema conhecido de cliques em vídeo USB no iPhone.

[Notas](RELEASE-1.3.0.md) · [Manifesto](../RELEASE.json)

A instalação inicial exige a V016 do modelo correto e usa o pacote com banco de fábrica; teclados com SMKSYNTH usam update preservando dados. Os testes recusam modelo errado, hash/CRC alterados, troca de dispositivo, loader estrangeiro e journal inválido antes de gravar. A retomada mantém a identidade de origem. O instalador 1.2.2 foi arquivado para transferências antigas pendentes. O simulador WebAssembly usa as mesmas fontes musicais e de interface com renderizador serial de host: demonstra som e controles, mas não mede a margem dual-core do teclado.
