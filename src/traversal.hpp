#pragma once
#include <rack.hpp>
#include "pic.hpp"
#include "primitives.hpp"
using namespace rack;
using namespace pachde;

namespace traversal {

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
    Vec position = Vec(0.0f,0.0f);
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
        //configure_rate(pixel_rate, sample_rate);
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

    Vec get_position() override { return position; }
};

enum class Traversal {
    SCANLINE,
    LAST_TRAVERSAL = SCANLINE
};

std::string TraversalName(Traversal id);
ITraversal * MakeTraversal(Traversal id);


// LRTB
struct Scanline : TraversalBase {
     ~Scanline() {}
    void process() override;
};

}