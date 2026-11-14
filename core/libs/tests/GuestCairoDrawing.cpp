#include "prx/libcairo/libcairo.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace {

constexpr int ArgB32 = 0;
constexpr std::uint32_t OpaqueRed = 0xffff0000;
constexpr std::uint32_t OpaqueGreen = 0xff00ff00;
constexpr std::uint32_t OpaqueBlue = 0xff0000ff;
constexpr std::uint32_t HalfGreen = 0x80008000;
constexpr std::uint32_t Transparent = 0x00000000;

[[noreturn]] void Fail(const char* message) {
    std::fprintf(stderr, "%s\n", message);
    std::abort();
}

void Require(bool condition, const char* message) {
    if (!condition) Fail(message);
}

std::uint32_t Pixel(CairoSurface* surface, int x, int y) {
    cairo_surface_flush(surface);
    const auto* data = cairo_image_surface_get_data(surface);
    const int stride = cairo_image_surface_get_stride(surface);
    std::uint32_t value = 0;
    std::memcpy(&value, data + y * stride + x * 4, sizeof(value));
    return value;
}

struct SolidSource {
    std::vector<std::uint32_t> pixels;
    CairoSurface* surface = nullptr;

    SolidSource(int width, int height, std::uint32_t color) : pixels(static_cast<std::size_t>(width) * height, color) {
        surface = cairo_image_surface_create_for_data(reinterpret_cast<unsigned char*>(pixels.data()), ArgB32, width, height, width * 4);
        Require(cairo_surface_status(surface) == 0, "the solid source surface was not created");
    }

    ~SolidSource() {
        cairo_surface_destroy(surface);
    }
};

CairoSurface* CreateTarget(int width, int height) {
    CairoSurface* surface = cairo_image_surface_create(ArgB32, width, height);
    Require(cairo_surface_status(surface) == 0, "the target surface was not created");
    return surface;
}

void UseSolidSource(CairoContext* context, CairoSurface* source) {
    CairoPattern* pattern = cairo_pattern_create_for_surface(source);
    Require(pattern != nullptr, "the solid source pattern was not created");
    cairo_set_source(context, pattern);
}

void CheckPaint() {
    CairoSurface* target = CreateTarget(8, 8);
    CairoContext* context = cairo_create(target);
    SolidSource red(8, 8, OpaqueRed);
    UseSolidSource(context, red.surface);
    cairo_paint(context);
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) Require(Pixel(target, x, y) == OpaqueRed, "paint must cover every pixel");
    }
    cairo_destroy(context);
    cairo_surface_destroy(target);
}

void CheckPaintWithAlpha() {
    CairoSurface* target = CreateTarget(4, 4);
    CairoContext* context = cairo_create(target);
    SolidSource green(4, 4, OpaqueGreen);
    UseSolidSource(context, green.surface);
    cairo_paint_with_alpha(context, 0.5);
    Require(Pixel(target, 0, 0) == HalfGreen, "paint_with_alpha must apply the alpha to the source");
    cairo_paint_with_alpha(context, 0.0);
    Require(Pixel(target, 0, 0) == HalfGreen, "paint_with_alpha with a zero alpha must not paint");
    cairo_paint_with_alpha(context, 1.0);
    Require(Pixel(target, 0, 0) == OpaqueGreen, "paint_with_alpha with a full alpha must paint the source");
    cairo_destroy(context);
    cairo_surface_destroy(target);
}

void CheckFillAndPreserve() {
    CairoSurface* target = CreateTarget(8, 8);
    CairoContext* context = cairo_create(target);
    SolidSource green(8, 8, OpaqueGreen);
    UseSolidSource(context, green.surface);
    cairo_rectangle(context, 2, 2, 4, 4);
    cairo_fill(context);
    Require(Pixel(target, 3, 3) == OpaqueGreen, "fill must paint the path interior");
    Require(Pixel(target, 0, 0) == Transparent, "fill must leave the exterior untouched");

    cairo_rectangle(context, 1, 1, 2, 2);
    cairo_fill_preserve(context);
    SolidSource blue(8, 8, OpaqueBlue);
    UseSolidSource(context, blue.surface);
    cairo_fill(context);
    Require(Pixel(target, 1, 1) == OpaqueBlue, "fill_preserve must keep the path for the next fill");

    cairo_destroy(context);
    cairo_surface_destroy(target);
}

void CheckStrokeAndPreserve() {
    CairoSurface* target = CreateTarget(9, 9);
    CairoContext* context = cairo_create(target);
    SolidSource green(9, 9, OpaqueGreen);
    UseSolidSource(context, green.surface);
    cairo_set_line_width(context, 2);
    cairo_move_to(context, 4, 0);
    cairo_line_to(context, 4, 8);
    cairo_stroke(context);
    Require(Pixel(target, 4, 4) == OpaqueGreen, "stroke must paint the line");
    Require(Pixel(target, 0, 0) == Transparent, "stroke must leave the exterior untouched");

    cairo_move_to(context, 0, 4.5);
    cairo_line_to(context, 8, 4.5);
    cairo_stroke_preserve(context);
    SolidSource blue(9, 9, OpaqueBlue);
    UseSolidSource(context, blue.surface);
    cairo_set_line_width(context, 1);
    cairo_stroke(context);
    Require(Pixel(target, 2, 4) == OpaqueBlue, "stroke_preserve must keep the path for the next stroke");

    cairo_destroy(context);
    cairo_surface_destroy(target);
}

void CheckClip() {
    CairoSurface* target = CreateTarget(8, 8);
    CairoContext* context = cairo_create(target);
    cairo_rectangle(context, 2, 2, 4, 4);
    cairo_clip(context);
    SolidSource red(8, 8, OpaqueRed);
    UseSolidSource(context, red.surface);
    cairo_paint(context);
    Require(Pixel(target, 3, 3) == OpaqueRed, "clip must keep the interior");
    Require(Pixel(target, 0, 0) == Transparent, "clip must reject the exterior");
    cairo_destroy(context);
    cairo_surface_destroy(target);
}

}

int main() {
    CheckPaint();
    CheckPaintWithAlpha();
    CheckFillAndPreserve();
    CheckStrokeAndPreserve();
    CheckClip();
    return 0;
}
