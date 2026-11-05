# SMKSYNTH ENDLESS E1 — protótipo

Versão experimental `1.0.3-e1`, identidade `SMK-37 Pro_19623`. Parte da pública 1.0.3 e inclui a correção UI1 de redesenho. Não promove uma nova versão pública.

## Primeiro teste

1. Salve o trabalho e exporte um backup. Use um pattern vazio, como P6, para montar a primeira frase.
2. Escolha uma parte ANALOG com K6 e use SINGLE. Sem teclas pressionadas, toque SEQ até a vista ENDLESS, depois de TRANSPOSE.
3. Com o transporte parado, toque REC: ativa ENDLESS e abre entrada por passos. Uma track realmente vazia começa em zero; uma frase existente é preservada e REC acrescenta no final.
4. Toque nota ou acorde e solte todas as teclas. Avança quando a última tecla é solta. Até quatro notas por passo; no máximo 256 passos.
5. Durante a entrada: `>` sem teclas insere uma pausa; com nota/acorde segurado acrescenta um passo ligado. `<` desfaz o último passo, com as teclas soltas. REC encerra a entrada. Segurar REC por 1,2 s mantém o comando de limpar a track; em ENDLESS a extensão volta a zero. PARA + OCT permite o undo existente.
6. PLAY inicia o transporte mestre. Uma tecla também inicia o transporte e transpõe a frase da parte selecionada: C3 é zero. Outras tracks ligadas seguem o transporte compartilhado normal. A última tecla pressionada define a transposição; a oitava física da parte é aplicada, MIDI recebido permanece absoluto. A transposição global KEY TR continua sendo um deslocamento adicional; deixe-a zerada no primeiro teste.
7. K1 RATE, K2 GATE, K3 DIR (FWD/REV/RND), K4 HOLD. HOLD ON continua após soltar; HOLD OFF libera as notas e reinicia do começo ao próximo disparo. Para acordes ligados, valide FWD primeiro. Notas/sustain já existentes na engine mantêm suas regras de release.
8. PARA com toque curto na vista ENDLESS liga/desliga o modo, conservando notas. Fora da entrada, setas e pads de steps apenas selecionam a posição quando parado. Sair da vista encerra a entrada; o modo de reprodução permanece na track. PLAY para o transporte.
9. Salve o pattern/cena pelo banco habitual de pads. RATE/GATE/DIR/HOLD ficam na track; posição de execução, teclas seguradas e transposição disparada são estados temporários.

## Escopo e cuidados do protótipo

- ANALOG em SINGLE. O teclado físico e MIDI OMNI/canal base usam o fluxo de entrada; os canais dedicados B–D mantêm a entrada normal da base. SPLIT/LAYER e DRUM-X não são o alvo deste teste.
- Reutiliza as notas/acordes/pausas/ligações e o relógio mestre da engine; não há DSP novo. SUPER, oito vozes compartilhadas, sends e USB da 1.0.3 permanecem.
- GATE é um controle de reprodução para a frase ENDLESS; ligações não são cortadas por ele. Não há swing novo, máscara de disparo nem avanço musical manual nesta etapa.
- Track e pattern mantêm exatamente 8 e 14476 bytes. Os dois bytes reservados de cada track guardam uma marca e os controles novos. Dados antigos não marcados seguem o player original. A pública 1.0.3 ignora esses controles e toca as notas como uma sequência comum.
- Alterar o modo numa track existente não apaga a frase. REC acrescenta ao final; `<` desfaz o último passo e REC segurado limpa a track, como descrito acima.
- Contador segmentado grande: posição de entrada em vermelho, passo reproduzido em laranja. São atualizados somente dígitos/regiões alterados; não há redesenho contínuo em silêncio.
- Validação automatizada cobre workflow no loop real WASM, 4 partes isoladas, limite256, acordes/pausas/ligações, direções/HOLD, releases após troca de foco, persistência e atualização/retorno em flash simulada. Sanitização de memória e aritmética com signed-shift do DSP legado excluído. A avaliação sonora e visual no instrumento ainda depende do teste físico.

## Instalação e retorno

Use somente o instalador experimental desta pasta/site. Aceita a pública 1.0.x e os testes SUPER SS1/SS2/SS3, UI1 e E1. Primeira instalação a partir da V016 é recusada. Inclui retorno à pública 1.0.3 original; ambos preservam o banco. Durante transferência interrompida, retome na mesma página/navegador com o mesmo pacote.

## Créditos

Interação inspirada no [Endless do OP-1 / teenage engineering](https://teenage.engineering/guides/op-1/original/sequencers). Implementação e interface próprias no SMKSYNTH; não copia código, imagens ou fontes do OP-1 e não implica vínculo com a teenage engineering. ANALOG: OberVave / zvenson. SUPER: [Clipper / 3dSam](https://github.com/isod89/clipper-fm1). DRUM-X: JIANT. Direção, proposta e testes físicos: Marcus Padrini; código e ferramentas com assistência do ChatGPT / OpenAI. GPL-3.0-only; avisos em LICENSING.md.
