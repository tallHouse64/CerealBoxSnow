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
    int r, g, b, a;
} prts[MAX_PRTS];

D_Surf * out = D_NULL;
D_Event e;
int running = 1;
int prtsInUse = 100;
int prtW = 10;
int prtH = 10;
int mouseDown = 0; //This is mouse button state
int mousePressed = 0;
int mouseReleased = 0;
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
    return n;
};

int rng2(){
    static int n = 718377;
    //n = (((n * n * n) + n + 3) % 65535);
    n = ((449 * n) + 137 + (n % 5)) % 65536;

    //make sure number is not repeating odd, even, odd, even, odd, even
    /*if(n % 32368 == 0){
     *      n += 8;
     */
    return n;
};

void setGameType(enum gameType_t nextType){

    switch(nextType){
        case GAME_TYPE_FIRE:

            for(int i = 0; i < MAX_PRTS; i++){
                prts[i].x = 0;
                prts[i].y = -prtH;
            };

            break;
        case GAME_TYPE_SNOW:

            for(int i = 0; i < MAX_PRTS; i++){
                prts[i].r = 255;
                prts[i].g = 255;
                prts[i].b = 255;
                prts[i].a = 255;

                prts[i].x = (rng() + (rng2() % 100)) % out->w;
                prts[i].y = -((rng() % out->h) + (rng2() % 500));

                //prts[i].x += ((i % 100) * 20);
                //prts[i].y -= ((i % 5) * (rng() % out->w));

                //prts[i].x += rng() % 11;
                //prts[i].x += (rng() % 3) - 1;

                /*if(i % 12 == 0){
                    prts[i].x += (rng() % 40) - 20;
                };*/
            };

            break;
    };

    gameType = nextType;
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

int drawGameTypeButtonRight(){
    D_Rect b = {out->w - 70, out->h - 75, 40, 65};

    if(D_PointInRect(&mouse, &b) && mouseReleased){
        setGameType((gameType + 1) % NUM_GAME_TYPES);
        //gameType += 1;
        //gameType = gameType % NUM_GAME_TYPES;
    };

    D_FillRect(out, &b, D_rgbaToFormat(out->format, 200, 170, 160, 255));
};

int draw(){
    D_FillRect(out, D_NULL, D_rgbaToFormat(out->format, 20, 20, 20, 255));

    int r = 0, g = 0, b = 0, a = 0; //Temp storage for the particles colour

    D_Rect rect = {0, 0, prtW, prtH};
    int i = 0;
    while(i < prtsInUse){
        rect.x = prts[i].x;
        rect.y = prts[i].y;

        D_BlendNormal(prts[i].r, prts[i].g, prts[i].b, prts[i].a, 0, 0, 0, 0, &r, &g, &b, &a);
        D_FillRect(out, &rect, D_rgbaToFormat(out->format, r, g, b, a));

        rect.w = prtW;
        rect.h = prtH;
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

    int respawn = 0;

    //Move the particle down
    p->y -= (rng() % 10) + 5;

    //Another way of moving the particle down
    /*p->y -= (rng() % 7) - 1;
    p->y -= p->g / 32;*/

    //Move left or right randomly
    if((rng() % 2) == 1){
        p->x += (rng() % 15) + 5;
    }else{
        p->x -= (rng() % 15) + 5;
    };

    //Make it more transparent randomly
    p->a -= (rng() % 25) - 10;
    if(p->a < 0)p->a = 0;
    if(p->a > 255)p->a = 255;

    //Make it more red randomly
    p->g -= (rng() % 10);
    if(p->g < 0)p->g = 0;
    if(p->g > 255)p->g = 255;

    //Randomly respawn particles (more likely higher up)
    if(p->y != 0){
        if(rng() % (p->y) == 0){
            respawn = 1;
        };
    };

    //Respawn if off screen (or on a 1 in 16 chance respawn anyway)
    if(p->x < -prtW || p->x >= out->w || p->y <= -prtH || (rng() % 16) == 0){
        respawn = 1;
    };

    if(respawn){
        p->x = ((out->w / 2) - (prtW / 2)) + ((rng() % 20) - 5);
        p->y = ((out->h - prtH) - 20);

        p->r = 255;
        p->g = (rng() % 20) + 236; //(rng() % 220) + 35; //used to be 83
        p->b = 0;
        p->a = 255;

        //Make it more likely red colours are chosen
        /*if(rng() % 2 != 0){
            p->g = p->g / 2;
        };*/

    };
};

int updatePhysics(){
    int i = 0;
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

    setGameType(GAME_TYPE_SNOW);

    D_StartEvents();

    while(running){
        D_PumpEvents();
        mousePressed = 0;
        mouseReleased = 0;
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
                    mouseReleased = 1;
                    framesSinceMouseEvent = 0;
                    break;

                case D_MOUSEDOWN:
                    mouseDown = 1;
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

        if(framesSinceMouseEvent < framesTillUiHide){
            drawPrtSlider();
            drawGameTypeButtonRight();
        };

        D_FlipOutSurf(out);

        D_Delay(DELAY);
    };

    D_StopEvents();

    D_FreeOutSurf(out);
    out = D_NULL;
};
