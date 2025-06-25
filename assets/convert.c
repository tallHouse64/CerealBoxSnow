#include<stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

//char image[] = {};

int main(int argc, char ** argv){

    if(argc < 4){
        printf("Usage:\nconvert image-file output-file unique-identifier\n\nThis program converts an image file to an array\nin the form of a header file that can be copied\nfrom or included in C source code.\n\nThe unique identifier is used for the name of the\narray and the header file include guards for\nexample \"#ifndef IMAGE_unique_identifier_H\".\n");
        return 0;
    };

    int w = 0, h = 0, n = 0;
    unsigned char * data = stbi_load(argv[1], &w, &h, &n, 4);
    if(!data){
        printf("Couldn't open the image file.\n");
    };

    FILE * f = fopen(argv[2], "w");

    fprintf(f, "/*\n");
    fprintf(f, " * Don't edit this file, it is generated.\n");
    fprintf(f, " * Please read note.txt and use the ./convert\n");
    fprintf(f, " * command instead.\n");
    fprintf(f, " */\n");

    fprintf(f, "\n#ifndef IMAGE_%s_H\n", argv[3]);
    fprintf(f,   "#define IMAGE_%s_H\n", argv[3]);

    fprintf(f, "\nint %sW = %d;\n", argv[3], w);
    fprintf(f,   "int %sH = %d;\n", argv[3], h);

    fprintf(f, "\nchar %s[] = {", argv[3]);

    int x = 0, y = 0;
    while(y < h){

        x = 0;
        while(x < w){

            fprintf(f, "%d", data[(y * w) + x]);

            if(y == h - 1 && x == w - 1){

            }else{
                fprintf(f, ", ");
            };

            x++;
        };

        fprintf(f, "\n");

        y++;
    };

    fprintf(f, "\n};");
    fprintf(f, "\n#endif\n");

    fclose(f);
};
