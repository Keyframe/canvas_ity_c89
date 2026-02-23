/* canvas_ity WASM wrapper -- thin shim for Emscripten export.
 * Includes the single-header library and provides setter functions
 * for struct fields that JS cannot access through the opaque pointer.
 */
#include <emscripten.h>

#define CANVAS_ITY_IMPLEMENTATION
#include "canvas_ity.h"

/* Setters for direct struct fields (not covered by API functions) */

EMSCRIPTEN_KEEPALIVE
void ci_wasm_set_line_cap(ci_canvas_t *c, int v)
{
    c->line_cap = (ci_cap_style)v;
}

EMSCRIPTEN_KEEPALIVE
void ci_wasm_set_line_join(ci_canvas_t *c, int v)
{
    c->line_join = (ci_join_style)v;
}

EMSCRIPTEN_KEEPALIVE
void ci_wasm_set_shadow_offset_x(ci_canvas_t *c, float v)
{
    c->shadow_offset_x = v;
}

EMSCRIPTEN_KEEPALIVE
void ci_wasm_set_shadow_offset_y(ci_canvas_t *c, float v)
{
    c->shadow_offset_y = v;
}

EMSCRIPTEN_KEEPALIVE
void ci_wasm_set_line_dash_offset(ci_canvas_t *c, float v)
{
    c->line_dash_offset = v;
}

EMSCRIPTEN_KEEPALIVE
void ci_wasm_set_composite_op(ci_canvas_t *c, int v)
{
    c->global_composite_operation = (ci_composite_operation)v;
}

EMSCRIPTEN_KEEPALIVE
void ci_wasm_set_text_align(ci_canvas_t *c, int v)
{
    c->text_align = (ci_align_style)v;
}

EMSCRIPTEN_KEEPALIVE
void ci_wasm_set_text_baseline(ci_canvas_t *c, int v)
{
    c->text_baseline = (ci_baseline_style)v;
}

EMSCRIPTEN_KEEPALIVE
int ci_wasm_character_to_glyph(ci_canvas_t *c, char const *text, int *index)
{
    return ci_character_to_glyph(c, text, index);
}
