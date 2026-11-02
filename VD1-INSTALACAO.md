# Obervave Voice + Drums VD1 — teste no SMK37 Pro

Versão experimental `3.0.1-vd1`, identidade `SMK-37 Pro_19601`.
Base: Player modificado PF12, preservado em `pf12-preserved`, commit
`21fddca9039eb1cdacedc0c8cc49d1ab5dd9dd23`. Sequenciador aprovado no simulador:
Checkpoint 4 `cb5ca435c9e5b51d49055aa37f41f32be9c3b313`.
PF9 é a última referência com aprovação física registrada; PF12 não recebeu
essa aprovação. Esta VD1 inicia uma nova rodada de validação no teclado.

## Instalar

1. Abra https://obervave-voice-lab.padrini.chatgpt.site/keyboard/ no Chrome ou Edge
   de um computador. Conecte o SMK37 Pro por USB de dados e feche DAWs, editores
   MIDI e outras abas de instalação.
2. A página verifica automaticamente os três arquivos por tamanho, identidade,
   SHA-256 e CRC. Clique **Conectar e consultar** e permita o acesso MIDI/SysEx.
3. Confira o SMK37 Pro e clique **Instalar Voice + Drums VD1**. A janela seguinte
   mostra origem e destino; confirme a gravação.
4. Mantenha alimentação, cabo e aba abertos até a confirmação da identidade
   `SMK-37 Pro_19601`. Confira `3.0.1-vd1` na inicialização. A primeira partida
   pode demorar alguns segundos para copiar e verificar os dados.

Nenhuma conexão MIDI ou gravação ocorre ao abrir a página. O instalador recusa
outros modelos e identidades não reconhecidas. Se houver falha, baixe o log.
O arquivo .fwsc isolado está no pacote, mas esta página é o caminho recomendado:
ela também reconhece a nova identidade ao retornar.

## Dados e salvamento

O banco original do Player permanece nos endereços originais, fora da janela
escrita pelo loader. A VD1 bloqueia gravações nesses endereços e usa um banco
privado em `0x60000..0x87FFF`, fora dos arquivos cujo CRC é verificado no boot.
Na primeira partida, copia configurações, programas, favoritos e oito patterns
quando encontra o identificador original Obervave. Uma família diferente não
é importada. A cópia é verificada e seu marcador de conclusão é gravado por
último; se interrompida, é refeita na próxima partida. Falha de cópia desabilita
salvamento para evitar uso de dados incompletos.

**Desligar e ligar conserva o banco VD1. Reinstalar ou atualizar o firmware
reinicia esse banco e copia novamente o banco original.** Os testes e edições
salvos na VD1 não aparecem automaticamente no Player ao retornar. Registre
configurações e sequências importantes antes de reinstalar. Ainda não há
exportação USB completa do banco experimental.

SAVE continua manual, sem autosave. Aguarde a conclusão do salvamento antes de
desligar. Programas e patterns antigos são lidos; o sequenciador DRUMS usa uma
extensão própria, sem apagar as notas melódicas do pattern.

## Tocar e testar ao longo do dia

A seleção de engine fica em **PART SETUP**, ao final do ciclo GLOBE:
GLOBAL → VOICE → OUTPUT → PART SETUP. K6 escolhe a parte; K1 escolhe engine e
K2 o preset/kit nessa página. PATCH retorna à performance.

Com uma parte DRUMS, SEQ abre a grade de oito instrumentos. K1 escolhe a lane,
K2 o step, K3 velocity e K4 ratchet. Os 16 pads alternam os steps da lane.
K5 ou setas mudam a página; K6 continua escolhendo parte. O próximo SEQ abre
DRUM+ (chance, nudge, on/off), seguido pelos ajustes de track e patterns.
Com REC, os oito pads superiores disparam os instrumentos para gravação.
Velocity, chance, ratchet e nudge são compartilhados pelos hits de cada step.
Closed/open hi-hat usam choke. Veja `DRUMS4-RELATORIO.md` para o workflow completo.

Comece com uma parte DRUMS e depois combine VOICE, ANALOG e efeitos. Teste a
mod wheel/morph, pitch bend, pads, bancos de faders, transporte, swing e troca
de patterns. Salve um pattern de teste, desligue e ligue para verificar a
persistência. Depois toque por períodos mais longos, elevando a carga aos poucos.
Não há limite de polifonia fisicamente certificado: ANALOG/VOICE compartilham
oito vozes e DRUMS dispõe de oito hits por parte. Isso é a configuração atual,
não uma comprovação de capacidade em carga máxima.

Em GLOBAL, `P…us` é o maior tempo do callback de renderização desde que ligou;
`!` indica ao menos um bloco acima do prazo (256 frames / 44,1 kHz ≈ 5805 µs).
INT/CLK/AUTO continuam indicando a sincronização. Esse indicador não mede todo
uso de CPU nem a latência física. Anote partes/engines/efeitos ativos, tempo,
passos para reproduzir, tempo até a falha e foto de GLOBAL se houver glitches.

## Retorno ao Player e recuperação

Use **a mesma página experimental**, reconecte e escolha **Retornar ao Player
PF12** (base preservada) ou **PF9** (última referência aprovada fisicamente).
Os arquivos são cópias idênticas dos pacotes originais; seus hashes constam em
`manifest.json`. Os instaladores estáveis antigos não reconhecem necessariamente
`_19601`. Não foram modificados ou substituídos.

Em atualização interrompida, consulte novamente. O loader conhecido
`ota-SMK-37 Pro_900` permite retomar a VD1 quando há registro local compatível,
ou transferir um dos pacotes Player de retorno. Não limpe os dados do navegador
no meio da atualização. Loaders desconhecidos são recusados.

Se o aplicativo não responder, tente ligar segurando **OCT−**: o resgate USB
existente apresenta `SMK-37 Pro_000`. Consulte o teclado nesta página e selecione
a instalação ou o retorno. Não repita gravações se a identidade for inesperada:
conserve o log para diagnóstico. O resgate depende de o código de boot/aplicativo
continuar executável; não é uma garantia para qualquer falha elétrica ou flash.

Instalação, retorno e interrupções passaram por simulação automatizada. USB
físico, resgate, qualidade sonora, latência e uso prolongado ainda precisam ser
confirmados no SMK37 real.

## Código e licenças

`source.zip` contém o código correspondente, build e testes. O pacote completo
inclui também os fontes originais PF12/PF9 usados nos retornos. Obervave,
camada SMK37 e adaptações JIANT são GPL-3.0-only; créditos em `LICENSING.md` e
avisos nos arquivos. Componentes embarcados JieLi vêm do SDK AC79, Apache-2.0;
fontes e demais componentes mantêm suas licenças. Não há samples de terceiros.
Não redistribua somente o binário sem o código correspondente e as licenças.
