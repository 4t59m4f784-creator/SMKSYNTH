# CT1 — investigação e validação dos controles físicos

Base: `13a8edafe4e18d4f182872ab0a866a687e2b1428` (SMKSYNTH 1.0.0 + entrada MIDI do simulador). Candidato: 1.0.1-ct1. Validação física pendente.

## Achados

1. **Decoder de encoder:** a sequência de estados estabilizados `0,1,3,2,1,3,2,0` contém uma transição impossível de dois bits (`2→1`). O decoder anterior emite um passo negativo; o novo descarta o ciclo parcial e emite zero. Rotação Gray válida, reversões e bounce foram testados em todas as fases. A exigência anterior de dois frames estáveis foi mantida; não há temporizador extra de bloqueio. O acumulador relativo agora satura em vez de inverter sinal por overflow.
2. **Propriedade de eventos:** K5/K6/K7 e botões podem mudar o destino antes do consumo dos deltas K1–K4. Há proteção no mesmo lote e entre iterações, inclusive após troca de pattern pelo ISR. O delta pendente do contexto anterior é descartado; o próximo detent continua relativo ao valor atual.
3. **Faders:** antes, apenas os três sends do banco B tinham pickup. Agora JUMP/CATCH cobre os quatro faders em ambos os bancos. A mudança de destino, página/layer, parte, engine, banco, recall (inclusive de mesmo valor), pattern, valor externo ou master por K8 rearma o CATCH. Uma posição anterior a uma troca não pode capturar falsamente o novo destino. Escritas do próprio fader mantêm a captura; operações modais de salvamento não recebem alterações dos faders.
4. **Attack:** nenhum dos destinos fixos dos faders é Attack. O mapa físico e lógico dos oito encoders permanece igual. Attack pode mudar legitimamente por encoder na página de envelope, MIDI CC, recall ou automação. `seq_auto` escreve os valores gravados em `live_p`, inclusive Attack quando essa lane existe; essa função foi preservada. Nos cinco patterns de fábrica, a única lane preenchida encontrada foi VOLUME na parte A do pattern 3 (oito passos), sem Attack.
5. **ADC:** o caminho existente já tem mediana de três amostras, IIR e dead band de ±1 no valor de sete bits; não foram alterados o mux, a curva, os limiares, a frequência ou os filtros analógicos sem evidência física que justificasse isso.

Esses achados são mecanismos reproduzíveis no código. Sem uma captura dos eventos no momento do problema ou identificação do preset/pattern afetado, não estabelecem a causa específica da ocorrência relatada.

## Dados e salvamento

Nenhum crescimento ou mudança de versão de registros: globals `set7` 67 bytes, patch 80 bytes, favoritos PFV2 708 bytes; patterns e cenas preservados. O modo global usa bits altos previamente não usados de `click`, com marcador e os dois bits baixos intactos. Todos os leitores existentes usam `click % 4`, incluindo o aplicativo público 1.0.0. Não há conversão ou escrita automática no boot. O leitor antigo continua enxergando o mesmo click; se ele editar essa opção, pode descartar apenas a preferência nova de faders, que volta ao padrão CATCH na CT1.

O loader exige banco SMK16P1 reconhecido, preserva os dados e rejeita journal de restauração pendente/inválido antes de apagar o aplicativo. Há pacote de retorno com o aplicativo **byte a byte idêntico** ao público 1.0.0. A identidade USB do candidato permanece 19618; identifique a CT1 pelo nome/versão de boot e pelos hashes, não pela identidade USB.

## Validação

- Testes nativos com ASan/UBSan: decoder inválido, fases/direções/bounce, isolamento de Attack, ambos os modos, faders A/B/master, alterações por mixer/FX, contextos e dados salvos.
- Comparação do mapa de encoders: 782 cenários válidos idênticos à base; 65 cenários artificiais com página DRUM/VOICE em parte ANALOG passam a descartar o evento antigo ao corrigir a página. Novos cenários cobrem a página acrescentada.
- Regressões de sequenciador, oitavas, Split/Layer, salvamento/overwrites, 16 patterns, Drum Pads, recall de cenas e boot.
- WASM compilado do candidato: modo salvo após reboot; patterns preservam a preferência global; delta pendente descartado na troca de página; próximo detent relativo. Áudio dos cinco patterns de fábrica **PCM bit a bit igual** ao WASM público.
- Loader de ida e volta: 41 pontos de interrupção/retomada por direção, banco byte a byte preservado; ranges protegidos intactos, proprietário desconhecido e restauração pendente rejeitados.
- CRCs do pacote, limites de app/RAM/pool, hashes das fontes e artefatos e app original no retorno conferidos.
- Resposta elétrica, sensação dos encoders/faders e ocorrência real do Attack ainda exigem teste no instrumento. O teste WASM não exercita o scanner elétrico; o decoder foi exercitado separadamente em teste nativo.

Logs: [validation/controls-ct1](../validation/controls-ct1). Downloads e roteiro físico: [experimental/controls-ct1](../experimental/controls-ct1/README.md).

## Reproduzir

```sh
sh tests/run_physical_controls_tests.sh
JIELI_TOOLCHAIN=/caminho/toolchain python3 tools/build.py --voice-hardware \
  --board smk37 --model pro --release 1.0.1-ct1 --experimental-id 19618 --sdk /caminho/SDK
ZIG=/caminho/zig WASM_OPT=/caminho/wasm-opt sh tools/build_controls_sim.sh
python3 tools/package_controls_candidate.py
python3 tools/check_controls_candidate.py
node tests/physical_controls_wasm_test.mjs experimental/controls-ct1/SMKSYNTH-1.0.1-ct1.wasm
```

Toolchain validado: JieLi `20250324.1`; hashes do SDK iguais aos da release. WASM: Zig 0.13.0, Binaryen 123. Artefatos oficiais 1.0.0 e a instalação pública não são substituídos por este candidato.
