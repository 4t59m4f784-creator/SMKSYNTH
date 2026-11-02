# VOICE — clareza após teste físico da VD2

O usuário confirmou que o problema DRUMS foi totalmente resolvido no teclado.
VOICE teve menos travamentos severos, mas todos os presets ainda apresentam
crackles, inclusive mono, com piora em acordes. A voz mono antes considerada boa
na VD1 foi descrita como “ardida” na VD2. Não classificar a engine como validada.
A correção DRUMS deve ser preservada. Não houve nova instalação ou publicação.

## Comparação adicional executada

`tests/voice_all_presets_compare.c` executa os oito presets com uma, duas e quatro
notas de entrada, sem sends e com rodas zeradas. Presets mono conservam a alocação
mono original. Compara VD1 preservada com VD2, usando o mesmo compilador host -O2.

Resultado: 1474560 frames estéreo; 2892990 amostras não nulas; PCM idêntico.
SHA256 dos dois fluxos: adca544471f4b2b7f7516840adc93f7fd5b8d1d5da66509a88c5c2cd7f8e6f4a.
É uma comparação de amostras calculadas fora de tempo real. Não valida prazo do
AC791N, código gerado pelo compilador pi32v2, latência, SPI, USB ou DAC. Tampouco
cobre todas as notas, posições de rodas, efeitos ou parâmetros possíveis.

A igualdade exclui diferença das amostras nesse cenário host; não exclui regressão
específica do alvo nem prova que os crackles sejam exclusivamente excesso de CPU.
A piora do mono exige comparação controlada de VD1/VD2 no chip e observação do
indicador GLOBAL P…us/!. O prazo é 256/44100 segundos (~5805 µs). Registrar se o
ruído persiste com VOICE isolada e depois de reiniciar, sem caudas de outras partes.
Um trecho de áudio ajuda a distinguir estalos descontínuos de distorção tonal.

## Alternativas ainda não implementadas ou validadas

1. Diagnóstico A/B no alvo: caminho vocal original versus otimizado, mantendo
   DRUMS corrigido e medindo tempo/overruns. Recuperar mono limpo é o primeiro critério.
2. Síntese VOICE em taxa interna menor (ex.: 22050 Hz), com reconstrução filtrada
   para a saída 44100 Hz. Pode reduzir bastante trabalho por segundo preservando
   a família de síntese. Exige adaptar coeficientes/formatos numéricos dos filtros,
   controle e antialias; não é simplesmente pular samples. Pode alterar brilho e
   BREATH; precisa de comparação sonora e de custo no dispositivo. Nenhum ganho
   percentual ou polifonia segura foi medido. Não reduzir a taxa global do Obervave.
3. Banco de formants compartilhado por parte (parafonia): fontes de várias notas
   somadas antes de um banco vocal comum. Pode reduzir o custo de filtros para
   acordes, mas muda a independência do tratamento por nota e a resposta sonora.
   É uma mudança de arquitetura que exige decisão do usuário antes de integração.

Reduzir somente a polifonia não resolve um ruído que já existe com uma voz. Não
prometer oito vozes limpas nem fidelidade idêntica de uma alternativa ainda não
construída. Não abandonar a síntese vocal sem comparar os caminhos de forma audível.
