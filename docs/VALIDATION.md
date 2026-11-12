# Validação da versão pública 1.1.0

- Marcus aprovou todas as funções da base MIDI M1.2 no SMK-37 Pro: MIDI TRS/USB, pitch bend e mod wheel por parte, pedal de sustain, MIDI PANIC e correção de notas presas. Um usuário real validou o fluxo de instalação no SMK-37 Elite.
- Os pacotes finais 1.1.0 foram recompilados dessa base com identificação própria (19634 em ambos os modelos). A validação física citada é a da base testada e do fluxo Elite, não uma nova instalação destes quatro arquivos finais.
- Aplicação: 167200 bytes; RAM estática: 94480/98304 bytes; pool: 325984/344064 bytes; loader: 7703 bytes.
- Instalador unificado: quatro pacotes separados por modelo/operação, hashes e CRCs, seleção automática, recusa de troca de modelo/dispositivo, origem desconhecida e loader incompatível. Retomada vinculada ao modelo, pacote e origem inicial. Instalação inicial prepara o banco; atualização o preserva.
- Regressão em host: notas por parte; CC1/pitch bend/CC64; Panic em 16 canais; liberação de notas após mudança de modo; sustain com outras teclas pressionadas; split/layer e sequenciador; USB; backup; simulador e PT/EN. Testes de flash Elite incluem interrupção/retomada e preservação do banco.
- Os quatro pacotes finais são inspecionados quanto a modelo, identidade, loader, aplicação e dados de fábrica. Documentação e links públicos são verificados na publicação.
- ENDLESS permanece separado. USB áudio mantém a limitação conhecida na gravação de vídeo do iPhone. Testes de host não medem CPU nem substituem a validação no instrumento.

Hashes: [RELEASE.json](../RELEASE.json). Download checksums: `releases/1.1.0/SHA256SUMS.txt`.

## Registro histórico da 1.0.3

# Validação da versão pública 1.0.3

- Runtime: SUPER SS3 (`0b4ccdd4892dd6d6d5a1a4f206aa939aca46a853`), aprovado por Marcus no SMK-37 Pro após testes de acordes.
- Build público: aplicação 164052 bytes, RAM estática 86624/98304 bytes, pool 325984/344064 bytes; loader de atualização 7703 bytes. Código crítico em RAM sem chamadas externas.
- DSP: 32768 blocos SUPER, 1572840 divisões e 32768 blocos SAW equivalentes à referência; acordes, notas agudas, release, modulações, dois SUPER e efeitos no teste integrado. Hash PCM: `7c7688bb6186498d7f9abe83db713e8ef7f4c98b69bfe094007965aa50471fba`.
- Loader em flash simulada: atualização 1.0.2 → 1.0.3 e SS3 → 1.0.3 preserva o banco byte a byte, inclusive 42 pontos de interrupção e retomada. Áreas protegidas e journal verificados. Primeira instalação V016 usa pacote separado e inicializa conteúdo de fábrica.
- Instalador: hashes, identidade, modelos, atualização com mesma identidade e origens SS1/SS2/SS3; retomada vinculada ao pacote e à origem. Pacotes históricos e suas páginas de retomada preservados.
- Simulador recompilado com SUPER: boot real do firmware, cinco patterns, oito favoritos, 90 BPM, slots 6–16 livres e ausência de autosave; MIDI e controles físicos exercitados.
- USB: implementação UA1/1.0.2 preservada. Testada no Mac e no iPhone com AudioShare; vídeos USB no iPhone ainda podem apresentar cliques/cortes.

Hashes de pacotes, WASM e conteúdo de fábrica: [RELEASE.json](../RELEASE.json). Checksums dos downloads: `releases/1.0.3/SHA256SUMS.txt`. Créditos SUPER: [Clipper](https://github.com/isod89/clipper-fm1).

Testes de host e flash simulada não medem o tempo de CPU real. A aprovação sonora e de performance da SS3 é o teste de Marcus no instrumento; não implica garantia para qualquer combinação de unison, efeitos e polifonia.
