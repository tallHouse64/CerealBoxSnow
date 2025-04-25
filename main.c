#define D_IMPLEMENTATION
#include"d.h"

#define DEVENTS_IMPLEMENTATION
#include"devents.h"

#define D_PLATFORM_IMPLEMENTATION
#include"platform/sdld.h"

#define DELAY 1000/30
#define MAX_PRTS 4096

enum gameType_t {
    GAME_TYPE_SNOW = 0,
    GAME_TYPE_FIRE,
    NUM_GAME_TYPES
} gameType = GAME_TYPE_SNOW;

struct prt_t{
    int x, y;
} prts[MAX_PRTS];

D_Surf * out = D_NULL;
D_Event e;
int running = 1;
int prtsInUse = 100;
int prtW = 10;
int prtH = 10;
int mouseDown = 0; //This is mouse button state
D_Point mouse = {0};
int framesTillUiHide = 25; //A little less than 1 sec
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

    if(D_PointInRect(&mouse, &s) && mouseDown){
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

int drawGameTypeButton(){
    D_Rect b = {out->w - 100, out->h - 50, 70, 40};

    if(D_PointInRect(&mouse, &b) && mouseDown){
        gameType += 1;
        gameType = gameType % NUM_GAME_TYPES;
    };

    D_FillRect(out, &b, D_rgbaToFormat(out->format, 200, 170, 160, 255));
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

void updateSnowPrt(struct prt_t * p){
    p->y += (rng() % 10) + 5;

    if((rng() % 2) == 1){
        p->x += 10;
    }else{
        p->x -= 10;
    };

    //If the particle is off the left teleport it to the right
    if(p->x < -prtW){
        p->x += out->w;
    };

    //If the particle is off the right teleprt it to the left
    if(p->x >= out->w){
        p->x -= out->w;
    };

    //If the partic is off the bottom then teleport it to the top
    if(p->y >= out->h){
        p->y = -prtH;
    };
};

void updateFirePrt(struct prt_t * p){
    p->y -= (rng() % 10) + 5;

    if((rng() % 2) == 1){
        p->x += (rng() % 15) + 5;
    }else{
        p->x -= (rng() % 15) + 5;
    };

    if(p->y != 0){
        if(rng() % (p->y) == 0){
            p->x = ((out->w / 2) - (prtW / 2)) + ((rng() % 20) - 5);
            p->y = ((out->h - prtH) - 20);
        };
    };

    if(p->x < -prtW || p->x >= out->w || p->y <= -prtH || (rng() % 64) == 0){
        p->x = ((out->w / 2) - (prtW / 2)) + ((rng() % 20) - 5);
        p->y = ((out->h - prtH) - 20);
    };
};

int updatePhysics(){
    int i = 0;
    int dir = 0;
    while(i < prtsInUse){

        switch(gameType){
            case GAME_TYPE_SNOW:
                updateSnowPrt(&prts[i]);
                break;
            case GAME_TYPE_FIRE:
                updateFirePrt(&prts[i]);
                break;
        };

        i++;
    };
};

int main(){

    out = D_GetOutSurf(50, 50, 640, 480, "Cereal Box Snow", 0);

    int i = 0;
    while(i < MAX_PRTS){
        prts[i].x = rng() % out->w;
        prts[i].y = -(rng() % out->h);
        i++;
    };

    D_StartEvents();

    while(running){
        D_PumpEvents();
        framesSinceMouseEvent++;

        while(D_GetEvent(&e) != -1){
            switch(e.type){

                case D_MOUSEMOVE:
                    mouse.x = e.mouse.x;
                    mouse.y = e.mouse.y;
                    framesSinceMouseEvent = 0;
                    break;

                case D_MOUSEUP:
                    mouseDown = 0;
                    framesSinceMouseEvent = 0;
                    break;

                case D_MOUSEDOWN:
                    mouseDown = 1;
                    framesSinceMouseEvent = 0;
                    break;

                case D_QUIT:
                    running = 0;
                    break;

            };

        };

        updatePhysics();

        draw();

        if(framesSinceMouseEvent < framesTillUiHide){
            drawPrtSlider();
            drawGameTypeButton();
        };

        D_FlipOutSurf(out);

        D_Delay(DELAY);
    };

    D_StopEvents();

    D_FreeOutSurf(out);
    out = D_NULL;
};
