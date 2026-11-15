# SMKSYNTH 1.2.1 - Instalação e recuperação

O mesmo [instalador](../install/?lang=pt) atende SMK-37 Pro e SMK-37 Elite. Ele identifica o modelo e escolhe automaticamente um pacote específico. Use Chrome/Edge no computador em HTTPS, cabo USB de dados e alimentação estável; feche outros aplicativos MIDI.


## Preparação e cuidados

- **Primeira instalação:** o teclado precisa estar na V016 oficial do seu modelo. Se estiver em outra versão M-VAVE, use [M-UPGRADE para Windows](https://yms-file-store.oss-cn-hongkong.aliyuncs.com/software/pc/M-UPGRADE.zip) ou [macOS](https://yms-file-store.oss-cn-hongkong.aliyuncs.com/software/pc/M-UPGRADE.dmg), com a V016 [SMK-37 Pro](https://yms-file-store.oss-cn-hongkong.aliyuncs.com/software/firmware/SMK-37Pro/SMK-37%20Pro_016.fwsc) ou [SMK-37 Elite](https://yms-file-store.oss-cn-hongkong.aliyuncs.com/software/firmware/SMK-37Elite/SMK-37%20Elite_016.fwsc). Nunca troque os arquivos entre modelos. Se já estiver na V016 correta, não repita esse passo.
- **Já usa SMKSYNTH:** atualize diretamente pelo instalador. Não é necessário voltar à V016; esse retorno pode apagar seu banco. Salve suas edições e exporte um backup antes.
- **Durante a gravação:** nunca remova o cabo USB nem desligue o teclado. Não mude de aba, não feche nem recarregue a página e evite a suspensão do computador. Aguarde Concluído e a confirmação da identidade final. Uma pausa na etapa Loader não significa que a gravação terminou.


1. Conecte e consulte. O teclado só é acessado após clicar em Conectar.
2. Partindo da V016 oficial do modelo conectado, a página oferece a instalação inicial. Ela inicializa/substitui o banco com cinco patterns e oito favoritos.
3. Em versões SMKSYNTH compatíveis, a página oferece Atualizar, preservando programas, favoritos, patterns e configurações salvas. Salve as edições e exporte um [backup](../backup/?lang=pt) antes.
4. Confira o modelo e a operação, confirme e mantenha a conexão até a identidade de destino `SMK-37 Pro_19653` ou `SMK-37 Elite_19653` ser confirmada.
5. Conecte o pedal de sustain antes de reiniciar e mantenha-o solto na inicialização.

## Transferências interrompidas

Retome na mesma página/navegador. A operação pendente fica vinculada ao modelo, origem, SHA do pacote e tipo de operação. Um loader Pro não aceita pacote Elite e vice-versa. Um loader desconhecido sem operação pendente correspondente é recusado.

Para uma operação antiga interrompida, use o [instalador Pro 1.0.3 preservado](../install/legacy-1-0-3/) ou o [instalador Elite2 preservado](../install-elite/legacy-elite2/). Os pacotes e chaves de retomada originais foram mantidos. Os links Pro anteriores continuam na página principal. Não troque de versão durante uma restauração/atualização pendente.

## USB RESCUE

Após uma falha, somente quando não houver gravação em andamento, desligue, segure apenas OCTAVE − e ligue para tentar abrir USB RESCUE. Use M-UPGRADE com a V016 exata do modelo para retornar ao original; isso pode apagar os dados SMKSYNTH. Se o USB não for reconhecido, guarde o log e busque orientação, sem forçar arquivos de outro modelo.

## Retorno ao firmware oficial

Exporte o banco antes de voltar ao firmware M-VAVE. Use M-UPGRADE do fabricante com o arquivo oficial V016 exato do seu modelo. A página Elite2 preservada também mantém o retorno Elite V016 verificado. O retorno ao oficial pode substituir os dados SMKSYNTH. Reinstalar SMKSYNTH é uma instalação inicial; depois restaure o backup salvo.

## Depois de instalar

Veja no [manual](MANUAL.md) o roteamento USB/TRS, sustain físico e Panic por GLOBE + PLAY. A 1.2.1 inclui ENDLESS e MIDI MODE. Atualizar preserva o banco salvo e os registros separados de tema/mapas compatíveis. Para desfazer edições feitas em outra linha, restaure o backup anterior.


## Endless MIDI 1.2.1

Edição oficial: Endless MIDI, identidade 19653 para Pro e Elite. Inclui ENDLESS com MIDI USB/TRS por parte, seleção e layers de canais do teclado pelo banco B do MODO MIDI, rodas e sustain, bateria e identificação da versão. Pads A, encoders e faders mantêm os canais de seus mapas. Consulte o [manual atual](MANUAL.md) e as [notas da versão](RELEASE-1.2.1.md) para o fluxo completo. ML1 /19650, ML1a /19651 e Endless MIDI A /19652 atualizam diretamente, preservando os dados salvos.
