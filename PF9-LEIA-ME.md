# OberVave 3.0.1-pf9 — SMK-37 Pro, salvamento manual

Identidade SMK-37 Pro_19549. Firmware experimental para avaliar a fluidez das rodas durante o redesenho da tela. Inclui o banco de padrões testado no simulador PF4, a correção de latência LF1 e o cabeçalho/boot da PF3. Mantém algoritmos dos osciladores e efeitos, patches de fábrica, formatos/setores de memória e loader; modifica apenas os controles descritos abaixo. A ausência de glitches no teclado ainda depende do teste físico.

## PF9 — rodas durante o desenho da tela

Única mudança funcional sobre a PF8: chamada smk_wheels() em app_idle(), dentro de BOARD_SMK37 e da proteção busy/idle_ok existente. Mantém a chamada do loop principal. Não desenha tela nem escreve flash. Preserva ADC fracionário, calibração, zona morta e suavização da PF8. Sem mudanças no DSP, timbres, efeitos, salvamento ou formato de memória sobre a PF8.

Teste controlado do app_idle real, com leituras diferentes a cada yield de 1 ms por 50 ms: PF8=0 atualizações durante o trecho (gap sintético 50 ms); PF9=50 atualizações (gap sintético 1 ms). Testa os dois sentidos, retorno ao centro, modulação, idle_ok desativado e zero chamadas de gravação. Isso demonstra o atendimento em software; NÃO mede os intervalos no processador JieLi nem comprova melhora audível. Essas duas avaliações continuam pendentes no teclado físico. Não se acrescentaram contadores de diagnóstico ao firmware de produção.

## Recursos preservados da PF8 — pitch bend e volume do boot

Leitura das duas rodas com acumulador fracionário Q8: movimentos pequenos deixam de ser descartados pela divisão inteira. Pitch bend no motor SMK com suavização de aproximadamente 5,8 ms (constante de tempo), sem mudar a faixa do preset; não altera o motor FM-1. Testar fisicamente movimentos lentos nos dois sentidos e retorno ao centro. A auto-calibração e a zona morta central permanecem.

K1 é gravado somente junto da escrita manual das configurações pedida por SAVE de preset/padrão/cópia. Recuperado somente no boot; carregar preset, favorito ou padrão preserva o K1 atual. Favoritos não gravam K1. Sem auto-save. Codificação no byte reservado rec_q, mesmo tamanho de settings (67 bytes), versão e setor. Flag de validade evita confundir versões anteriores; sem valor salvo, padrão 724. Resolução de 7 bits, erro máximo de arredondamento de 4 em 1023, menor que o passo de 24 do encoder. Slots de padrões/favoritos e presets mantêm seus formatos.

Testes adicionais: 2759 verificações nativas de controles, rampas lentas de 1 count, suavização monotônica nos dois sentidos e retorno ao zero; teste WASM de boot/recall e salvamento exclusivamente manual.

## Acabamento dos pads preservado da PF7

Retroceder preset: vermelho C_RED, nível 32/256 em repouso. Avançar: mesmo azul dos favoritos PF_SLOT_BLUE, nível 32/256. Ambos vão a 255/256 enquanto o pad permanece pressionado e voltam ao nível baixo ao soltar, sem flash branco temporizado. Delay/reverb continuam estáticos; brilho ativo reduzido para 176/256, desligado mantém 10/256. Cores e gestos dos demais pads preservados.

## Iluminação dos favoritos e padrões

| Estado | Favoritos | Padrões |
|---|---|---|
| Vazio | Azul muito discreto | Vermelho muito discreto |
| Salvo, não selecionado | Azul discreto | Vermelho discreto |
| Salvo e selecionado | Vermelho forte | Azul forte |
| Selecionado e alterado sem salvar | Vermelho médio | Azul médio |

Brilhos relativos 16/256, 48/256, 240/256 e 96/256. São níveis de cor do código, não medições fotométricas. As indicações temporárias de gravação pendente, troca agendada e confirmação têm prioridade. Um slot vazio mantém a cor de vazio mesmo que seja o padrão corrente. O estado editado do padrão usa pat_edited/snd_dirty e comparação com o conjunto em RAM, sem ler a flash; não perde a indicação após snd_capture. Não se promete comparação byte a byte com a flash nem reconhecimento de toda reversão manual de edições. Nenhum formato de memória mudou.

## Todos os salvamentos automáticos desativados

Ajustar efeitos, faders, knobs, programas, mixer, oitava, gravar sequências, iniciar/parar e trocar padrões não autoriza escrita na flash. As mudanças ficam na RAM. Salve explicitamente antes de trocar de padrão ou desligar. Sons em RAM podem acompanhar navegação na sessão, mas isso não garante persistência nem preservação de notas da sequência ao sair sem salvar. Não há salvamento especial no desligamento.

## Quando os efeitos são salvos

- Salvar padrão manualmente guarda suas quatro partes completas, mixagem, envios, sequências, automações, efeitos e processamento MASTER. Também pede gravação das configurações gerais.
- Salvar preset manualmente (BT/SAVE duas vezes fora da tela de sequenciador) grava esse preset e pede gravação das configurações gerais atuais, incluindo efeitos globais. Não regrava automaticamente o padrão. Ao carregar um padrão, os efeitos próprios dele têm prioridade; para recuperar o conjunto completo de quatro partes/efeitos, salve o padrão.
- Salvar/copiar um padrão para outro slot também é um pedido manual e grava as configurações gerais.
- Favorito (segurar pad inferior por 700 ms em Performance) guarda a cópia do timbre e envios de delay/reverb da parte. Não é uma configuração independente dos motores globais de efeitos. Ao chamar um favorito, o OberVave mantém os efeitos compartilhados.

Pedidos manuais continuam podendo aguardar o sequenciador parar e engine_quiet() permitir escrita. O critério de silêncio do motor foi mantido: caudas de FX podem ser interrompidas durante uma gravação MANUAL. Faça saves em uma pausa. Aguarde FAVOURITES SAVED para favoritos. Para preset, SAVE QUEUED é apenas o pedido: aguarde SETTINGS SAVED. Para padrão/cópia, aguarde SAVED IN/COPIED TO e a conclusão posterior das configurações (SETTINGS SAVED). Não desligue com pedidos pendentes.

## Bancos de pads

KNOB BANK, toque curto: Performance → Sequenciador → Padrões. Segurar mantém o diagnóstico de painel. REC, tela de sequência ou layer SEQ têm prioridade temporária de sequenciador. FADER BANK mantém suas funções.

Performance: superiores A–D (seleção de parte), patch −/+, delay/reverb por envio; inferiores FAV1–8 azuis em repouso e vermelhos quando selecionados, toque carrega/700 ms memoriza. Delay estático. Padrões: superiores P1–P8; toque chama pat_go (parado carrega, tocando agenda compasso); 700 ms no atual salva, em outro copia sem carregar primeiro. Outro slot ocupado exige repetir o gesto longo em até 3 s. Atual salvo azul forte ou azul médio se editado; salvo não selecionado vermelho discreto; vazio vermelho muito discreto; branco é escrita pendente. Inferiores desativados. Durante uma escrita solicitada pelo banco, mudanças de padrão/save/copy são bloqueadas até concluir, para não trocar a origem de uma cópia pendente. Edições posteriores ao pedido podem entrar na escrita adiada ao parar, conforme as rotinas nativas.

## Validação

36 verificações específicas de iluminação passaram: estados inversos, detecção de edições de timbre/envios/mix/seq, persistência da indicação durante refresh de RAM e zero chamadas de persistência pelo desenho dos LEDs.

SMK-37 Pro compilado, pacote com SHA/CRC/identidade validados; loader mantém o hash da base oficial. Testes host: 245 Performance, 891 contatos, 332 despacho durante LCD e 31 UI, mapa de flash protegido. Testes WASM reais de 3 bancos, cópias e recuperação, confirmação, troca em compasso e interlock. Teste específico conta chamadas às quatro APIs de gravação: zero após boot e alterações/execução/navegação sem SAVE; somente pedidos explícitos escrevem; efeitos globais voltam dos bytes salvos manualmente. O simulador usa RAM/localStorage e não reproduz a pausa elétrica da flash do hardware.

## Instalar

Use o instalador privado /pf9/. Aceita instalação direta da PF8 _19548 ou PF7 _19547 ou PF6 _19546 ou PF5 _19545 ou PF3 _19543 (e PF1/PF2/LF1/oficial/V016 reconhecidos). Identidade final _19549, rótulo 3.0.1-pf9. Instalação física e avaliação de áudio desta PF9 pendentes. Mantenha USB estável e feche DAWs/MUpgrade e outros instaladores. O V016 original continua exigido como retorno; a cópia da rota LF1 aprovada está disponível neste instalador. A PF5, os instaladores anteriores e o SMKTRON foram preservados.

## Reproduzir

Linux x86-64, toolchain JieLi oficial 20260730.1 e três arquivos SDK AC79NN_SDK_V1.2.1_2023-12-13 incluídos com hashes no projeto. Extraia toolchain/jieli-toolchain.tar.xz e defina JIELI_TOOLCHAIN; AC79_SDK deve apontar para sdk/. Em source/: BOARD=smk37 MODEL=pro sh build.sh --release 3.0.1-pf9 --experimental-id 19549. Em simulator/: Zig 0.13 e Binaryen 132, sh build-simulator.sh. GPL-3.0-only para OberVave/adaptações; toolchain/SDK mantêm licenças próprias. Créditos upstream preservados.
