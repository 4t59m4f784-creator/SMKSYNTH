# Atualização de validação — 02/11/2026

Marcus confirmou no SMK37 físico que a exportação e a restauração funcionam perfeitamente e estão muito mais rápidas. O registro técnico abaixo foi escrito antes desse teste; suas indicações de teste físico pendente são históricas. Não foi relatado teste físico de corte de energia.

# SMKSYNTH B2 — exportação e restauração

Versão experimental `3.0.1-b2`, identidade `SMK-37 Pro_19615`, ramo `smksynth-backup-b2`. Base musical P16.2 aprovada, com o código B1 recuperado do pacote de fontes do commit original `57ac3fd271383d24f2ddc350e7e67b5e25ac3c35`. A versão oficial P16.2 e a B1 não foram substituídas.

## Instalação e primeiro teste

1. Ainda na B1, exporte e guarde um arquivo `.smksynth`. O primeiro banco enviado por Marcus já passou na validação e extração.
2. Use o instalador B2 privado no Chrome ou Edge do computador. Origens aceitas: SMK37 Pro 19613, 19614 e 19615. Mantenha cabo, alimentação e aba até confirmar a identidade 19615.
3. Confira os presets, favoritos, patterns e oitavas existentes. A transição foi testada com flash simulada; a preservação no seu teclado ainda deve ser conferida.
4. Abra a ferramenta B2, salve pelos controles habituais, pare PLAY/REC, solte teclas, botões e sustain e feche outros programas MIDI. Exporte o banco. A tela e o arquivo mostram duração, pedidos e retentativas. Envie o arquivo e o tempo observado para comparação.
5. Para testar restauração com perda controlada: guarde esse arquivo como **A**, altere e salve somente um preset descartável e exporte o estado atual como **B**, que é a proteção mais recente. Selecione **A**, confira o resumo, digite RESTAURAR e confirme. Ao concluir, use **Reiniciar após restaurar** e confira se o preset voltou ao estado de A, junto aos favoritos e patterns/cenas.

A restauração substitui o banco inteiro, incluindo configurações salvas. Não combina bancos, não importa sons individualmente e não inclui edições não salvas. Uma exportação do banco atual na mesma sessão é exigida antes de uma restauração nova. Na retomada de uma operação pendente, essa exigência é dispensada porque o banco já pode estar parcial.

Não faça um teste de corte de energia com dados sem cópia. Os testes de interrupção fornecidos são automatizados; não são comprovação física.

## Se houver interrupção

O instrumento mostra **RESTORE PENDING / RESUME VIA USB**. Selecione o mesmo arquivo na ferramenta B2 e retome. A identidade continua 19615. O progresso persistente vincula a operação ao SHA-256 e CRC do banco escolhido. Se o registro de progresso estiver incompleto, uma nova restauração integral do arquivo é necessária.

A restauração **não é atômica para todo o banco**: durante o processo podem existir setores antigos e novos. O uso musical e os salvamentos ficam bloqueados para que esse estado não seja carregado ou sobrescrito como se estivesse completo. Após confirmar o banco inteiro, o instrumento permanece bloqueado até reiniciar, para recarregar os dados em RAM.

Não use outro instalador, B1, P16.2, recuperação manual ou atualização de firmware enquanto houver restauração pendente. O loader B2 recusa atualização quando o registro indica restauração pendente ou inválida. Ferramentas/firmwares antigos não conhecem essa proteção.

Guarde o arquivo original: a retomada não depende de uma cópia armazenada no navegador ou servidor. Não existe desfazer automático de todo o banco; para voltar a um estado anterior, restaure a cópia correspondente após concluir a operação atual.

## Exportação otimizada

O arquivo mantém `smksynth-authoring-backup`, versão 1, layout `SMK16P1`. A B2 aceita arquivos da B1 e produz arquivos legíveis pelo extrator B1. Nenhum dado salvo foi movido ou reformulado.

B1: duas leituras integrais de 286720 bytes em blocos de 128 — 4480 pedidos READ.
B2: uma leitura em 560 blocos de 512 bytes, comprimidos por repetições quando vantajoso, seguida de 70 pedidos de CRC de setores relidos no dispositivo. A transferência confere CRC por resposta, CRC independente de cada setor e calcula SHA-256 do banco no computador. A restauração faz leitura integral de volta e compara o SHA-256 com o arquivo antes de confirmar a conclusão.

O código musical e o DSP não foram alterados. Durante a restauração, a saída de áudio fica silenciada e o uso musical é bloqueado. Durante exportação, a edição fica bloqueada como na B1. A B1 continua funcionando na ferramenta B2 com seu método original de leitura.

No teste com o banco real enviado, o fluxo B2 transportou aproximadamente 200 KB de respostas para exportar, contando protocolo e conferência, versus cerca de 757 KB de respostas na leitura dupla B1. O número de pedidos de dados/conferência caiu de 4480 para 630. Isso comprova redução de tráfego e de pedidos, **não um tempo de USB**. Não foi medido tempo de transferência em hardware nesta entrega. Não há promessa de duração em segundos.

## Segurança de gravação e mapa

- Área do banco inalterada: `0x57000..0x9d000`, 70 setores de 4096 bytes.
- Registro de progresso: `0x55000..0x56000`.
- Preparação temporária de um setor: `0x56000..0x57000`.
- Essas duas áreas estão fora dos arquivos declarados de boot/CRC e fora dos bancos antigos Obervave, calibração, recursos de fábrica e identificação do chip. O empacotador e o teste verificam esse limite.
- O setor temporário é recebido e conferido antes de apagar o setor de destino. Cada destino é relido e conferido antes de marcar seu progresso. Um setor já igual pode ser conferido e marcado sem apagamento.
- O registro de progresso é confirmado antes de alterar destinos. No boot ele tem prioridade sobre inicialização/migração de dados e escalonamento de recuperação por reinicializações repetidas. Entradas MIDI são descartadas durante manutenção para não saturar a fila.
- Escritas do protocolo são limitadas a essas áreas. Não existe comando para gravar endereço arbitrário, firmware ou calibração.

## Testes e limites

Passaram: protocolo B1 preservado; interoperabilidade cliente JS com código C real do firmware usando flash simulada; arquivo B1, SHA inválido, compressão e resposta sem compressão de tamanho máximo; repetição após resposta corrompida; interrupção, reinício do processo e retomada vinculada ao arquivo; restauração e leitura de volta exatas; 68 pontos de apagamento/programação parcial; atualização B1→B2 mantendo banco byte a byte, inclusive 41 pontos de interrupção/retomada; recusa de update com restauração pendente; pins e identidade do instalador; CRCs de boot após gravações na área de progresso/preparação/banco; OTA e regressões de roteamento, REC, armazenamento, áudio ANALOG exato e DRUM-X com sanitizers.

O banco real enviado também passou pelo protocolo C/JS e restauração simulada byte a byte. Nenhum conteúdo desse banco está embutido no firmware, fontes ou site B2.

Build: aplicativo 160108 / 327680 bytes; RAM estática 82880 / 98304; pool 325984 / 344064; loader 7703 bytes; pacote 651819 bytes. Os buffers de transferência são locais e pequenos; o banco completo não é alocado na RAM do SMK.

Pendentes: instalação, desempenho USB, comportamento após restauração e interrupção física no SMK37. O simulador musical oficial P16.2 continua preservado e não simula a flash/USB B2. Compilação e testes automatizados não comprovam esses itens físicos.

## Reprodução

Toolchain JieLi e SDK conforme README; build:

```sh
python3 tools/build.py --voice-hardware --board smk37 --model pro --release 3.0.1-b2 --experimental-id 19615
sh tests/run_backup_b2_tests.sh /caminho/obervave-3.0.1-b1-smk37.fwsc
sh tests/run_sl1_tests.sh
```

Os arquivos usados nos testes de rotina são sintéticos. O teste com coleção pessoal pode ser executado localmente passando o banco bruto validado como argumento de `tests/backup_b2_client_test.mjs`, sem adicionar esse banco ao repositório.

Créditos e licenças originais preservados em LICENSE, LICENSING.md, avisos nos arquivos e licenças dos componentes. GPL-3.0-only; nenhum sample foi incluído. A inclusão da coleção de fábrica na distribuição pública continua sendo uma etapa posterior, após seleção do banco e implementação de inicialização compatível para usuários novos e existentes.
