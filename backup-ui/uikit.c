#include "uikit.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifndef PI
#define PI 3.14159265358979323846f
#endif


/* =========================================================
   Helpers
   ========================================================= */

static float clampf(float v, float lo, float hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static Uint8 bytef(float v)
{
    v = clampf(v, 0.0f, 255.0f);
    return (Uint8)v;
}

static SDL_FColor sdl_color(UK_Color c)
{
    return (SDL_FColor) {
        c.r / 255.0f,
        c.g / 255.0f,
        c.b / 255.0f,
        c.a / 255.0f
    };
}


/* =========================================================
   Theme
   ========================================================= */

static UK_Theme default_theme(void)
{
    UK_Theme t;

    memset(&t, 0, sizeof(t));

    t.bg_top =
        (UK_Color){ 5, 7, 18, 255 };

    t.bg_bottom =
        (UK_Color){ 15, 13, 34, 255 };

    t.orb1 =
        (UK_Color){ 100, 70, 255, 255 };

    t.orb2 =
        (UK_Color){ 30, 150, 255, 255 };

    t.orb3 =
        (UK_Color){ 230, 60, 150, 255 };

    t.orb4 =
        (UK_Color){ 50, 220, 160, 255 };


    t.text =
        (UK_Color){ 235, 239, 248, 255 };

    t.text_dim =
        (UK_Color){ 145, 150, 170, 255 };

    t.text_bright =
        (UK_Color){ 255, 255, 255, 255 };


    t.accent =
        (UK_Color){ 105, 155, 255, 255 };

    t.accent2 =
        (UK_Color){ 150, 190, 255, 255 };

    t.success =
        (UK_Color){ 70, 210, 145, 255 };

    t.danger =
        (UK_Color){ 255, 85, 105, 255 };


    /*
       These values deliberately use low alpha.
       The background remains visible through the glass.
    */

    t.glass =
        (UK_Color){ 255, 255, 255, 24 };

    t.glass_tint =
        (UK_Color){ 175, 195, 255, 12 };

    t.glass_border =
        (UK_Color){ 255, 255, 255, 75 };

    t.glass_highlight =
        (UK_Color){ 255, 255, 255, 120 };

    t.glass_shadow =
        (UK_Color){ 0, 0, 0, 80 };


    t.slider_track =
        (UK_Color){ 255, 255, 255, 35 };

    t.slider_fill =
        (UK_Color){ 110, 165, 255, 230 };

    t.slider_knob =
        (UK_Color){ 250, 252, 255, 255 };


    t.toggle_off =
        (UK_Color){ 255, 255, 255, 32 };

    t.toggle_on =
        (UK_Color){ 75, 205, 145, 235 };

    t.toggle_knob =
        (UK_Color){ 250, 252, 255, 255 };


    t.radio_off =
        (UK_Color){ 255, 255, 255, 30 };

    t.radio_on =
        (UK_Color){ 110, 165, 255, 235 };


    t.check_off =
        (UK_Color){ 255, 255, 255, 30 };

    t.check_on =
        (UK_Color){ 110, 165, 255, 235 };


    t.button =
        (UK_Color){ 255, 255, 255, 30 };

    t.button_hover =
        (UK_Color){ 255, 255, 255, 55 };

    t.button_pressed =
        (UK_Color){ 255, 255, 255, 18 };

    t.button_text =
        (UK_Color){ 242, 245, 252, 255 };


    t.corner = 18.0f;
    t.spacing = 7.0f;

    return t;
}


/* =========================================================
   Core
   ========================================================= */

UK uk_make(SDL_Renderer *ren, TTF_Font *font)
{
    UK uk;

    memset(&uk, 0, sizeof(uk));

    uk.ren = ren;
    uk.font = font;

    uk.th = default_theme();

    uk.scale = 1.0f;

    return uk;
}


void uk_begin(
    UK *uk,
    float mx,
    float my,
    bool md,
    bool mp,
    bool mr,
    float dt,
    int sw,
    int sh
)
{
    uk->mx = mx;
    uk->my = my;

    uk->mdown = md;
    uk->mpress = mp;
    uk->mrelease = mr;

    uk->dt = clampf(dt, 0.0f, 0.05f);
    uk->time += uk->dt;

    uk->win_w = sw;
    uk->win_h = sh;


    /*
       Scale based on a 800x700 reference design.

       This prevents the UI becoming enormous on wide
       screens while still making it responsive.
    */

    float sx = (float)sw / 800.0f;
    float sy = (float)sh / 700.0f;

    uk->scale = fminf(sx, sy);

    if (uk->scale < 0.65f)
        uk->scale = 0.65f;

    if (uk->scale > 1.65f)
        uk->scale = 1.65f;


    uk->cx = 0;
    uk->cy = 0;
    uk->cw = (float)sw;
}


void uk_end(UK *uk)
{
    (void)uk;
}


void uk_shutdown(UK *uk)
{
    /*
       No persistent textures are owned by this implementation.
       Therefore there is nothing to destroy here.
    */

    (void)uk;
}


/* =========================================================
   Layout
   ========================================================= */

void uk_set_pos(UK *uk, float x, float y)
{
    uk->cx = x;
    uk->cy = y;
}

void uk_set_width(UK *uk, float w)
{
    uk->cw = w;
}

void uk_indent(UK *uk, float dx)
{
    uk->cx += dx;
    uk->cw -= dx;
}

float uk_scale(UK *uk, float v)
{
    return v * uk->scale;
}


/* =========================================================
   Utility
   ========================================================= */

UK_Color uk_rgba(float r, float g, float b, float a)
{
    return (UK_Color){ r, g, b, a };
}

UK_WidgetState *uk_state(UK *uk, unsigned id)
{
    return &uk->pool.states[id % UK_STATE_MAX];
}


/* =========================================================
   Rounded rectangle
   ========================================================= */

/*
   Dynamic allocation is intentional.

   Earlier versions used arrays sized for one particular
   tessellation count and then wrote past them, causing:

       *** stack smashing detected ***

   This implementation allocates exactly what it needs.
*/

static void rounded_geometry(
    SDL_Renderer *ren,
    UK_Rect r,
    float radius,
    UK_Color color
)
{
    if (r.w <= 0 || r.h <= 0)
        return;

    radius = clampf(
        radius,
        0.0f,
        fminf(r.w, r.h) * 0.5f
    );

    if (radius < 0.5f) {
        SDL_FRect rr = {
            r.x,
            r.y,
            r.w,
            r.h
        };

        SDL_SetRenderDrawColor(
            ren,
            bytef(color.r),
            bytef(color.g),
            bytef(color.b),
            bytef(color.a)
        );

        SDL_RenderFillRect(ren, &rr);
        return;
    }


    const int segments = 16;

    /*
       Four corners + center.

       4 * (segments + 1) boundary vertices.
    */

    const int boundary = 4 * (segments + 1);
    const int vertex_count = boundary + 1;
    const int index_count = boundary * 3;

    SDL_Vertex *vertices =
        malloc(sizeof(SDL_Vertex) * vertex_count);

    int *indices =
        malloc(sizeof(int) * index_count);

    if (!vertices || !indices) {
        free(vertices);
        free(indices);
        return;
    }


    SDL_FColor fc = sdl_color(color);

    vertices[0].position.x =
        r.x + r.w * 0.5f;

    vertices[0].position.y =
        r.y + r.h * 0.5f;

    vertices[0].color = fc;
    vertices[0].tex_coord.x = 0;
    vertices[0].tex_coord.y = 0;


    /*
       Clockwise corner centers.
    */

    float centers_x[4] = {
        r.x + radius,
        r.x + r.w - radius,
        r.x + r.w - radius,
        r.x + radius
    };

    float centers_y[4] = {
        r.y + radius,
        r.y + radius,
        r.y + r.h - radius,
        r.y + r.h - radius
    };

    float start_angle[4] = {
        PI,
        -PI * 0.5f,
        0.0f,
        PI * 0.5f
    };


    int v = 1;

    for (int corner = 0; corner < 4; ++corner) {

        for (int i = 0; i <= segments; ++i) {

            float a =
                start_angle[corner] +
                (float)i /
                (float)segments *
                (PI * 0.5f);

            float px =
                centers_x[corner] +
                cosf(a) * radius;

            float py =
                centers_y[corner] +
                sinf(a) * radius;

            vertices[v].position.x = px;
            vertices[v].position.y = py;
            vertices[v].color = fc;
            vertices[v].tex_coord.x = 0;
            vertices[v].tex_coord.y = 0;

            ++v;
        }
    }


    int ic = 0;

    for (int i = 0; i < boundary; ++i) {

        int a = i + 1;
        int b = ((i + 1) % boundary) + 1;

        indices[ic++] = 0;
        indices[ic++] = a;
        indices[ic++] = b;
    }


    SDL_RenderGeometry(
        ren,
        NULL,
        vertices,
        vertex_count,
        indices,
        index_count
    );

    free(vertices);
    free(indices);
}


/* =========================================================
   Basic drawing
   ========================================================= */

void uk_rect_fill(
    UK *uk,
    UK_Rect r,
    UK_Color c
)
{
    SDL_SetRenderDrawColor(
        uk->ren,
        bytef(c.r),
        bytef(c.g),
        bytef(c.b),
        bytef(c.a)
    );

    SDL_FRect rr = {
        r.x,
        r.y,
        r.w,
        r.h
    };

    SDL_RenderFillRect(
        uk->ren,
        &rr
    );
}


void uk_rect_fill_r(
    UK *uk,
    UK_Rect r,
    float radius,
    UK_Color c
)
{
    rounded_geometry(
        uk->ren,
        r,
        radius,
        c
    );
}


/* =========================================================
   Rounded stroke
   ========================================================= */

void uk_rect_stroke_r(
    UK *uk,
    UK_Rect r,
    float radius,
    UK_Color c,
    float thickness
)
{
    if (r.w <= 0 || r.h <= 0 || thickness <= 0)
        return;

    radius = clampf(
        radius,
        0,
        fminf(r.w, r.h) * 0.5f
    );

    const int segments = 16;
    const int count = 4 * (segments + 1);

    SDL_Vertex *vertices =
        malloc(sizeof(SDL_Vertex) * count * 2);

    int *indices =
        malloc(sizeof(int) * count * 6);

    if (!vertices || !indices) {
        free(vertices);
        free(indices);
        return;
    }


    SDL_FColor fc = sdl_color(c);

    float cx[4] = {
        r.x + radius,
        r.x + r.w - radius,
        r.x + r.w - radius,
        r.x + radius
    };

    float cy[4] = {
        r.y + radius,
        r.y + radius,
        r.y + r.h - radius,
        r.y + r.h - radius
    };

    float angles[4] = {
        PI,
        -PI * 0.5f,
        0,
        PI * 0.5f
    };


    float half = thickness * 0.5f;

    int vn = 0;

    for (int q = 0; q < 4; ++q) {

        for (int i = 0; i <= segments; ++i) {

            float a =
                angles[q] +
                (float)i /
                (float)segments *
                (PI * 0.5f);

            float co = cosf(a);
            float si = sinf(a);

            /*
               Outer ring.
            */

            vertices[vn].position.x =
                cx[q] + co * (radius + half);

            vertices[vn].position.y =
                cy[q] + si * (radius + half);

            vertices[vn].color = fc;

            ++vn;


            /*
               Inner ring.
            */

            float inner =
                fmaxf(radius - half, 0.01f);

            vertices[vn].position.x =
                cx[q] + co * inner;

            vertices[vn].position.y =
                cy[q] + si * inner;

            vertices[vn].color = fc;

            ++vn;
        }
    }


    int pairs = count;
    int ic = 0;

    for (int i = 0; i < pairs; ++i) {

        int next =
            (i + 1) % pairs;

        int outer0 = i * 2;
        int inner0 = i * 2 + 1;

        int outer1 = next * 2;
        int inner1 = next * 2 + 1;


        indices[ic++] = outer0;
        indices[ic++] = inner0;
        indices[ic++] = outer1;

        indices[ic++] = inner0;
        indices[ic++] = inner1;
        indices[ic++] = outer1;
    }


    SDL_RenderGeometry(
        uk->ren,
        NULL,
        vertices,
        vn,
        indices,
        ic
    );

    free(vertices);
    free(indices);
}


/* =========================================================
   Circle
   ========================================================= */

void uk_circle(
    UK *uk,
    float cx,
    float cy,
    float radius,
    UK_Color c
)
{
    if (radius <= 0)
        return;

    const int segments = 32;

    SDL_Vertex vertices[34];
    int indices[96];

    SDL_FColor fc = sdl_color(c);

    vertices[0].position.x = cx;
    vertices[0].position.y = cy;
    vertices[0].color = fc;


    for (int i = 0; i <= segments; ++i) {

        float a =
            (float)i /
            (float)segments *
            PI * 2.0f;

        vertices[i + 1].position.x =
            cx + cosf(a) * radius;

        vertices[i + 1].position.y =
            cy + sinf(a) * radius;

        vertices[i + 1].color = fc;
    }


    int ic = 0;

    for (int i = 0; i < segments; ++i) {

        indices[ic++] = 0;
        indices[ic++] = i + 1;
        indices[ic++] = i + 2;
    }


    SDL_RenderGeometry(
        uk->ren,
        NULL,
        vertices,
        segments + 2,
        indices,
        ic
    );
}


/* =========================================================
   Line
   ========================================================= */

void uk_line(
    UK *uk,
    float x0,
    float y0,
    float x1,
    float y1,
    UK_Color c,
    float thickness
)
{
    float dx = x1 - x0;
    float dy = y1 - y0;

    float len =
        sqrtf(dx * dx + dy * dy);

    if (len < 0.001f)
        return;

    float nx =
        -dy / len * thickness * 0.5f;

    float ny =
        dx / len * thickness * 0.5f;


    SDL_FColor fc = sdl_color(c);

    /*
       Correct SDL3 SDL_Vertex initialization.

       position is SDL_FPoint, not a scalar.
    */

    SDL_Vertex vertices[4];

    vertices[0].position =
        (SDL_FPoint){ x0 + nx, y0 + ny };

    vertices[0].color = fc;

    vertices[1].position =
        (SDL_FPoint){ x0 - nx, y0 - ny };

    vertices[1].color = fc;

    vertices[2].position =
        (SDL_FPoint){ x1 + nx, y1 + ny };

    vertices[2].color = fc;

    vertices[3].position =
        (SDL_FPoint){ x1 - nx, y1 - ny };

    vertices[3].color = fc;


    int indices[6] = {
        0, 1, 2,
        1, 3, 2
    };


    SDL_RenderGeometry(
        uk->ren,
        NULL,
        vertices,
        4,
        indices,
        6
    );
}


/* =========================================================
   Background
   ========================================================= */

static void draw_gradient(
    UK *uk
)
{
    int w = uk->win_w;
    int h = uk->win_h;

    if (h <= 0)
        return;


    for (int y = 0; y < h; ++y) {

        float t =
            (float)y /
            (float)(h - 1);

        UK_Color c =
            uk_color_lerp(
                uk->th.bg_top,
                uk->th.bg_bottom,
                t
            );

        SDL_SetRenderDrawColor(
            uk->ren,
            bytef(c.r),
            bytef(c.g),
            bytef(c.b),
            255
        );

        SDL_RenderLine(
            uk->ren,
            0,
            (float)y,
            (float)w,
            (float)y
        );
    }
}


/*
   Soft ambient circles.

   SDL's geometry renderer does not provide a Gaussian blur,
   so we approximate volumetric light with many translucent
   concentric circles.
*/

static void ambient_orb(
    UK *uk,
    float x,
    float y,
    float radius,
    UK_Color color
)
{
    const int layers = 20;

    for (int i = layers; i >= 1; --i) {

        float t =
            (float)i /
            (float)layers;

        float r =
            radius * t;

        float alpha =
            1.8f * (1.0f - t) *
            255.0f;

        if (alpha > 18)
            alpha = 18;

        UK_Color c = color;
        c.a = alpha;

        uk_circle(
            uk,
            x,
            y,
            r,
            c
        );
    }
}


void uk_bg_render(UK *uk)
{
    draw_gradient(uk);

    float w = (float)uk->win_w;
    float h = (float)uk->win_h;

    /*
       Slow animation keeps the background alive.
    */

    float t = uk->time;

    ambient_orb(
        uk,
        w * 0.18f + sinf(t * 0.18f) * 30.0f,
        h * 0.20f,
        190.0f * uk->scale,
        uk->th.orb1
    );

    ambient_orb(
        uk,
        w * 0.82f + cosf(t * 0.14f) * 35.0f,
        h * 0.28f,
        170.0f * uk->scale,
        uk->th.orb2
    );

    ambient_orb(
        uk,
        w * 0.58f,
        h * 0.72f + sinf(t * 0.16f) * 25.0f,
        200.0f * uk->scale,
        uk->th.orb3
    );

    ambient_orb(
        uk,
        w * 0.20f,
        h * 0.80f,
        140.0f * uk->scale,
        uk->th.orb4
    );
}


void uk_bg_draw(UK *uk)
{
    uk_bg_render(uk);
}


/* =========================================================
   Liquid glass
   ========================================================= */

void uk_glass_surface(
    UK *uk,
    UK_Rect r
)
{
    float s = uk->scale;

    float radius =
        22.0f * s;


    /*
       Large soft shadow.
    */

    for (int i = 8; i >= 1; --i) {

        float spread =
            i * 2.0f * s;

        float alpha =
            3.0f + (8 - i) * 1.5f;

        UK_Color shadow =
            uk->th.glass_shadow;

        shadow.a = alpha;

        uk_rect_fill_r(
            uk,
            (UK_Rect){
                r.x - spread * 0.5f,
                r.y + spread,
                r.w + spread,
                r.h + spread * 0.7f
            },
            radius + spread,
            shadow
        );
    }


    /*
       Main translucent body.

       This is intentionally not an opaque panel.
    */

    uk_rect_fill_r(
        uk,
        r,
        radius,
        uk->th.glass
    );


    /*
       Cool optical tint near the top.
    */

    UK_Color tint =
        uk->th.glass_tint;

    for (int i = 0; i < 8; ++i) {

        float t =
            (float)i / 7.0f;

        tint.a =
            10.0f * (1.0f - t);

        uk_rect_fill_r(
            uk,
            (UK_Rect){
                r.x,
                r.y + r.h * t * 0.45f,
                r.w,
                r.h * 0.09f
            },
            radius,
            tint
        );
    }


    /*
       Top specular edge.

       This is the important visual cue that separates
       translucent glass from a Windows-style flat panel.
    */

    uk_rect_fill_r(
        uk,
        (UK_Rect){
            r.x + radius * 1.2f,
            r.y + 1.0f,
            r.w - radius * 2.4f,
            2.0f * s
        },
        2.0f * s,
        uk->th.glass_highlight
    );


    /*
       Thin outer rim.
    */

    uk_rect_stroke_r(
        uk,
        r,
        radius,
        uk->th.glass_border,
        1.0f * s
    );
}


void uk_glass_begin(
    UK *uk,
    float x,
    float y,
    float w,
    float h
)
{
    uk_glass_surface(
        uk,
        (UK_Rect){ x, y, w, h }
    );


    float pad =
        18.0f * uk->scale;

    uk->cx = x + pad;
    uk->cy = y + pad;
    uk->cw = w - pad * 2.0f;
}


void uk_glass_end(
    UK *uk,
    float x,
    float y,
    float w,
    float h
)
{
    (void)uk;
    (void)x;
    (void)y;
    (void)w;
    (void)h;
}


/* =========================================================
   Text
   ========================================================= */

float uk_text_w(
    TTF_Font *font,
    const char *text
)
{
    if (!font || !text || !*text)
        return 0.0f;

    int w = 0;

    if (!TTF_GetStringSize(
            font,
            text,
            0,
            &w,
            NULL))
        return 0.0f;

    return (float)w;
}


int uk_text_h(
    TTF_Font *font
)
{
    if (!font)
        return 16;

    int h = 16;

    TTF_GetStringSize(
        font,
        "Ag",
        0,
        NULL,
        &h
    );

    return h > 0 ? h : 16;
}


void uk_text_font(
    UK *uk,
    TTF_Font *font,
    const char *text,
    float x,
    float y,
    UK_Color color
)
{
    if (!font || !text || !*text)
        return;


    SDL_Color c = {
        bytef(color.r),
        bytef(color.g),
        bytef(color.b),
        bytef(color.a)
    };


    SDL_Surface *surface =
        TTF_RenderText_Blended(
            font,
            text,
            0,
            c
        );

    if (!surface)
        return;


    SDL_Texture *texture =
        SDL_CreateTextureFromSurface(
            uk->ren,
            surface
        );

    if (texture) {

        SDL_SetTextureBlendMode(
            texture,
            SDL_BLENDMODE_BLEND
        );

        SDL_FRect dst = {
            x,
            y,
            (float)surface->w,
            (float)surface->h
        };

        SDL_RenderTexture(
            uk->ren,
            texture,
            NULL,
            &dst
        );

        SDL_DestroyTexture(texture);
    }

    SDL_DestroySurface(surface);
}


void uk_text(
    UK *uk,
    const char *text,
    float x,
    float y,
    UK_Color color
)
{
    uk_text_font(
        uk,
        uk->font,
        text,
        x,
        y,
        color
    );
}
