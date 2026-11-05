# SMKSYNTH 1.0.3-ui1 — atualização do display

Teste isolado, identidade SMK-37 Pro_19622. Não substitui a versão pública 1.0.3.

Correção: consumir dirty_steps quando uma página comum é desenhada, antes do desenho para preservar eventos recebidos durante a transferência. O sequenciador, REC e mixer conservam seus próprios consumidores. Antes, dirty_steps permanecia ativo e rearmava dirty_page em cada volta do loop, causando redesenho contínuo em silêncio (800160 pixels/s só em blits na tela Split/Layer no simulador).

Não altera o DSP SUPER, efeitos, áudio USB, presets ou formatos salvos. Atualização e retorno usam loader com preservação do banco. Retorno incluído é a versão pública 1.0.3 original.

Teste no teclado: comparar KNOB BANK, vistas SEQ e retorno PATCH, imediatamente e após 3–10 minutos em silêncio. Depois conferir edição, notas, rodas, faders e sequenciador. A limpeza completa nas trocas permanece nesta etapa; o foco é corrigir o redesenho contínuo. A hipótese de piora adicional no hardware com o tempo permanece em investigação.

Origem da SUPER: [Clipper / 3dSam](https://github.com/isod89/clipper-fm1). Base ANALOG: OberVave / zvenson. Direção e testes: Marcus Padrini. Adaptação e documentação com ChatGPT / OpenAI. GPL-3.0-only, veja LICENSING.md.
