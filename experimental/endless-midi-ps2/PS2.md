# Endless MIDI PS2 — 1.3.0-ps2 /19666

Experimental Pro/Elite sobre PS1 (pública 1.3.0 intacta).

1. ARP: a ativação libera somente vozes tocadas diretamente; preserva vozes pertencentes ao sequenciador. Caso reproduzido com pattern 6 → 1 do banco de Marcus: as duas vozes da parte B eram encerradas no bloco seguinte.
2. Reverb: soma de sete taps int16 pode exceder 109226; multiplicar por 19661 em int32 transbordava antes da escala Q15. Produtos da saída L/R agora usam intermediário int64, sem mudar ganho, buffers ou feedback. UBSan reproduziu o estouro após cerca de 224 segundos de trocas com BPM variável. Não é um limiter nem limpeza de caudas.
3. Zoom: identificadores do Mixer são desenhados no canvas antes da janela de edição, sem um segundo passe que a cubra.
4. Diagnóstico GLOBE + SCALE, K3 página 4: estado ao vivo das quatro partes, notas/posição, vozes com gate, engine e fase (0 pronta, 1 saída, 2 entrada), RUN, ON e MUTE (máscaras), pico de áudio e overruns acumulados. Pode ser aberto enquanto toca; PATCH sai. K1 muda 1C/2C e K2 reinicia a captura, portanto não os ajuste durante o registro da falha.

Validação: 160 trocas em ordem 1–16 (dez ciclos), BPM variável por slot, banco privado carregado somente no teste local; aproximadamente cinco minutos de áudio renderizado com ASan/UBSan e nenhum novo estouro. Não reproduzimos um pattern inteiro em silêncio. Isso não comprova ausência de dropouts no hardware: falta comparar os contadores no instrumento. Testes dedicados de primeira nota, liberação de teclas pelo ARP, reverb alto e seis popups do Mixer; regressões PS1/Endless/MIDI/SEQ/SU3. Formato persistido, RAM, polifonia e arquitetura dual-core preservados.

Instalação para SMKSYNTH existente. Backup e retorno oficial 1.3.0 incluídos. Não remova USB nem mude de aba durante a gravação.
