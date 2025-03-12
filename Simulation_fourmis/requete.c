#include <stdlib.h>
#include "requete.h"

coord creer_coord(int y, int x) {
    coord c;
    c.y = y;
    c.x = x;
    return c;
}

requete* creer_requete(int type, coord c1, coord c2) {
    requete *r = malloc(sizeof(requete));
    r->type_req = type;
    r->c1 = c1;
    r->c2 = c2;
    return r;
}
    