# VD2 — teste das correções, SMK37 Pro

Abra https://obervave-voice-lab.padrini.chatgpt.site/keyboard-vd2/ no Chrome/Edge
no computador, entre com sua conta, conecte USB de dados e feche os aplicativos MIDI.
Clique Conectar e consultar → Instalar Voice + Drums VD2 e confirme. Aguarde a
identidade final **SMK-37 Pro_19602**, versão **3.0.1-vd2**, sem desligar/desconectar.

**Instalar a VD2 reinicia os saves experimentais da VD1**, conforme o limite já
explicado. Desligar e ligar depois mantém os saves VD2. Os dados originais do
Player permanecem separados. Nenhum autosave foi acrescentado. Se quiser manter
algo criado na VD1, registre-o antes de atualizar; ainda não existe exportação USB
completa dos saves experimentais.

A página nova inclui **retorno à VD1 para comparação**, PF12 (base preservada) e
PF9 (referência com aprovação física anterior). Use a página nova para voltar;
a página antiga da VD1 pode recusar a identidade _19602. Retornar também reinicia
o banco experimental. Os pacotes anteriores são os arquivos originais, sem rebuild.

Em falha de transferência, baixe o log e consulte novamente. Loader conhecido
_900 permite retomada compatível; OCT− ao ligar oferece o resgate _000 quando o
aplicativo/boot ainda executa. Não são garantias para qualquer falha elétrica/flash;
ver as instruções completas da VD1 também incluídas. Nenhuma atualização física
ou resgate foi executado pelo assistente nesta rodada.

## Reteste proposto

1. **DRUMS:** no teclado e nos pads, repita cada peça alternando toques fortes e
   suaves. Experimente também reduzir LEVEL durante uma cauda. Registre kit/peça
   se restar um estalo anormal. Sons de ruído/clap naturalmente variam entre hits.
2. **VOICE:** após uma nova partida, teste primeiro uma parte e um acorde de duas
   notas, depois três/quatro. Solte e espere o release terminar entre tentativas;
   depois teste rearticulações e, por último, junte as demais partes/efeitos.
   Se houver dropout, tente fotografar **GLOBAL P…us/!** e anote preset, notas,
   partes e efeitos ativos. O limite do bloco é aproximadamente **5805 µs**.
   O indicador mostra pico desde que ligou, não média de CPU; não é preciso salvar.
3. **Roda:** faça uma sequência em VOICE A, ajuste o morph, selecione B sem mover
   a roda e confira que A conserva o timbre. Mova a roda em B (se VOICE), alterne
   para ANALOG e volte a A. A posição da roda só assume a nova parte ao movimentar.
   O valor latched é de sessão: não há promessa de retenção ao desligar.

DRUMS e retenção da roda têm defeitos reproduzidos/corrigidos em testes. VOICE tem
uma otimização candidata; os dropouts polifônicos **ainda precisam desse teste
físico**. A VD1 e a página anterior permanecem disponíveis.

Sem acesso ao Site, o pacote inclui instalador local: execute `python3 -m
http.server 8000 --directory installer` na pasta extraída e abra
http://localhost:8000/ no Chrome/Edge. Abrir index.html como arquivo não funciona.
