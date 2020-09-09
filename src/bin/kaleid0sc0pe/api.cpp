#include "frei0r.hpp"

#include "libkaleidoscope.h"

class kaleid0sc0pe : public frei0r::filter, public libkaleidoscope::Kaleidoscope
{
public:
    kaleid0sc0pe(unsigned int width, unsigned int height):
        libkaleidoscope::Kaleidoscope(width,height,1,4),
        m_segmentation(16/128.0),
        m_direction("cw"),
        m_corner("tl"),
        m_corner_direction("cw"),
        m_edge_threshold(0),
        m_bg_alpha(1),
        m_dirty(true)
    {
        m_origin.x = 0.5;
        m_origin.y = 0.5;
        
        m_bg_colour.r = 1.0;
        m_bg_colour.g = 0.0;
        m_bg_colour.b = 1.0;

        register_param(m_origin,
                                "origin",
                                "origin of the kaleidoscope. default 0.5,0.5");
        register_param(m_segmentation,
                                "segmentation",
                                "kaleidoscope segmentation / 128, segmentations of 1, 2 or multiples of 4 work best. default 16/128");
        register_param(m_direction,
                                "direction",
                                "direction to reflect in, either 'cw' or 'ccw'. default 'cw'");
        register_param(m_edge_threshold,
                                "edge_threshold",
                                "edge threshold / 4, reflections outside the image but within this distance clamp to the edge. default 0");
        register_param(m_corner,
                                "preferred_corner",
                                "preferred corner for source segment, either 'tl', 'tr', 'bl' or 'br'. default 'bl'");
        register_param(m_corner_direction,
                                "corner_search",
                                "direction to search in to find furthest corner, either 'cw' or 'ccw'. default 'cw'");
        register_param(m_bg_colour,
                                "bg_color",
                                "color to use if reflection lies outside of source image. default 1,0,1");
        register_param(m_bg_alpha,
                                "bg_alpha",
                                "alpha to use if reflection lies outside of source image. default 1");

        set_background_colour(m_background);
    }

    virtual void update(double time,
        uint32_t* out,
        const uint32_t* in) {

        if (m_dirty) {
            update_params();
        }
        process(in, out);
    }

    virtual void set_param_value(f0r_param_t param, int param_index)
    {
        fx::set_param_value(param, param_index);
        m_dirty = true;
    }

private:
    void update_params()
    {
        set_origin(static_cast<float>(m_origin.x), static_cast<float>(m_origin.y));
        set_segmentation(static_cast<std::uint32_t>(m_segmentation * 128));
        if (m_direction == "cw") {
            set_segment_direction(libkaleidoscope::Kaleidoscope::Direction::CLOCKWISE);
        } else if (m_direction == "ccw") {
            set_segment_direction(libkaleidoscope::Kaleidoscope::Direction::ANTICLOCKWISE);
        }
        if (m_corner == "tl") {
            set_preferred_corner(libkaleidoscope::Kaleidoscope::Corner::TL);
        } else if (m_corner == "tr") {
            set_preferred_corner(libkaleidoscope::Kaleidoscope::Corner::TR);
        } else if (m_corner == "bl") {
            set_preferred_corner(libkaleidoscope::Kaleidoscope::Corner::BL);
        } else if (m_corner == "br") {
            set_preferred_corner(libkaleidoscope::Kaleidoscope::Corner::BR);
        }
        if (m_corner_direction == "cw") {
            set_preferred_corner_search_direction(libkaleidoscope::Kaleidoscope::Direction::CLOCKWISE);
        } else if (m_corner_direction == "ccw") {
            set_preferred_corner_search_direction(libkaleidoscope::Kaleidoscope::Direction::ANTICLOCKWISE);
        }
        set_edge_threshold(static_cast<std::uint32_t>(m_edge_threshold * 4));

        m_background[0] = static_cast<std::uint8_t>(m_bg_colour.r * 255);
        m_background[1] = static_cast<std::uint8_t>(m_bg_colour.g * 255);
        m_background[2] = static_cast<std::uint8_t>(m_bg_colour.b * 255);
        m_background[3] = static_cast<std::uint8_t>(m_bg_alpha * 255);

        m_dirty = false;
    }

    f0r_param_position m_origin;

    double m_segmentation;
    std::string m_direction;

    std::string m_corner;
    std::string m_corner_direction;

    double m_edge_threshold;

    f0r_param_color m_bg_colour;
    double m_bg_alpha;

    std::uint8_t m_background[4];

    bool m_dirty;
};

frei0r::construct<kaleid0sc0pe> plugin("Kaleid0sc0pe", "Applies a kaleidoscope effect to a source image", "Brendan Hack", 1, 0, F0R_COLOR_MODEL_RGBA8888);