# SMKSYNTH

**Sintetizador de performance para o M-VAVE SMK-37 Pro.**

[English](README.md) · [Instalação](docs/INSTALLATION.md) · [Backup](docs/BACKUP.md) · [Compilação](docs/BUILD.md) · [Créditos](CREDITS.md)

Do ponto de vista sonoro, o SMKSYNTH é a união do excelente sintetizador [OberVave](https://github.com/zvenson/obervave) com a engine DRUM-X, do também excelente [JIANT](https://github.com/juanjiant-bit/JIANT).

Mas um instrumento vai além do som. A ideia deste projeto é transformar o M-VAVE SMK-37 em um instrumento voltado à performance, criando recursos e reimaginando uma experiência que aproveita os pads e controles do teclado.

Alterne facilmente entre partes, salve timbres favoritos e acesse-os com um toque, grave sequências, crie layers e splits e salve cenas para recuperar suas configurações durante a performance.

Os pads são o coração do SMKSYNTH. Neste projeto, busquei implementar minha visão de um instrumento ideal para performance nesse formato compacto de 37 teclas.

> **Material para revisão, ainda sem lançamento público.** A base é a B2, identidade `SMK-37 Pro_19615`, com exportação e restauração confirmadas no teclado físico de Marcus. A seleção de fábrica está em preparação. O instalador incluído atualiza instalações compatíveis do SMKSYNTH; ainda não faz a primeira instalação a partir do firmware de fábrica.

## Guias dos pad banks

[Performance e favoritos](docs/pad-banks/01-performance-favoritos.png) · [SPLIT / LAYER](docs/pad-banks/02-split-layer.png) · [Cenas e patterns](docs/pad-banks/03-cenas-patterns.png)

PNGs em alta resolução e fontes SVG editáveis em `docs/pad-banks/`.

## Recursos

| Recurso | Base atual |
| --- | --- |
| Partes | Quatro: A–D |
| ANALOG | Oito vozes melódicas compartilhadas; dois osciladores, filtros, envelopes, LFO e arpejador |
| DRUM-X | Oito instrumentos de bateria sintetizados, cinco kits, sem samples |
| Sequenciador | Quatro tracks, até 256 steps por track, automação e trocas sincronizadas |
| Patterns | 16 posições com timbres, mixer e cenas |
| Presets e favoritos | 99 posições de programa e oito favoritos; salvamento manual |
| Teclado | Oitava por parte; SINGLE, SPLIT e LAYER; duas configurações salvas de teclado |
| Efeitos | Sends de chorus, delay e reverb, drive e processamento master |
| Segurança do banco | Exportação e restauração integral por USB-MIDI, em arquivos `.smksynth` |

BT salva presets. Patterns são salvos pelos pads, com confirmação de sobrescrita. PLAY e REC mantêm o banco de pads selecionado. Consulte o [guia rápido](docs/QUICKSTART.md).

## Tudo em um site

A pasta `site/` reúne instalador, backup/restaurador e simulador, pronta para hospedagem estática. Não depende de uma conta ChatGPT quando hospedada publicamente. A publicação será manual, após aprovação.

O simulador usa a base musical P16.2 aprovada. Ele não instala firmware nem reproduz o protocolo USB da B2. Sua sessão de navegador não é intercambiável com o backup físico `.smksynth`.

## Distribuição

A build validada B2 permanece inalterada. O pacote contém fontes, ferramentas, documentação e registros de testes. A primeira instalação, o banco de fábrica e a numeração da release pública ainda precisam ser fechados. Leia [a proposta de publicação](publication/REVIEW.pt-BR.md).

## Autoria e licença

Direção do projeto e workflow de performance: **Marcus Padrini** (crédito proposto para revisão). Autores originais e colaboradores: [CREDITS.md](CREDITS.md). Código sob GPL-3.0-only, com componentes identificados separadamente em [LICENSING.md](LICENSING.md). O texto integral está em [LICENSE](LICENSE). Este pacote não contém samples de Mellotron nem material do SMKTRON. Projeto independente, sem vínculo oficial com a M-VAVE.

FADER MODE JUMP/CATCH: GLOBAL. [Changelog](CHANGELOG.md).
