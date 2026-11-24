# SMKSTAGE B3 — 0.1.1 isolated crash forensic branch

**DO NOT FLASH YET.** This branch is based exactly on the last user-verified playable SMKSTAGE Alpha 0.1.1, commit `d8a86ef44fd396886d38b629477cb779be35d3f1`. No new Dattorro reverb, flash persistence or 0.1.2 work is included. Production/rollback installer is unchanged.

## Recorded crash on physical SMK-37 Pro

Photo: `VEC=00000001`, `PC=030004A8`, `RETS=02003188`.

Exact 0.1.1 binary was independently reconstructed in GitHub Actions `45328541818` (same source commit and build command); full `smkstage.elf` and `smkstage.dis` are retained as forensic artifacts.

Symbolication:
- `0x020004A6`: `isr_timer5` wrapper (10 kHz keyboard/encoder scan). `0x020004A8` is its second instruction (`[--sp] = {r3-r0}`).
- Fault `0x030004A8` has **exactly bit 24** different from valid `0x020004A8`. It is outside the XIP and RAM-code valid instruction windows. This pattern supports erroneous vector fetch, program-counter corruption, or interrupted state, but does **not prove** which without DBG/EMU/vector snapshots.
- `0x02003188`: inside `fm1_alnk0_irq` while core 0 publishes the second core's tonewheel job check word; the 10 kHz timer interrupts audio (by design).

## Diagnostic changes on this branch

1. Additional crash fields saved when exception is raised: actual interrupt vector 63 and 11, plus all existing DBG, EMU, SP. Screen shows `VEC`, `PC`, `RETS`, `DBG`, `EMU`, `T63`, `SP` for photos.
2. During normal drawbar turns on PERFORMANCE screen, re-render **only the affected 26 x 124 pixel lane** instead of the entire 240 x 124 pixel canvas (the prior path transferred ~60 KB vs ~6.4 KB per lane). Full screen remains on page/manual/bank transitions. Reduced cross-traffic should lower risk of display DMA contention under high-rate encoder movement.
3. No modification to CPU1 job scheduling, audio ISR priorities, flash map, memory persistence or users' known-good installer.

## Next steps

- Confirm Jiēlì cross-build and static checks without deployment.
- Independently verify high-rate encoder/pad interactions on hardware only with explicit user acceptance, retaining original 0.1.1 recovery.
- If the problem reproduces, compare crash `T63` to the expected IRQ wrapper address from the *diagnostic build*, and `DBG/EMU/SP` to hardware guard bits.
- Avoid attributing the cause to drawbar processing until vector corruption vs PC/return corruption is established.

Source: `firmware/src/smkstage.c`, `firmware/src/platform.c`, `firmware/hal/fm1_irq.h`.
