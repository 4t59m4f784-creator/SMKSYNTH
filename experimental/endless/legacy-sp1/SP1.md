# SMKSYNTH SUPER SP1 — teste de performance

`1.0.3-sp1` · `SMK-37 Pro_19628` · baseado na ENDLESS E5 (`8a927e6`). Inclui ENDLESS com C4 neutro, UI1 e USB da 1.0.3. A versão pública 1.0.3 não é promovida por este teste.

## O que mudou

- SUPER disponível somente no OSC1. Mantém suas sete serras, fases, correção antialias, curva DETUNE, MIX, normalização e envelopes. OSC2: OFF, SAW, PULSE ou SAW+PLS. A página SUPER mostra K1 DETUNE e K2 MIX; K3/K4 não editam parâmetros ocultos do OSC2.
- DSP: fatores de desafinação são calculados quando o valor efetivo de DETUNE muda; cada atualização de frequência reutiliza esses fatores. As seis serras laterais são processadas em pares, com menos passagens pelo buffer. A aritmética de saída permanece exata.
- O estado da segunda SUPER foi removido. A SAW de acompanhamento usa somente o pequeno cache de que precisa. `.data + .bss`: 86.928 → 86.160 bytes, redução de 768 bytes; o código em RAM possui região reservada própria e passou pela auditoria de tamanho e ausência de chamadas externas.
- Trocas de pattern/cena em transporte ativo: no modo de salvamento manual, deixa de copiar 14.476 bytes para o cache de edição que esse modo não usa. A troca do pattern de destino e de seus sons permanece no mesmo instante musical.
- Leituras da flash no SMK usam no máximo 32 bytes de payload por bloqueio de interrupções, antes 256. Mantém o caminho SPI bruto, os endereços, a política de criptografia, CRC e escrita. Há mais transações de leitura; o ganho pretendido é reduzir os intervalos sem atendimento ao áudio/timer, não prometer carregamento total mais rápido.

## Compatibilidade e som

Uma SUPER antiga no **OSC2** é reproduzida como **SAW**. Os bytes do patch ficam preservados, sem conversão ou gravação automática. A tela mostra a forma efetivamente reproduzida. Ao editar a forma, somente as quatro opções normais ficam disponíveis.

Essa restrição muda o timbre de patches que usavam SUPER no OSC2. A garantia de equivalência cobre SUPER no OSC1 com qualquer forma normal no OSC2, inclusive OSC2 sem volume. Os dados antigos não são apagados. [Voltar à E5](legacy-e5/) permite comparar o comportamento anterior.

Não reduz serras, não encurta release, não diminui a taxa de amostragem nem altera as oito vozes compartilhadas. SYNC/XMOD continuam com o bypass já adotado quando OSC1 usa SUPER. Os recursos que não estão no caminho SUPER mantêm a engine original.

## Verificações realizadas

- 32.768 blocos de comparação com a referência aprovada: todas as 128 notas MIDI, DETUNE/MIX variáveis, mudanças de pitch, tamanhos de bloco e avanço silencioso; saída e estados de oscilação idênticos.
- 1.572.840 divisões por recíproco e 32.768 blocos da SAW acompanhante conferidos exatamente.
- Fluxo completo de 5.701.632 bytes comparado com a E5: SUPER no OSC1 + todas as formas normais no OSC2, efeitos, ataques de 3/4/8 notas, notas agudas, releases sobrepostos e mudanças de parâmetros de cena. Mesmo SHA-256 `e5bb37e06c1dc145a62d1ec58db5b89d805ec2c305edd7a640d33a7ac1a82000`. A compilação separada do kernel `-O2` com o restante `-Os` também produz o mesmo fluxo.
- Sanitizadores de memória/aritmética; signed-shift do DSP legado excluído como nos testes anteriores.
- 80 recalls parados e quatro recalls no ISR: patches completos, efeitos/mixer/steps/automação preservados, zero gravações automáticas. O cache de saída fica intacto no modo manual.
- Leitura SPI simulada: bytes exatos, comprimentos ímpares, falhas com desbloqueio e atendimento entre blocos de até 32 bytes.
- Loop real WASM: limites OSC1/OSC2 por painel e MIDI, apenas dois knobs SUPER, ataques repetidos de oito notas; ENDLESS E5, exemplos de fábrica e display ocioso.
- Atualização E5 → SP1 e retornos à E5/1.0.3 em flash simulada, mantendo o banco; firmware validado por identidade/CRC. Instaladores antigos preservam pacotes e chaves de retomada.

Os microbenchmarks no computador ajudam a comparar o kernel, mas não medem o orçamento do SMK37, interrupções USB ou sua carga real de performance. Não se afirma um percentual de CPU economizado no instrumento.

## Teste físico sugerido

1. Exporte um backup e use uma cópia de seus patterns. Reinicie antes de cada comparação para começar com os contadores no mesmo estado.
2. Use a SUPER no OSC1, depois acrescente OSC2 SAW e os efeitos habituais. Na página SUPER, anote os valores de pico `P` e estouros `O`.
3. Repita os acordes de três, quatro e oito notas, incluindo agudos. Aumente o release até a configuração que apresentava ruídos e faça novos ataques antes de as caudas terminarem.
4. Alterne as mesmas cenas/patterns com notas ainda soando, tanto com o transporte parado quanto ativo. Compare saída de fone/linha; teste USB separadamente se o usa na performance.
5. Verifique se `O` aumenta e se há estalos, atrasos ou mudanças de timbre. Se houver, relate notas aproximadas, partes/efeitos ativos, release e valores P/O. [Retorno à E5 anterior](legacy-e5/) disponível no mesmo site.

A polifonia continua sendo **oito vozes no total**, incluindo notas em release e vozes usadas por DUAL/UNISON. Quando todas estão ocupadas, a engine pode reutilizar vozes. Estas otimizações não garantem preservar todas as caudas nem eliminar ruídos sob qualquer carga. Não há validação física desta SP1 antes do teste no teclado.

Créditos da SUPER: Clipper / isod89 / 3dSam (Leo Kuroshita), com referência à análise de Adam Szabo. Integração e otimizações SMKSYNTH: Marcus Padrini e assistência do ChatGPT/OpenAI. GPL-3.0-only; ver LICENSING.md.
