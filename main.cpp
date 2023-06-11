#include <iostream>
#include <windows.h>
#include "snake.h"

std::string GetCurrentDirectory(){
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos);
}
int main (int argc, char *argv[]){
    std::string exePath = GetCurrentDirectory();
    std::string fontPath = (exePath + "/font/arial.ttf");
    srand(time(0));
    GameManager game;
    if (game.Init()){
        game.clean();
       return 1;
    }
    game.snake = new Snake(500,300);
    game.font = TTF_OpenFont(fontPath.c_str(),24);
    game.fontB = TTF_OpenFont(fontPath.c_str(),44);
    game.Loop();
    game.clean();
    return 0;
}
