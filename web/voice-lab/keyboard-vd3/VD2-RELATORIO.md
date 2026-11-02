# VD2 — investigação dos três problemas físicos relatados na VD1

Base exata: VD1, commit `b789a35a16b7409c595a03bd08e345d37160f48a`, preservada na
branch `vd1-preserved`. Ramo de trabalho: `obervave-voice`. Versão candidata para
SMK37 Pro: `3.0.1-vd2`, identidade `SMK-37 Pro_19602`. O Player estável não foi alterado.

## 1. DRUMS: estalos e notas desproporcionalmente altas

**Defeito reproduzido e corrigido no código.** A interpolação de ganho usava
`(dg * (i+1)) / CTL`. `dg` é negativo ao reduzir o ganho, mas `CTL` é unsigned:
C convertia o numerador negativo para unsigned antes da divisão. Uma redução
virava um número positivo enorme, causando saturação no estágio de saída.
O mesmo caminho podia ocorrer ao mover LEVEL ou VOLUME de um hit em andamento.

Além disso, `h->gain` sobrevivia ao fim do hit. Um hit suave depois de um forte
começava com o ganho da velocity anterior e só então chegava ao novo. A correção
usa divisão explicitamente signed e reinicia o ganho de ataque em zero para cada
hit novo, preservando a rampa curta de 16 samples (~0,36 ms) do ataque inicial.
Não foi reduzido o volume global para esconder o defeito; a curva de velocity,
níveis, kits, envelopes e choke continuam configuráveis como antes.

Reprodução isolada com velocity 127, decaimento completo e depois velocity 8:
no kit 808/kick, o pico interno do primeiro bloco suave era **269418505**, versus
**9** no mesmo hit suave a frio. Na correção, passa a **9**. São unidades internas
antes do mixer/limitação, não valores PCM finais. Todos os cinco kits × oito
instrumentos foram exercitados; ruído aleatório pode mudar o pico entre hits.

O teste novo verifica 40 retriggers forte→suave e 40 reduções de LEVEL em cauda,
com limites de saída e silêncio depois da rampa. Esses testes não substituem a
audição física; ataques percussivos e variação de ruído normais continuam existindo.

## 2. VOICE polifônica: glitches e dropouts

**Hipótese principal: custo de processamento por voz.** A correlação física
relatada — mono estável mesmo junto a DRUMS e duas partes ANALOG, polifonia VOICE
com problemas — é compatível com ultrapassar o prazo do callback. Ainda não há
medição do pico no SMK37 para fechar a causa ou afirmar que a VD2 resolve a carga.
Nenhum limite seguro de polifonia é declarado nesta entrega.

Mudanças de custo sem alterar a arquitetura sonora:

- Os quatro ressonadores tinham chamadas separadas por sample: 4 ×256 = 1024
  chamadas por voz a cada bloco de áudio. Foram inlined na compilação do teclado.
- O acumulador continua 64-bit, mas a comparação de saturação agora ocorre após
  conversão segura para int32. Os limites de frequência, largura, fonte e estados
  limitados deixam o resultado deslocado dentro de int32. Isso evita comparações
  largas desnecessárias no laço dos filtros.
- Cache exato dos coeficientes por voz: só reutiliza quando frequência e largura
  quantizadas são idênticas. Não é decimação de controles; mudança de vogal,
  morph, glide, Q ou pitch é aplicada no mesmo bloco que antes.
- VOICE deixa de calcular controles de oscilador 2/PWM e coeficientes SVF analógicos
  que seu renderer não consome. Pitch, amp/envelopes e modulações necessárias continuam.

No cenário de oito notas sustentadas CHOIR AAH e 4000 blocos, os coeficientes
foram calculados 32 vezes (quatro por voz), em vez das 2048000 avaliações que o
caminho sem cache executaria. Esse benefício depende do som: TALK/morph/LFO em
movimento invalidam o cache quando seus valores mudam.

A comparação do DSP gerou áudio **byte a byte idêntico** à VD1 no cenário
ANALOG+VOICE de 409600 frames; SHA256
`08d7ce3bc6886ae0c264b54359eaee90bdb2ffa7a214300b0be44c6221873781`.
O teste emula somente a regra antiga de direcionamento da roda para isolar as
otimizações; a nova retenção da roda é verificada em teste próprio. A polifonia
configurada continua em oito vozes melódicas compartilhadas, sem limitação a mono.

Benchmarks do host acompanham o pacote. Com -O2, a diferença foi inferior a 1%;
com -Os houve aproximadamente 5% no cenário executado. São variações de tempo no
computador, não uma previsão de ganho no AC791N. O disassembly real do alvo confirma
que as chamadas a fmt_res foram eliminadas; **só o novo teste físico dirá a margem
real e se os acordes deixam de apresentar dropouts**. Se persistirem, precisamos
confrontar P…us/! em GLOBAL com o prazo de aproximadamente 5805 µs e investigar
os outros custos/interrupts conforme a medição. Pads com release longo mantêm
vozes trabalhando após soltar as teclas, o que deve entrar na carga de teste.

## 3. Mod wheel VOICE perde o efeito ao trocar de parte

**Causa confirmada no código e correção testada.** O renderer aplicava `ctl_vib`
apenas quando `voice.part == ctl_wheel_part`; K6 muda `ctl_wheel_part` imediatamente.
A sequência anterior deixava de receber esse morph, embora continuasse tocando.

A VD2 mantém um latch de morph físico por parte VOICE. Movimento da roda atualiza
somente a parte selecionada; as demais conservam o último valor durante a sessão.
Trocar de parte e voltar com a roda parada não sobrescreve o valor. Mover a roda
na nova parte passa a controlar essa parte. A regra ANALOG de vibrato e o pitch
bend existentes não foram substituídos. MIDI CC1 continua editando o parâmetro
MORPH do patch, como antes.

Esse latch é **estado de performance em RAM**, não um novo autosave. Não acrescenta
escrita de flash e não promete conservar a posição física ao desligar. O parâmetro
MORPH do patch mantém seu próprio salvamento manual. A combinação existente usa o
maior valor entre MORPH do patch e o valor físico latched.

Teste usa o mapper ADC real e o renderer de formants com duas partes VOICE:
ajustar A, selecionar B com roda parada, mover em B, selecionar ANALOG, voltar a A,
continuar renderizando A e confirmar posição estável e ausência de escritas.

## Recursos da compilação real

| Recurso | VD1 | VD2 |
| --- | ---: | ---: |
| Imagem app | 173412 B | 173668 B |
| RAM principal | 74048 B | 74720 B |
| Pool | 325900 B | 325900 B |
| Limite RAM principal | 98304 B | 98304 B |
| Limite pool | 344064 B | 344064 B |
| Pacote | 651327 B | 651327 B |

O cache e os latches acrescentam 672 B na RAM principal. Instalação VD1→VD2,
retorno VD2→VD1 e VD2→PF12, banco privado e CRCs de boot passaram em simulação
com o loader C. Medidas de CPU/stack em uso e uso prolongado continuam pendentes.

## Verificação e preservação

- 15 testes nativos de controles/engines/storage/seq, incluindo testes específicos
  de retrigger e retenção; idle das rodas; sanitizers isolados VOICE e DRUM-X.
- ANALOG permanece byte a byte igual à referência PF12: SHA256
  `4ae6e1508a661e6e362824033f358902a80a81fe527cbcf52b36117c5ed42932`.
- O PCM DRUMS muda intencionalmente por corrigir o ganho e o ataque do retrigger;
  testes de ritmo, grid e persistência permanecem. Não se exige reproduzir o bug.
- Instalador separado: hashes de quatro pacotes, validação do modelo/identidade,
  confirmação, retorno VD1/PF12/PF9 e retomada testados com transporte/DOM simulados.
- O simulador hospedado continua na versão anterior, sem receber silenciosamente
  este firmware. Não se alegam testes WASM da VD2; os testes desta rodada são C e
  instalador JS em harness, além da compilação pi32v2 e validação de pacote.
- Nenhuma gravação USB real foi feita pelo assistente. A VD1 já tem as observações
  físicas do usuário; a VD2 ainda não recebeu teste físico.

Fontes correspondentes, branches preservadas, logs, build e instalador acompanham
esta entrega. Licenças e créditos da VD1/JIANT continuam preservados.

A prévia da página separada foi inspecionada no navegador, com quatro opções de
instalação/retorno e avisos de saves. A prévia HTTP bloqueia crypto.subtle como
esperado; os hashes reais foram validados no teste Node. Não houve conexão MIDI
no navegador, e a publicação HTTPS privada exige a sessão ChatGPT do proprietário.
