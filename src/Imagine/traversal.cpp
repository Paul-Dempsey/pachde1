#include "traversal.hpp"

using namespace pachde;

namespace traversal {

void Scanline::process()
{
    position.x += advance;
    if (position.x >= image_size.x) {
        position.x = 0.0f;
        position.y += 1.0f;
        if (position.y >= image_size.y) {
            position.y = 0.0f;
        }
    }
}

void Vinyl::process() {
    theta += advance / (3.0f *simd::sqrt(r));
    r += direction * advance / TWO_PI;
    if (r < 0.0f) {
        direction = -direction;
        r = 0.0f;
    } else if ( r >= r_limit) {
        direction = -direction;
        r = r_limit - PIC_EPSILON;
    }
    if (theta > TWO_PI) {
        theta = 0.0f;
    }
    polar_to_pos();
}

void Vinyl::pos_to_polar() {
    auto x = position.x - image_size.x/2.0f;
    auto y = position.y - image_size.y/2.0f;
    r = std::min(r_limit - PIC_EPSILON, simd::sqrt(x*x + y*y));
    theta = simd::atan2(y, x);
}

void Vinyl::polar_to_pos() {
    sincosf(theta, &position.y, &position.x);
    position.x *= r;
    position.x += image_size.x/2.0f;
    position.y *= r;
    position.y += image_size.y/2.0f;
    clip_position();
}

void Vinyl::reset()
{
    position.x = image_size.x/2.0f + r_limit - PIC_EPSILON;
    position.y = image_size.y/2.0f;
    pos_to_polar();
}

void Vinyl::configure_image(Vec size)
{
    image_size = size;
    r_limit = std::min(image_size.x, image_size.y)/2.0f;
    reset();
}

void Vinyl::set_position(Vec pos)
{
    TraversalBase::set_position(pos);
    clip_position();
    pos_to_polar();
}


void Bounce::reset() {
    position.x = image_size.x/2.0f;
    position.y = image_size.y/2.0f;
    angle = random::uniform()*TWO_PI;
}

void Bounce::process() {
    float dx, dy;
    sincosf(angle, &dy, &dx);
    position.x += (dx * advance)*2;
    position.y += (dy * advance)*2;
    if (position.x < 0.0f ) {
        angle = angle + PI/4.0 + (random::uniform() - 0.5f)/1000.0;
        position.x -= dx;
        position.y += dy;
    } else if (position.x >= image_size.x) {
        angle = angle - PI/4.0 + (random::uniform() - 0.5f)/1000.0;
        position.x -= dx;
        position.y += dy;
    } else if (position.y < 0.0f || position.y >= image_size.y) {
        angle = -angle + (random::uniform() - 0.5f)/10000.0;
        position.x += dx;
        position.y -= dy;
    }
    clip_position();
}

void Wander::reset() {
    position = Vec(image_size.x / 2.f, image_size.y / 2.f);
    max = 0;
}

int Wander::new_max() {
    return 100 + random::uniform() * 500; 
}

void Wander::process() {
    ++count;
    if (count > max) {
        count = 0;
        max = new_max();
        dcount++;
        if (dcount > dmax) {
            dir = -dir;
            dcount = 0;
            dmax = new_max();
        }
        angle += random::uniform() * 0.01f * dir;
    }
    float dx, dy;
    sincosf(angle, &dy, &dx);
    position.x += (dx * advance) * 2.0f;
    position.y += (dy * advance) * 2.0f;
    wrap_position();
}

ITraversal * MakeTraversal(Traversal id) {
    switch (id) {
        default:
        case Traversal::SCANLINE:
            return new Scanline();
        case Traversal::BOUNCE:
            return new Bounce();
        case Traversal::VINYL:
            return new Vinyl();
        case Traversal::WANDER:
            return new Wander();
        case Traversal::XYPAD:
            return new XYPad();
    }
}

const char * TraversalNames[] = {
    "Scanline",
    "Bounce",
    "Vinyl",
    "Wander",
    "X/Y Pad"
};

std::string TraversalName(Traversal id)
{
    if (id >= Traversal::SCANLINE && id < Traversal::NUM_TRAVERSAL) {
        return TraversalNames[static_cast<int>(id)];
    }
    return "???";
}

}