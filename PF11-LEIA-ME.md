# OberVave PF11 — SMK-37 Pro — teste de recuperação de cenas

Base: PF10 (inclui rodas físicas apenas na parte selecionada). Firmware experimental, ainda sem teste físico.
Ao tocar num slot salvo do banco 3, os dados permanentes são relidos: sequência, quatro timbres completos, arpejadores, efeitos e mixagem. Edições temporárias são descartadas; tocar na própria cena também restaura. Tocando: troca na próxima virada de compasso. K1 continua independente, somente persistência de boot. Sem autosave. Formato de armazenamento, calibração e loader preservados.

Teste no teclado: salve A, copie para B; carregue B antes de editar; altere ARP, timbres, mix, FX e notas e salve B. Alterne A/B parado e tocando. Edite A sem salvar e recarregue A. Compare após reiniciar. Aguarde SAVED antes de desligar.

Instalador de teste: https://obervave-smk37-pf1.padrini.chatgpt.site/pf11/
Versão principal e simulador não foram promovidos. SMKTRON não foi alterado.
Compilação: JIELI_TOOLCHAIN=<toolchain> AC79_SDK=<sdk> BOARD=smk37 MODEL=pro sh build.sh --release 3.0.1-pf11 --experimental-id 19551
Teste: cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o build/host/scene_test tests/smk_scene_recall_test.c && build/host/scene_test
Leia LICENSE e LICENSING.md no código-fonte.
