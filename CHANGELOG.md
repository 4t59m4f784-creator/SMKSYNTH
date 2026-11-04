# Changelog

## 1.0.1 — 2026-11-04

- GLOBAL: FADER MODE JUMP/CATCH. Em CATCH, o fader assume o parâmetro ao cruzar seu valor; a proteção é reativada ao mudar de contexto.
- Encoders relativos com filtragem de transições inválidas e proteção contra deltas pendentes após mudanças de contexto.
- Atualização direta do SMKSYNTH com preservação do banco.
- USB dedicado a MIDI, backup/restauração e atualizações; áudio pela saída física. Diagnósticos de desenvolvimento removidos.

### English

- GLOBAL: FADER MODE JUMP/CATCH. CATCH engages when the fader crosses the current value and rearms when its context changes.
- Relative encoders reject invalid transitions and stale deltas across context changes.
- Direct SMKSYNTH update preserves the saved bank.
- USB provides MIDI, backup/restore and firmware updates; record audio from the physical output. Development diagnostics removed.

## 1.0.0 — preparação do lançamento · 2026-11-02

- Base DP2 testada e aprovada no SMK‑37 Pro.
- Cinco patterns com cenas completas e oito favoritos de Marcus Padrini.
- Banco da base preservado; zero presets de usuário extras.
- Instalação inicial a partir da M‑VAVE V016; identidade estrita e retomada vinculada ao pacote.
- Simulador com o mesmo conteúdo; backup reconhece 19618.
- README, créditos, guias, fontes, hashes e evidências.

O pacote inicial foi validado em simulação. Primeira instalação física por USB ainda não confirmada. Checkpoints técnicos de origem permanecem documentados no repositório.
