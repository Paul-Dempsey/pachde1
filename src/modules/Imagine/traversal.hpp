#pragma once
#include <rack.hpp>
#include "../services/colors.hpp"
#include "../widgets/pic.hpp"
#include "imagine_layout.hpp"

#if defined ARCH_MAC
#define sincosf __sincosf
#endif

using namespace ::rack;
using namespace ::pachde;
using namespace ::widgetry;

namespace traversal {

inline float random_direction() {
    return random::uniform() > 0.5f ? 1.0f : -1.0f;
}

struct ITraversal
{
    virtual ~ITraversal() {}
    // pixel_rate: pixels/sec
    // sample_rate: Rack sample rate
    virtual void configure_rate(float pixel_rate, float sample_rate) = 0;
    virtual void configure_image(Vec image_size) = 0;
    virtual void reset() = 0;
    virtual void set_position(Vec location) = 0;
    virtual Vec get_position() = 0;
    virtual void process() = 0;
};

struct TraversalBase : ITraversal
{
    ~TraversalBase() {}
    Vec image_size = Vec(0.f,0.f);
    Vec position = Vec(0.f, 0.f);
    float pixel_rate = 100.f;
    float sample_rate = 44100;
    float advance = 0.f;;
    void configure_rate(float pixel_rate, float sample_rate) override
    {
        if (pixel_rate == this->pixel_rate && sample_rate == this->sample_rate) {
            return;
        }
        this->pixel_rate = pixel_rate;
        this->sample_rate = sample_rate;
        advance = pixel_rate * 1.0f/sample_rate;
    }

    void configure_image(Vec size) override
    {
        image_size = size;
        position = Vec(0.f, 0.f);
    }

    void reset() override {
        position = Vec(0.f, 0.f);
    }

    void set_position(Vec pos) override
    {
        //assert(pos.x == 0.f || (pos.x >= 0.f && pos.x < image_size.x));
        //assert(pos.y == 0.f || (pos.y >= 0.f && pos.y < image_size.y));
        position = pos;
        clip_position(); // HACK: find bad positioning code.
    }

    void clip_position() {
        position.x = std::max(0.f, std::min(position.x, image_size.x-PIC_EPSILON));
        position.y = std::max(0.f, std::min(position.y, image_size.y-PIC_EPSILON));
    }

    void wrap_position() {
        if (position.x < 0.f) {
            position.x = std::max(0.f, image_size.x-PIC_EPSILON);
        } else if (position.x >= image_size.x) {
            position.x = 0.f;
        }
        if (position.y < 0.f) {
            position.y = std::max(0.f, image_size.y-PIC_EPSILON);
        } else if (position.y >= image_size.y) {
            position.y = 0.f;
        }
    }
    Vec get_position() override { return position; }
};

enum class Traversal {
    SCANLINE,
    BOUNCE,
    VINYL,
    WANDER,
    XYPAD,
    TBLR,
    RLBT,
    NUM_TRAVERSAL
};

std::string TraversalName(Traversal id);
ITraversal * MakeTraversal(Traversal id);
extern const char * TraversalNames[];

// LRTB
struct Scanline : TraversalBase {
     ~Scanline() {}
    void process() override;
};

// TBLR
struct Tabalar : TraversalBase {
     ~Tabalar() {}
    void reset() override;
    void process() override;
};

// RLBT
struct Ralabat : TraversalBase {
    ~Ralabat() {}
    void reset() override;
    void process() override;
};

struct Vinyl : TraversalBase {
    float direction = -1.0f;
    float r = 0.9999f;
    float theta = 0.f;
    float r_limit = 1.f;
    float half_x;
    float half_y;

    ~Vinyl() {}
    void reset() override;
    void configure_image(Vec size) override;
    void set_position(Vec pos) override;
    void process() override;

    void pos_to_polar();
    void polar_to_pos();
};

struct Bounce : TraversalBase {
    float angle;

    Bounce()
    {
        position.x = image_size.x/2.0f;
        position.y = image_size.y/2.0f;
        angle = random::uniform()*TWO_PI;
    }
    void process() override;
    void reset() override;
};


struct Wander : TraversalBase {
    float angle = 0.f;
    int count = 0;
    int max = 0;
    float dir = 1.f;
    int dcount = 0;
    int dmax = 0;
    int new_max();
    void process() override;
    void reset() override;
};

struct XYPad : TraversalBase {
    void process() override {}
    void reset() override {
        position = Vec(image_size.x/2.f, image_size.y/2.f);
    }
    void configure_rate(float, float) override {}
    void configure_image(Vec size) override
    {
        image_size = size;
        if (image_size.x <= 0.f || image_size.y <= 0.f) {
            image_size.x = PANEL_IMAGE_WIDTH;
            image_size.y = PANEL_IMAGE_HEIGHT;
        }
        reset();
    }
};

}