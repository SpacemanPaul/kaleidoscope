// libkaleidoscope.cpp : Defines the functions for the static library.
//
#include "libkaleidoscope.h"
#include <cmath>

#ifndef M_PI
#define M_PI  3.14159265358979323846
#endif
#ifndef M_2PI 
#define M_2PI 6.28318530717958647693
#endif
#ifndef MF_PI
#define MF_PI  3.14159265358979323846f
#endif
#ifndef MF_2PI 
#define MF_2PI 6.28318530717958647693f
#endif

namespace libkaleidoscope {

Kaleidoscope::Kaleidoscope(std::uint32_t width, std::uint32_t height, std::uint32_t component_size, std::uint32_t num_components, std::uint32_t stride):
m_width(width),
m_height(height),
m_component_size(component_size),
m_num_components(num_components),
m_stride(stride ? stride : width * component_size * num_components),
m_origin_x(0.5f),
m_origin_y(0.5f),
m_segmentation(16),
m_segment_direction(Direction::CLOCKWISE),
m_preferred_corner(Corner::BR),
m_preferred_search_dir(Direction::CLOCKWISE),
m_n_segments(0),
m_start_angle(0),
m_segment_width(0)
{
}

std::int32_t Kaleidoscope::set_origin(float x, float y)
{
    if (x < 0 || y < 0 || x > 1 || y > 1) {
        return -2;
    }
    m_origin_x = x;
    m_origin_y = y;
    m_n_segments = 0;

    return 0;
}

float Kaleidoscope::get_origin_x() const
{
    return m_origin_x;
}

float Kaleidoscope::get_origin_y() const
{
    return m_origin_y;
}

std::int32_t Kaleidoscope::set_segmentation(std::uint32_t segmentation)
{
    if (segmentation == 0) {
        return -2;
    }
    m_segmentation = segmentation;
    m_n_segments = 0;

    return 0;
}

std::uint32_t Kaleidoscope::get_segmentation() const
{
    return m_segmentation;
}

std::int32_t Kaleidoscope::set_preferred_corner(Corner corner)
{
    m_preferred_corner = corner;
    m_n_segments = 0;
    return 0;
}

Kaleidoscope::Corner Kaleidoscope::get_preferred_corner() const
{
    return m_preferred_corner;
}

std::int32_t Kaleidoscope::set_preferred_corner_search_direction(Direction direction)
{
    m_preferred_search_dir = direction;
    m_n_segments = 0;
    return 0;
}

Kaleidoscope::Direction Kaleidoscope::get_preferred_corner_search_direction() const
{
    return m_preferred_search_dir;
}

static double distance_sq(double x1, double y1, double x2, double y2)
{
    return std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2);
}

std::int32_t inc_idx(std::int32_t start_idx, std::int32_t inc, std::int32_t max)
{
    start_idx += inc;
    return (start_idx < 0) ? max - 1 : start_idx % max;
}

void Kaleidoscope::init()
{
    // find origin rotation
    std::uint32_t corners[4][2] = {
        { 0, 0 },
        { 1, 0 },
        { 1, 1 },
        { 0, 1 }
    };
    std::int32_t start_idx(0);
    switch (m_preferred_corner) {
        case Corner::TL: start_idx = 0; break;
        case Corner::TR: start_idx = 1; break;
        case Corner::BR: start_idx = 2; break;
        case Corner::BL: start_idx = 3; break;
    }
    std::int32_t dir = m_preferred_search_dir == Direction::CLOCKWISE ? 1 : -1;
    std::uint32_t idx = start_idx;
    float origin_x = m_origin_x;
    float origin_y = m_origin_y;
    double dist = distance_sq(origin_x, origin_y, corners[idx][0], corners[idx][1]);
    std::int32_t corner = idx;
    idx = inc_idx(idx, dir, 4);
    while (idx != start_idx) {
        double d = distance_sq(origin_x, origin_y, corners[idx][0], corners[idx][1]);
        if (d > dist) {
            dist = d;
            corner = idx;
        }
        idx = inc_idx(idx, dir, 4);
    }

    float start_line_x = corners[corner][0] - origin_x;
    float start_line_y = corners[corner][1] - origin_y;
    m_start_angle = std::atan2(start_line_y, start_line_x) + MF_PI;

    m_n_segments = m_segmentation * 2;
    m_segment_width = MF_PI * 2 / m_n_segments;

}
std::uint8_t colours[63][3] = {
    { 0x00, 0xFF, 0x00 },
    { 0x00, 0x00, 0xFF },
    { 0xFF, 0x00, 0x00 },
    { 0x01, 0xFF, 0xFE },
    { 0xFF, 0xA6, 0xFE },
    { 0xFF, 0xDB, 0x66 },
    { 0x00, 0x64, 0x01 },
    { 0x01, 0x00, 0x67 },
    { 0x95, 0x00, 0x3A },
    { 0x00, 0x7D, 0xB5 },
    { 0xFF, 0x00, 0xF6 },
    { 0xFF, 0xEE, 0xE8 },
    { 0x77, 0x4D, 0x00 },
    { 0x90, 0xFB, 0x92 },
    { 0x00, 0x76, 0xFF },
    { 0xD5, 0xFF, 0x00 },
    { 0xFF, 0x93, 0x7E },
    { 0x6A, 0x82, 0x6C },
    { 0xFF, 0x02, 0x9D },
    { 0xFE, 0x89, 0x00 },
    { 0x7A, 0x47, 0x82 },
    { 0x7E, 0x2D, 0xD2 },
    { 0x85, 0xA9, 0x00 },
    { 0xFF, 0x00, 0x56 },
    { 0xA4, 0x24, 0x00 },
    { 0x00, 0xAE, 0x7E },
    { 0x68, 0x3D, 0x3B },
    { 0xBD, 0xC6, 0xFF },
    { 0x26, 0x34, 0x00 },
    { 0xBD, 0xD3, 0x93 },
    { 0x00, 0xB9, 0x17 },
    { 0x9E, 0x00, 0x8E },
    { 0x00, 0x15, 0x44 },
    { 0xC2, 0x8C, 0x9F },
    { 0xFF, 0x74, 0xA3 },
    { 0x01, 0xD0, 0xFF },
    { 0x00, 0x47, 0x54 },
    { 0xE5, 0x6F, 0xFE },
    { 0x78, 0x82, 0x31 },
    { 0x0E, 0x4C, 0xA1 },
    { 0x91, 0xD0, 0xCB },
    { 0xBE, 0x99, 0x70 },
    { 0x96, 0x8A, 0xE8 },
    { 0xBB, 0x88, 0x00 },
    { 0x43, 0x00, 0x2C },
    { 0xDE, 0xFF, 0x74 },
    { 0x00, 0xFF, 0xC6 },
    { 0xFF, 0xE5, 0x02 },
    { 0x62, 0x0E, 0x00 },
    { 0x00, 0x8F, 0x9C },
    { 0x98, 0xFF, 0x52 },
    { 0x75, 0x44, 0xB1 },
    { 0xB5, 0x00, 0xFF },
    { 0x00, 0xFF, 0x78 },
    { 0xFF, 0x6E, 0x41 },
    { 0x00, 0x5F, 0x39 },
    { 0x6B, 0x68, 0x82 },
    { 0x5F, 0xAD, 0x4E },
    { 0xA7, 0x57, 0x40 },
    { 0xA5, 0xFF, 0xD2 },
    { 0xFF, 0xB1, 0x67 },
    { 0x00, 0x9B, 0xFF },
    { 0xE8, 0x5E, 0xBE }
};

std::int32_t Kaleidoscope::set_segment_direction(Direction direction)
{
    m_segment_direction = direction;
    m_n_segments = 0;
    return 0;
}

libkaleidoscope::Kaleidoscope::Direction Kaleidoscope::get_segment_direction() const
{
    return m_segment_direction;
}

std::int32_t Kaleidoscope::process(const void* in_frame, void* out_frame)
{
    if (in_frame == nullptr || out_frame == nullptr) {
        return -2;
    }
    if (m_n_segments == 0) {
        init();
    }
    std::uint32_t comp_size = m_num_components * m_component_size;
    float origin_x = m_origin_x * m_width;
    float origin_y = m_origin_y * m_height;

    for (std::uint32_t y = 0; y < m_height; ++y) {
        for (std::uint32_t x = 0; x < m_width; ++x) {
            std::uint8_t* out = ((std::uint8_t*)out_frame) + m_stride * y + x * comp_size;
            float screen_x = (x - origin_x) / m_width;
            float screen_y = (y - origin_y) / m_height;

            float pt_angle = (m_segment_direction == Direction::CLOCKWISE ? 1 : -1) * (std::atan2(screen_y, screen_x) + MF_PI - m_start_angle);
            while (pt_angle < 0) {
                pt_angle += MF_2PI;
            }
            while (pt_angle > MF_2PI) {
                pt_angle -= MF_2PI;
            }
            std::uint32_t segment_number = std::uint32_t(pt_angle / m_segment_width);
            std::uint32_t col_idx = segment_number % 63;
            out[0] = colours[col_idx][0];
            out[1] = colours[col_idx][1];
            out[2] = colours[col_idx][2];
            if (m_num_components > 3) {
                out[3] = 0xff;
            }            
        }
    }
    return 0;
}

}
