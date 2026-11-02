# SMKSYNTH U1 — pad banks e atalhos de navegação

Base BR1 c58e260 (sobre SL2), com identidade visual e áudio preservados. Build 3.0.1-u1, SMK-37 Pro_19609. Backup de software: fontes BR1 e pacote BR1 preservados; não contém dados físicos do usuário.

## Mudanças

1. Bank Performance/Favoritos: pads 1–4 partes; 5 Delay; 6 Reverb; 7 preset anterior; 8 próximo preset; 9–16 favoritos. Mesmas ações, cores e níveis de brilho seguem a função na nova posição.
2. KNOB BANK curto: Performance/Favoritos → SPLIT/LAYER → Patterns/SAVE → Performance/Favoritos. O banco de steps não faz parte desse ciclo. Os códigos internos dos bancos são preservados; a ordem visível é 1/2/3.
3. KNOB BANK longo (700 ms): mostra PANEL CHECK enquanto segurado e seleciona imediatamente Performance/Favoritos. Ao soltar retorna à tela Performance. Funciona saindo de edição, sequenciador, mixer ou camada travada. Não para transporte nem encerra REC.
4. PATCH curto preserva a navegação existente Performance → Mixer e suas linhas. PATCH longo (700 ms) volta sempre à tela Performance; não muda a configuração musical de SPLIT/LAYER. Toques curtos são diferenciados no release; o gesto PATCH para travar uma camada já aberta continua disponível.
5. SEQ, início via PLAY e REC assumem os pads do sequenciador. KNOB BANK curto sai desse contexto para Performance/Favoritos; os próximos toques seguem o ciclo manual. REC e transporte continuam em execução. Novo acionamento de SEQ/REC ou início de PLAY assume os pads novamente.

A propriedade dos pads agora é explícita, independente da tela ou de REC estar ligado. Pedidos de tela passam pelo mesmo ponto da navegação, com limpeza completa, invalidação de cabeçalho, LEDs e scope. Isso evita resíduos ao sair da interface de sequência.

Patterns/SAVE mantém as operações existentes: toque carrega, segure salva/copia, confirmação de sobrescrita. O pattern contém as quatro partes/sons, sequência e duas configurações de teclado. Não foi adicionado um formato separado de cena, nem alterado o funcionamento das cenas existentes. Nada é salvo automaticamente por navegar.

## Validação

Harness nativo: retorno de camada SEQ travada com REC e transporte ativos; ciclo de 30 trocas; papéis dos LEDs. WASM: pads FX/preset, ciclo repetido, diagnóstico/retorno, PATCH longo a partir do mixer, saída repetida de SEQ, PLAY/REC e overrides manuais. Regressões de SPLIT/LAYER, gravação/persistência, DRUM-X, SEQ TRANSPOSE e PCM ANALOG/DRUM-X idêntico à AD1. Logs e fontes incluídos.

Firmware compilado para SMK37 Pro. CPU e latência física não medidos; teste físico pendente. A instalação continua reinicializando o banco experimental e recopiando o banco original do Player. Isso é uma limitação anterior, não mudança desta versão. Saves persistem ao desligar/ligar normalmente.

Instale em Chrome/Edge com cabo USB de dados e DAWs fechadas. Confirme U1/19609 e mantenha alimentação/cabo até concluir. O instalador inclui retorno BR1 e versões anteriores; interrompido, consulte novamente no mesmo instalador. Sem alteração no protocolo de atualização, loader, flash, engines ou dados salvos.
