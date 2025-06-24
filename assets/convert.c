#include<stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

//char image[] = {};

int main(int argc, char ** argv){

    if(argc < 2){
        printf("Usage:\nconvert [image]\n\nThis program converts an image file to an array\nthat can be copied into C source code.\n");
        return 0;
    };

    int w = 0, h = 0, n = 0;
    unsigned char * data = stbi_load(argv[1], &w, &h, &n, 0);
    if(!data){
        printf("Couldn't open file.\n");
    };


    printf("char image[] = {");

    int x = 0, y = 0;
    while(y < h){

        x = 0;
        while(x < w){

            printf("%d", data[(y * w) + x]);

            if(y == h - 1 && x == w - 1){

            }else{
                printf(", ");
            };

            x++;
        };

        printf("\n");

        y++;
    };

    printf("\n};");
};
