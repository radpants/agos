//
// Created by AJ Austinson on 12/26/2020.
//

#ifndef AGOS_HEX_H
#define AGOS_HEX_H

typedef struct {
    int x, y, z, gx, gy;
    int neighbors[6];
} Hex;

Hex HexMake(int x, int y);
Hex HexMakeCube(int x, int y, int z);

static inline int HexGetIndex(Hex h, int worldWidth) { return h.gy * worldWidth + h.gx; }

static inline void HexGetNeighbors(Hex* h, int worldWidth) {
    static int offsets[] = {
        1,-1,0, 1,0,-1, 0,1,-1,
        -1,1,0, -1,0,1, 0,-1,1
    };
    for(int i = 0; i < 6; ++i ) {
        Hex n = HexMakeCube(h->x + offsets[i*3], h->y + offsets[i*3+1], h->z + offsets[i*3+2]);
        h->neighbors[i] = HexGetIndex(n, worldWidth);
    }
}

#endif //AGOS_HEX_H
