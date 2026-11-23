# Endless MIDI PS3 — 1.3.0-ps3 /19667

Base PS2 /19666. Pública 1.3.0 e PS2 arquivada intactas.

Na troca quantizada de pattern, partes que mudam de engine encerram a propriedade das notas anteriores e ficam prontas antes de receber os eventos do passo 1. Notas novas não ficam na fila do fade antigo, nem são apagadas por uma limpeza posterior. Partes que permanecem na mesma engine usam o caminho anterior sem reinicialização.

O último sample seco do bloco de controle anterior é retido em 32 bytes de RAM. Na mudança de engine, uma cauda linear de 64 samples (~1,45 ms a 44,1 kHz), no guard já existente, sobrepõe a entrada nova. Não há pausa inserida, vozes novas de polifonia ou reset do reverb/delay. Trocas manuais de engine fora de um recall conservam o comportamento anterior.

Validação local: ANALOG ↔ DRUM-X e mesmo-engine; todos os patterns do banco privado → pattern 1; envelope da primeira nota ativo no mesmo CTL de commit, sem fila/fade de entrada. PCM em recalls sem troca de engine idêntico à PS2 byte a byte; regressões PS1/PS2/Endless/MIDI/SEQ/SU3; comparação serial/worker e falhas; 10128 pedidos rápidos, 253 commits quantizados. RAM estática +32 bytes, pool/polifonia inalterados. CPU física ainda não medida. A perda completa do bumbo relatada não foi reproduzida: validar se esta abordagem resolve no instrumento e se preserva as transições.

Nesta página, Voltar à PS2 instala os arquivos PS2 imutáveis. Após esse retorno, o instalador PS2 permite retornar à oficial 1.3.0. Backup/atualização preservam os formatos de dados. Não remover USB nem mudar de aba durante a gravação.
