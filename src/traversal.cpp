#include "pic.hpp"
#include "traversal.hpp"

namespace traversal {

// Point Scanline::next(const Pic & pic, int x, int y, int count)
// {
//     if (0 == count) { return Point(x, y); }

//     auto ptr = pic.pixel_address(x, y);
//     ptr += pic.pixel_advance() * count;
//     auto end = pic.end();
//     if (ptr >= end) {
//         ptr = reinterpret_cast<unsigned char *>(pic.data() + reinterpret_cast<intptr_t>(ptr) - end);
//     } else if (ptr < pic.data()) {
//         ptr = end - (pic.data() - ptr);
//     }
//     return pic.position(ptr);
// }

// Point TBLR::next(const Pic & pic, int x, int y, int count)
// {
//     if (0 == count) return Point(x, y);
//     auto h = pic.height();
//     auto w = pic.width();
//     if (count > 0) {
//         while (count >= h) {
//             count -= h;
//             ++x;
//             if (x >= w) {
//                 x = 0;
//             }
//         }
//         y += count;
//         if (y >= h) {
//             y = 0;
//             ++x;
//             if (x >= w) {
//                 x = 0;
//             }
//         }
//         return Point(x, y);
//     }

//     count = -count;
//     while (count >= h) {
//         count -= h;
//             --x;
//         if (x < 0) {
//             x = w-1;
//         }
//     }
//     y -= count;
//     if (y < 0) {
//         y = h-1;
//         x--;
//         if (x < 0) {
//             x = w-1;
//         }
//     }
//     return Point(x, y);
// }

// // broken
// Point Diagonal::next(const Pic & pic, int x, int y, int count) {
//     if (0 != count) {
//         auto w = pic.width();
//         auto h = pic.height();
//         if (count > 0) {
//             for (int n = 0; n < count; ++n) {
//                 ++x;
//                 if (x >= w) {
//                     x = 0;
//                 }
//                 ++y;
//                 if (y >= h) {
//                     y = 0;
//                     x -= w-1;
//                     if (x < 0) {
//                         x = w-1;
//                     }
//                 }
//             }
//         } else {
//             count = -count;
//             for (int n = 0; n < count; ++n) {
//                 --x;
//                 if (x < 0) {
//                     x = w-1;
//                 }
//                 --y;
//                 if (y < 0) {
//                     y = h-1;
//                 }
//             }
//         }
//     }
//     return Point(x,y);
// }

// void Scanline::configure(float pixel_rate, float sample_rate, Vec size)
// {
//     TraversalBase::configure(pixel_rate, sample_rate, size);
// }

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


ITraversal * MakeTraversal(Traversal id) {
    switch (id) {
        default:
        case Traversal::SCANLINE:
            return new Scanline();
    }
}

static constexpr const char * TraversalNames[] = {
    "Scanline",
};

std::string TraversalName(Traversal id)
{
    if (id >= Traversal::SCANLINE && id <= Traversal::LAST_TRAVERSAL) {
        return TraversalNames[static_cast<int>(id)];
    }
    return "???";
}

}