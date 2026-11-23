# SMKSYNTH SU2 — experimental 1.2.2-su2 / 19662

Base: DC3 approved dual-core → SU1. Public 1.2.2 /19655 unchanged. No new persisted fields; patch80/pattern/favourites/backup layout unchanged.

## PT — alterações
- SUB aditivo: OSC1/OSC2 não são reduzidos. O nível máximo do SUB é ~50% de um oscilador simples no máximo (coeficiente64 contra127); nenhuma nova voz. Mantidos antialiasing, fase, rampas e teto de polifonia. A soma continua passando pelo drive/clamp e filtro existentes; extremos de ganho podem saturar e precisam de avaliação no instrumento. Sem novo limiter/compressor.
- Split/Layer Zoom: dois cartões com parte/timbre/preset/nível ou oitava, LOW/HIGH, EDIT/PLAY e estado SINGLE; somente os controles K1/K2/K3/K4/K5 já existentes. Em SINGLE toca a parte selecionada, mesmo que diferente do par lembrado. Níveis da combinação não afetam SINGLE.
- Mixer Zoom: boxes A/B/C/D e linhas nas cores das partes em Levels, Pan e Sends. Master não recebe cores por parte.
- Rodapé PADS: banco atual. PATTERNS é o nome adotado. No osciloscópio permanece somente aviso contextual/status Endless, sem rodapé permanente de pads.
- Endless da parte selecionada: READY aguarda tecla; passo/total durante execução; REC vermelho durante entrada de notas. Na página Endless o contador original permanece. No Mixer/Sends Obervave o status ocupa a linha de tags do cabeçalho enquanto ativo; medidores intactos.
- REC mantido: overlay Zoom acompanha limiar real de1200ms; soltar antes não apaga; confirmação só após trk_clear real.
- GLOBE: página INITIALIZE após MIDI. K1 escolhe PART PATCH/PATTERN; PLAY revisa; confirmação inicia em CANCEL; K1 INITIALIZE + PLAY executa. PATCH cancela/sai. PLAY/PATCH não executam ações normais e suas solturas são consumidas. Confirmação cancela se alvo mudar; gravação/salvamento/troca pendentes bloqueiam execução.
- PART PATCH: INIT ANALOG na parte alvo; preserva sequência, mix, outros patches e Split/Layer. Notas dessa parte são encerradas.
- PATTERN: partes A/B/C INIT ANALOG, D kit básico DRUM-X 808, nível100 igual/pan central/sends0, FX e Master drive/comp desligados, filtro Master central, arps/Endless desligados, sequências vazias16passos e duas linhas SINGLE. Preserva tempo, afinação, MIDI, tema e volume físico. Encerra transportes/notas via Panic existente e aplica após confirmação do áudio. Não grava presets, patterns ou favoritos em flash; salvar explicitamente para manter.
- Master: Volume, DJ Filter, Drive e Compressor agora chegam imediatamente aos controles do DSP; algoritmos existentes preservados.

## Testes e limites
Validação automatizada cobre controles reais, cancelamentos, confirmação, persistência sem escrita implícita, comparação serial/dual-core, falhas de worker, SUB aditivo, polifonia e mistura DRUM-X. Não substitui teste no teclado físico. Pacotes Pro/Elite preservam banco e journal e usam o loader protegido existente. Conferir métricas finais em BUILD.json e logs validation/su2.

Faça backup pela ferramenta desta página. Voltar à oficial ignora SUB; patches SU podem recuperar SUPER MIX padrão em firmware antigo. Restaurar backup anterior para retornar exatamente aos timbres originais.

## EN — test guide
SU2 is an isolated DC3-based dual-core experiment. SUB is now additive and capped at roughly half one full simple oscillator; it never attenuates OSC1/OSC2. Existing gain saturation/filter behavior remains and needs hardware listening tests. UI adds patch names/parts to Zoom Split/Layer, colored mixer identifiers, pad-bank and Endless READY/progress/REC status, and hold-REC feedback. GLOBE INITIALIZE supports part patch or full pattern initialization: PLAY opens review, K1 defaults to CANCEL; select INITIALIZE then PLAY. PATCH cancels/exits; these keys do not operate transport/performance while this page owns them. No automatic preset/pattern flash overwrite. Pattern reset creates three INIT ANALOG parts plus basic DRUM-X808 on D, equal levels, centered pan, FX off, empty16step tracks, arps/Endless off and SINGLE keyboard rows. Master controls now update the audio immediately. Public release remains unchanged. Back up before testing. Hardware audio/performance validation is still required.
