#define D_IMPLEMENTATION
#include"../d.h"
#undef D_IMPLEMENTATION

#include"../sdltod/tod.h"

D_Surf * out = D_NULL;

int rng(){
    static int n = 42444;
    //n = (((n * n * n) + n + 3) % 65535);
    n = ((55 * n) + 47 + (n % 3)) % 65536;

    //make sure number is not repeating odd, even, odd, even, odd, even
    /*if(n % 32368 == 0){
        n += 8;
    };*/
};


int main(){
    out = D_GetOutSurf(50, 50, 640, 480, "Rng Test");

    int i = 0;
    int n = 0;
    while(i < out->w * out->h){
        n = rng();
        printf("rng: %d\n", n);
        ((D_uint32 *)out->pix)[i] = n;
        i++;
    };

    D_FlipOutSurf(out);

    D_Delay(3000);

    D_FreeOutSurf(out);
    out = D_NULL;

    return 0;
};
