JIANT e924445ae0e7c64636f15bd0758418ee312ce7ee — GPL-3.0-only.

Os fixtures mantêm os corpos originais de render/glide e dynamics, com tipos/símbolos adaptados e caminhos WORDS/LOWCUT opcionais excluídos.
As tabelas de seno, helpers ratio/vowel e tanh são compartilhados com o porte, previamente auditados; esta suíte não é uma implementação independente desses helpers. Os envelopes de referência são comparados pelo teste de controle.

SHA256 dos fontes fixados:

f3bb643947d1d80e46aeb23985f0f61cefc02c222952a06d79f79153acdf01e7  dsp.c
d81eda926fe9661d052276b73c4c4febc5affb3e7976f70b56021190ed01cb01  eng_formant.c
992d1b4a6870bc8ecac390e1907c852111394b1147003ade0cd2be9064e618b5  fx.c
a1896486ffa6605c5ccb1dfbccac2e5c79e96e3a65cc938a5df52a99c15e3ad8  gen_tables.py
