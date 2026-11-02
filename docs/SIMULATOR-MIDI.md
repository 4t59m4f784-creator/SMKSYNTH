# Entrada MIDI do simulador / Simulator MIDI input

## Português

1. Conecte um teclado MIDI USB ao computador. Para uma saída MIDI DIN, use uma interface MIDI USB.
2. Abra o [simulador](https://4t59m4f784-creator.github.io/SMKSYNTH/simulator/) em um navegador com Web MIDI, como Chrome ou Edge no computador.
3. Clique em **Iniciar simulador** para ativar o áudio.
4. Clique em **Conectar teclado MIDI** e autorize o acesso solicitado pelo navegador.
5. Se houver uma única entrada, ela será selecionada. Se houver várias, escolha a porta do teclado em **Entrada MIDI**.
6. Em GLOBAL, use **MIDI CH = OMNI** para tocar a parte selecionada por K6 e a configuração Split/Layer ativa. Se escolher um canal base, as notas seguem o roteamento MIDI do firmware para as quatro partes.

A entrada recebe Note On/Off com velocity, sustain (CC64), pitch bend e modulação (CC1). Note On com velocity zero é tratado como Note Off. A nota MIDI é preservada, inclusive fora do teclado virtual; para mudar a oitava enviada, use o controle de oitavas do teclado externo. As oitavas das linhas Split/Layer continuam seguindo o firmware. Notas MIDI podem ser gravadas pelos modos de gravação do simulador.

**Soltar tudo**, trocar a entrada, desconectar o dispositivo ou sair da janela libera as notas e o sustain. Ao ocultar a página, novas notas são ignoradas até ela ficar visível novamente. A saída de áudio é a do computador; não há envio MIDI ao teclado físico. Esta integração não recebe Program Change, clock, transporte ou outros CCs.

A interface mostra uma orientação se o navegador não oferecer Web MIDI ou se a permissão for negada. Sem um teclado detectado, conecte-o e selecione a entrada; você também pode continuar usando mouse, toque e teclado do computador.

O simulador usa memória própria do navegador e não instala nem altera o firmware do teclado conectado.

## English

1. Connect a USB MIDI keyboard to your computer. For MIDI DIN output, use a USB MIDI interface.
2. Open the [simulator](https://4t59m4f784-creator.github.io/SMKSYNTH/simulator/?lang=en) in a Web MIDI browser, such as desktop Chrome or Edge.
3. Click **Start simulator** to activate audio.
4. Click **Connect MIDI keyboard** and grant the browser's requested permission.
5. A single input is selected automatically. If several are available, choose the keyboard's port under **MIDI input**.
6. In GLOBAL, use **MIDI CH = OMNI** to play the part selected with K6 and the active Split/Layer configuration. With a base MIDI channel selected, notes follow the firmware's four-part MIDI routing.

Input supports Note On/Off with velocity, sustain (CC64), pitch bend and modulation (CC1). Note On with zero velocity is treated as Note Off. MIDI note numbers are preserved, including notes outside the virtual keyboard; use the external keyboard's octave controls to change the transmitted octave. Split/Layer row octave shifts still follow the firmware. Incoming MIDI notes can be recorded using the simulator's recording modes.

**Release all**, switching inputs, unplugging the device or leaving the window releases notes and sustain. When the page is hidden, new notes are ignored until it becomes visible again. Audio plays through the computer; no MIDI is sent to the physical keyboard. This integration does not receive Program Change, clock, transport or other CC messages.

The interface provides guidance when Web MIDI is unavailable or permission is denied. If no keyboard is detected, connect it and choose its input; mouse, touch and computer-keyboard controls remain available.

The simulator uses its own browser memory and does not install or alter the connected keyboard's firmware.
