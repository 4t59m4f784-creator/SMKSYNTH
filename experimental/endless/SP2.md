# SUPER SP2 — controle explícito do ENDLESS

Base: SUPER SP1 (`68f2e40`). Identidade `SMK-37 Pro_19635`; versão `1.0.3-sp2`.

| Ação | Resultado |
|---|---|
| Entrar na página com ENDLESS OFF | Continua OFF; tocar continua normal |
| Girar RATE/GATE/DIR/HOLD enquanto OFF | Não ativa; indica PARA/REC |
| PARA na página com modo OFF | Ativa para tocar a frase existente |
| REC com transporte parado | Ativa e entra em gravação; preserva a frase existente e acrescenta ao final |
| Sair da página com modo ON | Continua ON |
| PLAY durante a reprodução | Para, mantendo o modo ON |
| PARA com modo ON | Desliga e para o transporte; mantém as notas |
| PARA novamente | Reativa a frase existente |
| Carregar um pattern salvo | Restaura a seleção ON/OFF desse pattern, como antes |

Não altera DSP, supersaw, oito vozes, transposição C4, quantização, formato do banco ou modo de salvamento. Mantém o comportamento anterior de PARA ao desligar (controles ENDLESS são reinicializados; notas preservadas). Não incorpora as novidades MIDI da oficial 1.1.0.

Validação automatizada: navegação por SEQ, reentrada após OFF, knobs, PARA ON/OFF, REC explícito, PLAY/retomada, frases intactas e recalls no loop real WASM. Regressão ENDLESS e comparação do áudio SP1. A confirmação física deste ajuste depende do teste no SMK-37 Pro.
