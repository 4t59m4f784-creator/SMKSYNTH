> Histórico do checkpoint 1. Estado atual e correção da comparação de áudio em **VOICE2-RELATORIO.md**. O teste antigo com hash `76ed5c9c…` comparou silêncio e foi invalidado.

# Obervave Voice + Drums — auditoria e checkpoint 1

Data: 30/10/2026 (America/Sao_Paulo). Projeto experimental; não é uma release.

## Base exata e isolamento

A base de trabalho é **nosso OberVave 3.0.1-pf12 para SMK-37 Pro**, derivado da PF11,
recuperado do pacote `OberVave-PF12-SMK37-Sends-Knobs.zip`. Não foi utilizado
o repositório público como substituto da versão modificada.

| Identificador | Valor |
| --- | --- |
| SHA-256 do pacote PF12 | `eeed92deec36c4859b3634aac9662a2770a2b9d286951dac8f5ac80550961201` |
| Identidade do firmware PF12 existente | `SMK-37 Pro_19552` |
| SHA-256 do firmware PF12 existente | `934a048261d349d1995a980e03e48290937fd2d7b104389713fa359230a190cd` |
| Tamanho do firmware PF12 existente | 651.327 bytes |
| Origem dos fontes | `firmware-source.zip`, contido no pacote PF12 |
| Snapshot Git preservado | branch `pf12-preserved`, commit `21fddca` |
| Desenvolvimento | branch local `obervave-voice` |
| Última aprovação física recuperada | PF9, `_19549` |
| Situação física da PF12 | o próprio pacote informa que aguarda validação no teclado |

O commit `21fddca` é um **novo snapshot local dos fontes recuperados**, não um SHA
atribuído ao upstream. O histórico Git anterior não estava no ZIP. Nenhum repositório
Obervave foi localizado nos repositórios acessíveis pela conexão GitHub; somente os
repositórios dos sites do usuário apareceram. Não foram usados nem modificados.
O bundle Git entregue contém as duas branches e permite recuperar o trabalho.

Instaladores e simuladores existentes não foram alterados nem publicados. URLs de
referência recuperadas, sem nova publicação ou teste de instalação nesta auditoria:

- PF12 instalador: https://obervave-smk37-pf1.padrini.chatgpt.site/pf12/
- PF12 simulador: https://obervave-smk37-simulator.padrini.chatgpt.site/pf12/
- PF9 instalador: https://obervave-smk37-pf1.padrini.chatgpt.site/pf9/

## O que foi preservado na base

Quatro tracks, oito cards melódicos compartilhados, 99 programas de usuário,
oito patterns, quatro tracks de 256 steps, sequenciamento, ARP, favoritos, cenas,
mixer, sends chorus/delay/reverb/drive e efeitos existentes. Salvamentos manuais.
Os mapeamentos PF12 permanecem: K1–K4 contextuais; K5 tempo na performance e
navegação contextual; K6 parte; K7 preset/destino; K8 master. Banco B:
F1–F3 sends da parte com pickup; F4 master. Faders A preservados.

PF12 já lê favoritos PFV1 e grava PFV2 apenas por comando manual. PF11 e versões
anteriores não leem PFV2. Isso é uma limitação herdada, não uma migração criada aqui.

## Arquitetura proposta

1. Cada track possui uma escolha ANALOG / VOICE / DRUMS, independente do mixer.
2. ANALOG e VOICE usam o alocador melódico compartilhado. Cada card registra sua
   engine ao nascer; uma troca de engine nunca reinterpreta o estado antigo.
3. DRUMS usa slots de hits independentes e só processa os hits ainda ativos.
   Quantidade e eventual roubo de hits dependerão das medições físicas.
4. Todas geram sinal estéreo no bus existente da track, antes de volume, pan,
   mute/solo e dos quatro sends. Os retornos globais continuam onde estão.
5. A escolha passa pela fila de eventos e é consumida pelo áudio. A troca faz
   fade-out de 256 samples, libera o estado da track e faz fade-in de 256 samples.
   Não é uma medição de latência física. Notas recebidas durante o fade-out são
   descartadas; teclas já sustentadas não são transferidas para a engine nova.
   O relógio do sequenciador continua; sua propriedade das notas antigas é limpa.
   Caudas nos efeitos compartilhados podem terminar normalmente.
6. Páginas contextuais futuras usam a paleta/tipografia e os controles PF12.
   VOICE: espectro de formants. DRUMS: lanes, steps e hits. Sem faces/personagens.

## Fundação implementada neste checkpoint

- IDs e registro de engines, incluindo recusa explícita de engines indisponíveis.
- Estado independente por track e engine registrada por card melódico.
- Pedido de troca na fila existente; transição e limpeza pertencem ao áudio.
- Hooks para geração melódica, hits independentes, renderização de hits e reset.
- Roteamento central usado por teclas, ARP e sequenciador.
- Reset de engine percussiva pelo panic; proteção contra fila cheia.
- Nenhuma alteração nos formatos persistentes, knobs, pads ou páginas.

**Apenas ANALOG está implementada no produto neste checkpoint.** VOICE/DRUMS
não aparecem como opções silenciosas. Os adaptadores usados no teste verificam
o despacho; não são síntese vocal/percussiva nem áudio JIANT portado.
Portanto, a Fase 1 ainda precisa do seletor real e da configuração persistente.

## Referência JIANT e dependências

Referência fixada, consultada pelo GitHub:
https://github.com/juanjiant-bit/JIANT/commit/e924445ae0e7c64636f15bd0758418ee312ce7ee

- `firmware/src/eng_formant.c`, blob `0640822b0c9ef04bb09bfc10ac35658e2ed2e4e9`.
  Fonte glotal, BUZZ/BREATH e quatro ressonadores em cascata, ponto fixo.
  Portar apenas vogais/morph/TALK/SHIFT/Q e auxiliares necessários.
  Excluir caminhos WORDS, `sing_*`, dicionários, letras e UI do JIANT.
- `firmware/src/drumx_voice.c`: oito instrumentos já correspondem ao escopo;
  oscilador, envelope de pitch, ruído filtrado, FM, choke e kits.
  A referência atual inclui camadas e outras funções além do mínimo: a seleção
  do subconjunto e o custo ainda precisam ser avaliados; não importar tudo.
- Adaptar tipos de voice/track, gerador de ruído, seno/BLEP, tabelas de envelope,
  escalas de pitch/amplitude e tamanho do bloco. Não substituir tabelas sem
  comparação sonora. O Obervave usa 44.100 Hz; conferir todos os coeficientes.
- Compilação física: toolchain JieLi pi32v2 e SDK AC79 compatíveis com a base.
  Host: compilador C, Python/Pillow. WASM: Zig e Binaryen.

## Recursos: evidência e riscos

Os números abaixo são do **log de build da PF12 recuperado**, não de uma nova
medição física nem do futuro firmware com três engines:

| Região PF12 | Uso | Limite informado pelo build | Saldo aritmético |
| --- | ---: | ---: | ---: |
| Imagem de aplicação | 151.504 B | 619.520 B | 468.016 B |
| RAM .data + .bss | 69.008 B | 98.304 B | 29.296 B |
| Pool | 318.708 B | 344.064 B | 25.356 B |

RAM e pool são regiões diferentes; não somar saldos como se fossem livremente
intercambiáveis. Código livre em flash não comprova CPU ou RAM suficientes.
O estado básico novo mede 40 bytes no host (quatro estados + oito IDs de card),
excluindo descritores/ponteiros e futuras engines. Não é medição de RAM JieLi.
CPU, polifonia mista, latência e folga sob todos os efeitos: **não medidos**.

Estruturas medidas no host: patch 80 B; pattern 13.416 B; sons/mix do pattern
380 B; settings 67 B. O payload atual ocupa 13.796 de 16.368 B por slot.
Os steps atuais têm quatro notas, velocity/gate/ratchet/chance/nudge compartilhados.
Não comportam oito hits simultâneos como oito notas comuns.

Proposta a verificar na Fase 4: formato versionado, com payload tipado por track;
ANALOG/VOICE preservam os atributos melódicos, DRUMS usa oito velocities por step
e atributos rítmicos. Primeiro medir tamanho com quatro tracks DRUMS/256 steps,
incluindo kits e parâmetros; não prometer encaixe antes do teste.
Dados legados devem ser lidos como ANALOG, sem escrita automática de migração.

Risco crítico: `smk_owner_check()` apaga settings/programas/patterns/favoritos
quando muda a família. Trocar `OBV_DATA_FAMILY` por um nome novo **não isola os
dados no teclado**. Manter a família com um formato incompatível também não é
seguro para downgrade. Antes de instalar: definir armazenamento experimental,
importação legada e backup/retorno verificáveis. Nenhuma migração física foi feita.

## Checkpoints seguintes e critérios de passagem

| Etapa | Resultado testável | Situação |
| --- | --- | --- |
| 0 — Auditoria | fonte exata, referência preservada, branch, licenças e mapa | concluída para os arquivos disponíveis; PF12 física pendente |
| 1a — Fundação | despacho por track e ANALOG idêntica à PF12 | concluída no host; compilação WASM verificada |
| 1b — Configuração | seletor contextual e configuração versionada sem destruir dados | pendente |
| 2 — VOICE | vogais/morph/TALK/SHIFT/BUZZ/BREATH/Q, ADSR/glide/mod wheel, presets | pendente |
| 3 — DRUMS | kit sintetizado, oito notas MIDI, velocity/choke e sends | pendente |
| 4 — Sequência | oito lanes, hits simultâneos, timing, pads/REC, armazenamento e migração | pendente |
| 5 — Interface | visual unificado; revisão de todos os contextos e displays | pendente |
| 6 — Otimização | matriz de cargas, CPU física, RAM/flash, polifonia segura | pendente |
| 7 — RC | simulador consistente, firmware isolado, instalação/recuperação, fontes | pendente |

## Testes executados e limites

- Saída ANALOG comparada byte a byte com PF12: 1.500 blocos de 256 frames,
  384.000 frames estéreo / aproximadamente 8,7 s. Quatro tracks, eventos de notas,
  sequenciador, pitch bend, mod wheel e sends. SHA-256 dos dois fluxos:
  `76ed5c9c326ba62322d464a22d3fcf7b76c7352c6d33badc36ccea7a33325e97`.
  Evidência limitada a esse cenário, não prova universal de equivalência.
- Regressões host: contatos/latência sintética, performance, flash guards,
  44 checks PF12, 2.759 checks de roda/volume, targeting das quatro partes,
  idle wheels PF9, 36 estados de LEDs, cenas e 396 headers.
- Matriz dos encoders comparada com a base PF12; logs incluídos.
- Teste novo: recusa de engine ausente, troca no ISR, fades, isolamento,
  despacho melódico/percussivo, sequenciador, panic e fila cheia, zero flash writes.
- AddressSanitizer: teste novo executado com detecção de leaks desativada;
  LeakSanitizer não opera neste ambiente. Não é teste de vazamento de memória.
- Compilação C para WebAssembly e validação do módulo: executadas.
  Isso não equivale a operar a interface no navegador; simulador de três engines
  e teste físico **não realizados**. Nenhum resultado de áudio JIANT validado.

O empacotador de firmware está bloqueado neste checkpoint e o workflow de release
foi desativado somente na branch experimental. Não há novo `.fwsc` nem instalador.
Binários antigos eventualmente presentes no snapshot são os arquivos herdados,
não uma build de Obervave Voice. O ZIP PF12 original é entregue separado deles.

## Retomada e licenciamento

Para recuperar o Git: `git clone obervave-voice.bundle obervave-voice`, depois
`git switch obervave-voice`. A branch `pf12-preserved` contém o snapshot inicial.
O arquivo `source.zip` oferece o código sem exigir Git. Leia `VOICE-TESTES.md`.

GPL-3.0-only confirmada em Obervave e JIANT; avisos e copyright preservados.
Créditos: zvenson, Leo Kuroshita/Hügelton Instruments, M. Geisler/genkigenki,
isod89/SLOOP e demais componentes listados em `LICENSING.md`; para o futuro porte
JIANT, Juan Ignacio Antonucci (Juan Jiant) e os titulares dos cabeçalhos.
As licenças de fontes e componentes permanecem no snapshot. Nenhum sample novo
foi incluído. Ao distribuir binários futuros, acompanhar fontes correspondentes,
scripts e avisos. Esta entrega ainda não contém código JIANT integrado.
