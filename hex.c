//
// Created by AJ Austinson on 12/26/2020.
//

#include "hex.h"

Hex HexMake(int x, int y) {
    Hex h = {0};
    h.gx = x;
    h.gy = y;
    h.x = h.gx - (h.gy - (h.gy & 1)) / 2;
    h.z = h.gy;
    h.y = -h.x - h.z;
    return h;
}

Hex HexMakeCube(int x, int y, int z) {
    Hex h = {0};
    h.x = x;
    h.y = y;
    h.z = z;
    h.gx = h.x + (h.z - (h.z & 1)) / 1;
    h.gy = h.z;
    return h;
}
