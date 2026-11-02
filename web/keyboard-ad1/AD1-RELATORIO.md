# SMKSYNTH AD1 — arquitetura, auditoria e validação

## Base e isolamento

A VD4 publicada foi recuperada do repositório privado do simulador/instalador, commit `dbbde78e45eb030e694d4be40a9d5b039a8d294d`. O checkout local anterior era VD2 e não foi usado como substituto da versão atual.

- Fonte VD4 publicado: SHA-256 `ce17ac123af5898a33aef2c8b1a20176302eddb89c129b28f0f601cd0c696a7c`.
- Firmware VD4: SHA-256 `8e24365af8f9bc58efed9632cb9a62dcf3a91d3ed7bbee6e9ade04ff77860184`.
- O fonte recuperado foi registrado no Git como `cb4de0e`, branch `vd4-preserved`. Esse é um commit local de recuperação, não o SHA original do desenvolvimento VD4.
- Desenvolvimento independente: `smksynth-analog-drumx-transpose`.
- Instalação e simulador AD1 em caminhos novos. Páginas VD1–VD4, raiz experimental anterior e projeto estável original não foram sobrescritos.
- Backup VD4 separado: instalador, firmware, fontes correspondentes, pacotes de retorno e simulador.

## Remoção de VOICE

Os renderizadores formant/JIANT, seus estados, processamento vocal de master, presets, páginas vocais, visualização espectral e controles específicos foram removidos. O slot de engine 1 fica indisponível. O identificador DRUM-X permanece 2 para preservar patches, favoritos e cenas.

A DRUM-X dependia do seno interpolado da VOICE. A tabela e a função foram extraídas, com valores e aritmética idênticos, para `firmware/gen/obv_drum_sine.h`. Nenhuma mudança foi feita nos cálculos de síntese da DRUM-X além do nome dessa função. Licenças e créditos DRUM-X/JIANT foram preservados.

Programas vocais antigos recebem um som INIT ANA somente na cópia ativa de RAM. Não há autosave, conversão de slots ou regravação automática. Os testes e relatórios VOICE anteriores que permanecem no repositório são históricos; a suite vigente é `tests/run_ad1_tests.sh`.

## Transposição e notas

Cada voz melódica carrega uma marca de origem `seq_owned`. Os disparos do sequenciador e as notas diferidas durante uma troca de engine preservam essa marca. A renderização ANALOG soma o controlador global em Q8 semitons ao pitch, com limites de nota MIDI 0–127. O keytracking do filtro acompanha a nota efetiva. Note-offs, ties, ratchets e nudge mantêm as identidades canônicas das notas gravadas.

Não há reescrita de pattern, multiplicação de transposições em cada repetição ou retransmissão de notas para retunar um acorde. As fases dos osciladores não são reinicializadas pela transposição. As vozes de sequência em release também acompanham o valor global. Notas tocadas ao vivo são isoladas desse ajuste. DRUM-X usa seu pool de hits e não executa o caminho de pitch melódico.

A captura ocorre antes de tocar/gravar/escrever steps. Notas capturadas têm seus releases consumidos mesmo após sair da visualização; um novo disparo normal da mesma tecla transfere corretamente a propriedade do release. Não foram adicionados atalhos ou funções de pads.

## Recursos medidos no build físico

| Região | VD4 | AD1 | Limite do linker AD1 |
|---|---:|---:|---:|
| Imagem flash | 181.052 B | 170.544 B | 327.680 B |
| RAM .data + .bss | 79.600 B | 73.808 B | 98.304 B |
| Pool de RAM | 325.900 B | 325.900 B | 344.064 B |

Economia: 10.508 B de flash e 5.792 B de RAM estática. O pool é uma região distinta; não deve ser confundido com RAM livre total nem com uma medição de pico de stack. Permanecem 8 vozes ANALOG compartilhadas e 8 slots de instrumento DRUM-X por parte (32 slots de hits ao todo, 2.688 B de estado). Closed/open hi-hat obedecem choke.

O loader e as seções de acesso a hardware passaram nos checks do build. Perfil experimental: app até 0x50000 e banco privado 0x60000–0x87FFF; dados do Player original permanecem preservados. O arquivo .fwsc tem 651.327 B e identidade SMK-37 Pro_19605.

**Não foi medida a carga de CPU no JieLi, latência física, stack máximo, áudio do DAC, estabilidade prolongada ou instalação USB real.** Sucesso no host/WASM/build não comprova estabilidade física nem polifonia segura sob qualquer combinação de efeitos.

## Testes executados

- ANALOG, quatro partes, notas, sequenciador, rodas e sends: PCM exatamente igual à VD4 com TR 00. SHA-256 `4ae6e1508a661e6e362824033f358902a80a81fe527cbcf52b36117c5ed42932`.
- Groove DRUM-X legado com swing, velocity, ratchet, nudge e ties: PCM exatamente igual à VD4. SHA-256 `5ac7a82852462679e74d0976eefb6d5183bf4aba9619079ce2b50ea84f837dfa`.
- Quatro partes ANALOG retunadas juntas; identidades e ties preservados; notas ao vivo isoladas; releases após troca de parte/modo; tecla repetida entrando/saindo da captura; limites e reset; pattern parado/agendado; stop/restart; quatro partes DRUM-X; origem das notas diferidas; mono/unison legato.
- Kits, oito instrumentos, velocity, retrigger/choke, sends, favoritos, programas, automação, gravação por pads, masks de oito lanes, undo/cópia/shift, formatos legado/estendido e cenas.
- DRUM-X isolada com AddressSanitizer e UBSan: 640 combinações de parâmetros; 10.240.000 amostras, sem erros. Leak detection desligada para esse teste.
- WASM com eventos reais do painel: C3=0, D3=+2, B2=−1, RESET, latch, captura, navegação, notes-offs, ausência de gravação automática e APIs VOICE ausentes.
- WASM DRUM-X: gravação simultânea, save/cópia/reboot, máscara 255 e áudio finito não silencioso.
- Código real do AudioWorklet em harness: 448.000 frames estéreo, ANALOG + hit DRUM-X, saída finita, pico 0,157867, nenhum erro. Não é teste da saída do navegador/dispositivo.
- Instalador: SHA-256, identidade e CRC dos quatro pacotes; corrupção/truncamento recusados; modelo e identidade errados recusados; instalação/retorno/retomada simulados; acesso MIDI ausente ao carregar a página; diálogo cancelar/confirmar e estado final. Serviço USB mockado.

Logs da suite AD1 acompanham o pacote. O painel 240×240 foi renderizado nativamente; o navegador exibiu a visualização TRANSPOSE. A prévia HTTP permite modo de interface sem áudio; a saída de áudio física segue pendente.

## Build reproduzível

Ferramentas usadas: toolchain JieLi 20260730.1, SDK AC79 com arquivos fixados/verificados pelo build, Zig 0.13.0 e Binaryen. Veja o README de origem para instalar a toolchain e obter o SDK.

```sh
JIELI_TOOLCHAIN=/caminho/toolchain AC79_SDK=/caminho/sdk \
 python3 tools/build.py --voice-hardware --board smk37 --model pro \
 --release 3.0.1-ad1 --experimental-id 19605
ZIG=/caminho/zig WASM_OPT=/caminho/wasm-opt sh tools/build_voice_sim.sh
sh tests/run_ad1_tests.sh
```

`--voice-hardware` é o nome histórico do perfil privado de armazenamento/loader; não ativa VOICE. Para os testes de instalador, coloque os quatro .fwsc exatos do pacote de entrega em `web/keyboard-ad1/firmware/`, conforme `manifest.json`. O código-fonte ZIP não embute esses binários; o pacote de entrega contém ambos. Os hashes de PCM de referência estão fixados na suite; o backup VD4 contém o fonte exato usado para conferir a referência.
