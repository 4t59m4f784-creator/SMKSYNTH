# SMKSYNTH P16 — 16 patterns completos

Base: U2, commit d157a7b3d9c5b335628ddea322a3f476a08a7584. Ramo experimental smksynth-patterns16-p16. Firmware 3.0.1-p16, identidade SMK-37 Pro_19611. U2 e AD1 oficial preservadas.

## Uso

KNOB BANK percorre Performance/Favoritos → Split/Layer → Patterns. No banco Patterns, pads superiores 1–8 acessam patterns 1–8; inferiores 9–16 acessam patterns 9–16. Toque carrega, com a sincronização anterior quando o transporte está tocando. Segure 0,7 s para salvar/copiar. Slot ocupado: solte e dê um novo toque curto no MESMO pad em até 4 s; o aviso dedicado e o pad vermelho piscando indicam a confirmação. Sem confirmar, nada é gravado. As gravações efetivas aguardam STOP e notas soltas; aguarde a mensagem de conclusão antes de desligar.

Na lista PATTERNS do sequenciador, K1 percorre os slots 1–16; a tela muda automaticamente entre páginas 1/2 e 2/2. K3 carrega o slot indicado. Os pads continuam sendo o único caminho para salvar patterns. BT permanece exclusivo dos presets: curto salva o atual, longo escolhe novo destino K7, BT confirma, PATCH cancela.

Cada slot mantém o mesmo conteúdo completo: quatro tracks de até 256 steps, quatro notas por step, quatro lanes de automação por track, oito peças DRUM-X simultâneas por step, cenas, parte selecionada, as duas configurações de teclado e a linha ativa. Os parâmetros e DSPs não mudaram. SEQ TRANSPOSE, sustain, REC da parte selecionada e sends mantidos. Não há autosave.

## Instalação e recuperação

**Instalar ou reinstalar P16 reinicia o banco experimental de presets, patterns e favoritos. Ainda não há exportação dos dados que estão no SMK.** O ZIP de fonte/firmware e o botão de retorno não são backups dos seus sons. O banco original do Player continua preservado, e é usado para iniciar o novo banco quando possui a identificação reconhecida. Não prometemos importar o banco U2 durante a instalação.

Use o instalador P16 em Chrome/Edge, cabo USB de dados e feche DAWs/aplicativos MIDI. Consulte o teclado, escolha P16, leia a confirmação e mantenha alimentação/cabo até informar SMK-37 Pro_19611. Aguarde o primeiro boot. Saves manuais persistem entre desligamentos normais; a limitação acima é da instalação/reinstalação.

Para retornar, use esta mesma página P16 e escolha U2 ou AD1. Páginas antigas podem recusar a identidade nova. Se o aplicativo não responder, tente ligar segurando OCT− e consulte novamente nesta página. Retorno também reinstala o banco experimental. Instalação, resgate e áudio físico ainda precisam ser testados no dispositivo.

## Recursos medidos pelo linker e pacote

| Recurso | U2 | P16 |
|---|---:|---:|
| Código/imagem | 155196 B | 154416 B |
| Reserva do app | 327680 B | 327680 B |
| RAM .data + .bss | 79104 B | 82448 B |
| Limite .data + .bss | 98304 B | 98304 B |
| Pool de RAM | 325984 B | 325984 B |
| Limite do pool | 344064 B | 344064 B |
| Banco privado de flash | 163840 B | 286720 B |

Aumento de RAM estática: 3344 B. Oito vozes melódicas e todos os buffers de áudio/efeitos preservados. O mapa de flash usa apenas a região apagada fora de todos os arquivos declarados no pacote; o teste de CRC verifica isso. Esses números não são medição de CPU/latência nem validação física sob carga.

## Compatibilidade e backup futuro

O payload de pattern e cena não mudou. Leitores dos prefixos antigos continuam disponíveis; patterns antigos abrem em SINGLE quando não têm configuração de teclado, e parte A quando não têm foco salvo. O banco possui família/layout SMK16P1. Formato de proprietário desconhecido desabilita escrita, em vez de apagar silenciosamente. Veja STORAGE-COMPATIBILITY.md para os limites e a direção de evolução.

O simulador P16 usa armazenamento separado da U2. Sua importação aceita sessões antigas de 11 blocos e reposiciona os favoritos para o formato de 19 blocos, preservando os oito slots originais e deixando 9–16 vazios. O export/import de sessão do navegador já existente não exporta dados do teclado e não resolve a atualização física.

## Verificação

Testes nativos: 16 slots completos com comparação integral, sobrescrita isolada do slot16, legado limitado a oito, pad16 hold/toque/timeout, duas páginas 240×240; 108 interrupções na primeira inicialização e recuperação, formato futuro recusado sem escritas; regiões do Player e faixas protegidas intactas; regressões ANALOG/DRUM-X, split/layer, sustain, REC, transporte e transpose. Golden PCM de ANALOG e DRUM-X idênticos aos anteriores nos cenários testados.

Testes WASM: pads1–16 salvam/carregam, foco, confirmação/timeout, reinício de 19 blocos, migração 11→19; regressões de salvamento, split/layer, DRUM-X, transpose, navegação e AudioWorklet. Simulação do loader e validação do pacote/modelo/hash/CRCs; não houve escrita USB no instrumento. Capturas nativas de ambas as páginas incluídas.

**Pendente:** teste prolongado no SMK37, CPU/latência reais, corte de alimentação durante sobrescrita de um slot, transporte USB e compatibilidade audível em uso real. Slot usa o CRC/cabeçalho por último já existente; isso detecta escrita incompleta, mas não torna overwrite do mesmo slot transacional.

## Reproduzir builds e testes

Requer o toolchain JieLi e a revisão do SDK verificada por tools/build.py; respeitar as licenças desses componentes. Configurar JIELI_TOOLCHAIN e AC79_SDK antes do comando. Para WASM, instalar Zig e Binaryen e configurar ZIG e WASM_OPT.

```sh
python tools/build.py --voice-hardware --board smk37 --model pro --release 3.0.1-p16 --experimental-id 19611
sh tools/build_voice_sim.sh
sh tests/run_p16_tests.sh
sh tests/run_sl1_tests.sh
```

O nome histórico --voice-hardware seleciona o mapa experimental do SMK; não reativa a engine removida. Não usar o build genérico como substituto da P16. A fonte inclui os créditos e licenças anteriores.
