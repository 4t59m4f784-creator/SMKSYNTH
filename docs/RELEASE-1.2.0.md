# SMKSYNTH 1.2.0 — ENDLESS + MIDI MODE

Versão oficial para SMK-37 Pro e SMK-37 Elite, promovida da MO1a (/19648) aprovada nos testes de Marcus. A identidade da 1.2.0 é /19649. A promoção mantém o comportamento validado; muda a versão e os materiais de publicação.

- **ENDLESS integrado:** gravação por passos de notas/acordes, pausas, prolongamentos, direção e HOLD; quatro partes com comprimentos independentes. C4 é a referência de transposição. PARA ativa/desativa; abrir a página não ativa. REC entra na gravação; segurar REC apaga a parte. Notas também saem por USB e TRS/DIN, no canal de cada parte.
- **MIDI MODE independente:** sintetizador silencioso e controlador USB/TRS com quatro mapas, canal principal e canais por controle, encoders ABS/REL com aceleração, faders, pads configuráveis e cores selecionáveis. Tudo editável no instrumento. Tela com controles, pads e teclado virtual de 37 teclas. Sem Bluetooth nesta versão.
- **Tema Zoom opcional:** nomes e valores maiores; Obervave permanece padrão. Seleção em GLOBE > THEME, com salvamento automático em silêncio. Identificação por cor na linha, box da parte com letra branca e número do preset; indicador de pattern preservado na performance.
- **Osciloscópio dedicado:** PATCH curto passa de valores de performance para logo/osciloscópio/atividade de vozes ou bateria; o próximo toque entra no Mixer. PATCH longo volta aos valores. Edições aparecem brevemente em Zoom sobre o osciloscópio.
- **Nomes de presets:** save rápido com BT; BT longo abre nome e destino. K1 cursor, K2 caractere, K7 posição. O programa já salvo no destino fica visível. BT revisa, outro BT confirma; PATCH volta/cancela.
- **Supersaw somente no OSC 1 para preservar a estabilidade**, com limite de cinco vozes quando SUPER está em uso e substituição de vozes em release. OSC 2 mantém as formas convencionais; programas de testes antigos com SUPER no OSC 2 são normalizados para SAW ao carregar.
- **Correções:** retorno do MIDI MODE sem travamento ou modulação residual; atualização independente de faders, encoders, pads e notas; contraste dos indicadores K1–K4 e cores de partes ajustados.

Mantidos: MIDI OUT USB/TRS nas quatro partes (teclas, pads, Split/Layer, ARP e sequenciadores), pitch bend/mod wheel na parte selecionada, sustain físico com polaridade automática ao ligar e MIDI PANIC com GLOBE + PLAY. Não há envio de MIDI Clock/Start/Stop ou Program Change automático. Áudio USB para gravação de vídeo no iPhone pode apresentar cliques/cortes.

## Instalação e dados

O mesmo [instalador](https://4t59m4f784-creator.github.io/SMKSYNTH/install/) identifica Pro/Elite e seleciona o pacote. **Atualizar preserva o banco salvo**, inclusive de ENDLESS/MIDI/Zoom compatíveis até MO1a. Exporte um .smksynth antes. Primeira instalação é exclusiva da V016 oficial do modelo e inicializa o banco de fábrica (cinco patterns e oito favoritos).

O backup .smksynth contém o banco do synth; mapas MIDI e tema usam armazenamento separado e não estão incluídos nesse arquivo. Transferências antigas interrompidas devem ser retomadas no instalador original antes de mudar de versão. O instalador mantém o acesso à retomada 1.1.0.

A base MO1a foi aprovada no Pro por Marcus. O fluxo Elite já foi validado por um usuário real; os novos pacotes dos dois modelos passaram por verificações automatizadas de identidade, integridade e proteção do banco. A promoção 1.2.0 não representa novo teste físico de cada pacote.

[Manual](MANUAL.md) · [Guia rápido](QUICKSTART.md) · [English](RELEASE-1.2.0.en.md)
