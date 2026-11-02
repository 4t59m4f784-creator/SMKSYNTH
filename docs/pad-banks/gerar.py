from pathlib import Path
from PIL import Image,ImageDraw,ImageFont
import html,zipfile,json
OUT=Path(__file__).resolve().parent
W,H=2200,3100
BG='#090d14';INK='#f0f3fa';MUTED='#acb9cc';LINE='#303c50';PANEL='#121b2a';BLUE='#7ea8ff';RED='#ff625b';AMBER='#ffc778';WHITE='#ffffff';GREEN='#86d4ad';ORANGE='#ffb281';PINK='#e598d5'
FONT='/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf';BOLD='/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf';MONO='/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf'
class Canvas:
 def __init__(self,height=H):
  self.h=height
  self.im=Image.new('RGB',(W,height),BG);self.d=ImageDraw.Draw(self.im);self.s=[f'<svg xmlns="http://www.w3.org/2000/svg" width="{W}" height="{self.h}" viewBox="0 0 {W} {self.h}"><title>SMKSYNTH — Guia dos pad banks</title><rect width="{W}" height="{H}" fill="{BG}"/>']
 def rect(self,x,y,w,h,fill=PANEL,stroke=None,rad=18):
  self.d.rounded_rectangle((x,y,x+w,y+h),radius=rad,fill=fill,outline=stroke,width=2)
  self.s.append(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="{rad}" fill="{fill}" stroke="{stroke or "none"}" stroke-width="2"/>')
 def line(self,x,y,xx,yy,col=LINE,width=2):
  self.d.line((x,y,xx,yy),fill=col,width=width);self.s.append(f'<path d="M{x},{y} L{xx},{yy}" stroke="{col}" stroke-width="{width}"/>')
 def text(self,x,y,t,size=30,color=INK,bold=False,mono=False):
  font=ImageFont.truetype(MONO if mono else BOLD if bold else FONT,size)
  self.d.text((x,y),t,font=font,fill=color,anchor='lt')
  self.s.append(f'<text x="{x}" y="{y}" dominant-baseline="text-before-edge" font-family="{("DejaVu Sans Mono" if mono else "DejaVu Sans")}" font-size="{size}" font-weight="{700 if bold else 400}" fill="{color}">{html.escape(t)}</text>')
 def wrap(self,x,y,t,width,size=29,color=MUTED,bold=False,leading=1.48):
  font=ImageFont.truetype(BOLD if bold else FONT,size);lines=[]
  for para in t.split('\n'):
   line=''
   for word in para.split():
    test=(line+' '+word).strip()
    if self.d.textlength(test,font=font)>width and line:lines.append(line);line=word
    else:line=test
   lines.append(line)
  for line in lines:self.text(x,y,line,size,color,bold);y+=int(size*leading)
  return y
 def card(self,x,y,w,h,title,body,color=BLUE,size=29):
  self.rect(x,y,w,h,PANEL,LINE);self.rect(x+26,y+33,5,35,color,rad=2);self.text(x+50,y+32,title,34,INK,True)
  end=self.wrap(x+36,y+96,body,w-72,size)
  if end>y+h-15:raise ValueError((title,'overflow',end,y+h))
 def note(self,y,title,body,color=AMBER,h=150):
  self.rect(100,y,2000,h,'#182030',LINE);self.text(132,y+23,title,29,color,True);self.wrap(132,y+70,body,1936,27)
 def header(self,num,title,subtitle):
  self.text(100,70,'SMK',43,INK,True);self.text(204,70,'SYNTH',43,RED,True);self.line(400,95,480,95,BLUE,5)
  self.text(1500,80,'GUIA DE PERFORMANCE',23,MUTED,mono=True)
  self.text(100,166,'PAD BANK '+num,27,BLUE,True,True);self.text(100,223,title,69,INK,True)
  self.wrap(100,323,subtitle,1980,30)
  self.rect(100,418,2000,80,'#1a2333');self.text(128,441,'KNOB BANK:  1 Performance/Favoritos  →  2 Split/Layer  →  3 Patterns/Save',27,INK,mono=True)
 def hardware(self,active):
  x,y=100,532;self.rect(x,y,2000,220,'#1c2430','#4d5b6d',22)
  self.text(130,y+16,'SMK-37 PRO · ESQUEMA FUNCIONAL',17,MUTED,mono=True)
  for i in range(4):
   xx=140+i*60;self.line(xx,y+61,xx,y+118,'#68778d',3);self.rect(xx-12,y+78,24,12,'#d2dbe9',rad=2)
  self.rect(458,y+43,156,84,'#0b1422',BLUE,5);self.text(472,y+54,'SMKSYNTH',18,BLUE,mono=True);self.text(472,y+85,'BANK '+active,22,INK,True,True)
  for i in range(8):self.rect(695+i*62,y+54,30,30,'#718198',rad=15)
  for row in range(2):
   for i in range(8):self.rect(1335+i*72,y+47+row*43,52,30,'#395681' if row==0 else '#7e383e',rad=4)
  # Exactly 37 keys, C to C over three octaves: 22 white, 15 black.
  kx=140;kw=82
  for i in range(22):self.rect(kx+i*kw,y+143,kw-3,55,'#d9dfeb',rad=2)
  for octv in range(3):
   for step in [0,1,3,4,5]:self.rect(kx+(octv*7+step+1)*kw-18,y+143,36,33,'#11151c',rad=2)
 def pads(self,labels,colors,sub=None):
  self.text(100,789,'MAPA DOS 16 PADS · LENDO DA ESQUERDA PARA A DIREITA',24,MUTED,mono=True)
  for i,(lab,col) in enumerate(zip(labels,colors)):
   x=100+(i%8)*252;y=845+(i//8)*191;self.rect(x,y,235,171,'#172134',col,12)
   self.text(x+16,y+14,f'{i+1:02}',23,col,True,True)
   lines=lab.split('\n')
   for j,line in enumerate(lines):self.text(x+16,y+63+j*35,line,26,INK,True)
  self.text(100,1231,sub or 'Ilustração das funções. Os LEDs reais variam conforme o estado da performance.',23,MUTED)
 def footer(self):
  self.text(100,self.h-240,'KNOB BANK: toque troca o banco; segure 0,7 s para voltar à Performance/Favoritos.',23,MUTED)
  self.text(100,self.h-197,'PLAY e REC mantêm os pads. Somente SEQ assume o banco do sequenciador.',23,MUTED)
  self.line(100,self.h-130,2100,self.h-130)
  self.text(100,self.h-99,'SMKSYNTH · BASE MUSICAL P16.2 / B2 · SMK-37 PRO',22,MUTED,mono=True)
  self.text(100,self.h-58,'Base sonora: OberVave + DRUM-X / JIANT · Guia: Marcus Padrini',21,MUTED)
 def save(self,name):
  self.footer();self.im.save(OUT/(name+'.png'),optimize=True)
  (OUT/(name+'.svg')).write_text('\n'.join(self.s+['</svg>']))
# BANK 1
c=Canvas(3700);c.header('1','PERFORMANCE + FAVORITOS','Troque a parte tocada, acione os sends e recupere timbres sem sair da performance.');c.hardware('1')
c.pads(['PARTE\nA','PARTE\nB','PARTE\nC','PARTE\nD','DELAY\nON / OFF','REVERB\nON / OFF','PRESET\nANTERIOR','PRESET\nSEGUINTE']+[f'FAVORITO\n{i}' for i in range(1,9)],[BLUE,ORANGE,GREEN,PINK,AMBER,BLUE,RED,BLUE]+[BLUE]*8)
c.card(100,1300,980,387,'1 · PARTES: PADS 1–4','Toque A, B, C ou D para selecionar a parte.\nOs controles passam a editar essa parte. Cada parte mantém sua própria oitava de teclado.\nEm SINGLE, o teclado toca a parte selecionada. Em SPLIT/LAYER, vale a combinação ativa.',BLUE)
c.card(1120,1300,980,387,'2 · SENDS E PRESETS: PADS 5–8','5: liga/desliga o envio de delay da parte.\n6: liga/desliga o envio de reverb da parte. Ao ligar, o send retoma seu último nível.\n7 / 8: programa anterior / seguinte da parte selecionada. Não são comandos de save.',RED)
c.card(100,1725,980,460,'3 · FAVORITOS: PADS 9–16','Toque curto: carrega o favorito na parte selecionada. Os 8 favoritos são compartilhados e podem ser usados em partes diferentes.\nSegure 0,7 s: captura o timbre atual no slot escolhido, substituindo o favorito anterior.\nA captura leva o patch, sua oitava e os sends de chorus, delay e reverb.',BLUE)
c.card(1120,1725,980,460,'4 · O QUE O FAVORITO NÃO MUDA','O favorito é um snapshot de uma parte; não é a cena das quatro partes.\nCarregá-lo não troca as sequências nem as duas configurações de SPLIT/LAYER.\nEle não substitui o banco de presets. Para salvar um programa, use BT. Os efeitos são compartilhados; seus sends são por parte.',AMBER)
c.text(100,2240,'LEITURA DOS LEDs DOS FAVORITOS',28,INK,True)
items=[(BLUE,'AZUL MUITO DISCRETO','Slot vazio'),(BLUE,'AZUL DISCRETO','Salvo, não selecionado'),(RED,'VERMELHO FORTE','Favorito selecionado'),(RED,'VERMELHO MÉDIO','Timbre alterado')]
for i,(col,t,desc) in enumerate(items):
 x=100+i*505;c.rect(x,2300,30,30,col,rad=6);c.text(x,2355,t,20,col,True);c.wrap(x,2393,desc,450,25)
c.note(2490,'CAPTURADO AINDA NÃO É GRAVADO NA FLASH','O pad pode piscar em âmbar enquanto aguarda. Pare PLAY/REC, solte as notas e aguarde a mensagem FAVOURITES SAVED antes de desligar.',h=175)
c.note(2705,'ATENÇÃO À PÁGINA DRUM SOUND','Com DRUM-X selecionado, a página DRUM SOUND usa os 8 pads superiores para tocar instrumentos de bateria. Volte à performance para usar o mapa normal.',BLUE,h=175)
c.card(100,2915,980,500,'5 · KNOBS E FADERS','K1–K4: parâmetros da tela. K5: tempo na tela Performance; contextual nas demais telas. K6: parte. K7: preset. K8: master.\nFADER BANK A, em ANALOG: cutoff, ressonância, envelope e release; em DRUM-X: morph, tone, decay e fold.\nFADER BANK B: chorus send, delay send, reverb send da parte; F4 é o master.',BLUE,26)
c.card(1120,2915,980,500,'6 · PRESET ≠ FAVORITO','BT curto: salva o preset no programa atual.\nBT por 0,7 s: abre a escolha de destino; K7 escolhe o programa. A tela mostra o que existe no destino. BT confirma e PATCH cancela.\nSalvar favorito grava seu snapshot. Salvar preset grava um programa. Salvar pattern guarda o conjunto. Aguarde sempre a conclusão antes de desligar.',AMBER,27)
c.save('01-performance-favoritos')
# BANK 2
c=Canvas();c.header('2','SPLIT / LAYER','Prepare duas configurações de teclado e alterne entre elas durante a performance.');c.hardware('2')
base=['PARTE\nA','PARTE\nB','PARTE\nC','PARTE\nD','SINGLE','SPLIT','LAYER']
c.pads(base+['TOCAR\nLINHA 1']+base+['TOCAR\nLINHA 2'],[BLUE,ORANGE,GREEN,PINK,WHITE,BLUE,RED,WHITE]*2,'Linha superior = configuração 1. Linha inferior = configuração 2. Pads 8 / 16 escolhem a ativa.')
c.card(100,1300,980,465,'1 · ESCOLHA AS DUAS PARTES','Na mesma linha, segure um pad A–D e pressione outro A–D. Isso cria um LAYER com duas partes ANALOG.\nA primeira parte será a região inferior no SPLIT; a segunda, a superior. A tecla do split pertence à região superior.\nDRUM-X não entra na combinação.',BLUE)
c.card(1120,1300,980,465,'2 · ESCOLHA COMO TOCAR','SINGLE — toca só a parte selecionada.\nSPLIT — uma parte abaixo do ponto de divisão; a outra, a partir dele.\nLAYER — as duas partes em cada tecla.\nPad 8 ativa a linha 1. Pad 16 ativa a linha 2. O seletor da linha ativa fica branco forte.',WHITE)
c.card(100,1810,980,565,'3 · CONTROLES DA TELA','K1 — SINGLE / SPLIT / LAYER.\nK2 / K3 — nível da primeira / segunda parte.\nK4 — ponto de split.\nK5 — alterna K2/K3 entre níveis e oitavas.\nK6 — parte selecionada, inclusive para REC.\nK7 — preset da parte selecionada.\nK8 — volume master.\nNíveis: 0–100%. Oitava da camada: −2 a +2, somada à oitava própria da parte.',BLUE)
c.card(1120,1810,980,565,'4 · EDITAR ≠ ATIVAR','Tocar os controles de uma linha seleciona essa linha para edição. Isso não a ativa para tocar. A tela mostra EDIT 1/2 e PLAY 1/2.\nUse 8 ou 16 para escolher qual combinação o teclado toca. Só uma linha fica ativa.\nOs níveis das camadas afetam o teclado ao vivo; não alteram o mixer das sequências. REC grava somente a parte selecionada.',AMBER)
c.note(2420,'SALVE O PATTERN PARA GUARDAR AS DUAS LINHAS','O pattern guarda as duas configurações, seus níveis e oitavas, ponto de split e qual linha está ativa. Use o banco 3 para salvar. Navegar entre bancos não salva.',h=180)
c.note(2640,'POLIFONIA E NOTAS SEGURADAS','As 8 vozes ANALOG são compartilhadas. Um LAYER usa duas partes por tecla; UNISON e sequências também consomem vozes. Notas seguradas mantêm seu destino ao trocar de linha.',BLUE,h=180)
c.save('02-split-layer')
# BANK 3
c=Canvas();c.header('3','CENAS E PATTERNS','Nome no firmware: PATTERNS / SAVE. Uma posição recupera o conjunto da performance.');c.hardware('3')
c.pads([f'PATTERN\n{i:02}' for i in range(1,17)],[BLUE if i==0 else RED for i in range(16)],'16 posições completas: pads superiores 1–8; pads inferiores 9–16. Cores ilustram um estado possível.')
c.card(100,1300,980,448,'1 · TOQUE PARA RECUPERAR','Com transporte parado, a troca é imediata.\nCom PLAY em execução, o pattern entra na próxima virada sincronizada. O pad de destino pisca azul enquanto aguarda.\nOs sons e o mixer acompanham a troca. A parte selecionada salva também retorna; o SPLIT/LAYER ativo define o teclado.',BLUE)
c.card(1120,1300,980,448,'2 · SEGURE PARA SALVAR','Segure o pad de destino por 0,7 s.\nSlot vazio: solicita a gravação do conjunto atual.\nSlot ocupado, inclusive o atual: abre OVERWRITE PATTERN e o pad pisca vermelho.\nÉ preciso soltar e tocar novamente no mesmo pad para confirmar.',RED)
c.card(100,1790,980,520,'3 · CONFIRMAÇÃO EM 4 SEGUNDOS','O segundo toque no mesmo pad confirma a substituição do que estava salvo.\nSem confirmação em até 4 s, cancela sem gravar. Tocar outro pad ou PATCH também cancela a confirmação.\nSe a gravação estiver pendente, pare o transporte e aguarde as notas terminarem. Só desligue após a tela indicar a conclusão.',AMBER)
c.card(1120,1790,980,520,'4 · O QUE VAI NO CONJUNTO','• Sequências das quatro tracks e automações.\n• Timbres das quatro partes e suas oitavas.\n• Mixer, sends e parâmetros de efeitos.\n• Duas configurações de SPLIT/LAYER e a linha ativa.\n• Parte selecionada para tocar / editar.\nA cena sonora acompanha o pattern: não é apenas um favorito de uma parte.',BLUE)
c.text(100,2360,'LEITURA DOS LEDs DOS PATTERNS',28,INK,True)
items=[(RED,'VERMELHO DISCRETO','Slot salvo, não selecionado'),(BLUE,'AZUL FORTE / MÉDIO','Selecionado / alterado'),(BLUE,'AZUL PISCANDO','Troca agendada'),(RED,'VERMELHO PISCANDO','Confirmação de overwrite')]
for i,(col,t,desc) in enumerate(items):
 x=100+i*505;c.rect(x,2420,30,30,col,rad=6);c.text(x,2475,t,20,col,True);c.wrap(x,2513,desc,455,25)
c.note(2630,'NÃO HÁ AUTOSAVE','Salvar presets por BT e salvar favoritos são ações separadas. Para guardar o conjunto de performance, salve o pattern pelo pad. Slot vazio: vermelho muito discreto. Gravação pendente: branco.',h=185)
c.save('03-cenas-patterns')
(OUT/'LEIA-ME.txt').write_text('SMKSYNTH — Infográficos dos três pad banks\n\nBase: fontes B2, SMK-37 Pro_19615; comportamento musical P16.2 aprovado.\nDiagramas funcionais; não são fotografias nem capturas da tela.\n\nMapas: obv_perf.h (pf_event, pf_recall, pf_toggle, keyboard_pad, pb_event, pb_led); obv_smk.h (roteamento dos pads); obv_keyboard_config.h; obervave.c (keyboard_turn, psnd_t, pat_go, snd_capture).\n\nO terceiro banco é PATTERNS / SAVE, apresentado como Cenas e patterns. A cena sonora acompanha o pattern. Configurações de teclado são extensões do pattern.\nArquivos PNG para compartilhar; SVG para edição. Cores indicam funções e estados qualitativos, não medições do brilho físico.\n\nCréditos: SMKSYNTH / Marcus Padrini; base sonora OberVave e DRUM-X do JIANT.\n')
with zipfile.ZipFile(OUT/'SMKSYNTH-Guia-dos-3-Pad-Banks.zip','w',zipfile.ZIP_DEFLATED) as z:
 for f in sorted(OUT.iterdir()):
  if f.suffix in {'.png','.svg','.txt','.py'}:z.write(f,f.name)
print('Created 3 PNGs, 3 editable SVGs and archive.')
