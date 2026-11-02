# VD4 — faixa e ganho da VOICE

Base: VD3 (VOICE JIANT), ramo isolado voice-gain-calibration. Relato físico de Marcus: reduzir o volume dos patches vocais para 40 e aumentar o volume geral eliminou a distorção no teste realizado.

## Diagnóstico

O volume físico/master é aplicado depois de app_audio, em platform.c. O volume do patch atua antes do mixer, do controle de nível e dos sends. Portanto, reduzir o patch e aumentar o master não equivale a uma simples mudança de volume de escuta: altera a margem interna antes de possíveis saturações.

O porte da VD3 usava volume^2/10000 na VOICE, com parâmetro até 127 (ganho 1,6129), enquanto ANALOG conserva sua escala original. Na VOICE, o valor 40 corresponde a ganho 0,16 antes dos sends. A proteção de dinâmica do dry não protege os sends: eles eram somados antes dela e convertidos a 16 bits separadamente.

No ensaio nativo inicial (8 presets, C4/C5 de base, entradas de 1/4 notas, volume 40/100, sends 0/80), o volume 100 causou overflow da faixa de 16 bits dos sends em 12 de 64 casos; 40 não causou em nenhum. A saída dos efeitos estava desligada nessa primeira matriz, que serve para medir os sends, não a soma wet.

Um segundo ensaio ligou efetivamente os retornos (chorus 70, delay 55, reverb 70; sends 80) e incluiu entrada de 8 notas. No volume 100, 24/48 casos tiveram saturação dos sends e 42/48 saturação em alguma operação de 16 bits do caminho de efeitos. Em 40: 0/48 nos sends, mas 10/48 no caminho de efeitos, principalmente entradas de oito notas. São ensaios de software, não novos testes físicos. Presets mono continuam mono mesmo quando recebem quatro/oito notas.

## Correção

- O volume de TODOS os patches VOICE tem teto efetivo de 40 no renderer. Isso inclui VOICE legado perfil 1 e VOICE JIANT perfil 2.
- A tela VOICE mostra 0–100% dentro dessa faixa; 100% equivale ao antigo valor bruto 40. 50% equivale a bruto 20, com a curva quadrática existente. Não significa 50% de amplitude.
- Knobs e MIDI usam o limite contextual 40, com a escala/indicador correspondente. Os demais controles e seus mapeamentos ficam iguais.
- Bytes de programas ou automações antigos podem ainda conter 100/127, mas são limitados em execução. Os dados não são reescritos ao carregar ou tocar. Ao editar/salvar manualmente, o valor novo fica na faixa calibrada.
- Os níveis de fábrica foram ajustados por preset: CHOIR/OOH 32, VOX LEAD 38, TALKBOX/WHISPER/VOWEL SWP 40, CHANT 30, DARK CHOIR 36. Não foi acrescentado ganho pós-efeitos.
- ANALOG e DRUMS conservam faixa, fórmulas de ganho e defaults anteriores. O áudio ANALOG da regressão continua bit a bit igual (hash 4ae6e1508a661e6e362824033f358902a80a81fe527cbcf52b36117c5ed42932).
- A mudança de nível dos programas VOICE antigos é intencional, motivada pelo teste físico. A promessa anterior de identidade PCM do VOICE legado vale para o checkpoint anterior, não para esta correção de volume.

## Equilíbrio com ANALOG

Na comparação nativa de notas sustentadas C4/velocity 100, o RMS de CHOIR AAH em bruto 40 foi 4.088 unidades, enquanto seis patches ANALOG com sinal sustentado tiveram mediana de 2.224. Portanto, reduzir o ganho vocal não o torna necessariamente mais baixo que ANALOG. Os defaults foram aproximados dessa faixa sem ultrapassar o teto; Whisper preserva menor energia. RMS e comparação de poucos sons não equivalem a igualdade de loudness percebida em todas as regiões, timbres e acordes. O equilíbrio final ainda deve ser ouvido no teclado.

## Verificação e limites

A matriz corrigida repetiu 192 combinações: 8 presets x volumes brutos 40/100/127 x dry/wet x 1/4 notas x C4/C5. As medidas de saída, RMS e saturação foram idênticas para 40/100/127 em cada cenário correspondente: presets antigos e dados automatizados não ultrapassam o teto. Não houve saturação nos sends nessa matriz. Ainda houve três ocorrências de caso com saturação no caminho dos efeitos (o mesmo cenário em três valores equivalentes); isso não foi ocultado por um limiter global novo.

O teto reduz a sobrecarga identificada; não garante ausência de saturação em qualquer soma de partes, efeitos, feedback, drive, ressonância ou oito notas. A cadeia global de efeitos do Obervave continua compartilhada e não foi alterada. Não se afirma que CPU ou dropouts sob sobrecarga foram resolvidos. O próximo teste físico deve verificar o volume máximo novo e depois a combinação com outras partes/efeitos.

Passaram os testes do teto no setter e no DSP, display percentual, todas as posições MIDI, preservação de bytes carregados, ausência de escrita automática, defaults, conformance do núcleo JIANT, dinâmica de referência, envelopes/velocity com o novo ganho e suites de workflow/controles. Os logs e CSVs acompanham o fonte.

Build para SMK37 Pro: 3.0.1-vd4 / SMK-37 Pro_19604. Imagem 181.052/327.680 bytes; RAM principal 79.600/98.304 bytes; pool 325.900/344.064 bytes. São medidas do linker, não de CPU/pico de pilha físicos.

Instalar/reinstalar continua reiniciando os saves do banco experimental. O banco original do Player permanece separado. A validação musical desta correção começa no teclado do usuário.
