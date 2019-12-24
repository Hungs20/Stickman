#ifndef IMAGE_H_INCLUDED
#define IMAGE_H_INCLUDED

#include <string>
#define IDLE_IMAGE 0
#define RIGHT_IMAGE 1
#define LEFT_IMAGE 2
#define JUMP_IMAGE 3
#define SIT_IMAGE 4
#define SHOT_IDLE_IMAGE 5
#define SHOT_SIT_IMAGE 6

const std::string playerImage[] =
{
" o \n"
"/|\\\n"
"/ \\",

" o\n"
"/|~\n"
"~\\",

" o \n"
"~|\\\n"
" /~",

"\\o/\n"
" | \n"
"_ _",

"  o\n"
"/;;\\",

" o\n"
"/|-}\n"
"/ \\",

"  o\n"
"/;;-}"
};

const std::string enemyImage =
" \\ /\n"
"< o >\n"
" / \\";

const std::string bossImage =
" \\___/\\___/\n"
"={:.:}{:.:}=\n"
" ||^||||~||";

const std::string amoHeadImage = "i";
const std::string amoImage = "=>";
const std::string amoBossImage = "o";
#endif // IMAGE_H_INCLUDED
