/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef SMK_ENCODER_FILTER_H
#define SMK_ENCODER_FILTER_H
/* Called only after the existing two-frame contact settling. One complete Gray
 * cycle per detent, with reversals cancelling and impossible two-bit transitions
 * discarding the partial cycle. No time gate or acceleration is added here. */
static int smk_encoder_delta(unsigned previous, unsigned current, int *partial)
{
    static const signed char direction[16] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
    if ((previous ^ current) == 3u) { *partial = 0; return 0; }
    *partial += direction[(previous << 2) | current];
    if (*partial >= 4) { *partial -= 4; return 1; }
    if (*partial <= -4) { *partial += 4; return -1; }
    return 0;
}
#endif
