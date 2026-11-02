# SMKSYNTH DP1 — Drum Pads

Experimental 3.0.1-dp1, SMK-37 Pro_19616. Base musical e dados da B2 / 19615; B2 e P16.2 oficiais preservadas. Não é a release pública.

## Uso

- Parte ANALOG: KNOB BANK mantém o ciclo Performance/Favoritos → Split/Layer → Patterns/Save.
- Parte DRUM-X: o ciclo acrescenta Drum Pads depois de Patterns/Save. Entrar nesse banco é sempre explícito. Selecionar engine, entrar em DRUM SOUND, PLAY ou REC não assumem os pads.
- Drum Pads: oito pads superiores tocam KICK, SNARE, CLAP, HAT CL, HAT OP, TOM, RIM e BELL, com velocity. Os oito inferiores ficam apagados e sem ação.
- Violeta próprio: RGB de referência 166/66/255 escalado por 179/256, aproximadamente 70% do valor comandado. Um hit usa branco 255/255/255. São valores do firmware, não uma medição do brilho físico.
- O disparo manual fica branco durante a pressão. O feedback de notas MIDI e do sequenciador usa os mesmos timestamps de hits da tela, com pulso de aproximadamente 100 ms. Mostra somente o kit da parte selecionada; partes de bateria diferentes não se misturam na indicação.
- REC funciona na parte DRUM-X selecionada. PLAY/REC e mudanças de tela mantêm a propriedade dos pads. SEQ continua entrando explicitamente no banco de steps e preserva seu contexto de gravação existente.
- Trocar para uma parte ANALOG enquanto Drum Pads está ativo não muda o banco: os pads ficam apagados/inativos e a tela informa SELECT A DRUM-X PART. K6 pode voltar à parte de bateria; KNOB BANK volta ao ciclo de três bancos.
- Segure KNOB BANK por 0,7 s para voltar a Performance/Favoritos. O transporte continua no estado atual.

Os releases de pads guardam parte e canal MIDI da nota original, inclusive se a seleção de parte/banco mudar durante uma pressão. Não há novo formato salvo nem gravação automática do banco selecionado. Ao reiniciar, o instrumento mantém seu comportamento anterior de começar em Performance/Favoritos.

## Atualização e retorno

Guarde um backup .smksynth antes de instalar. O instalador DP1 verifica pacote e identidade e aceita 19613/19614/19615/19616. O alvo DP1 é 19616. Há retorno explícito ao arquivo B2 validado 19615, com fontes originais correspondentes.

O loader e o protocolo de backup B2 não foram alterados. O layout SMK16P1, seus 16 patterns, favoritos e presets continuam iguais. A ferramenta backup-dp1 aceita arquivos B1/B2/DP1 e dispositivos B1, B2 e DP1; restauração integral exige B2 ou DP1. O exportador antigo pode não reconhecer a identidade 19616; use a ferramenta desta entrega. A seleção de fábrica e a primeira instalação de fábrica continuam pendentes.

Se houver restauração pendente, retome com o mesmo arquivo antes de trocar firmware. Durante transferências, siga os avisos da página.

## Validação

Passaram os testes nativos e com sanitizers para ciclo condicional, oito hits, pads inferiores sem ação, pressão/soltura através de trocas de parte, inatividade com ANALOG e ausência de alterações nos formatos salvos. O teste WASM percorre os botões reais de PLAY/REC, grava os oito instrumentos, observa pulsos brancos da sequência sem pad pressionado e reinicia a sessão após salvar pattern.

Regressões de áudio: PCM ANALOG e DRUM-X idênticos às referências da base; pool de áudio inalterado. Passaram também SPLIT/LAYER, sustain, transposição, cenas, 16 patterns, oitavas, telas de save e a execução do AudioWorklet. O cliente DP1 foi testado contra o protocolo C real com flash simulada, arquivo exportado com firmware 19616, restauração e retomada.

Transições B2 → DP1 e DP1 → B2 verificadas com banco byte a byte e interrupções simuladas. Testes de CRCs, limites de boot/banco/journal e pins do instalador passaram. Isto não comprova USB físico, interrupção física, CPU do chip ou brilho percebido da DP1.

Recursos: aplicativo 161324/327680 B; RAM estática 82896/98304 B (+16 B frente à B2); pool 325984/344064 B, inalterado. O código de síntese, efeitos, estrutura dos dados e loader continuam sem alterações.

## Primeiro teste no teclado

1. Exporte e guarde seu banco pela ferramenta compatível; instale DP1 e confira a identidade 19616.
2. Na parte ANALOG, percorra os três bancos. Selecione uma parte DRUM-X e entre explicitamente no quarto banco.
3. Toque os oito instrumentos; confira violeta em repouso, branco nos hits e a linha inferior apagada.
4. Grave uma sequência com REC, encerre a gravação e confira as luzes durante a reprodução. PLAY/REC não devem trocar o banco.
5. Com um pad pressionado, selecione outra parte e solte-o. Em ANALOG, o banco deve continuar selecionado e inativo; ao voltar à parte DRUM-X ele reativa.
6. Confira seus presets/favoritos/patterns existentes, exporte uma cópia DP1 e teste a recuperação se desejar. Confirme o brilho e o uso musical antes de promover esta versão.

## Reprodução

```sh
python3 tools/build.py --voice-hardware --board smk37 --model pro --release 3.0.1-dp1 --experimental-id 19616 --sdk /caminho/SDK
sh tools/build_voice_sim.sh
node tests/drum_pads_wasm_test.mjs
node tests/drum_pads_installer_test.mjs
```

Créditos e licenças preservados. OberVave / zvenson e colaboradores; plataforma Felucca/Dinghy, port SMK37, JIANT/DRUM-X. Nenhum sample foi adicionado.

