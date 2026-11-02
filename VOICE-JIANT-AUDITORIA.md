# VOICE — auditoria comparativa com JIANT

Data: 31/10/2026. Pedido: investigar o controle de aspereza/saturação e o ruído durante as transições vocais.

## Identificação e isolamento

Base do instrumento: VD2 + diagnóstico 8397e08, firmware 3.0.1-vd2. Ramo de investigação criado: voice-headroom-audit. Nenhuma alteração no ramo estável, instaladores, bancos, padrões, simulador publicado ou firmware instalado.

Referência primária fixada: JIANT e924445ae0e7c64636f15bd0758418ee312ce7ee, mesma revisão citada pelo porte em LICENSING.md. Arquivos originais incluídos em reference/, com avisos GPL-3.0-only preservados. O HEAD remoto também foi identificado, mas os testes usam a revisão fixada; não foi confirmado qual revisão o usuário gravou.

## Resultados demonstrados

1. O kernel de vogais não foi substituído por uma síntese diferente: o teste isolado compara o caminho vowel-only extraído de eng_formant.c com o porte. Mesma fase/incremento, valores de parâmetros e seed, sem TALK, RAND, glide ou modulações. O harness usa blocos de 16 para ambos e neutraliza as diferenças de ganho e distribuição de vozes. Isso verifica a matemática do sinal, não a performance original do JIANT.

   Foram comparadas 512.000 amostras (8 conjuntos de parâmetros dos presets do porte, notas 48/60/72/84, 1.000 blocos de 16 por caso). Antes da curva de saída da voz: zero diferenças. Repetido com BREATH habilitado e seed alinhado: zero diferenças. É um teste estacionário, não comprova equivalência das transições entre vogais.

2. Erro concreto no porte da curva soft_knee:

   JIANT: 24000 + softclip((abs(y) - 24000) * 2) / 2.
   Porte VD2: 24000 + softclip(abs(y) - 24000) / 2.

   A tabela tanh efetiva é a mesma nos dois projetos (tools/gen_tables.py: tanh(i / 256 * 2)). O comentário sobre tanh em dsp.c do JIANT não explicita essa escala; a verificação do gerador evita interpretar o comentário como diferença de tabela.

   A falta do fator 2 reduz aproximadamente à metade a inclinação imediatamente acima do limiar, introduzindo uma dobra extra na curva. Esta é uma fonte potencial de novos harmônicos; não é um dropout e não comprova sozinha a origem de toda a aspereza gravada.

   Corrigido somente esse fator em firmware/src/obv_formant.h, no ramo de investigação. O teste contra a referência falha antes (exit 2, post knee mismatch) e passa depois, inclusive com BREATH. A comparação após a curva remove a escala VOICE_FS/amp/pan específica dos projetos. Não foi reduzido BUZZ/BREATH, alterado o filtro ou escurecido o timbre.

3. Cadeias de saída diferentes:

   JIANT fx.c tem leveler sempre ativo (alvo LEV_T=12000, adaptação de ganho suave), limitador de picos e softclip final. O porte preservou a cadeia do Obervave: soma das partes -> softclip de saída -> efeitos e master do Obervave. A referência também possui filtros de parte e modulação/acentos próprios. Portanto, valores iguais na tela não garantem amplitudes, modulações ou processamento final iguais. Não se pode atribuir a melhora à cadeia master sem um A/B com ganho compatível.

   O compressor opcional do master Obervave não torna as cadeias automaticamente equivalentes: é necessário avaliar a ordem das etapas, pois saturação anterior não é desfeita depois.

4. Medição da atuação dos limites no porte:

   Teste nativo instrumentado: 8 presets x notas 48/60/72/84 x entradas de 1/4 notas x posição fixa/alternância de vogal = 128 casos. Velocity 100; mixer de uma parte em 100; master volume em 100; sends/drive/comp zerados; vintage e glide zerados para isolar. Presets mono preservam a alocação mono mesmo com entrada de quatro notas. Cada caso renderiza 240 blocos de 256; não corresponde à execução gravada pelo usuário.

   CHOIR AAH, nota 60, 1 nota e vogal fixa: 36,71% das amostras renderizadas pela VOICE acima de 24000 antes da curva; pico 52087; nenhum clamp dos estados de formants. Com 4 notas: 29,62%, pico 53976; entrada esquerda do softclip master chegou a 41250.

   Limite de estados 2^28 foi atingido 741 vezes apenas nos dois casos CHANT/nota base 84/entrada de quatro notas, fixo ou alternando vogal. Esse limite também existe no F4 da referência; estes casos não demonstram um defeito exclusivo do porte. Nenhum limite dos estados F1/F2/F3 foi atingido nesta matriz.

   Todos os casos tiveram alguma atuação da curva de voz. Não significa erro por si só: a referência também a usa deliberadamente. Os dados mostram que a diferença matemática ocorre em uma região efetivamente usada pelo instrumento.

5. Custo de processamento não é a mesma questão:

   A engine FORMANT do JIANT fixa .poly=4. O porte usa o alocador de até 8 vozes compartilhado com ANALOG; não equivale a 8 vozes VOICE seguras no dispositivo. O JIANT usa CTL=32; Obervave CTL=16, mais atualizações de controle por segundo. O cache exato de coeficientes VD2 reduz recomputações estacionárias, mas durante morph os coeficientes mudam com frequência. O projeto JIANT também possui distribuição de trabalho entre cores em fx.c. Nenhuma comparação de CPU no hardware foi realizada aqui.

## Verificação da alteração experimental

- Kernel estacionário de referência, com e sem BREATH: 512.000 amostras por execução; pre-knee idêntico; post-knee normalizado idêntico após correção.
- Teste antigo do post-knee falha com a fórmula VD2, demonstrando regressão real capturada.
- Teste funcional nativo VOICE: áudio, parâmetros, sustain/release, bend/glide, sends, allocator e extremos passa.
- ANALOG: hash PCM 4ae6e1508a661e6e362824033f358902a80a81fe527cbcf52b36117c5ed42932, igual ao baseline preservado.
- Demais logs acompanham o pacote. Testes automatizados nativos não constituem validação auditiva nem física.

## O que permanece aberto

A correção da curva é comprovada matematicamente, mas ainda não foi validada musicalmente no SMK37. Pode elevar alguns picos que antes eram comprimidos mais abruptamente; a interação com a saturação posterior exige avaliação. Não é uma promessa de eliminação do ardido.

Ainda é necessário comparar transições com trajetórias de formants iguais, medir picos antes de cada não linearidade, testar compensação de ganho específica da VOICE antes da saturação global e verificar o áudio seco contra a cadeia de efeitos. Reduzir o volume depois da distorção não corrige harmônicos já gerados. Importar um leveler global alteraria ANALOG/DRUMS e não foi feito.

O morph/TALK do porte suaviza posição e interpola no caminho entre vogais; JIANT interpola frequências e larguras dos endpoints de TALK. Essas trajetórias não são necessariamente iguais quando atravessam várias vogais. A comparação estacionária acima deliberadamente não cobre isso.

Não foi gerado instalador ou publicado simulador novo. Não há teste físico ou medição de CPU no SMK37 nesta auditoria. O objetivo seguinte é uma correção da cadeia VOICE baseada nestes dados, sem alterações globais que atinjam a versão estável ou as outras engines.
