// hiptext - Image to Text Converter
// By Justine Tunney

#include "macterm.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iterator>
#include <utility>

#include <glog/logging.h>

#include "pixel.h"

using std::array;
using std::distance;
using std::min_element;

MactermColor::MactermColor(const Pixel& top, const Pixel& bot) {
  struct Match {
    uint8_t code;
    double dist;
  };
  Match best_fg_top = {0, 1e6};
  Match best_fg_bot = {0, 1e6};
  Match best_bg_top = {0, 1e6};
  Match best_bg_bot = {0, 1e6};
  double dist;
  for (int code = 16; code < 256; ++code) {
    dist = top.Distance(macterm_colors[0][code]);
    if (dist < best_fg_top.dist) {
      best_fg_top.code = code;
      best_fg_top.dist = dist;
    }
    dist = bot.Distance(macterm_colors[0][code]);
    if (dist < best_fg_bot.dist) {
      best_fg_bot.code = code;
      best_fg_bot.dist = dist;
    }
  }
  for (int code = 16; code < 256; ++code) {
    dist = top.Distance(macterm_colors[1][code]);
    if (dist < best_bg_top.dist) {
      best_bg_top.code = code;
      best_bg_top.dist = dist;
    }
    dist = bot.Distance(macterm_colors[1][code]);
    if (dist < best_bg_bot.dist) {
      best_bg_bot.code = code;
      best_bg_bot.dist = dist;
    }
  }

  array<double, 6> choices = {{
      best_fg_top.dist + best_bg_bot.dist,
      best_bg_top.dist + best_fg_bot.dist,
      best_fg_top.dist + bot.Distance(macterm_colors[0][best_fg_top.code]),
      best_fg_bot.dist + top.Distance(macterm_colors[0][best_fg_bot.code]),
      best_bg_top.dist + bot.Distance(macterm_colors[1][best_bg_top.code]),
      best_bg_bot.dist + top.Distance(macterm_colors[1][best_bg_bot.code])}};
  switch (distance(choices.begin(),
                   min_element(choices.begin(), choices.end()))) {
    case 0:
      symbol_ = kUpperHalfBlock;
      fg_ = best_fg_top.code;
      bg_ = best_bg_bot.code;
      break;
    case 1:
      symbol_ = kLowerHalfBlock;
      bg_ = best_bg_top.code;
      fg_ = best_fg_bot.code;
      break;
    case 2:
      symbol_ = kFullBlock;
      fg_ = best_fg_top.code;
      bg_ = best_fg_top.code;
      break;
    case 3:
      symbol_ = kFullBlock;
      fg_ = best_fg_bot.code;
      bg_ = best_fg_bot.code;
      break;
    case 4:
      symbol_ = kSpace;
      fg_ = best_bg_top.code;
      bg_ = best_bg_top.code;
      break;
    case 5:
      symbol_ = kSpace;
      fg_ = best_bg_bot.code;
      bg_ = best_bg_bot.code;
      break;
    default:
      CHECK(false);
  }
}

// Terminal.app on Mac OS X is interesting. First of all, it doesn't follow the
// xterm-256color standard, but that's probably for the best since xterm's
// palette was obviously chosen by engineers rather than designers. The problem
// is I'm not quite sure what Terminal.app is doing. It slightly ajusts the
// color when displaying foregrounds and backgrounds but I'm not sure what
// formula they're using to do it. They also seem to slightly alter colors
// depending on the terminal theme. The following colors are what I scraped
// from Mac OS X 10.8 Mountain Lion Terminal.app with a black background and
// the "Pro" theme.
const Pixel macterm_colors[2][256] = {
  // Foreground
  {
    Pixel::kClear,
    {192,  55,  41},
    { 48, 187,  49},
    {173, 172,  53},
    { 73,  76, 224},
    {209,  65, 209},
    { 60, 187, 199},
    {203, 204, 205},
    {154, 155, 156},
    {249,  59,  44},
    { 62, 229,  55},
    {234, 234,  62},
    { 89,  63, 251},
    {247,  67, 245},
    { 48, 239, 239},
    {233, 235, 235},
    { 47,  49,  49},
    { 60,  46, 142},
    { 67,  51, 180},
    { 74,  56, 218},
    { 82,  62, 248},
    { 89,  63, 251},
    { 46, 127,  43},
    { 46, 127, 127},
    { 49, 127, 167},
    { 60, 127, 207},
    { 63, 127, 241},
    { 70, 126, 251},
    { 57, 161,  50},
    { 47, 161, 122},
    { 50, 161, 161},
    { 60, 161, 200},
    { 53, 161, 237},
    { 65, 160, 252},
    { 45, 194,  51},
    { 58, 194, 118},
    { 51, 194, 156},
    { 59, 194, 194},
    { 48, 194, 233},
    { 63, 194, 252},
    { 62, 227,  55},
    { 55, 227, 115},
    { 48, 227, 151},
    { 63, 227, 189},
    { 52, 227, 227},
    { 66, 227, 253},
    { 51, 252,  57},
    { 53, 252, 111},
    { 63, 252, 147},
    { 58, 252, 184},
    { 59, 252, 222},
    { 62, 253, 251},
    {138,  49,  43},
    {134,  55, 134},
    {134,  58, 174},
    {133,  61, 213},
    {134,  67, 244},
    {134,  65, 251},
    {125, 124,  46},
    {124, 125, 125},
    {122, 125, 165},
    {123, 126, 205},
    {124, 126, 243},
    {125, 125, 251},
    {120, 159,  47},
    {118, 159, 121},
    {118, 159, 160},
    {117, 160, 199},
    {118, 160, 238},
    {119, 160, 252},
    {115, 193,  53},
    {113, 193, 117},
    {114, 193, 155},
    {111, 193, 193},
    {113, 194, 232},
    {112, 193, 252},
    {110, 226,  53},
    {108, 226, 114},
    {110, 226, 151},
    {106, 226, 189},
    {105, 227, 227},
    {105, 226, 252},
    { 99, 251,  59},
    {103, 251, 111},
    {104, 251, 146},
    { 97, 252, 184},
    {102, 252, 221},
    { 98, 254, 250},
    {175,  54,  40},
    {172,  58, 130},
    {170,  61, 170},
    {170,  66, 210},
    {169,  67, 245},
    {168,  69, 251},
    {164, 123,  47},
    {162, 123, 124},
    {161, 124, 163},
    {161, 124, 203},
    {160, 125, 238},
    {160, 125, 251},
    {158, 157,  47},
    {157, 158, 120},
    {157, 158, 159},
    {155, 158, 198},
    {155, 159, 236},
    {155, 158, 252},
    {153, 192,  55},
    {152, 192, 117},
    {151, 192, 154},
    {151, 192, 193},
    {150, 192, 231},
    {150, 192, 251},
    {148, 225,  53},
    {147, 225, 114},
    {146, 225, 150},
    {147, 226, 188},
    {145, 226, 226},
    {145, 226, 250},
    {142, 251,  61},
    {141, 251, 111},
    {141, 252, 146},
    {142, 253, 183},
    {139, 254, 221},
    {138, 255, 249},
    {211,  59,  40},
    {209,  63, 126},
    {207,  63, 166},
    {206,  64, 206},
    {205,  69, 243},
    {204,  72, 252},
    {202, 121,  45},
    {201, 122, 122},
    {200, 122, 161},
    {199, 123, 200},
    {199, 124, 238},
    {197, 124, 252},
    {197, 156,  51},
    {195, 156, 119},
    {195, 157, 157},
    {194, 157, 196},
    {193, 157, 234},
    {193, 157, 252},
    {191, 190,  54},
    {190, 190, 116},
    {189, 191, 153},
    {190, 191, 191},
    {188, 191, 229},
    {187, 191, 251},
    {186, 224,  55},
    {185, 224, 113},
    {184, 224, 150},
    {184, 224, 187},
    {184, 225, 225},
    {182, 224, 251},
    {180, 253,  62},
    {180, 253, 111},
    {179, 253, 146},
    {179, 253, 183},
    {179, 254, 220},
    {177, 252, 249},
    {244,  59,  43},
    {243,  62, 123},
    {241, 65, 162},
    {241, 69, 202},
    {240, 70, 238},
    {238,  69, 252},
    {239, 119, 50},
    {238, 120, 120},
    {236, 121, 159},
    {235, 121, 198},
    {235, 123, 236},
    {234, 123, 252},
    {234, 154,  53},
    {233, 154, 118},
    {232, 155, 156},
    {231, 155, 194},
    {231, 156, 233},
    {230, 156, 252},
    {229, 188,  53},
    {228, 189, 115},
    {227, 189, 152},
    {227, 189, 190},
    {226, 189, 228},
    {225, 189, 253},
    {223, 222,  60},
    {223, 223, 113},
    {222, 223, 149},
    {222, 223, 186},
    {222, 223, 224},
    {220, 223, 252},
    {218, 251,  61},
    {217, 251, 109},
    {217, 251, 145},
    {217, 251, 182},
    {216, 251, 219},
    {216, 251, 250},
    {252,  63,  43},
    {252,  64, 120},
    {252,  64, 159},
    {252,  65, 198},
    {252,  67, 236},
    {252,  72, 252},
    {253, 117,  47},
    {253, 118, 118},
    {253, 119, 156},
    {253, 120, 194},
    {253, 120, 233},
    {252, 121, 252},
    {253, 152,  49},
    {252, 152, 116},
    {252, 153, 153},
    {253, 153, 192},
    {252, 154, 229},
    {251, 154, 251},
    {253, 186,  56},
    {251, 187, 114},
    {251, 187, 151},
    {252, 187, 188},
    {252, 188, 226},
    {251, 188, 251},
    {251, 221,  61},
    {250, 221, 112},
    {250, 221, 148},
    {250, 221, 185},
    {251, 222, 222},
    {251, 222, 251},
    {251, 250,  58},
    {250, 250, 109},
    {249, 250, 144},
    {247, 251, 181},
    {247, 253, 218},
    {254, 255, 255},
    { 52,  53,  53},
    { 57,  58,  59},
    { 66,  67,  67},
    { 75,  76,  76},
    { 84,  85,  85},
    { 92,  93,  94},
    {101, 102, 102},
    {109, 111, 111},
    {118, 119, 119},
    {126, 127, 128},
    {134, 136, 136},
    {143, 144, 145},
    {151, 152, 153},
    {159, 161, 161},
    {167, 169, 169},
    {176, 177, 177},
    {184, 185, 186},
    {192, 193, 194},
    {200, 201, 202},
    {208, 209, 210},
    {216, 218, 218},
    {224, 226, 226},
    {232, 234, 234},
    {240, 242, 242},
  },

  // Background
  {
    Pixel::kClear,
    {151,   4,  12},
    { 23, 164,  26},
    {153, 152,  29},
    {  8,  43, 181},
    {177,  25, 176},
    { 26, 166, 177},
    {191, 191, 191},
    {132, 132, 132},
    {227,  10,  23},
    { 33, 215,  38},
    {229, 228,  49},
    { 11,  36, 251},
    {227,  35, 227},
    { 39, 229, 228},
    {230, 229, 230},
    {  0,   0,   0},
    {  1,   7,  93},
    {  3,  14, 133},
    {  5,  21, 172},
    {  7,  28, 211},
    { 11,  36, 251},
    {  8,  94,  11},
    { 10,  95,  95},
    { 11,  96, 133},
    { 13,  97, 173},
    { 15,  99, 212},
    { 18, 101, 251},
    { 17, 134,  20},
    { 18, 134,  96},
    { 18, 135, 134},
    { 20, 136, 173},
    { 21, 137, 212},
    { 23, 138, 251},
    { 25, 173,  29},
    { 26, 174,  98},
    { 26, 174, 136},
    { 27, 175, 174},
    { 28, 175, 213},
    { 30, 176, 252},
    { 33, 213,  38},
    { 34, 213, 101},
    { 34, 214, 137},
    { 35, 214, 175},
    { 36, 215, 214},
    { 37, 215, 253},
    { 41, 253,  47},
    { 42, 253, 104},
    { 42, 253, 140},
    { 43, 253, 177},
    { 44, 254, 215},
    { 45, 255, 254},
    { 94,   2,   4},
    { 94,   8,  94},
    { 94,  15, 133},
    { 94,  22, 172},
    { 95,  29, 211},
    { 95,  36, 251},
    { 95,  94,  14},
    { 95,  95,  95},
    { 95,  96, 134},
    { 95,  97, 173},
    { 96,  99, 212},
    { 96, 101, 251},
    { 96, 134,  22},
    { 96, 134,  96},
    { 96, 135, 135},
    { 97, 136, 173},
    { 97, 137, 212},
    { 97, 138, 252},
    { 98, 173,  30},
    { 98, 174,  98},
    { 98, 174, 136},
    { 98, 175, 174},
    { 98, 176, 213},
    { 99, 177, 252},
    {100, 213,  39},
    {100, 213, 101},
    {100, 214, 138},
    {100, 214, 176},
    {101, 215, 214},
    {101, 215, 253},
    {102, 253,  48},
    {103, 253, 104},
    {103, 253, 140},
    {103, 253, 177},
    {103, 254, 215},
    {104, 255, 254},
    {133,   3,   9},
    {133,  10,  94},
    {134,  16, 133},
    {134,  23, 172},
    {134,  30, 212},
    {134,  37, 251},
    {134,  94,  18},
    {134,  95,  96},
    {134,  96, 134},
    {134,  97, 173},
    {135,  99, 212},
    {135, 101, 251},
    {135, 134,  25},
    {135, 134,  97},
    {135, 135, 135},
    {135, 136, 174},
    {135, 137, 213},
    {136, 138, 252},
    {136, 173,  32},
    {136, 174,  99},
    {136, 174, 136},
    {136, 175, 175},
    {136, 176, 213},
    {137, 177, 252},
    {137, 213,  40},
    {137, 213, 102},
    {138, 214, 138},
    {138, 214, 176},
    {138, 215, 214},
    {138, 216, 253},
    {139, 253,  49},
    {139, 253, 105},
    {139, 253, 140},
    {139, 254, 178},
    {140, 254, 216},
    {140, 255, 254},
    {173,   6,  15},
    {173,  12,  95},
    {173,  18, 134},
    {173,  24, 173},
    {173,  31, 212},
    {174,  38, 251},
    {173,  95,  22},
    {174,  95,  96},
    {174,  96, 135},
    {174,  98, 173},
    {174,  99, 212},
    {174, 101, 252},
    {174, 134,  28},
    {174, 135,  98},
    {174, 135, 136},
    {174, 136, 174},
    {174, 137, 213},
    {175, 139, 252},
    {175, 174,  35},
    {175, 174, 100},
    {175, 174, 137},
    {175, 175, 175},
    {175, 176, 214},
    {175, 177, 253},
    {176, 213,  43},
    {176, 213, 102},
    {176, 214, 139},
    {176, 214, 176},
    {176, 215, 215},
    {176, 216, 253},
    {177, 253,  51},
    {177, 253, 105},
    {177, 253, 141},
    {177, 254, 178},
    {178, 254, 216},
    {178, 255, 254},
    {213,   9,  21},
    {213,  15,  96},
    {213, 20, 135},
    {241, 69, 202},
    {213, 32, 212},
    {213,  39, 251},
    {213,  95,  27},
    {213,  96,  97},
    {213,  97, 135},
    {213,  98, 174},
    {213, 100, 213},
    {213, 102, 252},
    {213, 134,  32},
    {213, 135,  99},
    {213, 135, 136},
    {214, 136, 175},
    {214, 137, 213},
    {214, 139, 252},
    {214, 174,  38},
    {214, 174, 101},
    {214, 175, 138},
    {214, 175, 176},
    {214, 176, 214},
    {214, 177, 253},
    {215, 213,  45},
    {215, 214, 103},
    {215, 214, 139},
    {215, 214, 177},
    {215, 215, 215},
    {215, 216, 254},
    {216, 253,  53},
    {216, 253, 106},
    {216, 253, 141},
    {216, 254, 178},
    {216, 254, 216},
    {216, 255, 255},
    {252,  13,  27},
    {252,  18,  98},
    {252,  22, 135},
    {252,  28, 174},
    {252,  34, 213},
    {252,  40, 252},
    {252,  96,  32},
    {252,  96,  99},
    {252,  97, 136},
    {253,  99, 175},
    {253, 100, 213},
    {253, 102, 252},
    {253, 135,  36},
    {253, 135, 100},
    {253, 136, 137},
    {253, 137, 175},
    {253, 138, 214},
    {253, 139, 253},
    {253, 174,  42},
    {253, 174, 102},
    {253, 175, 138},
    {253, 175, 176},
    {253, 176, 215},
    {254, 177, 253},
    {254, 213,  48},
    {254, 214, 105},
    {254, 214, 140},
    {254, 215, 177},
    {254, 215, 216},
    {254, 216, 254},
    {255, 253,  56},
    {255, 253, 108},
    {255, 253, 142},
    {255, 254, 179},
    {255, 254, 217},
    {255, 255, 255},
    {  8,   8,   8},
    { 18,  18,  18},
    { 28,  28,  28},
    { 38,  38,  38},
    { 48,  48,  48},
    { 58,  58,  58},
    { 68,  68,  68},
    { 78,  78,  78},
    { 88,  88,  88},
    { 98,  98,  98},
    {108, 108, 108},
    {118, 118, 118},
    {128, 128, 128},
    {138, 138, 138},
    {148, 148, 148},
    {158, 158, 158},
    {168, 168, 168},
    {178, 178, 178},
    {188, 188, 188},
    {198, 198, 198},
    {208, 208, 208},
    {218, 218, 218},
    {228, 228, 228},
    {238, 238, 238},
  },
};

// For Emacs:
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:2
// c-basic-offset:2
// c-file-style: nil
// End:
// For VIM:
// vim:set expandtab softtabstop=2 shiftwidth=2 tabstop=2:
