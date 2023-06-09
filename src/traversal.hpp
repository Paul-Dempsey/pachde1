#pragma once
#include <rack.hpp>
#include "pic.hpp"
#include "primitives.hpp"
#include "colors.hpp"
using namespace rack;
using namespace pachde;

namespace traversal {

inline float random_direction() {
    return random::uniform() > 0.5 ? 1.0f : -1.0f;
}

// TODO: reverse?
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
    Vec image_size = Vec(0.0f,0.0f);
    Vec position = Vec(0.0f, 0.0f);
    float pixel_rate = 100.0f;
    float sample_rate = 44100;
    float advance = 0.00;;
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
        position = Vec(0.0f, 0.0f);
    }

    void reset() override {
        position = Vec(0.0f,0.0f);
    }    

    void set_position(Vec pos) override
    {
        assert(pos.x >= 0.0f && pos.x < image_size.x);
        assert(pos.y >= 0.0f && pos.y < image_size.y);
        position = pos;
    }

    void clip_position() {
        position.x = std::max(0.0f, std::min(position.x, image_size.x-PIC_EPSILON));
        position.y = std::max(0.0f, std::min(position.y, image_size.y-PIC_EPSILON));
    }

    void wrap_position() {
        if (position.x < 0.0f) {
            position.x = std::max(0.0f, image_size.x-PIC_EPSILON);
        } else if (position.x >= image_size.x) {
            position.x = 0.0f;
        }
        if (position.y < 0.0f) {
            position.y = std::max(0.0f, image_size.y-PIC_EPSILON);
        } else if (position.y >= image_size.y) {
            position.y = 0.0f;
        }
    }
    Vec get_position() override { return position; }
};

enum class Traversal {
    SCANLINE,
    BOUNCE,
    VINYL,
    WANDER,
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

struct Vinyl : TraversalBase {
    float direction = -1.0f;
    float r = 0.999f;
    float theta = 0.0f;
    float r_limit = 1.0f;

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
    Bounce() { reset(); }
    void process() override;
    void reset() override;
};


struct Wander : TraversalBase {
    float angle = 0.0f;
    int count = 0;
    int max = 0;
    float dir = 1.0f;
    int dcount = 0;
    int dmax = 0;
    int new_max();
    void process() override;
    void reset() override;
};

}