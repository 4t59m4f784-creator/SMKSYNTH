# OberVave 3.0.1-pf5 — SMK-37 Pro, salvamento manual

Identidade SMK-37 Pro_19545. Firmware experimental para testar o diagnóstico de glitches após ajustes de efeitos. Inclui o banco de padrões testado no simulador PF4, a correção de latência LF1 e o cabeçalho/boot da PF3. Não modifica DSP, algoritmos de efeitos, patches de fábrica, mapa/formato da memória ou loader de atualização. A ausência de glitches no teclado ainda depende do teste físico.

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

Performance: superiores A–D (seleção de parte), patch −/+, delay/reverb por envio; inferiores FAV1–8 vermelhos, toque carrega/700 ms memoriza. Delay estático. Padrões: superiores P1–P8 azuis; toque chama pat_go (parado carrega, tocando agenda compasso); 700 ms no atual salva, em outro copia sem carregar primeiro. Outro slot ocupado exige repetir o gesto longo em até 3 s. Atual azul forte; salvo azul médio; vazio discreto; branco é escrita pendente. Inferiores desativados. Durante uma escrita solicitada pelo banco, mudanças de padrão/save/copy são bloqueadas até concluir, para não trocar a origem de uma cópia pendente. Edições posteriores ao pedido podem entrar na escrita adiada ao parar, conforme as rotinas nativas.

## Validação

SMK-37 Pro compilado, pacote com SHA/CRC/identidade validados; loader mantém o hash da base oficial. Testes host: 245 Performance, 891 contatos, 332 despacho durante LCD e 31 UI, mapa de flash protegido. Testes WASM reais de 3 bancos, cópias e recuperação, confirmação, troca em compasso e interlock. Teste específico conta chamadas às quatro APIs de gravação: zero após boot e alterações/execução/navegação sem SAVE; somente pedidos explícitos escrevem; efeitos globais voltam dos bytes salvos manualmente. O simulador usa RAM/localStorage e não reproduz a pausa elétrica da flash do hardware.

## Instalar

Use o instalador privado /pf5/. Aceita instalação direta da PF3 _19543 (e PF1/PF2/LF1/oficial/V016 reconhecidos). Identidade final _19545, rótulo 3.0.1-pf5. Instalação física e avaliação de áudio desta PF5 pendentes. Mantenha USB estável e feche DAWs/MUpgrade e outros instaladores. O V016 original continua exigido como retorno; a cópia da rota LF1 aprovada está disponível neste instalador. Os instaladores anteriores e o SMKTRON foram preservados.

## Reproduzir

Linux x86-64, toolchain JieLi oficial 20260730.1 e três arquivos SDK AC79NN_SDK_V1.2.1_2023-12-13 incluídos com hashes no projeto. Extraia toolchain/jieli-toolchain.tar.xz e defina JIELI_TOOLCHAIN; AC79_SDK deve apontar para sdk/. Em source/: BOARD=smk37 MODEL=pro sh build.sh --release 3.0.1-pf5 --experimental-id 19545. Em simulator/: Zig 0.13 e Binaryen 132, sh build-simulator.sh. GPL-3.0-only para OberVave/adaptações; toolchain/SDK mantêm licenças próprias. Créditos upstream preservados.
