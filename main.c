#define D_IMPLEMENTATION
#include"d.h"
#undef D_IMPLEMENTATION

#include"sdltod/tod.h"

#define DELAY 1000/30
#define MAX_PRTS 1

struct prt_t{
    int x, y;
} prts[MAX_PRTS];

D_Surf * out = D_NULL;
D_Event e;
int running = 1;
int prtsInUse = 1;
int prtW = 10;
int prtH = 10;
D_Point mouse = {0};

//Maybe make this better, I just guesse. Not tested.
int rng(){
    static int n = 42445;
    n = ((n * n) % 46718) >> 1;
};

int draw(){
    D_FillRect(out, D_NULL, D_rgbaToFormat(out->format, 20, 20, 20, 255));

    D_Rect r = {0, 0, prtW, prtH};
    int i = 0;
    while(i < prtsInUse){
        r.x = prts[i].x;
        r.y = prts[i].y;
        D_FillRect(out, &r, D_rgbaToFormat(out->format, 255, 255, 255, 255));
        i++;
    };

    D_FlipOutSurf(out);
};

int updatePhysics(){
    int i = 0;
    int dir = 0;
    while(i < prtsInUse){

        prts[i].y += 10;
        if((rng() % 2) == 1){
            prts[i].x += 10;
        }else{
            prts[i].x -= 10;
        };

        if(prts[i].y >= out->h){
            prts[i].y = -prtH;
        };
        i++;
    };
};

int main(){

    int i = 0;
    while(i < MAX_PRTS){
        prts[i].x = 10;
        prts[i].y = 10;
        i++;
    };

    out = D_GetOutSurf(50, 50, 640, 480, "Snow Sim");

    D_StartEvents();

    while(running){
        D_PumpEvents();

        while(D_GetEvent(&e) != -1){
            switch(e.type){
                case D_MOUSEMOVE:
                    mouse.x = e.mouse.x;
                    mouse.y = e.mouse.y;
                    break;
                case D_QUIT:
                    running = 0;
                    break;
            };

        };

        updatePhysics();

        draw();

        D_Delay(DELAY);
    };

    D_StopEvents();

    D_FreeOutSurf(out);
    out = D_NULL;
};
