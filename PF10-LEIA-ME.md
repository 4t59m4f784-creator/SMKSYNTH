# OberVave PF10 — rodas por track (experimental)

Rodas físicas de pitch e modulação atuam somente na track selecionada, tanto na execução pelas teclas quanto na sequência. Ao mudar de track com a roda acionada, a anterior perde a contribuição da roda e a nova passa a recebê-la. O pitch anterior retorna com a suavização curta existente. O movimento não fica memorizado por parte.

Separação do pitch bend MIDI e da roda física; mensagem MIDI continua com o comportamento anterior, somando à roda apenas na parte selecionada e limitada ao intervalo MIDI. Modulação interna dos presets é mantida. Fluidez PF9, volume do boot, pads e salvamento manual preservados. PF9 continua sendo a versão atual aprovada; PF10 requer teste físico.

Teste: reproduza sequência com duas ou mais tracks. Mova as rodas em A, depois selecione B com a roda acionada. Só a selecionada deve receber a roda; as demais mantêm sua modulação interna. Confira os dois sentidos, retorno ao centro e a fluidez. Installer /pf10/, retorno oficial incluído. Requer arquivo V016 para recuperação, sem instalá-lo antes.
