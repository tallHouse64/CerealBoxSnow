#define D_IMPLEMENTATION
#include"d.h"
#undef D_IMPLEMENTATION

#include"sdltod/tod.h"

#define DELAY 1000/30
#define MAX_PRTS 4096

struct prt_t{
    int x, y;
} prts[MAX_PRTS];

D_Surf * out = D_NULL;
D_Event e;
int running = 1;
int prtsInUse = 100;
int prtW = 10;
int prtH = 10;
int mousePressed = 0; //Only 1 on the same frame mouse down happened, otherwise 0
D_Point mouse = {0};
int framesTillUiHide = 45; //About 1.5 sec
int framesSinceMouseEvent = 0;

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

int drawPrtSlider(){
    //s for the slider itself
    D_Rect s = {out->w - 20, out->h - 210, 10, 200};

    D_FillRect(out, &s, D_rgbaToFormat(out->format, 150, 130, 120, 255));

    if(D_PointInRect(&mouse, &s) && mousePressed){
        //The number (mouse.y - s.y) is assumed to be between 0 and 199

        prtsInUse = (((mouse.y - s.y) * MAX_PRTS) / (s.h - 1));
        //printf("prtsInUse: %d\n", prtsInUse);
    };


    //The box is the slider handle
    D_Rect box = {s.x, 0, s.w, s.w};

    //(prtsInUse / MAX_PRTS) * (s.h / 1)
    box.y = (s.y + ((prtsInUse * s.h) / MAX_PRTS)) - (box.h / 2);
    D_FillRect(out, &box, D_rgbaToFormat(out->format, 200, 170, 160, 255));
};

int draw(){
    D_FillRect(out, D_NULL, D_rgbaToFormat(out->format, 20, 20, 20, 255));

    D_Rect r = {0, 0, prtW, prtH};
    int i = 0;
    while(i < prtsInUse){
        r.x = prts[i].x;
        r.y = prts[i].y;
        D_FillRect(out, &r, D_rgbaToFormat(out->format, 255, 255, 255, 255));
        r.w = prtW;
        r.h = prtH;
        i++;
    };

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
    while(i < MAX_PRTS){
        prts[i].x = rng() % out->w;
        prts[i].y = -(rng() % out->h);
        i++;
    };

    D_StartEvents();

    while(running){
        D_PumpEvents();
        mousePressed = 0;
        framesSinceMouseEvent++;

        while(D_GetEvent(&e) != -1){
            switch(e.type){

                case D_MOUSEMOVE:
                    mouse.x = e.mouse.x;
                    mouse.y = e.mouse.y;
                    framesSinceMouseEvent = 0;
                    break;

                case D_MOUSEDOWN:
                    mousePressed = 1;
                    framesSinceMouseEvent = 0;
                    break;

                case D_QUIT:
                    running = 0;
                    break;

            };

        };

        updatePhysics();

        draw();
        drawPrtSlider();

        D_FlipOutSurf(out);

        D_Delay(DELAY);
    };

    D_StopEvents();

    D_FreeOutSurf(out);
    out = D_NULL;
};
