#define D_IMPLEMENTATION
#include"d.h"

#define DEVENTS_IMPLEMENTATION
#include"devents.h"

#define D_PLATFORM_IMPLEMENTATION

#ifdef NDS
#include"platform/ndsd.h"
#include<nds.h>
#else
#include"platform/sdld.h"
#endif

#include"assets/font.h"
#include"assets/drwslib.h"

#define DELAY 1000/30
#define MAX_PRTS 4096

enum gameType_t {
    GAME_TYPE_SNOW = 0,
    GAME_TYPE_FIRE,
    GAME_TYPE_RAIN,
    NUM_GAME_TYPES
} gameType = GAME_TYPE_SNOW;

enum mouseFocus_t {
    MOUSE_FOCUS_NONE = 0,
    MOUSE_FOCUS_SLIDER,
    MOUSE_FOCUS_GAME_TYPE_BUTTON_RIGHT,
    MOUSE_FOCUS_BACKGROUND
} mouseFocus = MOUSE_FOCUS_NONE;

struct prt_t{
    int x, y;
    int xSpeed, ySpeed;
    int r, g, b, a;
} prts[MAX_PRTS];

D_Surf * out = D_NULL;
D_Event e;
int running = 1;
int prtsInUse = 100;
int mouseDown = 0; //This acts like a bool of the mouse down state
int mousePressed = 0;
int mouseReleased = 0;
D_MouseButton mouseButton = D_NOBUTTON; /*State of the mouse buttons*/
D_Point mouse = {0};
int framesTillUiHide = 25; //A little less than 1 sec
int framesSinceMouseEvent = 0;
int introFrameCount = 0;
int ndsMouseToggle = 1; /*0 for pull particles, 1 for push.*/
D_Surf * font = D_NULL;
D_Surf * drwslib = D_NULL;

#ifdef NDS
/*Make the snow 4 by 4 pixels on DS*/
const int prtW = 4;
const int prtH = 4;
#else
/*Make the snow 10 by 10 pixels on PC*/
const int prtW = 10;
const int prtH = 10;
#endif

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
        case GAME_TYPE_RAIN:

            for(int i = 0; i < MAX_PRTS; i++){
                prts[i].r = 35;
                prts[i].g = 83;
                prts[i].b = 255;
                prts[i].a = 255;

                prts[i].ySpeed = (rng() % 30) + 20;

                prts[i].x = (rng() + (rng2() % 100)) % out->w;
                prts[i].y = -((rng() % out->h) + (rng2() % 500));
            };

            break;
    };

    gameType = nextType;
};

int drawPrtSlider(){
#ifdef NDS
    const int sliderHeight = 170;
#else
    const int sliderHeight = 200;
#endif

    //s for the slider itself
    D_Rect s = {out->w - 20, out->h - (sliderHeight + 10), 10, sliderHeight};

    D_FillRect(out, &s, D_rgbaToFormat(out->format, 150, 130, 120, 255));

    if((D_PointInRect(&mouse, &s) && mouseDown && mouseFocus == MOUSE_FOCUS_NONE) || (mouseDown && mouseFocus == MOUSE_FOCUS_SLIDER)){

        /*If clicking the slider here does not
         * make the number of particles less than
         * 0 or more than MAX_PRTS:
         */
        if(mouse.y - s.y >= 0 && mouse.y - s.y < sliderHeight){
            /*Change the number of particles*/
            prtsInUse = (((mouse.y - s.y) * MAX_PRTS) / (s.h - 1));
        };

        //printf("prtsInUse: %d\n", prtsInUse);

        mouseFocus = MOUSE_FOCUS_SLIDER;
    };


    //The box is the slider handle
    D_Rect box = {s.x, 0, s.w, s.w};

    /*Draw the slider handle with it's height
     * lowered according to prtsInUse.
     */
    box.y = (s.y + ((prtsInUse * s.h) / MAX_PRTS)) - (box.h / 2);
    D_FillRect(out, &box, D_rgbaToFormat(out->format, 200, 170, 160, 255));
};

int drawGameTypeButtonRight(){
    D_Rect b = {out->w - 70, out->h - 50, 40, 40};

    if(D_PointInRect(&mouse, &b) && mousePressed && mouseFocus == MOUSE_FOCUS_NONE){
        mouseFocus = MOUSE_FOCUS_GAME_TYPE_BUTTON_RIGHT;
    };

    if(D_PointInRect(&mouse, &b) && mouseReleased && mouseFocus == MOUSE_FOCUS_GAME_TYPE_BUTTON_RIGHT){
        setGameType((gameType + 1) % NUM_GAME_TYPES);
        //gameType += 1;
        //gameType = gameType % NUM_GAME_TYPES;
    };

    D_FillRect(out, &b, D_rgbaToFormat(out->format, 200, 170, 160, 255));
};

/*int drawGameTypeButtonLeft(){
    D_Rect b = {out->w - 117, out->h - 75, 40, 65};

    if(D_PointInRect(&mouse, &b) && mouseReleased){

        //Set the game type to the previous one, (or the last game type if 0)
        if(gameType <= 0){
            setGameType(NUM_GAME_TYPES - 1);
        }else{
            setGameType(gameType - 1);
        };

    };

    D_FillRect(out, &b, D_rgbaToFormat(out->format, 200, 170, 160, 255));
};*/

int drawIntro(){

    static D_Surf * intro1 = D_NULL;
    static D_Surf * intro2 = D_NULL;

    /* The number in textSize controls more than
     *  just the size of the text, it also
     *  controls the size of the entire intro
     *  text and logo ("Cereal Box Snow" and
     *  "Powered by drws lib").
     */
#ifdef NDS
    const int textSize = 10;
#else
    const int textSize = 20;
#endif

    if(introFrameCount * DELAY > 10000 || introFrameCount == -1){ //After about 10 sec dont't draw

        if(introFrameCount != -1){

            //This runs once after the intro
            font->alphaMod = 255;
            drwslib->alphaMod = 255;
            introFrameCount = -1;

            if(intro1 != D_NULL){
                D_FreeSurf(intro1);
                intro1 = D_NULL;
            };

            if(intro2 != D_NULL){
                D_FreeSurf(intro2);
                intro2 = D_NULL;
            };
        };

        return 0;
    };

    introFrameCount++;
    //printf("introFrameCount %d\n", introFrameCount);

    //D_Rect r = {(out->w / 2) - 150, (out->h / 2) - 38, 300, 75}; //The original rectangle
    D_Rect r = {(out->w / 2) - ((textSize * 15) / 2), (out->h / 2) - ((textSize * 4) / 2), textSize * 15, textSize * 4};
    D_Point p = {r.x + 10, r.y + 100};
    //D_Rect drwslibRect = {r.x + 10, r.y + 10, (textSize * 3) - 4, (textSize * 3) - 4}; //Old rect, relative to the window top left
    D_Rect drwslibRect = {10, 10, (textSize * 3) - 4, (textSize * 3) - 4};

    if(introFrameCount == 1){ /*This runs once.*/


#ifdef NDS
        intro1 = D_CreateSurf(r.w, r.h, D_FindPixFormat(0x001F, 0x03E0, 0x7C00, 0x8000, 16));
        intro2 = D_CreateSurf(r.w, r.h, D_FindPixFormat(0x001F, 0x03E0, 0x7C00, 0x8000, 16));
        D_FillRect(intro1, D_NULL, D_rgbaToFormat(intro1->format, 30, 30, 30, 255));
        D_FillRect(intro2, D_NULL, D_rgbaToFormat(intro2->format, 30, 30, 30, 255));
#else
        intro1 = D_CreateSurf(r.w, r.h, D_FindPixFormat(0xFF, 0xFF00, 0xFF0000, 0xFF000000, 32));
        intro2 = D_CreateSurf(r.w, r.h, D_FindPixFormat(0xFF, 0xFF00, 0xFF0000, 0xFF000000, 32));
#endif
        /*Make the first part of the intro "Cereal Box Snow"*/
        D_Point introP = {10, 10};

        D_PrintToSurf(intro1, font, &introP, textSize, 0, "Cereal Box");
        introP.y += textSize + 5;
        D_PrintToSurf(intro1, font, &introP, textSize, 0, "Snow");



        /*Make the second part of the intro "Powered By Drws lib"*/
        introP.x = 10;
        introP.y = 10;

        D_SurfCopyScale(drwslib, D_NULL, intro2, &drwslibRect);
        introP.x += drwslibRect.w + 10;
        D_PrintToSurf(intro2, font, &introP, textSize, 0, "Powered by");
        introP.y += textSize + 5;
        D_PrintToSurf(intro2, font, &introP, textSize, 0, "Drws lib");
    };


    //D_FillRect(out, &r, D_rgbaToFormat(out->format, 0, 0, 0, 255));

    if(introFrameCount * DELAY < 3000){
        //This starts at 0 sec and ends at 3 sec

#ifdef NDS

        /* The below 5 lines are an optimisation
         *  for DS that does the same thing as
         *  the line
         *  "D_SurfCopyScale(intro1, D_NULL, out, &r);"
         */
        int i = 0;
        while(i < intro1->h){
            dmaCopyWords(0, ((char *)(intro1->pix)) + (i * intro1->w * 2), (((char *)(out->pix)) + (i * out->w * 2)) + (r.x * 2) + (r.y * out->w * 2), intro1->w * 2);
            i++;
        };
#else
        D_SurfCopyScale(intro1, D_NULL, out, &r);
#endif

        //Fade out after 2.5 sec
        if(introFrameCount * DELAY > 2500){

            if(intro1->alphaMod > 64){
                intro1->alphaMod -= 64;
            }else{
                intro1->alphaMod = 0;
            };

        };

        intro2->alphaMod = 0;

    }else if(introFrameCount * DELAY < 6000){
        //This starts at 3 sec and ends at 6 sec

        //D_SurfCopyScale(intro2, D_NULL, out, &r);
#ifdef NDS

        /* The below 5 lines are an optimisation
         *  for DS that does the same thing as
         *  the line
         *  "D_SurfCopyScale(intro2, D_NULL, out, &r);"
         */
        int i = 0;
        while(i < intro2->h){
            dmaCopyWords(0, ((char *)(intro2->pix)) + (i * intro2->w * 2), (((char *)(out->pix)) + (i * out->w * 2)) + (r.x * 2) + (r.y * out->w * 2), intro2->w * 2);
            i++;
        };
#else
        D_SurfCopyScale(intro2, D_NULL, out, &r);
#endif

        //Fade in starting at 3 sec, stop at 3.5 sec
        if(introFrameCount * DELAY < 3500){

            if(intro2->alphaMod < 191){
                intro2->alphaMod += 64;
            }else{
                intro2->alphaMod = 255;
            };

        };

        //Fade out at 5.5 sec
        if(introFrameCount * DELAY > 5500){

            if(intro2->alphaMod > 64){
                intro2->alphaMod -= 64;
            }else{
                intro2->alphaMod = 0;
            };

        };

    };

    return 0;
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
    p->y += (rng() % prtW) + (prtW / 2);

    if((rng() % 2) == 1){
        p->x += prtW;
    }else{
        p->x -= prtW;
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
    p->y -= (rng() % prtW) + (prtW / 2);

    //Another way of moving the particle down
    /*p->y -= (rng() % 7) - 1;
    p->y -= p->g / 32;*/

    //Move left or right randomly
    if((rng() % 2) == 1){
        p->x += (rng() % (prtW + (prtW >> 1))) + (prtW >> 1);
    }else{
        p->x -= (rng() % (prtW + (prtW >> 1))) + (prtW >> 1);
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
        p->y = ((out->h - prtH) - (prtW * 2));

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

void updateRainPrt(struct prt_t * p){

    p->y += p->ySpeed;

    /*If the particle has reached the bottom:*/
    if(p->y > out->h){

        /*Respawn at the top (teleport).*/
        p->y = (0 - prtH) - (rng() % 30);

        /*Wander the particle randomly left or right*/
        p->x = p->x + ((rng() % 11) - 5);

        /*If the particle wanders too far left:*/
        if(p->x < -prtW){
            /*Teleport it to the right of the screen.*/
            p->x = p->x + out->w;
        };

        /*If the particle wanders too far right:*/
        if(p->y > out->w){
            /*Teleport it to the left of the screen.*/
            p->x = p->x - out->w;
        };
    };

    return;
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
            case GAME_TYPE_RAIN:
                updateRainPrt(&prts[i]);
                break;
        };

        i++;
    };
};

/* This function attracts particles toward a
 *  point if they are close enough.
 *
 * The recommended move speed is prtW * 2.
 *
 * x: The x position the particles move to.
 * y: The y position the particles move to.
 * moveSpeed: The speed that particles move
 *  toward the point (pixels per frame).
 */
void attractPrts(int x, int y, int moveSpeed){
    int i = 0;
    int squaredDistance = 0;
    /*int numPrtsAffected = 0;*/
    while(i < MAX_PRTS){
        //prts[i].y = prts[i].y + 20;

        squaredDistance = ((x - prts[i].x) * (x - prts[i].x)) + ((y - prts[i].y) * (y - prts[i].y));
        //printf("squaredDistance: %d\n", squaredDistance);

        /*if(squaredDistance != 0 && (5000 / squaredDistance) != 0){
            numPrtsAffected++;
            prts[i].x = x + ((x - prts[i].x) / (5000 / squaredDistance));
            prts[i].y = y + ((y - prts[i].y) / (5000 / squaredDistance));
        };*/

        /*If the particle is within 100 pixels of
         * the x,y point: (100 pixels for PC)*/
        if(squaredDistance < (prtW * 10 * prtW * 10)){
            /*numPrtsAffected++;*/

            /*If the particle is to the left:*/
            if(prts[i].x < x){
                /*Move it right*/
                prts[i].x = prts[i].x + moveSpeed;

            /*If the particle is to the right:*/
            }else if(prts[i].x > x){
                /*Move it left*/
                prts[i].x = prts[i].x - moveSpeed;
            };

            /*If the particle is above:*/
            if(prts[i].y < y){
                /*Move it down*/
                prts[i].y = prts[i].y + moveSpeed;

            /*If the particle is below:*/
            }else if(prts[i].y > y){
                /*Move it up*/
                prts[i].y = prts[i].y - moveSpeed;
            };
        };

        i++;
    };
    /*printf("numPrtsAffected %d\n", numPrtsAffected);*/
};

int main(int argc, char ** argv){

    out = D_GetOutSurf(50, 50, 640, 480, "Cereal Box Snow", D_OUTSURFRESIZABLE);
    font = D_CreateSurfFrom(fontDataW, fontDataH, D_FindPixFormat(0xFF, 0xFF00, 0xFF0000, 0xFF000000, 32), fontData);
    drwslib = D_CreateSurfFrom(drwslibDataW, drwslibDataH, D_FindPixFormat(0xFF, 0xFF00, 0xFF0000, 0xFF000000, 32), drwslibData);

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
                    mouseButton = D_NOBUTTON;
                    break;

                case D_MOUSEDOWN:
                    mouseDown = 1;
                    mousePressed = 1;
                    framesSinceMouseEvent = 0;
                    mouseButton = e.mouse.button;
                    mouseFocus = MOUSE_FOCUS_NONE;
                    break;

                case D_OUTSURFRESIZE:
                    out = D_GetResizedOutSurf(out);
                    setGameType(gameType); /*Reset the simulation.*/
                    break;

                case D_QUIT:
                    running = 0;
                    break;

            };

        };

#ifdef NDS
        if(keysUp() & (KEY_UP | KEY_X)){
            ndsMouseToggle = ndsMouseToggle ? 0 : 1;
        };
#endif

        updatePhysics();

        draw();

        if(framesSinceMouseEvent < framesTillUiHide){
            drawPrtSlider();
            //drawGameTypeButtonLeft();
            drawGameTypeButtonRight();
        };

        /*Move particles on mouse click.*/
        if(mouseDown && (mouseFocus == MOUSE_FOCUS_NONE || mouseFocus == MOUSE_FOCUS_BACKGROUND)){
            mouseFocus = MOUSE_FOCUS_BACKGROUND;

#ifdef NDS
            if(ndsMouseToggle){
                attractPrts(mouse.x, mouse.y, prtW * 2);
            }else{
                attractPrts(mouse.x, mouse.y, -prtW * 2);
            };
#else
            if(mouseButton & D_LEFTBUTTON){
                attractPrts(mouse.x, mouse.y, prtW * 2);
            }else if(mouseButton & D_RIGHTBUTTON){
                attractPrts(mouse.x, mouse.y, -prtW * 2);
            };
#endif
        };

        drawIntro();

        D_FlipOutSurf(out);

#ifdef NDS
        swiWaitForVBlank();
#else
        D_Delay(DELAY);
#endif
    };

    D_StopEvents();

    D_FreeOutSurf(out);
    out = D_NULL;
};
