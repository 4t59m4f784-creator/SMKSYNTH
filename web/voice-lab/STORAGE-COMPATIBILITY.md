# SMKSYNTH — compatibilidade de dados e direção de backup

Esta alteração implementa 16 patterns. Não implementa export/import do SMK nem preservação de saves pelo atualizador. O usuário solicitou considerar essas necessidades nas decisões atuais, deixando a implementação para outra etapa.

## Contrato atual P16

- Família do banco: `SMK16P1` (tag de 16 bytes `SMK37DATASMK16P1`); layout 1, declarado como VOICE_LAYOUT_VERSION. A palavra VOICE nos nomes internos do banco é histórica; nenhuma engine VOICE foi adicionada.
- Owner 0x57000..0x58000; slots 0x58000..0x98000, 16 × 16384 B; globais 0x98000..0x99000; programas 0x99000..0x9b000; favoritos 0x9b000..0x9d000. End exclusivo.
- Aplicativo mantém reserva 320 KiB; arquivos boot/config/EQ do pacote terminam abaixo de 0x57000. Não escrever nas faixas originais do Player.
- SLOT existente: cabeçalho 16 B, comprimentos e CRC; pattern completo 14476 B + cena 380 B = 14856 B de payload, 14872 B com cabeçalho. O restante fica disponível no mesmo slot. Nenhuma compressão/quantização/redução de tracks ou steps.
- Prefixos lidos: SEQ4 13416 B; DRS1 14448 B; SL1 14460 B; KBD2/foco U2 14476 B. Magics e tamanhos atuais ficam estáveis. Cena PSN2 mantém 380 B; foco usa a extensão já introduzida em U2. Não duplicar configuração de teclado nas cenas.
- Presets de usuário: 99 posições no formato existente dos setores; favoritos: oito snapshots, com os leitores de versões anteriores já existentes. Não alterar envelopes, DSP nem dados de som para ganhar armazenamento.
- Importação 1.5 permanece restrita aos oito blobs reais de origem. Os oito slots extras nunca tentam ler faixas inexistentes como se fossem blobs legados.
- Owner tag é gravada por último na primeira inicialização. Tag desconhecida recusa escrita. Isso não é migração genérica: formatos futuros precisam de leitor/migrador explícito.

## Direção para exportação futura (ainda não implementada)

Usar um contêiner versionado de registros lógicos, não um dump de endereços de flash. Separar identidade/modelo, versão do contêiner, versão de cada tipo de registro, slot lógico, comprimento e checksum. Tipos esperados: presets de usuário, favoritos, patterns com cena/teclado/foco e globais pertinentes. Incluir versão de origem e um inventário verificável. Guardar arquivos em computador antes da atualização e só habilitar importação após validar tudo, com confirmação explícita de substituição.

Manter leitores dos formatos publicados e migradores testados por fixtures reais. Nunca tratar um formato desconhecido como banco vazio. O endereço físico poderá mudar em futuras versões sem mudar a identidade lógica de um preset/favorito/pattern.

Exportação física dependerá de um protocolo de comunicação bidirecional e ferramenta de computador; o atual protocolo de instalação não oferece backup de saves. Outra solução possível é modificar a cadeia de atualização para preservar as regiões de dados reconhecidas. Ambas exigem trabalho e testes próprios de interrupção, integridade e retomada. Não declarar atualizações sem perda até essa cadeia estar validada.

## Limite da atualização atual

O loader atual regrava 0x4000..0x9d000, incluindo o banco experimental. Mesmo uma atualização para outra build com o mesmo layout ainda apaga o banco se usar esse caminho. Mudar a tag ou manter o formato sozinho não protege dados da instalação. O instalador deve continuar avisando claramente, e o firmware estável não deve ser substituído automaticamente.

O arquivo de sessão JSON do simulador é apenas um recurso de navegador já existente; sua migração 11→19 blocos não é um protocolo de backup do SMK. Novas sessões trazem build e metadados de armazenamento para diagnóstico; o formato anterior continua importável.
