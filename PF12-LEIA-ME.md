# OberVave PF12 — SMK-37 Pro — Sends e knobs

Versão de teste baseada na PF11. Aguardando validação no teclado.

K1–K4: quatro parâmetros exibidos na tela (performance, edição, ARP, efeitos, global, mixer/sends, sequenciador e passos).
K5: funções contextuais do antigo K2 — tempo na performance, navegação/padrões nas telas que já usavam essa função.
K6: parte. K7: preset; com SAVE armado, destino. K8: volume geral, mesma curva e persistência de boot do antigo K1.
Faders A preservados. Banco B: F1 chorus SEND, F2 delay SEND, F3 reverb SEND da parte selecionada; F4 master. Sends aguardam pickup ao trocar parte/banco ou recuperar valores: mova até alcançar o valor indicado. Mixes/retornos globais permanecem nas telas de efeitos; send não torna audível um retorno global zerado.

Favoritos novos guardam timbre e três sends. Favoritos PFV1 são lidos sem gravação automática: mantêm o chorus atual, pois não o armazenavam; ao salvar manualmente passam a PFV2. A PF11 e versões anteriores não leem PFV2. O banco anterior permanece até ser substituído pela alternância normal de salvamentos. Não se promete recuperação desses favoritos ao retornar à PF11. As cenas mantêm formato e recuperação completa da PF11. Volume K8 permanece independente das cenas. Não há autosave.

Instalador de teste: https://obervave-smk37-pf1.padrini.chatgpt.site/pf12/
Simulador de teste: https://obervave-smk37-simulator.padrini.chatgpt.site/pf12/
Retorno PF11: https://obervave-smk37-pf1.padrini.chatgpt.site/pf12/pf11-return/
As versões principais e o SMKTRON foram preservados. O simulador PF12 tem sessão local separada e aceita importar sessões antigas. Não mede carga/latência física do JieLi.

Validação: 651 comparações do estado dos controles com a PF11, 44 verificações de sends/pickup/favoritos e migração, regressões nativas de teclas/pads/flash/rodas/volume/cenas e testes WASM/AudioWorklet, favoritos após reboot, padrões e salvamento manual. Interface também operada no navegador; preview usou modo sem áudio. Teste no hardware ainda necessário.

Teste sugerido: confirme K1–K4 em performance, PARA, FX, GLOBE, mixer/sends, SEQ e passo segurado; confira K5 nas páginas de passos/padrões e K7 no SAVE. Mude de parte e ajuste F1–F3 até pickup, verifique isolamento, salve favorito, recupere e reinicie. Salve/recupere duas cenas com sends diferentes. Aguarde conclusão de SAVE.

Compilar: JIELI_TOOLCHAIN=<toolchain> AC79_SDK=<sdk> BOARD=smk37 MODEL=pro sh build.sh --release 3.0.1-pf12 --experimental-id 19552
Testes principais: sh tests/run_performance_tests.sh
Teste PF12: cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen -o build/host/pf12 tests/smk_pf12_test.c && build/host/pf12
Matriz de controles: tests/smk_encoder_matrix_test.c (logs de referência PF11 incluídos).
O SDK/toolchain necessários estão no BuildKit PF9 preservado anteriormente. Licenças e créditos em LICENSE/LICENSING.md no código.
