# SMKSYNTH

**1.2.0 oficial: ENDLESS + MIDI MODE.** Sequências com MIDI por parte, controlador USB/TRS com mapas editáveis no instrumento, tema Zoom e renomeação de presets. **Supersaw somente no OSC 1 para preservar a estabilidade.**

[Novidades da 1.2](docs/RELEASE-1.2.0.md)


**SMK‑37 Pro e SMK‑37 Elite como instrumentos de performance.**

[Site e instalação](https://4t59m4f784-creator.github.io/SMKSYNTH/) · [Simulador](https://4t59m4f784-creator.github.io/SMKSYNTH/simulator/) · [Backup e restauração](https://4t59m4f784-creator.github.io/SMKSYNTH/backup/) · [Instalação](docs/INSTALLATION.md) · [English](docs/README.en.md)

## Documentação

| Documento | Português | English |
|---|---|---|
| Quick start | [Ler](docs/QUICKSTART.md) · [PDF](docs/pdf/SMKSYNTH-1.2.0-Quick-Start-PT.pdf) | [Read](docs/QUICKSTART.en.md) · [PDF](docs/pdf/SMKSYNTH-1.2.0-Quick-Start-EN.pdf) |
| Manual completo | [Ler](docs/MANUAL.md) · [PDF](docs/pdf/SMKSYNTH-1.2.0-Manual-PT.pdf) | [Read](docs/MANUAL.en.md) · [PDF](docs/pdf/SMKSYNTH-1.2.0-Manual-EN.pdf) |

Mapas de pads, botões, knobs e faders; gravação, cenas, efeitos, MIDI, salvamento e backup. Diagramas conferidos no código da versão 1.2.0.

Do ponto de vista sonoro, o SMKSYNTH é o casamento do excelente sintetizador [OberVave](https://github.com/zvenson/obervave) com a engine DRUM-X do também excelente [JIANT](https://github.com/juanjiant-bit/JIANT).

Porém, um instrumento é muito mais que o som. A ideia aqui é transformar o M‑VAVE SMK‑37 Pro em um instrumento voltado para a performance, criando recursos e reimaginando uma experiência que usa os pads e controles do teclado.

Alterne entre partes, grave e acesse timbres favoritos com um toque, grave sequências, faça layers e splits, salve cenas e recupere seu conjunto com um botão. Os pads são o coração do SMKSYNTH: este projeto traduz minha visão de um instrumento para performance nesse formato compacto de 37 teclas.

— **Marcus Padrini** · direção do projeto, fluxo de performance, seleção de sons, patterns e testes no instrumento

## SUPER / Supersaw

Sete serras no OSC 1, com DETUNE e MIX. Supersaw disponível somente no OSC 1 para preservar a estabilidade; OSC 2 mantém as formas convencionais. Alocação de até cinco vozes quando SUPER está em uso, incluindo release, com substituição das caudas. A página SUPER reúne DETUNE / MIX. Presets, favoritos, cenas e backups guardam os ajustes. [Controles no manual](docs/MANUAL.md#super--supersaw).

A supersaw se originou no **[Clipper, de 3dSam](https://github.com/isod89/clipper-fm1)**, sobre Felucca, que referencia a análise de Adam Szabo. Adaptamos a curva e os offsets de desafinação, com MIX independente e otimizações próprias. Créditos e avisos originais preservados; não é uma reprodução exata do Roland JP-8000/JP-8080.

Áudio USB estéreo 16 bits, 44,1/48 kHz, testado no Mac e no iPhone com AudioShare. Gravação de vídeos USB no iPhone ainda pode apresentar cliques e cortes.

## MIDI TRS/DIN e USB

A parte A transmite no canal GLOBAL (1–13); B, C e D nos três seguintes. OMNI transmite em 1–4. Todas as partes transmitem notas, incluindo teclado, pads, split/layer, ARP e sequenciador. Pitch bend e CC1 seguem a parte selecionada; o pedal envia CC64 nos quatro canais. GLOBE + PLAY libera notas e sustain, interrompe PLAY/REC e envia Panic nos 16 canais. Não há transmissão de Clock/Start/Stop nem Program Change automático. [Manual MIDI e sustain](docs/MANUAL.md).

## Teclado MIDI no simulador

O simulador aceita entrada MIDI com velocity, sustain, pitch bend e modulação. Inicie o áudio, clique em **Conectar teclado MIDI** e escolha a entrada. Use um navegador com Web MIDI, como Chrome ou Edge no computador. [Como conectar e tocar](docs/SIMULATOR-MIDI.md).

## Demonstração dos recursos de performance

[Assista à breve demonstração do SMKSYNTH no YouTube](https://www.youtube.com/watch?v=Ax-RqjtUFjc).

## Desenvolvimento com IA

O SMKSYNTH foi desenvolvido com uso de IA. Marcus Padrini definiu os objetivos e o fluxo de performance, orientou o trabalho, selecionou os sons, criou os cinco patterns e realizou os testes e a aprovação no instrumento. O ChatGPT, da OpenAI, contribuiu com propostas e soluções técnicas e gerou e revisou código das adaptações, ferramentas web, documentação e materiais gráficos sob sua orientação. As engines e os componentes de origem pertencem aos autores creditados abaixo. O projeto é independente e não tem vínculo ou endosso da OpenAI.

![Tela real do sequenciador SMKSYNTH](docs/images/sequence.png)

## O que está incluído

| Recurso | SMKSYNTH 1.2.0 |
|---|---|
| Partes | Quatro, com seleção de ANALOG ou DRUM-X por parte |
| Synth ANALOG | OberVave; oito vozes compartilhadas; SUPER somente no OSC 1; até cinco vozes com SUPER |
| Bateria | DRUM-X; oito instrumentos por kit |
| Performance | Favoritos, sends por parte, split/layer e transpose de sequências pelas teclas |
| Sequenciador | Quatro tracks; até 256 steps por track; gravação ao vivo ou passo a passo |
| Patterns/cenas | 16 posições; sequência, sons, mixer e configurações de teclado por pattern |
| Conteúdo inicial | Cinco patterns de Marcus Padrini e oito favoritos |
| Presets | Banco da base mantido; nenhum preset de usuário extra instalado |
| Salvamento | Manual; backup/restauração pelo navegador |

Os sons editados das cenas e favoritos ficam nos próprios snapshots, sem depender de presets extras. Patterns 6–16 começam livres. O tempo inicial dos exemplos é 90 BPM.

## Pads persistentes

KNOB BANK escolhe o banco de pads. PLAY, REC, seleção de engine e mudança de página não tomam essa escolha automaticamente. SEQ acessa explicitamente os steps do sequenciador.

| Banco | Uso |
|---|---|
| Performance / Favoritos | Pads superiores: partes A–D, delay/reverb e −/+ preset. Inferiores: oito favoritos. Toque recupera; segure para memorizar. |
| Split / Layer | Duas configurações independentes; escolha partes, SINGLE/SPLIT/LAYER e a linha tocada. |
| Patterns / Save | Toque carrega um dos 16 patterns; segure para salvar. Posição ocupada pede confirmação. |
| Drum Pads | Entra no ciclo apenas com DRUM-X selecionado. Oito pads superiores tocam/gravam bateria com velocity; inferiores reservados. |

Drum Pads usam violeta em repouso e branco no toque ou nos hits da sequência. Trocar para ANALOG preserva esse banco, inativo. Selecionar uma voz de bateria, sozinho, **não** transforma os pads em bateria.

## Instalação e atualização

No [instalador público](https://4t59m4f784-creator.github.io/SMKSYNTH/install/), escolha **Atualizar** se já usa uma versão pública anterior ou experimental compatível até MO1a. O banco salvo é preservado. Exporte um backup antes e aguarde a identidade final `SMK-37 Pro_19649`.

Para **Instalar** pela primeira vez, use a V016 oficial do modelo conectado (`SMK-37 Pro_016` ou `SMK-37 Elite_016`). Essa opção inicializa o banco com cinco patterns e oito favoritos. O instalador seleciona automaticamente o pacote Pro ou Elite. Use Chrome/Edge no computador, cabo USB de dados e alimentação estável. Firmware oficial M-VAVE não é redistribuído aqui.

Transferência interrompida: retome pelo mesmo instalador, navegador e pacote. O instalador mantém links para retomar versões anteriores. [Guia completo de instalação e recuperação](docs/INSTALLATION.md).

## Fonte, validação e estado do lançamento


- [Validação e hashes](docs/VALIDATION.md)
- [Build a partir das fontes](docs/BUILD.md)
- [Mudanças](CHANGELOG.md)
- [Contribuir](CONTRIBUTING.md)
- [Guia de uso](docs/USER_GUIDE.md)

O simulador executa o mesmo código C de síntese, sequenciamento e interface em WebAssembly. Tem memória independente e começa com os mesmos cinco patterns e oito favoritos da instalação.

## Créditos e licença

OberVave e Dinghy: **zvenson**. JIANT / DRUM-X: **Juan Ignacio Antonucci (Juan Jiant)** e colaboradores. Felucca: **Leo Kuroshita / Hügelton Instruments**. SLOOP: **isod89** e colaboradores. Port/ferramentas SMK-37: **M. Geisler / genkigenki** e colaboradores. Direção e validação do SMKSYNTH: **Marcus Padrini**. Soluções técnicas, código das adaptações, ferramentas e documentação produzidos com **ChatGPT / OpenAI**, sob sua orientação.

Firmware e adaptações sob **GPL-3.0-only**. Consulte [LICENSE](LICENSE), [CREDITS](CREDITS.md) e [LICENSING](LICENSING.md) para atribuições e licenças de componentes. Não há samples de terceiros nesta distribuição. Projeto independente, sem vínculo ou endosso da M‑VAVE ou dos autores dos projetos de origem.

FADER MODE JUMP/CATCH: GLOBAL. [Changelog](CHANGELOG.md).
