#include "draw-logo.hpp"

namespace widgetry {

// draw #d logo
void DrawLogo(NVGcontext * vg, float x, float y, NVGcolor fill, float scale)
{
    bool scaling = scale != 1.0f;
    nvgSave(vg);
    nvgTranslate(vg, x, y);
    if (scaling) {
        nvgScale(vg, scale, scale);
    }
    nvgFillColor(vg, fill);

    // #
    nvgBeginPath(vg);
    nvgMoveTo(vg, 3.0225791, 5.3090902);
    nvgLineTo(vg, 3.9164512, 3.5427485);
    nvgLineTo(vg, 5.0031212, 3.8339209);
    nvgLineTo(vg, 4.1042182, 5.5989145);
    nvgLineTo(vg, 5.7593776, 6.0424131);
    nvgLineTo(vg, 6.6582806, 4.2774195);
    nvgLineTo(vg, 7.7399197, 4.5672439);
    nvgLineTo(vg, 6.8460476, 6.3335855);
    nvgLineTo(vg, 7.6308648, 6.5438766);
    nvgLineTo(vg, 7.3396924, 7.6305466);
    nvgLineTo(vg, 6.3284856, 7.3595946);
    nvgLineTo(vg, 5.6659891, 8.6649013);
    nvgLineTo(vg, 6.9689869, 9.0140385);
    nvgLineTo(vg, 6.6778145, 10.100708);
    nvgLineTo(vg, 5.1484271, 9.6909104);
    nvgLineTo(vg, 4.254555, 11.457252);
    nvgLineTo(vg, 3.1678851, 11.16608);
    nvgLineTo(vg, 4.0617572, 9.399738);
    nvgLineTo(vg, 2.4065978, 8.9562394);
    nvgLineTo(vg, 1.5127257, 10.722581);
    nvgLineTo(vg, 0.4260557, 10.431409);
    nvgLineTo(vg, 1.3199278, 8.665067);
    nvgLineTo(vg, 0.54014147, 8.4561239);
    nvgLineTo(vg, 0.83131382, 7.3694539);
    nvgLineTo(vg, 1.8425206, 7.640406);
    nvgLineTo(vg, 2.4999863, 6.3337512);
    nvgLineTo(vg, 1.2020194, 5.985962);
    nvgLineTo(vg, 1.4931917, 4.899292);
    nvgClosePath(vg);
    nvgPathWinding(vg, NVGsolidity::NVG_HOLE);
    nvgMoveTo(vg, 5.2418157, 7.0684222);
    nvgLineTo(vg, 3.5866563, 6.6249236);
    nvgLineTo(vg, 2.9241597, 7.9302303);
    nvgLineTo(vg, 4.5793191, 8.3737289);
    nvgClosePath(vg);
    nvgFill(vg);

    //d
    nvgBeginPath(vg);
    nvgMoveTo(vg, 13.386835,10.647075);
    nvgLineTo(vg, 13.086226,11.768961);
    nvgLineTo(vg, 10.731774,11.138087);
    nvgLineTo(vg, 10.886797,10.559536);

    nvgQuadTo(vg, 10.063919,11.034625, 9.3042558,10.831074);
    nvgQuadTo(vg, 8.3936666,10.587083, 7.9931509,9.6871288);
    nvgQuadTo(vg, 7.5926352,8.7871751, 7.8743714,7.7357213);
    nvgQuadTo(vg, 8.1601517,6.6691748, 8.9693819,6.1041558);
    nvgQuadTo(vg, 9.7799602,5.5341058, 10.665395,5.7713573);
    nvgQuadTo(vg, 11.515614,5.9991727, 11.89242,6.8065002);
    nvgLineTo(vg, 12.306262,5.2620202);
    nvgLineTo(vg, 11.546599,5.0584691);
    nvgLineTo(vg, 11.847208,3.936583);
    nvgLineTo(vg, 14.161412,4.5566722);
    nvgLineTo(vg, 12.586925,10.43274);
    nvgClosePath(vg);

    nvgPathWinding(vg, NVGsolidity::NVG_HOLE);
    nvgMoveTo(vg, 11.402005,8.6971241);
    nvgQuadTo(vg, 11.58938,7.9978318, 11.36706,7.6201287);
    nvgQuadTo(vg, 11.149771,7.2437736, 10.767424,7.1413241);
    nvgQuadTo(vg, 10.309614,7.0186543, 9.962361,7.3084457);
    nvgQuadTo(vg, 9.6214865,7.5945543, 9.4664642,8.1731054);
    nvgQuadTo(vg, 9.31953,8.7214713, 9.4649416,9.1648403);
    nvgQuadTo(vg, 9.612,9.603, 10.08,9.729);
    nvgQuadTo(vg, 10.467,9.833, 10.84,9.609);
    nvgQuadTo(vg, 11.219,9.382, 11.402,8.697);
    nvgClosePath(vg);
    nvgFill(vg);

    nvgRestore(vg);
}

}