# SMKSYNTH B1 — banco de trabalho e exportação

Base: P16.2 aprovada, commit `c13629f81758d643bbbd69622648ed51a396c3dc`.
Ramo independente: `smksynth-backup-b1`. Versão: `3.0.1-b1`, identidade `SMK-37 Pro_19614`.
A versão oficial P16.2 / 19613 permanece intacta.

## Para começar

1. Abra o instalador B1 no Chrome ou Edge do computador e conecte o SMK37 Pro por USB de dados. A origem aceita é P16.2 / 19613 ou B1 / 19614.
2. Instale B1 e espere a identidade final 19614. Confira seus presets, favoritos e patterns anteriores. A preservação foi testada em flash simulada; instalação, transferência USB e comportamento no SMK físico ainda precisam ser validados.
3. Faça primeiro uma coleção pequena: salve um preset com nome reconhecível, um favorito e um pattern com cena, usando os controles habituais. Reinicie e confira se voltam corretamente.
4. Na página de exportação, conecte o teclado, pare PLAY e REC, solte teclas, botões e sustain e aguarde o fim dos saves e do som. Não edite nem toque durante a leitura. Feche outros programas e abas que utilizem o MIDI do SMK.
5. Clique em **Exportar banco salvo**. Guarde o arquivo `.smksynth` baixado e envie-o nesta conversa. Primeiro verificaremos esse teste antes de você dedicar tempo à coleção completa.

O arquivo contém somente a flash salva. Edições pendentes não são salvas automaticamente. As cenas acompanham os patterns; não existe exportação separada de uma cena ainda não salva. Presets de fábrica sem edição não são duplicados como presets de usuário, mas os sons capturados em favoritos e cenas são incluídos.

## Uso diário e coleta

Depois do teste, programe normalmente. Salve presets por BT e patterns/cenas pelo pad bank correspondente, como na P16.2. Exporte novas cópias em momentos importantes e mantenha as anteriores. A ferramenta não envia dados a um servidor: o arquivo é baixado no computador. Compartilhe-o aqui quando quiser entregar uma revisão da coleção.

ANALOG, DRUM-X, DSPs, vozes, sequenciador, SEQ TRANSPOSE, split/layer, oitavas por parte e controles musicais não foram modificados. O simulador oficial P16.2 continua sendo a referência musical; ele não emula o transporte USB desta B1.

## O que esta etapa entrega

- Atualizador B1 que preserva a área reconhecida `SMK16P1`, consumindo e verificando o pacote sem apagar os setores do banco existente. Owner desconhecido é recusado antes do apagamento do aplicativo. Pacotes com conteúdo de fábrica nessa área ainda são recusados.
- Exportação somente de leitura, limitada a `0x57000..0x9d000`. Não lê calibração nem permite endereços arbitrários ou comandos de escrita.
- Duas leituras completas comparadas, CRC por bloco e registro e SHA-256 do banco. Falha, cancelamento ou leituras divergentes não produzem arquivo utilizável pela página.
- Bloqueio temporário da edição durante a coleta; entradas acumuladas são descartadas. Uma sessão sem tráfego expira em cinco segundos.
- Extrator offline que valida o banco bruto e recupera presets, favoritos e patterns/cenas em registros separados.

No computador, após descompactar o código-fonte:

```sh
python3 tools/extract_author_bank.py meu-banco.smksynth banco-extraido
```

A pasta de destino deve estar vazia. O extrator revalida o banco bruto, não confia nos registros ou caminhos fornecidos pelo JSON e escreve um manifesto com origem, posições e hashes. Use Python sem a opção `-O`.

## Inclusão na versão pública

O arquivo permite recuperar seus sons e estados salvos sem transcrição manual. Depois de recebermos sua coleção, revisaremos nomes, posições, duplicatas e cenas e construiremos o conteúdo inicial da distribuição pública. A gravação desse conteúdo de fábrica é uma etapa posterior: deverá inicializar instalações novas e preservar bancos já existentes, com testes próprios de interrupção e compatibilidade. A B1 não contém esse gerador de build com banco de fábrica.

**Esta etapa não inclui importação ou restauração do arquivo para o teclado.** O arquivo exportado é recuperável no computador para revisão e preparação da distribuição; não é ainda um botão de recuperação do instrumento.

**Retornar à P16.2 pelo pacote antigo apaga o banco durante a reinstalação.** Evite downgrade depois de começar a coleção. A preservação oferecida pela B1 não altera atualizadores antigos. Em caso de instalação interrompida, mantenha alimentação e retome B1 no mesmo navegador e página; não use outro pacote como tentativa de retomada.

## Validação e recursos

Automatizados: protocolo C com sanitizers; interoperabilidade da resposta C com cliente JS; limites, CRC, timeout e cancelamento; leitura dupla; extração de estruturas nativas salvas; pins, corrupção e identidade do instalador; loader com banco mantido byte a byte e 40 pontos de interrupção/reexecução; regressões de roteamento, REC, armazenamento, PCM ANALOG e DRUM-X. Logs acompanham a entrega.

Build SMK37 Pro: aplicativo 157.576 / 327.680 bytes; RAM estática 82.752 / 98.304; pool 325.984 / 344.064; loader 7.493 bytes; pacote 651.609 bytes. A exportação usa blocos de 128 bytes e não aloca o banco inteiro na RAM do instrumento.

Testes físicos pendentes: transição preservando dados reais, USB-MIDI, download/extração do primeiro arquivo e uso prolongado. Compilação, testes nativos e mocks não comprovam esses itens.

## Créditos e fontes

Licenças e créditos originais Obervave, Felucca, Dinghy, port SMK37 e DRUM-X/JIANT preservados em `LICENSE`, `LICENSING.md` e avisos no código. GPL-3.0-only e licenças dos componentes acompanham o código-fonte. Nenhum sample novo foi incluído. Veja os ZIPs de fontes B1 e P16.2 no instalador.
