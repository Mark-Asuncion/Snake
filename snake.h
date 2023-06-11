#ifndef __SNAKE__
#define __SNAKE__
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <deque>
#include <string>
#include <ctime>

#ifndef WIDTH
#define WIDTH 800
#endif
#ifndef HEIGHT
#define HEIGHT 600
#endif
enum Directions {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

typedef std::deque<SDL_Rect> VSDL_SnakeBody;
typedef std::deque<SDL_Rect>::iterator VSDL_SnakeBody_IT;

class Snake{
private:
    const int W = 10;
    const int H = 10;
    VSDL_SnakeBody body;
    int speed = -10;
    // int speed = -5; // WARNING For Testing
    const short MAXSPEED = 5;
    int delay = 10;
    int smp = 7;
    int mp = 10;
public:
    int cTail = 0;
    bool isDead = false;
    short direction = RIGHT;
    Snake(int x,int y){
        this->body.push_back({x,y,W,H});
        // for (int i=0;i<20;i++) addTail(); // WARNING For Testing
    }
    SDL_Rect& head() { return body.front(); }
    VSDL_SnakeBody& gBody() { return body; }
    int width() { return this->W; }
    int height() { return this->H; }
    int gSpeed() { return speed; }
    void render(SDL_Renderer *renderer){
        if(body[0].x < 0) body[0].x = WIDTH - W;
        if(body[0].x > WIDTH ) body[0].x = 0;
        if(body[0].y < 0) body[0].y = HEIGHT - H;
        if(body[0].y > HEIGHT) body[0].y = 0;

        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderFillRect(renderer,&(body.front()));
        for(int i=1;i<body.size();i++){
            if(isDead) SDL_SetRenderDrawColor(renderer,255,0,0,255);
            else if (i % 2 == 0) SDL_SetRenderDrawColor(renderer,0,255,255,255);
            else SDL_SetRenderDrawColor(renderer,255,0,255,255);
            SDL_RenderFillRect(renderer,&(body[i]));
        }
    }
    void move(){
        if(isDead) return;
        int s = speed;
        if (speed <= 0){
            delay--;
            s = 0;
        }
        if (speed <= 0 && delay == 0) {
            delay = -speed;
            s = 1;
        }
        for(int i=0;i<s;i++){
            SDL_Rect newHead{body.front().x,body.front().y,W,H};

            if (direction == UP){
                newHead.y = body.front().y - H;
            }
            else if (direction == RIGHT){
                newHead.x = body.front().x + W;
            }
            else if (direction == DOWN){
                newHead.y = body.front().y + H;
            }
            else{
                newHead.x = body.front().x - W;
            }
            body.push_front(newHead);
            body.pop_back();
            selfCollision();
        }
    }
    void addTail(){
        cTail++;
        SDL_Rect newTail{body.back().x,body.back().y,W,H};
        if (direction == UP){
            newTail.y = body.back().y + H;
        }
        else if (direction == RIGHT){
            newTail.x = body.back().x - W;
        }
        else if (direction == DOWN){
            newTail.y = body.back().y - H;
        }
        else{
            newTail.x = body.back().x + W;
        }
        body.push_back(newTail);
    }
    void incSpeed(int score) {
        if (speed == MAXSPEED) { return; }
        if (smp != score) { return; }
        smp += mp;
        speed++;
        if (speed == 0) { speed = 1; }
        if (smp % 5 == 0) mp+=1;
    }
    void selfCollision(){
        for(VSDL_SnakeBody_IT i=body.begin()+1;i!=body.end();i++){
            bool xcol = i->x == body[0].x;
            bool ycol = i->y == body[0].y;
            if(xcol && ycol) { isDead = true; }
        }
    }
};

struct Food : SDL_Rect{
    bool isEaten = false;
    Food(){
        this->w = 5;
        this->h = 5;
        this->x = rand() % int(WIDTH - this->w);
        this->y = rand() % int(HEIGHT - this->h);
    }
};
enum CollisionCode {
    FINE,
    PLAYERDEAD
};
class GameManager{
private:
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event event;

    // need for text
    SDL_Surface* surface = NULL;
    SDL_Texture* texture = NULL;

    int score = 0;
public:
    Snake* snake = NULL;
    std::vector<Food> foods{};

    TTF_Font* font = NULL;
    TTF_Font* fontB = NULL;
    int Init(){
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            SDL_Quit();
            return 1;
        }
        window = SDL_CreateWindow("Snake",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,WIDTH,HEIGHT,0);
        SDL_Delay(100);
        renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
        if(TTF_Init() == -1){ std::cout << "Failed to start SDL_ttf\n"; }
        if (window == NULL){
            SDL_Quit();
            return 1;
        }
        /* for (int i = 0;i<5;i++){ foods.push_back(Food()); } */
        foods.push_back(Food());
        return 0;
    }
    void clean(){
        delete snake;
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        TTF_CloseFont(font);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
        TTF_Quit();
        SDL_Quit();
        std::cout << "Successfully Quited";
    }
    void renderFoods(){
        for(int i=0;i<foods.size();i++){
            if(foods[i].isEaten){
                foods[i].isEaten = false;
                foods[i].x = rand() % ((WIDTH - 30) - foods[i].w);
                foods[i].y = rand() % ((HEIGHT - 30) - foods[i].h);
            }
            SDL_SetRenderDrawColor(renderer,0,255,0,255);
            SDL_RenderFillRect(renderer,(SDL_Rect*) &foods[i]);
        }
    }
    void Loop(){
        const SDL_Color mWhite{255,255,255,255};
        bool isRunning = true;
        SDL_Rect txtRect;
        while(isRunning){
            // start_time = SDL_GetTicks();

            SDL_SetRenderDrawColor(renderer,0,0,0,255);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer,255,0,0,255);

            surface = TTF_RenderText_Solid(font,("Score: " + std::to_string(score)).c_str(),mWhite);
            texture = SDL_CreateTextureFromSurface(renderer,surface);
            SDL_QueryTexture(texture, NULL, NULL, &txtRect.w, &txtRect.h);
            txtRect.x = 10;
            txtRect.y = 0;
            SDL_RenderCopy(renderer, texture, NULL, &txtRect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);

            // surface = TTF_RenderText_Solid(font,("Speed: " + std::to_string(snake->gSpeed())).c_str(),mWhite);
            // texture = SDL_CreateTextureFromSurface(renderer,surface);
            // SDL_QueryTexture(texture, NULL, NULL, &txtRect.w, &txtRect.h);
            // txtRect.x = 0;
            // txtRect.y = HEIGHT - txtRect.h - 5;
            // SDL_RenderCopy(renderer, texture, NULL, &txtRect);
            // SDL_FreeSurface(surface);
            // SDL_DestroyTexture(texture);

            surface = TTF_RenderText_Solid(font,("Length: " + std::to_string(snake->gBody().size())).c_str(),mWhite);
            texture = SDL_CreateTextureFromSurface(renderer,surface);
            SDL_QueryTexture(texture, NULL, NULL, &txtRect.w, &txtRect.h);
            txtRect.x = WIDTH - txtRect.w - 10;
            txtRect.y = 0;
            SDL_RenderCopy(renderer, texture, NULL, &txtRect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);

            int col_code = handleCollision();
            if(col_code == PLAYERDEAD) {
                SDL_Delay(500);
                surface = TTF_RenderText_Solid(fontB,"Game Over",mWhite);
                texture = SDL_CreateTextureFromSurface(renderer,surface);
                SDL_QueryTexture(texture, NULL, NULL, &txtRect.w, &txtRect.h);
                txtRect.x = 5;
                txtRect.y = (HEIGHT / 2) / 2;
                int ow = txtRect.w;
                std::cout << txtRect.w << ' ' << txtRect.h << '\n';
                txtRect.w = WIDTH - 5;
                txtRect.h = (WIDTH * txtRect.h) / ow;
                std::cout << txtRect.w << ' ' << txtRect.h << '\n';
                SDL_RenderCopy(renderer, texture, NULL, &txtRect);
                SDL_RenderPresent(renderer);
                SDL_Delay(2000);
                break;
            }

            renderFoods();
            snake->move();
            snake->render(renderer);
            // Inputs
            while(SDL_PollEvent(&event)){
                if (event.type == SDL_QUIT) {
                    isRunning = false;
                    break;
                }
                if (event.type == SDL_KEYDOWN){
                    switch(event.key.keysym.sym){
                        case SDLK_w: {
                            if (snake->direction != DOWN) snake->direction = UP;
                            break;
                        }
                        case SDLK_s: {
                            if (snake->direction != UP) snake->direction = DOWN;
                            break;
                        }
                        case SDLK_d: {
                            if (snake->direction != LEFT) snake->direction = RIGHT;
                            break;
                        }
                        case SDLK_a: {
                            if (snake->direction != RIGHT)snake->direction = LEFT;
                            break;
                        }
                        default: break;
                    }
                }
            }
            SDL_RenderPresent(renderer);

            SDL_Delay(33);
        }
    }
    int handleCollision(){
        if(snake->isDead) { return PLAYERDEAD; }
        SDL_Rect sb = snake->head();
        int ll = (snake->gSpeed() <= 0)? 1:snake->gSpeed();
        if(ll > snake->gBody().size()) { ll = 1; }
        for(int i = 0;i<ll;i++){
            bool bb = false;
            for(int j=0;j<foods.size();j++){
                bool eatX = sb.x <= foods[j].x && sb.x + snake->width() >= foods[j].x;
                bool eatY = sb.y <= foods[j].y && sb.y + snake->height() >= foods[j].y;
                if (eatX && eatY){
                    foods[j].isEaten = true;
                    snake->addTail();
                    snake->incSpeed(score);
                    score++;
                    bb = true;
                    if(snake->gBody().size() % 10 == 0) { foods.push_back(Food()); }
                    break;
                }
            }
            if(bb) break;
            sb = snake->gBody()[i];
        }
        return FINE;
    }
};
#endif
