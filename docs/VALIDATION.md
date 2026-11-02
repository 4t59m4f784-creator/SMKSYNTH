# Validação de lançamento 1.0.0

## Estado

DP2 aprovada por Marcus no SMK‑37 Pro. A 1.0.0 conserva a base funcional, acrescentando o seed musical e o perfil de primeira instalação. **Primeira instalação física V016 → 1.0.0 por USB e retorno físico ainda não confirmados.** Teste de flash simulada não comprova USB real, alimentação ou CPU do chip.

## Conteúdo

- Backup de origem validado por SHA-256 e CRCs, extração usando os tipos reais.
- Somente patterns 1–5 e respectivas cenas; slots 6–16 apagados.
- Oito favoritos, incluindo as duas variações POLY BRASS e o INIT selecionado pelo autor.
- Nenhum preset de usuário importado; ambos os setores de presets começam apagados e usam a base de fábrica.
- Globals limpos: 90 BPM, afinação/oitava neutras, Pattern 1, MIDI omni e SYNC INTERNAL. Não foi copiado o conjunto pessoal de globals do autor.
- Notas, sons e mixer preservados. Somente geração dos favoritos e badges de edição ajustados para representar snapshots independentes dos presets públicos.

## Testes executados

| Área | Evidência |
|---|---|
| Origem real | Pacote oficial enviado `SMK-37 Pro_016(1).fwsc`, SHA-256 `2d98ce72530e71d617384963423820536f094501299e2cfec0a8d3c3fb6bcfb0`; 36 marcadores de interleave |
| Loader | Imagem final byte a byte; retomada em 154 pontos de erase e 22 pontos de program; repetição idempotente; boot head/BT/calibração/USR/key_mac intactos |
| Pacote | CRCs do boot/app/cfg/EQ; seed fora de arquivos de boot; layout de app 320 KiB e dados SMK16P1 |
| Instalador | Pin SHA/modelo/CRCs; somente Pro V016; outras versões/modelos e DP2 recusados antes da escrita; retomada vinculada ao pacote/origem |
| Simulador | Boot do C real, cinco examples audíveis, oito recalls, onze slots livres, tempo 90, zero gravações automáticas |
| Regressões | Oitava/transpose persistente, split/layer, bateria; PCM ANALOG e DRUM-X idênticos aos hashes da base |
| Backup | Cliente 19618 contra servidor C real em flash simulada: exportação, retry, SHA/CRC, restore e retomada |

Logs em `validation/release-*.log`. Hashes de entrega em `releases/1.0.0/SHA256SUMS.txt` e `RELEASE.json`.

## Limites e recursos

App 161660/327680 B; RAM estática 82896/98304 B; pool 325984/344064 B; loader de primeira instalação 7211 B. O cliente recusa origens diferentes, mas o pacote inicial é destrutivo para a área de aplicação/banco: não deve ser apresentado como atualizador que preserve o trabalho de um usuário existente.

O firmware oficial foi usado apenas localmente nos testes, sem redistribuição. Os dados selecionados foram incluídos com autorização de Marcus. A ocorrência isolada de sessão de backup expirada na DP2 foi resolvida por reinício e permanece sem causa confirmada.

Conferência visual local: apresentação, navegação para simulador e estado de fábrica P1–P5 preenchidos / P6–P16 vazios. O preview HTTP executa apenas a interface; o AudioWorklet foi verificado em Node e o áudio do dispositivo continua limitado à aprovação da base DP2.
