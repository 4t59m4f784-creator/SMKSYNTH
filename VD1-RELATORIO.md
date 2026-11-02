# VD1 — primeiro pacote experimental para o teclado

## Identificação e preservação

Hardware: M‑VAVE SMK37 Pro, firmware `3.0.1-vd1`, identidade
`SMK-37 Pro_19601`. Branch `obervave-voice`. Base modificada PF12
`21fddca9039eb1cdacedc0c8cc49d1ab5dd9dd23`; Checkpoint 4 do sequenciador
`cb5ca435c9e5b51d49055aa37f41f32be9c3b313`, preservado também na branch
`checkpoint4-simulator-preserved`. Nenhum firmware foi gravado remotamente.
O Site, instalador, pacotes e branch estáveis não foram modificados.

Esta build usa a mesma síntese e o sequenciador do Checkpoint 4. As diferenças
para o alvo são banco privado com cópia inicial, empacotamento limitado a um
arquivo app de 320 KiB, identidade própria e diagnóstico de pico de renderização.
O build padrão continua bloqueado: exige `--voice-hardware` explícito, modelo
Pro, versão com sufixo e identidade experimental.

## Medidas da compilação real para AC791N

| Recurso | Usado | Limite | Livre |
| --- | ---: | ---: | ---: |
| Imagem do aplicativo | 173412 B | 327680 B | 154268 B |
| RAM principal (.data + .bss) | 74048 B | 98304 B | 24256 B |
| Pool de buffers | 325900 B | 344064 B | 18164 B |
| Loader em RAM (imagem) | 8476 B | conforme linker próprio | — |
| Loader no pacote | 7211 B | conforme empacotador | — |
| Pacote .fwsc completo | 651327 B | formato SMK37, 20 blocos | — |

Valores extraídos dos símbolos e arquivos da compilação com o toolchain JieLi
pi32v2 20260730.1, clang 4.0.1. O verificador confirmou entrada, limites, 980
instruções da seção de flash em RAM sem chamadas e MMIO restrito ao HAL.
RAM livre é espaço de link, não medida de stack em uso. Não é medição de CPU.

Polifonia configurada: ANALOG/VOICE compartilham oito vozes; DRUMS tem oito
hits por parte, um hit retriggerável por instrumento e choke CH/OH. **Polifonia
segura sob carga física ainda não foi determinada.** O pico de renderização
GLOBAL é medido em execução no dispositivo e precisa ser observado pelo usuário.
Prazo atual: 256 / 44100 s, aproximadamente 5805 µs por bloco. O diagnóstico
não cobre toda CPU, latência MIDI ou interrupções externas ao callback.

## Isolamento e persistência

Banco privado: `0x60000..0x87FFF`. Configuração `0x60000`, programas
`0x61000..0x62FFF`, favoritos `0x63000..0x64FFF`, marcador `0x65000`,
oito patterns de quatro setores em `0x68000..0x87FFF`. Banco original permanece
em `0x9D000..`, com slots em `0xC2000..0xE1FFF`; todos esses endereços de dados
originais são recusados pelo guard de escrita VD1. OTA mantém staging próprio.

A cópia inicial é feita por SPI bruto, antes do áudio, com readback e watchdog;
não usa o conteúdo XIP descriptografado. Marcador `SMK37DATAOBVVOX1` escrito por
último. Só importa a família original `SMK37DATAOBERVAV`. O buffer temporário
ocupa 4096 B do pool. Falha impede salvamentos e permite tentar novamente.

Os arquivos declarados no boot acabam abaixo de `0x60000`: app termina em
`0x54120`, cfg_tool em `0x5429F`, seguido por cfg/EQ. O teste independente verifica
CRCs dos arquivos e diretórios e que gravações privadas não os invalidam. A tabela
stock de regiões e flash.bin de `0x9D000` bytes permanecem iguais ao formato v016.

**Reiniciar conserva saves VD1; reinstalar ou atualizar o firmware apaga o banco
experimental**, pois ele ocupa espaço reservado dentro da área regravada pelo
loader. A próxima partida copia novamente o banco original. Não existe migração
automática VD1 → Player nem exportação USB completa nesta release. Os formatos
legados e DRS1 permanecem conforme `DRUMS4-RELATORIO.md`.

## Verificação executada

- Compilação real pi32v2 e verificação de pacote, identidade, CRCs e regiões.
- Banco privado em NOR simulado: cópia/readback, segunda partida sem escrita,
  76 pontos de interrupção com retry, família estrangeira e guard de todos os
  setores/endereço com wrap e cruzamento de limites. Dados fora do banco intactos.
- Loader C real em NOR/USB simulado: PF12 → VD1 e VD1 → PF12/PF9. Áreas originais
  de configuração/programas/favoritos/patterns adicionadas às regiões protegidas
  do teste e verificadas byte a byte. Corrupção, falha de transporte e rejeição
  de chip desconhecido mantêm as verificações existentes.
- Protocolo OTA C: staging, record, CRC, limpeza e falhas simuladas.
- Instalador JS: três arquivos completos fixados por SHA, corrupção/truncamento,
  identidades/modelos, troca de identidade, instalação/retorno/retomada e registro
  de operação. Transporte Updater substituído por stub nesses testes.
- Controller real em DOM simulado: fetch/validação, nenhum MIDI ao abrir,
  bloqueio de botões, diálogo/cancelamento/confirmação, lock e identidade final.
- Suítes VOICE, DRUMS e grid: áudio ANALOG idêntico à PF12, VOICE preservada,
  groove DRUMS preservado, presets/controles/compatibilidade e WASM/worklet.
- Navegador: página e botões renderizados e bloqueados antes da conexão. A prévia
  HTTP não oferece crypto.subtle e corretamente bloqueou os pacotes; validação
  SHA real passou no Node. A publicação HTTPS privada depende da sessão do dono.
  Não houve teste de USB nem audição no navegador para esta release.

**Não executado fisicamente:** flash, boot, cópia no chip real, leitura de controles,
latência, áudio, CPU, stress de efeitos, resgate e uso prolongado. Compilar e testar
no simulador não valida esses itens. Esta entrega é para iniciar essa avaliação.
As limitações herdadas de sanitizer e validação anteriores continuam documentadas
nos relatórios dos checkpoints; não se declara a aplicação inteira limpa em UBSan.

## Reproduzir

Com o SDK AC79 e o toolchain nas versões/hashes exigidos pelo script:

```sh
JIELI_TOOLCHAIN=/caminho/jieli-linux-toolchains-20260730.1 \
AC79_SDK=/caminho/fw-AC79_AIoT_SDK \
python3 tools/build.py --voice-hardware --board smk37 --model pro \
  --release 3.0.1-vd1 --experimental-id 19601
sh tests/run_voice_foundation_tests.sh
sh tests/run_drums_tests.sh
sh tests/run_drums_grid_tests.sh
sh tests/run_vd1_tests.sh
```

O bundle de Git inclui refs preservadas exigidas pelas regressões. Os fontes
isolados também compilam o firmware, mas as comparações históricas pedem essas
refs. `validation/vd1-*.log`, manifest e símbolos/mapas acompanham o pacote.
Instalação e recuperação: `VD1-INSTALACAO.md`. Licenças e créditos:
`LICENSING.md`, `LICENSE`, avisos JIANT e licença Apache dos componentes SDK.
