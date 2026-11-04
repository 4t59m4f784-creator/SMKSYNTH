> Atualização de 02/11/2026: Marcus confirmou a primeira instalação física por USB no SMK-37 Pro. Os registros abaixo descrevem também os testes automatizados realizados antes dessa confirmação.

## Atualização 1.0.1

No instalador, escolha Atualizar se já usa SMKSYNTH: o banco é preservado, sem reinstalar V016. Escolha Instalar somente para a primeira instalação a partir da M‑VAVE oficial V016; essa opção substitui o banco e carrega os exemplos de fábrica. Exporte um backup antes de atualizar. USB oferece MIDI, backup/restauração e atualizações; grave áudio pela saída física usando uma interface.

GLOBAL → FADER MODE: JUMP aplica a posição do fader; CATCH assume o parâmetro ao cruzar o valor atual e rearma ao trocar páginas, partes, bancos ou patterns. Salve GLOBAL para conservar a escolha. Os oito encoders permanecem relativos.


# Instalação SMKSYNTH 1.0.0

Modelo: **M‑VAVE SMK‑37 Pro**. Origem obrigatória: **oficial V016**, identidade `SMK-37 Pro_016`. Destino: `SMK-37 Pro_19618`. Outros modelos, V015/V017, nomes genéricos e firmwares alternativos são recusados.

Obtenha firmware e M‑UPGRADE na [página oficial M‑VAVE](https://www.cuvave.com/appdownload), entrada SMK‑37 PRO / V016. O arquivo oficial analisado, fornecido por Marcus, tem SHA-256 `2d98ce72530e71d617384963423820536f094501299e2cfec0a8d3c3fb6bcfb0` e não é redistribuído. O dispositivo é consultado antes do comando de atualização.

Se já usa SMKSYNTH, salve edits no painel e exporte seu `.smksynth` antes de retornar à V016. O retorno ao firmware oficial e a instalação inicial podem substituir os dados SMKSYNTH. Não há mesclagem com o banco de fábrica.

## Instalar

1. Instale a V016 pelo M‑UPGRADE conforme o fabricante e reinicie o teclado.
2. Use Chrome/Edge no computador, HTTPS e cabo USB de dados. Feche DAWs, editores MIDI e outras abas de instalador/backup. Mantenha alimentação estável.
3. Abra o instalador. Ele verifica SHA-256, identidade e CRCs antes de liberar a operação.
4. Clique **Conectar e consultar** e confira `SMK-37 Pro_016`.
5. Escolha **Instalar SMKSYNTH 1.0.0**. Confira origem, destino e preparação dos dados de fábrica; confirme.
6. Mantenha a aba aberta até a reinicialização com `SMK-37 Pro_19618`.
7. Pattern 1 abre parado, a 90 BPM. PLAY toca o exemplo. Em KNOB BANK → Patterns, P1–P5 trazem exemplos e P6–P16 estão livres. Oito favoritos nos pads inferiores de Performance.

A instalação escreve a área da aplicação e prepara o banco SMK16P1. BTIF, calibração USRTRIM, USR, key_mac e o cabeçalho de boot ficam fora das janelas de escrita. A conferência foi feita em flash simulada com interrupções; a primeira instalação física por USB foi posteriormente confirmada por Marcus.

## Interrupção

Use o mesmo pacote, navegador e origem do site. A operação pendente é vinculada ao SHA-256 e à origem V016. A retomada só é liberada para `ota-SMK-37 Pro_900` com esse vínculo. Um loader encontrado sem operação conhecida não é autorizado automaticamente.

Reconecte e consulte novamente. Se houver retomada, confirme e aguarde. Guarde o log se não concluir. Não inicie restauração de banco enquanto o firmware estiver pendente. A primeira instalação não é atômica: o loader permanece responsável pela retomada até terminar.

## Backup e restauração

A ferramenta exporta os dados **salvos**, não edits ainda em RAM. Salve no painel antes. O arquivo inclui presets de usuário, favoritos, patterns/cenas e configurações salvas.

Restauração substitui todo o banco, inclusive posições vazias; não é importação seletiva. Exporte o destino antes de confirmar. Com RESTORE PENDING, retome o mesmo arquivo e conclua antes de trocar firmware. Uma sessão de exportação expirada foi resolvida ao reiniciar o instrumento nos testes DP2; causa definitiva não reproduzida. Exportação incompleta não é apresentada como backup verificado.

## Voltar à M‑VAVE

Exporte o banco SMKSYNTH. Use M‑UPGRADE com o arquivo oficial V016 de SMK‑37 Pro, conforme o fabricante. Não use Elite/MKE/FM-1. Firmware não importa `.smksynth`; quando retornar ao SMKSYNTH, use restauração para recuperar seu trabalho. O retorno físico desta distribuição também deve ser conferido no instrumento.
