# Build reproduzível

Firmware: SMKSYNTH 1.0.3 / `SMK-37 Pro_19618`. Base musical/interface: SUPER SS3, commit `0b4ccdd4892dd6d6d5a1a4f206aa939aca46a853`. O código de síntese, sequenciador, interface e formatos dessa base foi preservado.

## Requisitos

Linux x86-64 (ou o fluxo Docker descrito pelos scripts originais), toolchain JieLi `pi32v2`/`common`, Python 3 e SDK AC79NN. Para simulador: Zig e Binaryen `wasm-opt`. As referências e licenças dos componentes são as de LICENSING.md. O SDK não é substituído por arquivos aleatórios; o build confere hashes dos componentes usados.

## Conteúdo de fábrica

`factory/` contém somente os cinco patterns selecionados, suas cinco cenas, os oito favoritos e configurações iniciais limpas. Não inclui o backup privado integral ou os três presets de usuário encontrados nele. Dois patterns mantêm o prefixo SEQ4 legado, lido pelo caminho de compatibilidade normal do firmware.

```sh
python3 tools/build_factory_bank.py factory factory
python3 tests/release_factory_test.py
```

Não é necessário ter o backup original para recompilar. `tools/prepare_factory_seed.c` documenta a normalização inicial: marca de edição das cenas/favoritos em relação aos presets da base, geração inicial dos favoritos e globals limpos com tempo de demonstração 90 BPM. Dados de notas, patches e mixer dos exemplos foram preservados.

## Firmware

```sh
export JIELI_TOOLCHAIN=/caminho/jieli-linux-toolchains
python3 tools/build.py --voice-hardware --board smk37 --model pro \
  --release 1.0.3 --identity 19618 --first-install \
  --factory-bank factory/bank-SMK16P1.bin --sdk /caminho/SDK
```

Saída: `build/obervave-1.0.3-smk37.fwsc`; nome de distribuição `SMKSYNTH-1.0.3-SMK37-Pro.fwsc`.

O perfil `--first-install` usa o loader completo padrão da plataforma para preparar a área `0x4000..0x9CFFF`, incluindo o seed SMK16P1. O app mantém o perfil de armazenamento SMK16P1. Esse pacote é somente para a instalação inicial a partir de M‑VAVE V016, conferida pelo cliente. Não é o loader de atualizações SUPER SS3 que preservava um banco existente.

O journal de restauração começa apagado em `0x55000..0x56FFF`; o seed ocupa `0x57000..0x9CFFF`. Nenhum boot-file CRC inclui essa área de dados. Cabeçalho, calibração e regiões protegidas do fabricante não são escritas. O fluxo de retomada do loader permanece o original, com comparação e verificação por setor.

## Simulador e distribuição web

```sh
sh tools/build_controls_sim.sh
cp build/controls/SMKSYNTH-1.0.3.wasm web/simulator/obervave-voice.wasm
node tests/release_demo_wasm_test.mjs
node tests/release_101_installer_test.mjs
```

O simulador usa `factory/simulator-session.bin`, uma representação dos mesmos registros no formato de memória do host, separada da flash física. Os sites estáticos são montados por `tools/package_release.py`. O pacote de fontes e os hashes fazem parte da entrega. Consulte VALIDATION.md para limites e comandos dos testes.

## Preserving update / Atualização com preservação

Use the same build command without `--first-install --factory-bank factory/bank-SMK16P1.bin` to build the bank-preserving update. The first-install and update packages are separate; do not interchange them. `obv_supersaw.c` is compiled at `-O2`; the main application and USB paths retain `-Os`. SUPER attribution: [Clipper](https://github.com/isod89/clipper-fm1); see LICENSING.md.
