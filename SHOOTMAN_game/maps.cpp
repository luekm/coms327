#include "maps.h"

void gen_map1()
{

    world_map map1;

    int x, y;
    tile tilee;
    tile walle;
    npc *emp = new npc();
    emp->exists = false;

    walle.type = wall;

    tilee.type = floore;

    for (y = 0; y < MAP_Y; y++)
    {
        for (x = 0; x < MAP_X; x++)
        {

            tilee.x = x;
            tilee.y = y;

            map1.map[y][x] = tilee;

            map1.npcMap[y][x] = emp;
        }
    }

    for (x = 0; x < MAP_X; x++)
    {
        walle.x = x;
        walle.y = 0;

        map1.map[0][x] = walle;

        walle.y = MAP_Y - 1;
        map1.map[MAP_Y - 1][x] = walle;
    }

    for (y = 0; y < MAP_Y; y++)
    {
        walle.y = y;
        walle.x = 0;
        map1.map[y][0] = walle;

        walle.x = MAP_X - 1;
        map1.map[y][MAP_X - 1] = walle;
    }
}