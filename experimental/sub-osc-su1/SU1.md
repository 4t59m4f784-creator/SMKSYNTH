# SMKSYNTH SU1 — SUB OSC sobre DC3

Experimental 1.2.2-su1 /19661 para SMK-37 Pro e Elite. Base DC3 /19659 escolhida por Marcus; não incorpora a DC4. Firmware público, instalador, backup e simulador oficiais permanecem inalterados. Atualização preservadora, sem banco de usuário e sem primeira instalação V016.

## Controles

PARA percorre OSC1 → OSC2 → SUB OSC → OSC MOD → FILTER → FILTER+ → SUPER. O ID da página foi acrescentado ao final do enum; nenhum índice antigo mudou. K1 WAVE (SQUARE / SAWTOOTH), K3 LEVEL (0–100%), K4 OCT (-1/-2); K2 vazio. Mesmas posições reais de onda/nível/intervalo dos osciladores. Obervave e Zoom usam os componentes existentes. K5–K8 mantêm suas funções globais.

## DSP

Um acumulador de fase e uma amostra atrasada por voz. Reutiliza osc_step e a correção de eventos BLEP de OSC1/2: pulso de 50% para SQUARE, rampa para SAW. Incremento fundamental de OSC1 dividido por 2 ou 4; exclui o detune da placa/grupo UNISON/DUAL e os componentes laterais SUPER. Mantém bend, glide, transposição, afinação e modulações de pitch comuns à voz. Não segue a FM instantânea de X-MOD. Não cria vozes. O pan, filtro, envelope, drive e sends continuam os da voz.

O nível sobe/desce em rampa de aproximadamente 6 ms. Trocas de forma/oitava primeiro levam o SUB suavemente a zero, aplicam a última seleção e retornam ao nível pedido, sem reiniciar a fase. Há uma breve redução de nível, não crossfade de dois osciladores. Com nível zero estável, uma versão especializada do loop original evita trabalho de SUB por amostra; a fase avança uma vez por bloco.

A mistura do SUB entra antes do drive/filtro. Os ganhos do conjunto são normalizados localmente ao orçamento dos osciladores existentes: ao aumentar SUB, o nível relativo dos osciladores anteriores diminui para abrir margem. Sem compressor/limiter novo. SUB=0 é bit-idêntico à DC3 numa sessão densa de synth, SUPER e DRUM-X. A saturação já existente do instrumento continua existindo.

## Persistência e compatibilidade

Patch continua 80 bytes, nome e NPARAM nos offsets originais; padrões, cenas e favoritos não cresceram. A extensão SU usa os seis bytes já reservados: dois níveis SUPER de 7 bits, bits de onda/oitava, nível e checksum. Leitores de SS/OVE antigos continuam aceitos, com SUB=0, sem regravação automática. Alterar SUPER MIX preserva SUB e vice-versa. DRUM-X não recebe essa extensão.

Use **backup/** desta experimental para exportar/importar SU1: aceita 19661 sem modificar a ferramenta oficial. Foi testada transferência B2 com banco contendo uma cena SUB, retries de CRC e retomada após interrupção, para Pro e Elite. Mapas MIDI/tema seguem as limitações já existentes do backup.

Ao retornar à oficial/DC3, os novos parâmetros não são entendidos; SUPER MIX de patches já editados na SU1 também pode voltar ao padrão nesse firmware antigo. Guarde o backup anterior para restaurar os timbres originais ao retornar. O botão de retorno usa os pacotes oficiais 1.2.2 preservadores, idênticos aos já publicados.

## RAM e CPU

+20 bytes por voz (fase, amostra anterior, incremento, modo e nível em rampa), +16 por parte (derivados/cache); +160 bytes na cópia privada do worker. Total +384 bytes: +224 RAM estática e +160 pool. A estrutura privada passa de 3620 a 3780 bytes. CPU1 continua com a mesma pilha de 7 KiB. RAM estática 96352/98304; topo da pilha CPU1 339360/344064. Sem novas alocações, locks ou transporte entre núcleos.

Benchmark de host em BENCH.json: oito vozes normais, 60000 blocos, mediana de cinco execuções, apenas render das amostras (sem controles, efeitos e sem paralelismo). Compilador x86-64 -O2; firmware mantém -Os. Os percentuais não são previsão da CPU total do SMK. O SUB gera uma onda simples; o custo real precisa ser medido no instrumento.

GLOBE + SCALE mantém o diagnóstico DC3. Para comparar, arme uma janela de 30 s com o mesmo pattern, polifonia, níveis e FX: primeiro SUB=0, depois SUB ligado; fotografe as páginas congeladas. CPU1 PEAK sobrepõe o trabalho de CPU0 e não deve ser somado a ele.

## Validação

- Áudio/estados iguais entre serial e worker pthread, com SUB ativo, oito vozes normais, SUPER, bateria e mudanças de parâmetros; despacho recusado, resultado privado corrompido e alternância 1C/2C. AddressSanitizer e ThreadSanitizer.
- Legacy SUB=0 comparado amostra por amostra à DC3 original.
- Frequência/fase das duas ondas e oitavas, fundamental sem detune UNISON, transições rápidas e desligamento.
- Polifonia normal/UNISON 8 e teto SUPER 5 preservados; quatro partes, Split/Layer e teste estéreo sem overflow de saída.
- Presets, cenas/patterns e favoritos; nomes, oitavas e SUPER MIX preservados; backup B2 e restauração do banco SU nos dois modelos.
- SHA/CRC, identidade, recusa de modelo trocado, retomadas de instalação/retorno e flash preservadora.

Validação física da SU1 ainda pendente. Não se promete ausência de glitches no hardware antes do teste com seus patterns.

Build: Jieli 20260730.1, SDK AC79NN_SDK_V1.2.1_2023-12-13, OBV_DEFS=-DOBV_DUAL_CORE=1, --release 1.2.2-su1 --experimental-id 19661. Sem --first-install.
