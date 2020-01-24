#include <SFML/Graphics.hpp>
#include <math.h>

using namespace sf;

const int H = 20;
const int W = 200;
float offsetX = 0, offsetY = 0;
String TileMap[H] = {
    "B                                                                                                                                                                                                     B"
    "B                                                                                                                                                                                                     B",
    "B                                                                                                                                                                                                     B",
    "B                                                                                                                                                                   nnnnnnnnnnnnnnnnnnnnnnnnnnnn      B",
    "B                                                                                                                                                                   n                          n      B",
    "B                             n      n                                                                                                                              n                          n      B",
    "B                             BBBBBBBB                                                                                n   0   n                                     n                          n      B",
    "B                                                n      n                                                              BBBBBBB                                      n                          n      B",
    "B                                                 BBBBBB                  BBB                                                                                       n                          n      B",
    "B                                                                                                                              n   BB  n                            n                          n      B",
    "B                                                                                                                                 BB            n     n             n                          n      B",
    "B                                                                 n   n                                           BBB                           BBBBBBB             n                          n      B",
    "B                                                          B      BBBBB                                                         BB                                  n           BBBBB          n      B",
    "B                                                        BBB                                 n  0   n                    n       n                                  n                          n      B",
    "B             BB                                 BB        BB                                 BBBBBBB                    BBBBBBBBB                                  n                          n      B",
    "B                              n           n       BB                          BB                        n     n                               BBBBB                n    BBB            BBB    n      B",
    "B                      BB       BBBBBBBBBBBB                                                             BBBBBBB                                                    n                          n      B",
    "B                          BBB                          BB                                                                                                          n                          n      B",
    "B BB                                                    BB                                                                                                          n                          n      B",
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",

};