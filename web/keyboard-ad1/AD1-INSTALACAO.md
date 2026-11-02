# SMKSYNTH AD1 — instalação experimental no SMK37 Pro

Versão 3.0.1-ad1; identidade SMK-37 Pro_19605. Exclusiva para **SMK-37 Pro**.

## Instalar e retornar

Abra o instalador AD1 no Chrome/Edge de computador, conecte por USB de dados, feche outros programas MIDI, clique Conectar e consultar e confira o modelo. Escolha AD1 e confirme a gravação. Não desligue nem retire o cabo até a identidade final SMK-37 Pro_19605. O primeiro boot pode demorar durante a cópia dos dados.

O instalador AD1 também oferece os pacotes VD4, Player PF12 e PF9, conferidos por identidade, SHA-256 e CRC. Use a página AD1 para retornar: páginas anteriores podem recusar a nova identidade. Em caso de aplicativo sem resposta, tente ligar segurando OCT− para o resgate USB e consulte novamente no instalador AD1. O protocolo de interrupção/retomada passou em simulação, sem teste físico nesta entrega.

**Instalar, reinstalar ou retornar reinicia os saves experimentais.** O banco original do Obervave Player permanece preservado e é copiado novamente para iniciar o banco experimental. O backup VD4 contém firmware, instalador, fonte e simulador; **não contém os presets/patterns que estão na memória do seu teclado**. Registre ajustes importantes antes de instalar.

Desligar e ligar normalmente conserva os salvamentos manuais AD1. Autosave continua desativado. Aguarde a conclusão do SAVE antes de desligar. O banco experimental e o perfil do loader continuam os mesmos da VD4; os nomes internos `OBV_VOICE_HW` / `smk_voice_store.h` identificam esse perfil de armazenamento, não uma engine vocal ativa.

## Seleção de engine

GLOBE percorre GLOBAL → POLYPHONY → OUTPUT → PART SETUP. Em PART SETUP, K1 escolhe ANALOG / DRUM-X e K2 escolhe o kit DRUM-X. POLYPHONY é a página anterior de modo/unison/glide do ANALOG, mantida e renomeada para evitar confusão com a engine removida.

DRUM-X conserva oito instrumentos sintetizados, cinco kits, edição, sends, gravação por pads, grade de oito lanes, chance, ratchet, velocity e nudge. Nenhum novo atalho físico ou função de pads foi criado.

## SEQ TRANSPOSE

Toque SEQ até TRANSPOSE, após PATTERNS. A visualização existe tanto na sequência ANALOG quanto na DRUM-X.

| Controle | Função |
|---|---|
| K1 KEYS | Ativa/desativa captura do teclado; girar à direita liga, à esquerda desliga |
| K2 VALUE | Ajusta semitons, de −48 a +79 |
| K3 RESET | Qualquer giro retorna a TR 00 |
| K4 TARGET | Indicador ANALOG; não edita |
| K5 | Navegação de pattern existente, mantida |
| K6 | Seleção da parte existente, mantida |
| K7 / K8 | Funções anteriores de preset/destino contextual e master |

Com KEYS ON, C3 (MIDI 48) = 0, D3 = +2, B2 = −1. A tecla define um valor absoluto em relação a C3, e não incrementos acumulados. Se houver várias teclas, vale a última nota recebida. A oitava do teclado afeta a nota enviada; use OCT− para alcançar B2 ou K2 para valores negativos.

A transposição afeta simultaneamente **todas as vozes de sequência ANALOG**, inclusive notas ligadas em reprodução. DRUM-X nunca é transposta, em nenhuma parte. As notas tocadas normalmente, fora da captura, continuam sem essa transposição. A transposição individual já existente na track continua sendo somada; o valor global não altera esse campo.

O valor permanece após soltar a tecla, desligar KEYS, sair da visualização, trocar parte, trocar pattern ou parar/reiniciar o transporte. Sair da visualização desliga a captura e devolve as teclas à função normal. **Ao ligar/reinicializar: KEYS OFF e TR 00.** O valor global não é salvo em programas ou patterns. K3 sempre permite voltar a zero.

As teclas capturadas não disparam timbres, não são gravadas e não escrevem steps. Note-offs continuam ligados à sua origem mesmo ao sair da captura. A captura usa o teclado físico e o fluxo MIDI da parte selecionada; mensagens externas endereçadas diretamente a outras partes preservam o roteamento existente.

## Compatibilidade e limite dos testes

Os formatos de patch, favoritos, cenas e patterns foram preservados. ANALOG continua com id 0 e DRUM-X com id 2; id 1 permanece reservado. Patches VOICE antigos abrem como INIT ANA na memória ativa, conservando os parâmetros globais de efeitos. O programa/pattern salvo não é convertido automaticamente. Um SAVE explícito pode gravar o novo som ANALOG.

Build JieLi, testes nativos, testes WASM, processamento do AudioWorklet e simulações do instalador passaram. O painel foi inspecionado no navegador em modo sem áudio, pois a prévia HTTP não disponibiliza AudioWorklet. **CPU, latência, estabilidade prolongada, áudio físico e USB no SMK37 não foram medidos/testados nesta entrega.** Splits e layers novos não foram implementados.
