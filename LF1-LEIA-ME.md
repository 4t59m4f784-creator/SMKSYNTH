# OberVave 3.0.1-lf1 — SMK-37 Pro

Experimental. Base bcddc4b08d819296b1e507d5fa9395eb0f2d88af; correção adaptada do snapshot e9dc9a76cc6caef75ba6d49dc85e2e8f2b627067.

Mudanças: smk_outer no app_idle, preservando note_in/MIDI_OUT_CH da 3.0.1; fallback de contato lento aos 40 ms; etiqueta LF1 no boot e identidade própria SMK-37 Pro_19531. Número restrito a este teste, sem alegação de reserva global. Motor DSP, presets, quatro tracks, efeitos, loader e armazenamento não foram modificados. Nenhum projeto SMKTRON, OberVave oficial ou ShineOn hospedado foi atualizado.

A recompilação da base oficial, antes do patch, reproduziu exatamente o SHA-256 c80ecec5affb925b4145fb7a1ac07082ac151379443d96ba0fa5520450c3087f.

Validação: 891 verificações de contatos das 37 teclas (incluindo timer wrap e bounce), 332 de despacho em app_idle, 31 de interface SMK37. Os testes novos falham na base sem patch. Suíte tests/run_tests.sh passou após configurar a expectativa estrita de identidade LF1 no teste de pacote. Foi gerado também o FM-1 apenas para executar os testes que dependem dele, sem mudar seu código ou instalá-lo. O teste do simulador nessa suíte usa o WASM upstream incluído, não um simulador LF1 recompilado. Áudio e telas do script host padrão continuam byte-idênticos aos da base.

Limites: não há instalação física nem medição de latência real desta LF1. Overflow de filas e eventuais problemas MIDI/ARP fora dos cenários cobertos não são considerados resolvidos por este patch. A troca de track durante nota mantém o destino do note-off do motor da 3.0.1; o canal MIDI de saída segue a política preexistente, sem nova alteração. Não colocar bootloader alternativo nem apagar flash manualmente.

Instalação: usar o instalador privado LF1 em Chrome/Edge. Fechar MUpgrade, DAWs e demais instaladores. Carregar o V016 original (SHA-256 2d98ce72530e71d617384963423820536f094501299e2cfec0a8d3c3fb6bcfb0), consultar identidade e confirmar explicitamente. O teclado deve informar SMK-37 Pro, com uma das identidades da allowlist. O instalador não escreve ao abrir ou consultar. Retorno: arquivo V016 original, inclusive loader ota-SMK-37 Pro_016 reconhecido. Teste físico de ida/volta por este novo site pendente.

Comparação no instrumento: mesmo patch de ataque curto, mesmos efeitos e volume, pontas versus centro, tela parada versus girar knobs; depois testar sustain, arp, REC, tracks, pads, faders e rodas. Validar inicialização LF1 e anotar qualquer regressão.

Licenças: fontes GPL-3.0-only, conforme LICENSE e LICENSING.md do projeto. O toolchain JieLi e os três arquivos do SDK mantêm suas licenças próprias; não se atribui GPL a eles. Toolchain: arquivo oficial jieli-linux-toolchains-20260730.1.tar.xz, clang 4.0.1. SDK: três arquivos da referência AC79NN_SDK_V1.2.1_2023-12-13, todos com os hashes esperados em tools/build.py. A cópia local dos arquivos de SDK serve ao empacotamento; não é um checkout completo do SDK.

Recompilação (Linux x86-64): extraia toolchain/jieli-toolchain.tar.xz em uma pasta e aponte JIELI_TOOLCHAIN para a raiz que contém pi32v2/ e common/. Aponte AC79_SDK para sdk/. Execute na pasta source/: BOARD=smk37 MODEL=pro sh build.sh --release 3.0.1-lf1 --experimental-id 19531. Depois rode sh tests/run_latency_tests.sh. Para a suíte global, faça a build SMK37 primeiro e a FM-1 por último, como explicado no script upstream; seus intermediários compartilham build/. Não use o arquivo FM-1 no SMK37.
