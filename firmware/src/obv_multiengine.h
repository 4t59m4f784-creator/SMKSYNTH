/* SPDX-License-Identifier: GPL-3.0-only */
/* Experimental engine routing. Included after voice_t; no persistent layouts.
 * The ISR owns activation and state. Requests travel in the existing event ring.
 * Missing engines are rejected, never substituted or exposed as silent choices. */
enum { OBV_ENGINE_READY, OBV_ENGINE_OUT, OBV_ENGINE_IN };
typedef struct {
    const char *name;
    void (*start)(voice_t *v);          /* melodic card reinitialisation */
    void (*render)(voice_t *v, int32_t *stereo); /* exactly CTL frames */
    void (*hit)(uint32_t part, uint32_t note, uint32_t vel);
    uint32_t (*track_render)(uint32_t part, int32_t *stereo);
    void (*reset)(uint32_t part);
    uint32_t (*busy)(uint32_t part); /* independent hit tails also block flash erase */
} obv_engine_ops_t;
static const obv_engine_ops_t obv_analog_ops = {"ANALOG", 0, 0, 0, 0, 0};
static const obv_engine_ops_t *obv_engines[OBV_ENGINE_COUNT] = {&obv_analog_ops, 0, 0};
typedef struct { uint8_t active, target, phase; int32_t gain; } obv_engine_track_t;
static obv_engine_track_t obv_engine_track[NPART];
static uint8_t obv_voice_engine[NV];
static uint8_t obv_patch_seen[NPART], obv_flushing[NPART];
typedef struct { uint8_t note,vel,physical,sequenced,combined,external; } obv_pending_t;
static obv_pending_t obv_pending[NPART][NHELD];
static uint8_t obv_pending_n[NPART];
static int obv_engine_available(uint32_t id)
{
    return id < OBV_ENGINE_COUNT && obv_engines[id] != 0;
}
static void seq_engine_reset(uint32_t part);
static void obv_engine_accept(uint32_t part, uint32_t id)
{
    obv_engine_track_t *s = &obv_engine_track[part];
    if (!obv_engine_available(id)) return;
    if (s->active == id && s->phase == OBV_ENGINE_READY) return;
    s->target = (uint8_t)id;
    s->phase = OBV_ENGINE_OUT;
}
