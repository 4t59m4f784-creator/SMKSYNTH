# SMKSYNTH ENDLESS E3 — protótipo

Versão experimental `1.0.3-e3`, identidade `SMK-37 Pro_19625`. Parte da pública 1.0.3 e inclui a correção UI1 de redesenho. Não promove uma nova versão pública.

## Primeiro teste

1. Salve o trabalho e exporte um backup. Use um pattern vazio, como P6, para montar a primeira frase.
2. Escolha uma parte ANALOG com K6 e use SINGLE. Sem teclas pressionadas, toque SEQ até a vista ENDLESS, depois de TRANSPOSE.
3. Com o transporte parado, toque REC: ativa ENDLESS e abre entrada por passos. Uma track realmente vazia começa em zero; uma frase existente é preservada e REC acrescenta no final.
4. Toque nota ou acorde e solte todas as teclas. Avança quando a última tecla é solta. Até quatro notas por passo; no máximo 256 passos.
5. Durante a entrada: `>` sem teclas insere uma pausa; com nota/acorde segurado acrescenta um passo ligado. `<` desfaz o último passo, com as teclas soltas. REC encerra a entrada. Segurar REC por 1,2 s mantém o comando de limpar a track; em ENDLESS a extensão volta a zero. PARA + OCT permite o undo existente.
6. Ao entrar na vista ENDLESS, o modo é ativado para todas as partes ANALOG. Escolha outra parte com K6 para gravar uma frase independente. Cada track conserva sua quantidade de passos, RATE e direção; todas usam o relógio mestre. Por exemplo, 3, 5 e 7 passos repetem independentemente.
7. PLAY inicia todas as tracks. Cada tecla, inclusive em legato, agenda o reinício de todas do começo na grade de semicolcheias Q16 e transpõe as partes ANALOG: C3 é zero. A última tecla pressionada define a transposição global de performance; a oitava física da parte é aplicada, MIDI recebido permanece absoluto. A transposição global KEY TR continua adicional; deixe-a zerada no primeiro teste. DRUM-X nunca transpõe.
8. K1 RATE, K2 GATE e K3 DIR (FWD/REV/RND) continuam por parte. **K4 HOLD é global**: ON continua após soltar; OFF para o transporte inteiro e apaga SEQ PLAY ao soltar a última tecla. Todos os cursores retornam ao início. Com duas teclas seguradas, soltar somente uma não para a reprodução. Releases e caudas naturais dos sons permanecem.
9. **K5 controla somente o TEMPO na tela ENDLESS**, de 40 a 240 BPM. O valor fica visível no alto à direita. Com clock MIDI externo, o valor efetivo tem prefixo E; o clock externo continua mestre, como na base. K5 mantém suas funções habituais nas demais telas.
10. PARA com toque curto na vista ENDLESS desliga/liga o modo global, conservando as frases. Fora da entrada, setas e pads de steps apenas selecionam a posição quando parado. Sair da vista encerra a entrada de notas, mas mantém o modo de performance. Ao trocar para DRUM-X, aparece seu editor normal; voltar para ANALOG restaura a vista ENDLESS. DRUM-X segue seu padrão, RATE e quantidade de passos, sincronizado ao transporte global: HOLD ON continua; HOLD OFF para com as demais partes. Cada novo disparo por tecla ou PLAY inicia todas do começo. Teclas no editor DRUM-X mantêm a edição normal de bateria; use uma parte ANALOG ou a tela de patch para disparar a performance pelo teclado.
11. Salve o pattern/cena pelo banco habitual de pads. A seleção ENDLESS e o HOLD global ficam no pattern; RATE/GATE/DIR e as frases continuam por track. Posição de execução, teclas seguradas e transposição disparada são temporárias. Patterns E1 migram preservando notas e controles; o HOLD da parte salva em foco passa a valer globalmente, com fallback para a primeira parte marcada.

## Escopo e cuidados do protótipo

- ANALOG em SINGLE. O teclado físico e MIDI OMNI/canal base usam o fluxo de entrada; os canais dedicados B–D mantêm a entrada normal da base. SPLIT/LAYER continuam fora deste protótipo. DRUM-X mantém seu sequenciador habitual e acompanha o transporte global, sem transposição.
- Reutiliza as notas/acordes/pausas/ligações e o relógio mestre da engine; não há DSP novo. SUPER, oito vozes compartilhadas, sends e USB da 1.0.3 permanecem.
- GATE é um controle de reprodução para a frase ENDLESS; ligações não são cortadas por ele. Não há swing novo, máscara de disparo nem avanço musical manual nesta etapa.
- Track e pattern mantêm exatamente 8 e 14476 bytes. Os dois bytes reservados de cada track guardam uma marca e os controles novos. Patterns antigos não marcados seguem o player original até entrar em ENDLESS. A pública 1.0.3 ignora esses controles e toca as notas como uma sequência comum.
- Ativar o modo em um pattern existente não apaga a frase. REC acrescenta ao final; `<` desfaz o último passo e REC segurado limpa a track, como descrito acima.
- Contador segmentado grande: em PLAY/STOP, números e valores na cor da parte/pad (inclusive vermelho); em REC, números brancos, selo REC vermelho e título na cor da parte. São atualizados somente dígitos/regiões alterados; não há redesenho contínuo em silêncio.
- Validação automatizada cobre workflow no loop real WASM, 4 partes isoladas, limite256, acordes/pausas/ligações, direções, HOLD global, última tecla/LED PLAY, reinício em legato, comprimentos 3/5/7 com bateria, K5 BPM, releases após troca de foco, persistência/migração E1 e atualização/retorno em flash simulada. Sanitização de memória e aritmética com signed-shift do DSP legado excluído. A avaliação sonora e visual no instrumento ainda depende do teste físico.

## Instalação e retorno

Use somente o instalador experimental desta pasta/site. Aceita a pública 1.0.x e os testes SUPER SS1/SS2/SS3, UI1, E1 e E2. Primeira instalação a partir da V016 é recusada. Inclui retorno à pública 1.0.3 original; ambos preservam o banco. Durante transferência interrompida, retome na mesma página/navegador com o mesmo pacote. Se a interrupção foi durante E1, use primeiro [o instalador E1 preservado](legacy-e1/); ele mantém os hashes e a chave de retomada originais.

## Créditos

Interação inspirada no [Endless do OP-1 / teenage engineering](https://teenage.engineering/guides/op-1/original/sequencers). Implementação e interface próprias no SMKSYNTH; não copia código, imagens ou fontes do OP-1 e não implica vínculo com a teenage engineering. ANALOG: OberVave / zvenson. SUPER: [Clipper / 3dSam](https://github.com/isod89/clipper-fm1). DRUM-X: JIANT. Direção, proposta e testes físicos: Marcus Padrini; código e ferramentas com assistência do ChatGPT / OpenAI. GPL-3.0-only; avisos em LICENSING.md.

## E3 — quantização suave de performance

- Grade global fixa em 1/16, independente da parte em foco e dos RATEs individuais. Primeiro ataque após parada longa é imediato. Com transporte ativo, o pedido espera a próxima divisão, sem mudar a tonalidade antecipadamente. WAIT indica o pedido. Outra tecla durante a espera substitui seu destino. Não altera as notas gravadas.
- Tolerância de atraso: até 20 ms, limitada a ¼ da divisão, aplica logo e conserva a fase das tracks e do clock. Não pode antecipar áudio para corrigir o passado; o ataque atrasado continua ligeiramente atrasado, mas os passos seguintes retomam a grade.
- HOLD ON mantém a frase antiga até a fronteira. HOLD OFF libera o transporte/PLAY ao soltar a última tecla, cancela pedidos pendentes e mantém apenas uma referência silenciosa de clock por uma semínima (500 ms a 120 BPM). A próxima tecla nesse intervalo é quantizada; após ele, inicia de imediato. Não há notas fantasmas quando a tecla é solta antes da fronteira.
- A espera máxima é uma semicolcheia (aproximadamente 125 ms a 120 BPM). O período usa o BPM efetivo, inclusive o andamento externo já suportado pela base. Não promete correção retroativa de execução nem sincronização nova com posição de DAW.
- STOP explícito, panic, pattern/undo e entrada REC limpam o pedido e a referência. PLAY explícito inicia imediatamente e ancora nova grade. Nenhum byte é acrescentado ao armazenamento.
- [Comparar com E2 sem quantização](legacy-e2/): firmware e chave de retomada E2 preservados; instalador aceita também origem E3 para permitir o retorno direto. Voltar a E3 pela página principal. A pública 1.0.3 permanece a mesma.
