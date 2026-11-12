# SMKSYNTH 1.0.3-elite2 — teste SMK-37 Elite

**Experimental. Sem instalação ou recuperação validada em Elite físico.**

Base: versão pública SMKSYNTH 1.0.3, com SUPER nos dois osciladores, DRUM-X, controles JUMP/CATCH e áudio USB UA1. Não inclui ENDLESS ou SUPER SP1. A candidata anterior `experimental/elite1` era uma preparação da 1.0.2 e não é o pacote desta página.

## Instalação e retorno

1. Confirme o modelo físico **SMK-37 Elite**. O instalador recusa Pro, FM-1 e MKE-P37, inclusive em modo de atualização.
2. Faça backup com as ferramentas do firmware atual. O [port kit](https://github.com/genkigenki/fm1-to-smk37/blob/main/docs/02-update-and-recovery.md) documenta o backup completo da flash, que inclui a calibração individual. `.smksynth` é apenas o banco musical.
3. Guarde a **V016 oficial do Elite**, disponível nos [downloads M-VAVE](https://www.m-vave.com/appdownload). Origem aceita: `SMK-37 Elite_016`. Não use V016 do Pro ou FM-1. Um Elite que outro port identifica como Pro deve primeiro retornar ao oficial do Elite pelo instalador daquele port.
4. Abra a página em Chrome/Edge no computador, feche os demais programas MIDI e consulte o dispositivo. Primeira instalação substitui/inicializa o banco com cinco patterns e oito favoritos, sem presets extras.
5. Confirme a operação e digite `ELITE`. Aguarde `SMK-37 Elite_19629`. Para reinstalar o teste preservando o banco, use Atualizar a partir de Elite `_19618` ou `_19629`.
6. Para retornar, selecione na página o arquivo oficial Elite V016 baixado da M-VAVE. A página confere identidade, tamanho, SHA-256 e CRCs antes de habilitar a escrita. O arquivo oficial não é redistribuído neste repositório. O retorno pode substituir o banco SMKSYNTH: exporte-o antes.
7. Interrupção: mesma página, navegador, operação e pacote. A retomada fica vinculada ao hash e à origem. Para retorno oficial, selecione novamente o mesmo arquivo. Não troque de instalador no meio.
8. Se o teste não iniciar: ligue segurando OCT− para a recuperação USB. Esta página aceita retornar ao oficial a partir de `SMK-37 Elite_000`. A recuperação física no Elite ainda precisa ser confirmada. Se o USB não responder, pare e consulte o port kit; não use dumps de outro instrumento.

## Roteiro para os voluntários

- Anotar sistema operacional, navegador e identidade inicial; guardar o log completo.
- Confirmar boot, tela, saída/fone, USB MIDI e áudio USB. Ruídos em vídeos gravados pelo iPhone continuam sendo problema conhecido da base pública.
- Testar 37 teclas, velocity, 16 pads, oito encoders relativos, quatro faders em JUMP e CATCH, rodas e pedal.
- Testar os cinco patterns, favoritos, Drum Pads, Split/Layer, FX e trocas de partes/bancos.
- Salvar um preset, favorito e pattern; desligar/ligar e conferir recuperação.
- Exportar `.smksynth`, restaurar pelo backup e conferir os sons e sequências. O arquivo identifica o modelo Elite; não inclui calibração nem o firmware.
- Confirmar retorno ao oficial Elite V016 e funcionamento dos controles. Só depois considerar o caminho validado fisicamente.

## Referências e limites verificados

- [Mapa do hardware](https://github.com/genkigenki/fm1-to-smk37/blob/main/docs/01-hardware.md): Pro/Elite/MKE compartilham o hardware; os testes marcados como físicos na referência foram feitos em Pro. Isso não valida nosso Elite.
- [Instalador do port](https://genkigenki.github.io/smk37/): preserva o nome do modelo no pacote, loader e registro de retorno para permitir o firmware oficial correspondente.
- V016 Elite inspecionada: 705236 bytes, 20 marcadores, identidade `SMK-37 Elite_016`, SHA-256 `b2ee38abfef3ae922e4eecdb8a551e14345300ae6f270265feacd33b6ef99cb6`. O Pro V016 usa outro pacote; não foi renomeado para Elite.
- App compilado Elite2: 164100 bytes; RAM estática 86624 bytes; pool 325984 bytes. Primeira instalação e atualização têm o mesmo app e loaders diferentes.
- Primeira instalação escreve a área de aplicação/banco até `0x9D000`; o loader próprio não escreve o cabeçalho `0x0000–0x3FFF`, BT/calibração `0xC0000–0xC1FFF`, recursos USR nem key_mac. Atualização preserva o banco `0x57000–0x9CFFF` e bloqueia journal de restauração pendente.
- Testes: CRCs/modelo; 154 pontos de interrupção em apagamento e 22 em programação da primeira instalação; atualização com banco idêntico; validação do retorno oficial e registro `ota-SMK-37 Elite_016`; recusa de cabeçalho estrangeiro. Esses são testes com flash simulada, não USB/Elite físico. O loader fechado da M-VAVE não foi executado em hardware.

## Reproduzir

Use o toolchain e SDK fixados em `tools/build.py` (JieLi AC79NN SDK V1.2.1). Com `JIELI_TOOLCHAIN` configurado:

```sh
python3 tools/build_elite_test.py --sdk /caminho/AC79NN_SDK
```

Os dois pacotes devem reproduzir os hashes de `manifest.json`. Fontes, licenças e créditos acompanham a distribuição principal; firmware GPL-3.0-only, componentes com seus avisos preservados.
