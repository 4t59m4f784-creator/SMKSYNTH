# SMKSYNTH U2 — parte selecionada e salvamentos dedicados

Base exata: U1 32f278a52bffff51ccc8d7404c36381db08f2acb. Backup U1: release original e source-u1.zip preservados. Build experimental 3.0.1-u2, SMK-37 Pro_19610. Não altera engines, DSP, polifonia, sequenciamento, timbres de fábrica, oito slots ou mapas físicos de armazenamento.

## Pattern recupera a parte selecionada

Salvar/copiar pelo pad captura a parte selecionada (A–D). Carregar parado recupera essa parte; carregar tocando recupera-a junto da troca musical na virada existente. As duas configurações SPLIT/LAYER e a linha ativa continuam salvas e continuam determinando o teclado ao vivo, mesmo quando a parte selecionada é outra. REC continua em uma única parte selecionada, sem gravar o layer inteiro.

Armazenamento: tag de foco e índice usam os três bytes antes reservados na extensão KBD2. Tamanho total continua 14.476 bytes, cena 380 bytes, prefixos SEQ4/DRS1/KBD1 preservados. Patterns antigos sem tag entram com parte A; não se inventa uma seleção que não existia no arquivo. O foco é capturado quando o SAVE/cópia é confirmado, não por autosave. Nenhuma mudança é duplicada nas cenas.

## BT exclusivo de presets

- Toque curto: salva o timbre atual da parte selecionada no mesmo número de programa. Não exige segundo toque; a tela identifica PRESET, programa e parte.
- Segure por 700 ms: abre SAVE PRESET AS. K7 escolhe programa 1–99; BT confirma a sobrescrita do destino exibido. PATCH cancela. KNOB BANK também permite abandonar a escolha. PATCH longo mantém o retorno universal à Performance.
- Escolher o destino não carrega esse preset. O som salvo é o estado da parte de origem na confirmação. Mudança de pattern antes da confirmação invalida o pedido, evitando salvar a origem errada.
- A tela SAVING PRESET permanece durante a espera por STOP/áudio quieto. Só PRESET SAVED indica escrita concluída. A confirmação de sucesso permanece por cerca de dois segundos. Falha/pedido bloqueado fica visível até fechar ou tentar novamente.
- Botões PLAY/REC permanecem utilizáveis para parar; notas do teclado e transporte não são interrompidos por abrir o aviso. Durante escolha/espera, knobs e pads não mudam programas/tracks ou disparam operações de armazenamento por baixo da tela; K7 só atua na escolha de destino.
- Como na U1, salvar preset também solicita escrita das configurações globais pendentes. Não salva patterns nem favoritos. Programas compartilhados seguem a atualização já existente entre partes que usam o mesmo slot sem edições próprias.

BT não abre mais SAVE: PATTERNS, não carrega/copia patterns pelas teclas e não cria timbre aleatório com SCALE. A antiga operação de cópia pelo knob da página PATTERNS também não grava: informa SAVE VIA PATTERN PADS.

## Patterns: somente nos pads do bank 3

- Toque curto em pad: carrega o pattern correspondente, como antes.
- Segure 700 ms em slot vazio: solicita salvar/copiar imediatamente para ele.
- Segure 700 ms em slot ocupado, inclusive o atual: abre OVERWRITE PATTERN N, com contagem de 4 segundos e pad vermelho piscando.
- Soltar o primeiro pressionamento longo não confirma. Dê outro toque no mesmo pad antes do prazo. Manter o segundo toque até o prazo vencer também não confirma.
- Outro pad, PATCH, troca de bank ou mudança do pattern de origem cancelam a confirmação. Sem confirmação em quatro segundos, o aviso fecha e nenhum SAVE é solicitado.
- Depois de confirmar, durante reprodução aparece SAVES/COPIES WHEN STOPPED. Aguarde parar e terminar o som. SAVED IN N / COPIED TO N confirma escrita; falhas são indicadas. Não desligar enquanto houver pedido pendente.

Favoritos continuam nos pads 9–16 de Performance: toque recupera, segure memoriza. O botão BT não salva favorito. Não há autosave.

## Testes e limites

Harness nativo: BT preset-only em SEQ e SPLIT/LAYER, escolha/cancelamento de destino, pad ocupado incluindo slot atual, soltura sem confirmação, timeout sem writes, slot vazio, outro pad e origem trocada; foco parado/sincronizado, layer preservado, arquivos antigos e cenas byte-idênticas. Capturas reais do renderer 240×240 dos avisos incluídas. WASM executa o main loop para testar persistência/reboot e os fluxos físicos de botões/pads. Logs de build, áudio e regressões incluídos.

Validação física pendente. CPU/latência/brilho não comprovados em SMK37 real. O instalador experimental oferece retorno à U1 e versões anteriores e mantém o protocolo existente. A instalação ainda reinicializa o banco experimental e recopia o banco original do Player: backup de software não inclui patterns/favoritos salvos no teclado. Saves persistem ao desligar/ligar normalmente.

Recursos medidos na build final: flash 155196/327680 B; RAM estática 79104/98304 B; pool 325984/344064 B.
