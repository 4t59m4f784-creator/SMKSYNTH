# Proposta de publicação — revisão de Marcus

## Apresentação

Nome proposto: **SMKSYNTH**. Descrição: **Sintetizador de performance para o M-VAVE SMK-37 Pro**. Inglês no README principal, com versão em português. Crédito proposto: direção do projeto e workflow de performance por **Marcus Padrini**, com atribuição explícita ao OberVave e demais autores.

A apresentação atribui a base sonora ao OberVave e ao DRUM-X do JIANT. O texto de Marcus explica a proposta do SMKSYNTH pela experiência de performance, com os pads no centro, e acompanha os README em português e inglês.

O site reúne três ferramentas sem depender do ChatGPT: atualização de firmware, backup/restauração e simulador. As pastas relativas permitem hospedar tudo sob o caminho de um projeto GitHub Pages. Nesta revisão ele permanece no laboratório privado; nenhum endereço GitHub Pages é apresentado como publicado.

## O que já está pronto

- README em inglês/português, guia rápido, compatibilidade, instalação, backup e build.
- Créditos, licença original integral e registro de componentes.
- Changelog, orientação de contribuição e templates de issues/PRs.
- Site estático com as três ferramentas existentes, sem alterar o firmware 19615.
- Workflow manual de Pages e verificador de links, hashes e conteúdo publicado.
- Rascunho da primeira release, sem tag ou número público inventado.
- Fontes B2 e testes originais preservados, sem o banco pessoal exportado.

## Pendências antes de abrir ao público

1. Marcus revisa nome, autoria e textos desta proposta.
2. Repositório privado criado: https://github.com/4t59m4f784-creator/SMKSYNTH. Conteúdo apresentado na branch `prepare-publication` para revisão, antes de integrar à main.
3. Preparar e validar o caminho de primeira instalação a partir de uma origem de fábrica conhecida, incluindo retorno/recuperação documentados. O instalador B2 atual aceita somente 19613/19614/19615.
4. Receber a seleção `.smksynth` de Marcus. Implementar e testar banco inicial para novas instalações sem substituir dados de usuários existentes. O mecanismo de banco de fábrica ainda não está implementado por este pacote.
5. Definir tag/versão pública, compilar se houver mudança de firmware e vincular binário, fontes e hashes da mesma release.
6. Ativar o canal privado de vulnerabilidades no GitHub ou indicar um contato adequado.
7. Após autorização de Marcus, tornar o repositório público, publicar a release e habilitar Pages. O workflow só executa manualmente; não publica a cada push.

## Para Marcus verificar

- A descrição representa o que você quer apresentar aos músicos?
- O crédito “Marcus Padrini — direção do projeto e workflow de performance” está adequado?
- Inglês principal e português como segunda versão funcionam para o lançamento?
- A divisão Instalar / Backup / Simulador está clara?

## Validação desta preparação

O firmware B2 mantém seu SHA-256 original. A publicação não altera síntese, áudio, controles ou formato de banco. Foram preparados testes de apresentação/pacote e preservadas as evidências da B2. A aprovação física anterior confirma uso, exportação/restauração e melhora de velocidade; não confirma cortes reais de energia, novos modelos de hardware ou primeira instalação de fábrica.

Esta proposta é um material de revisão. Não cria uma release pública e não encerra as pendências acima.
