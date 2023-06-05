#pragma once
#include <rack.hpp>
#include "pic.hpp"
#include "primitives.hpp"
using namespace rack;
using namespace pachde;

namespace traversal {

struct ITraversal {
    virtual Point next(const Pic & pic, int x, int y, int delta) = 0;
};

enum class Traversal {
    SCANLINE,
    TBLR,
    DIAGONAL,
    LAST_TRAVERSAL = DIAGONAL
};

ITraversal * MakeTraversal(Traversal id);
std::string TraversalName(Traversal id);

// LRTB
struct Scanline : ITraversal {
    Point next(const Pic & pic, int x, int y, int count) override;
};

struct TBLR : ITraversal {
    Point next(const Pic & pic, int x, int y, int count) override;
};

struct Diagonal : ITraversal {
    Point next(const Pic & pic, int x, int y, int count) override;
};

}