# Padrão permanente do instalador público SMKSYNTH

Aplicar em `web/install/` em todas as próximas versões, em português e inglês.

Após o título e a chamada, manter esta ordem:

1. **Novidades da versão**: box fixo, com número e nome da edição atual claros. Usar `RELEASE.json` como referência; não criar um box fixo separado para cada recurso.
2. **1. Antes de começar**: box expansível/recolhível, recolhido por padrão. Manter preparação V016 para primeira instalação, atualização direta para quem já usa SMKSYNTH, downloads oficiais por modelo e cuidados prévios.
3. **2. Conecte e instale**: box fixo. Manter os controles e os avisos de nunca remover USB/desligar nem mudar de aba durante a gravação sempre visíveis. Preservar IDs usados pelo controller e os controles de confirmação.
4. **Depois da instalação**: box expansível, recolhido por padrão. Cada dica deve contextualizar seu objetivo (por exemplo, “Para usar o pedal de sustain”, “Para usar o MODO MIDI”), com instruções práticas.
5. **Problemas na instalação? FAQ**: box expansível, recolhido por padrão, com retomada, recuperação e link para o guia.
6. Preservar os demais links, instaladores antigos para retomada, log, fontes, créditos e aviso de projeto experimental/responsabilidade no rodapé.

Usar `<details>`/`<summary>` nativos sem `open` nos três boxes recolhíveis. A estrutura e o estado inicial precisam ser iguais em PT/EN. Não adicionar confirmações ou checkboxes obrigatórios não solicitados.

Não reestruturar os instaladores arquivados: eles preservam operações interrompidas. Verificar o site montado com `python3 tools/check_public_web.py build/public-site`; esse verificador também valida o contrato de layout acima.
