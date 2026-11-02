# SMKSYNTH P16.2 — pads estáveis e oitava por parte

Base P16.1 fonte093029ff96e4bca71b65b39c43a4be1ef11ed762. Build3.0.1-p16o1, identidadeSMK-37 Pro_19613. Ramo independente smksynth-p16o1; versão anterior preservada.

PLAY e REC não assumem os pads. Cada um mantém Performance/Favoritos, Split/Layer, Patterns ou Sequenciador conforme já escolhido. Somente SEQ ativa os pads do sequenciador. Para gravar drums pelos oito pads de instrumentos, selecionar SEQ explicitamente e depois REC. Os pads de step, inclusive DRUM-X, dependem desse banco; entrar numa página por REC não basta para trocar suas funções.

Cada parte mantém sua oitava de teclado, entre−2 e+2. OCT−/OCT+ alteram a parte selecionada nos contextos musicais habituais; ambos zeram. Nas páginas onde esses botões já controlam páginas/undo, suas funções são preservadas. O ajuste acompanha o patch ao salvar por BT, o snapshot do favorito e os quatro patches da cena/pattern. Trocar de parte recupera imediatamente sua própria oitava. Não há autosave.

A oitava adicional de cada camada SPLIT/LAYER continua independente e é somada à oitava da parte ao tocar o teclado físico. As notas das sequências existentes não são transpostas; REC continua registrando apenas a parte selecionada, com a altura efetivamente tocada. Entrada MIDI conserva seu comportamento anterior. A saída MIDI física envia a oitava selecionada e guarda nota/canal para corresponder o note-off. Notas seguradas mantêm o destino e a altura da sua origem, mesmo trocando oitava ou parte.

Não altera DSP, polifonia, efeitos, mapa de flash ou tamanhos de arquivos. KBO1 usa o byte reservado73 do patch de80bytes, anterior ao tag de engine74..79. Pattern14476B/cena380B permanecem iguais. Presets/favoritos/cenas antigos sem essa identificação recebem oitava0; o antigo ajuste global não contém quatro valores independentes para migrar.

Mantém16 patterns,99 programas,8 favoritos, telas de save revisadas e confirmação4s. Presets devem ser salvos explicitamente para reter edições; salvar pattern guarda sua cena completa. As partes mantêm seus ajustes em RAM enquanto se toca; isso não cria salvamento automático ao desligar.

**Instalar/reinstalar ainda reinicializa o banco experimental de saves.** Esta build não implementa exportação nem atualização preservando dados. Os arquivos de backup de código/firmware não contêm seus sons do dispositivo. A versão oficial AD1 permanece intacta.

Recursos do linker: imagem155388/327680B; RAM estática82752/98304B; pool325984/344064B. Testes nativos, sanitizers e WASM cobrem oitava por parte, persistência por patches/cenas/favoritos, soltura após trocas, offsets adicionais de layer, bancos em PLAY/REC/SEQ,16slots e regressões de áudio. Teste físico pendente: tocar e soltar notas/sustain, alternar partes, salvar/reiniciar, gravar ANALOG/DRUM-X e confirmar que os pads permanecem no contexto escolhido.

Instalação e recuperação usam o instalador experimental P16; identidade esperada19613. Chrome/Edge, cabo USB de dados, fechar DAWs; aguardar conclusão e primeiro boot. Retorno à U2/AD1 pela mesma página. Em resgate, ligar com OCT− e consultar. Ver documentos anteriores para limites.

Build: configurar JIELI_TOOLCHAIN e AC79_SDK, executar:

```
python tools/build.py --voice-hardware --board smk37 --model pro --release 3.0.1-p16o1 --experimental-id 19613
sh tools/build_voice_sim.sh
sh tests/run_p16_tests.sh
sh tests/run_sl1_tests.sh
```
