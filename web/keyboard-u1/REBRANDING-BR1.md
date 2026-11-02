# SMKSYNTH BR1 — identidade visual sobre SL2

Base exata: SL2 5a9e1e1853c0f2c568fce0b37b7ed71e02b3a051. Backup: pacote SL2 e bundle completo preservados. Nenhuma alteração em engines, DSPs, sequenciamento, roteamento, controles, banco físico, dados, cenas ou patterns.

Cabeçalho compartilhado: SMKSYNTH em FONT_S, alinhado à direita pela largura real. Aplica-se a performance ANALOG/DRUM-X, edição, efeitos, mixer, global, páginas do sequenciador, transpose e ambas as linhas SPLIT/LAYER. APP_NAME agora SMKSYNTH nas telas de crash e USB rescue. Mensagens técnicas de UPDATE/FAILED não alteradas.

Loader: somente LDR_NAME alterado de OBERVAVE para SMKSYNTH. São oito letras; cálculo existente de centralização e escala preservado. ldr_core.c, protocolo USB, validações, identidade de compatibilidade, CRCs, mapa de flash e resgate não modificados. O pacote recompilado gera seus novos CRCs automaticamente. Teste da tela do loader confirma limites 240x240, estados e progresso. UBOOT original do SDK não é alterado: não há solicitação de regravação de imagens de terceiros ou da ROM.

Boot: logotipo derivado do GIF v3 aprovado, codificado RGB565/RLE; subtítulo PERFORMANCE SYNTH porque a frase completa excede a largura de FONT_S. Traço angular vermelho e senoide azul desenhados somente até sua frente atual, mantendo polling e os 2,4 segundos existentes. É uma adaptação aos recursos do firmware, não reprodução quadro a quadro do GIF de 40 frames. Logo e GIF originais preservados como referências no pacote.

Simulador: identificações do produto e descrição da tela SMKSYNTH. Nomes internos de arquivos, módulos, storage, formatos e identificadores USB preservados. Atribuições a Obervave, JIANT, Felucca e autores mantidas. Páginas antigas/instaladores de versões anteriores não foram renomeados.

Capturas 240x240: páginas compartilhadas, modos de teclado, seis vistas de sequenciador, bateria, quadros de boot e telas do loader. Rescue/crash-heading no harness usam as mesmas chamadas gráficas e literais das fontes, sem provocar uma falha física. Algumas páginas internas históricas do harness não são acessíveis no painel atual e não indicam engines novas.

Testes: regressões nativas SL1/SL2, ganhos e rotas, REC, sustain, persistência, hashes de PCM ANALOG/DRUM-X da AD1 e sanitizer. Testes WASM e recursos medidos incluídos nos logs. Validação física do boot, áudio, USB e brilho pendente. Não afirmar certificação no dispositivo por compilação/simulação.

A instalação preserva a limitação do banco experimental: reinstalar firmware reinicializa esse banco e recopia o banco original do Player. Não é uma mudança deste rebranding. Exportação física não implementada.

Recursos medidos: flash 153736/327680 B; RAM estática 78992/98304 B; pool 325984/344064 B. RAM e pool idênticos à SL2.
