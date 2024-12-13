#define D_IMPLEMENTATION
#include"d.h"
#undef D_IMPLEMENTATION

#include"sdltod/tod.h"

#define DELAY 1000/30
#define MAX_PRTS 1024

struct prt_t{
    int x, y;
} prts[MAX_PRTS];

D_Surf * out = D_NULL;
D_Event e;
int running = 1;
int prtsInUse = 10;
int prtW = 10;
int prtH = 10;
D_Point mouse = {0};

//Linear congruential generator (almost)
int rng(){
    static int n = 42444;
    //n = (((n * n * n) + n + 3) % 65535);
    n = ((55 * n) + 47 + (n % 3)) % 65536;

    //make sure number is not repeating odd, even, odd, even, odd, even
    /*if(n % 32368 == 0){
     *      n += 8;
    */
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

        prts[i].y += (rng() % 10) + 5;

        if((rng() % 2) == 1){
            prts[i].x += 10;
        }else{
            prts[i].x -= 10;
        };

        //If the particle is off the left teleport it to the right
        if(prts[i].x < -prtW){
            prts[i].x += out->w;
        };

        //If the particle is off the right teleprt it to the left
        if(prts[i].x >= out->w){
            prts[i].x -= out->w;
        };

        //If the partic is off the bottom then teleport it to the top
        if(prts[i].y >= out->h){
            prts[i].y = -prtH;
        };
        i++;
    };
};

int main(){

    out = D_GetOutSurf(50, 50, 640, 480, "Snow Sim");

    int i = 0;
    while(i < prtsInUse){
        prts[i].x = rng() % out->w;
        prts[i].y = -(rng() % out->h);
        i++;
    };

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
