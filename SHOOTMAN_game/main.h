#ifndef MAIN_H_
#define MAIN_H_

#include <ncurses.h>
#include <vector>


#include "maps.h"

class printable {
   public:

};


class character : public printable {
    public:
        virtual ~character() {}
        int x;
        int y;
        char symbol;
        bool exists;
};

class pc : public character {
    public:
    int hp;
    bool isBuffed;

};

class npc : public character {
    public :


        int hp;
        char dir;
        char sym;
        int uid;
        int dx;
        int dy;

        bool exis() {return exists;}

};

class snake : public npc {
    public:
        /*
        snake(int rx, int ry, int rhp, bool exist, char symb) {
            x = rx;
            y = ry;
            hp = rhp;
            exists = exist;
            sym = symb;
        }
*/


};

enum tile_type {wall, floore, gate, item, health};


class tile {
    public:
        int x;
        int y;
        tile_type type;
};

class projectile : public printable {
    public:

        projectile(int dX, int dY, int rx, int ry, int t, char rdir)
        {
            deltaX = dX;
            deltaY = dY;
            x = rx;
            y = ry;
            damage = t;
            dir = rdir;
        }

        int deltaX;
        int deltaY;
        int x;
        int y;
        int damage;
        char dir;
};

class world_map {
    public:
        tile map[MAP_Y][MAP_X];
        character charMap[MAP_Y][MAP_X];
        projectile *printMap[MAP_Y][MAP_X];
        npc *npcMap[MAP_Y][MAP_X];
};

class world_all {
    public:
        world_map world[16][16];
        world_map cur_map;
};



void init();
void init_map();
void print_map();




#endif // MAIN_H_
