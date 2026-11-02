> Para o checkpoint 2, consulte **VOICE2-RELATORIO.md**. O script abaixo agora inclui testes VOICE/WASM e exige Node.js; a comparação de áudio foi corrigida para rejeitar silêncio. As limitações de hardware continuam.

# Reproduzir o checkpoint

Requisitos: Linux com compilador C, Python 3, Git e utilitários comuns.
No repositório recuperado do bundle:

```sh
git switch obervave-voice
sh tests/run_voice_foundation_tests.sh
```

O script compara o áudio e a matriz dos encoders com `pf12-preserved`.
Não grava flash, não opera MIDI e não instala firmware. Os logs ficam em
`validation/`. Nenhuma engine JIANT está incluída neste checkpoint.

Se estiver usando `source.zip` sem Git, os testes funcionais podem ser compilados
com o comando abaixo; a comparação automática com a branch preservada exige o bundle.

```sh
mkdir -p build/host
cc -O1 -DOBV_HOST -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen \
  -o build/host/multiengine_test tests/multiengine_test.c
build/host/multiengine_test
```

Verificação adicional de endereços, sem LeakSanitizer:

```sh
cc -O1 -g -fsanitize=address -fno-omit-frame-pointer -DOBV_HOST \
  -DBOARD_SMK37=1 -Ifirmware/src -Ifirmware/gen \
  -o build/host/multiengine-asan tests/multiengine_test.c
ASAN_OPTIONS=detect_leaks=0 build/host/multiengine-asan
```

Compilação WASM verificada com Zig 0.13.0, usando `web/sim/sim.c` e
`-DBOARD_SMK37=1`. Validação do módulo pela implementação WebAssembly do Node.
O checkpoint não entrega uma interface de simulador de três engines.

Instalação/recuperação experimental: **não aplicável ainda**. O empacotador está
bloqueado, pois a persistência e o retorno sem perda precisam ser implementados.
Não utilizar os binários legados do snapshot como se fossem builds de Voice.
