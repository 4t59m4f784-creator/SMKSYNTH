# Endless MIDI SU3 · 1.2.2-su3 · 19663

Experimental isolada para SMK-37 Pro e Elite, baseada na SU2/DC3 dual-core.
A versão pública 1.2.2 /19655 e os instaladores anteriores permanecem intactos.

## Correções

- Zoom Split/Layer: a atualização das vozes deixa de apagar a identificação dos pads.
- Endless: invalidar o desenho em cache após limpar a tela, inclusive ao sair do aviso REC, recupera os controles completos na primeira gravação/reprodução.
- INITIALIZE: após a confirmação e execução, exibe NOT SAVED por 2,2 segundos e retorna a Performance; PATCH antecipa esse retorno sem executar sua ação normal.
- Zoom SUB OSC e SUPER: indicadores de vozes no rodapé.
- Mantidos SUB aditivo, cores do Mixer, confirmação de inicialização, aviso REC e correção dos controles Master.

## Tempo

- K5 controla 40–240 BPM em todas as telas do synth. No MODO MIDI permanece configurável.
- Split/Layer: < seleciona LEVEL; > seleciona OCT. K2/K3 continuam editando os dois valores.
- K5 deixa de navegar entre patterns/compassos; os pads e botões de navegação existentes continuam disponíveis.
- BPM visível em Performance, osciloscópio, Split/Layer, Mixer/sends, ARP, SEQ e Endless, nos dois temas. No cabeçalho SEQ, R = gravando, A = armado, > = tocando, - = parado; M indica parte silenciada. O nome da vista é abreviado para preservar BPM e fila de pattern.
- Salvar/copiar um pattern grava também seu BPM interno atual.
- GLOBE → TEMPO SOURCE, K1: MASTER (padrão) mantém o andamento atual ao trocar patterns; PATTERN recupera o BPM gravado. Ao selecionar PATTERN, o andamento gravado do pattern atual é aplicado imediatamente, se existir.
- Patterns antigos sem BPM preservam o andamento atual, mesmo em PATTERN. A opção é persistida com o tema, quando parado, sem notas e fora do MODO MIDI; alterações de mapas MIDI ainda não salvas não são gravadas junto.
- Clock MIDI externo mantém precedência. Quando presente, o display mostra seu andamento; a gravação do pattern guarda o BPM interno ajustado.
- Alterar K5 não salva o pattern automaticamente: use o salvamento manual habitual para registrar o novo andamento. MASTER mantém o último andamento em uso, não um segundo BPM separado.

## Compatibilidade e validação

Pattern continua com 14.476 bytes, patch com 80 e cena com 380. Prefixos antigos continuam aceitos. BPM ocupa o último byte reservado de FCS1; a fonte de tempo usa um bit reservado no envelope CRC dos mapas, junto ao tema. Exportação/importação preserva os bytes completos.

Builds nativas: Pro 246.760 bytes; Elite 246.764 bytes. RAM estática 96.352/98.304 bytes, acréscimo de 16 bytes sobre SU2. Pool 332.180/344.064 e pilha CPU1 inalterados. Sem mudança no renderizador de áudio, alocação de vozes, polifonia ou ganho do SUB.

Testes automatizados: fluxo de interface real com ASan/UBSan; K5 em cada página; BPM no save/copy e troca na fronteira de áudio; fallback legado; retorno de INITIALIZE; preservação do rodapé; invalidação do cache Endless; regressões SUB aditivo, Supersaw, Split/Layer e dual-core; backup com BPM e SUB; pins e interrupções de gravação por modelo. Teste físico e medição de CPU no teclado dependem da avaliação do usuário.

Exporte seu backup nesta página antes de instalar. Retorno à oficial está disponível, preservando dados, mas ela não interpreta os recursos novos da experimental. Não desconecte USB nem mude de aba durante a gravação.

## English

SU3 is an isolated dual-core test build, not a public release replacement. It fixes disappearing Split/Layer pad labels, incomplete Endless redraws, missing SUB/SUPER voice meters and the INITIALIZE return flow. K5 sets synth tempo everywhere while remaining configurable in MIDI MODE. On Split/Layer, < selects LEVEL and > OCT. BPM is visible across performance/ARP/SEQ views. Save/copy a pattern to store its internal BPM; GLOBE TEMPO SOURCE offers MASTER (default, keep current tempo) or PATTERN (recall saved tempo). Older patterns without BPM keep current tempo. External MIDI clock still takes priority. Automatic initialization return shows NOT SAVED first; manual save remains required.
