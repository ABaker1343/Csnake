#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>
#include <stdlib.h>
#include <memory.h>

#define GRID_MAX 50

void mainloop();

const int FRAMERATE = 15;

int grid[GRID_MAX][GRID_MAX];
enum gridSpace {empty, food, snake};
int snakeSize = 1;
enum dir {up, down, left, right};
int snakeDir = up;
int wWidth = 800, wHeight= 800;
int gamerunning = 1;

SDL_Window *window;
SDL_Renderer *renderer;

struct snakePart {
    struct snakePart *next;
    struct snakePart *prev;
    int posx;
    int posy;
    int dir;
};

struct food{
    int posx;
    int posy;
};

struct snakePart head;
struct food gameFood;


int main(int argc, char* argv[]){

    SDL_Init(SDL_INIT_EVERYTHING);

    window = SDL_CreateWindow(
        "Csnake",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        wWidth, wHeight,
        SDL_RENDERER_ACCELERATED | SDL_WINDOW_RESIZABLE
    );

    renderer = SDL_CreateRenderer(
        window,
        0,
        0
    );

    srand(time(NULL));

    //set grids to 0

    mainloop();

    return 0;
}

void setup(){
    head.posx = GRID_MAX / 2;
    head.posy = GRID_MAX / 2;
    head.dir = up;
    head.next = NULL;
    head.prev = NULL;

    gameFood.posx = rand() % GRID_MAX;
    gameFood.posy = rand() % GRID_MAX;
}

void update(){
    SDL_Event evnt;
    while (SDL_PollEvent(&evnt)){
        switch (evnt.type){
            case SDL_KEYDOWN: {
                switch(evnt.key.keysym.sym){
                    case SDLK_w:
                        head.dir = up;
                        break;
                    case SDLK_a:
                        head.dir = left;
                        break;
                    case SDLK_d:
                        head.dir = right;
                        break;
                    case SDLK_s:
                        head.dir = down;
                        break;
                } 


                break;
            }

            case SDL_QUIT:
                gamerunning = 0;
                break;
        }
    }

    printf("pos = %d, %d\n", head.posx, head.posy);

    //handle moving the squares
    //get the end of the list and cascade up moving each to a new pos and then move the head
    struct snakePart *currentPart = &head;
    struct snakePart *endPart;

    while(currentPart->next != NULL){
        currentPart = currentPart->next;
    }

    endPart = currentPart;

    while(currentPart->prev != NULL){
        currentPart->posx = currentPart->prev->posx;
        currentPart->posy = currentPart->prev->posy;
        currentPart->dir = currentPart->prev->dir;
        currentPart = currentPart->prev;
    }

    //move the snake
    switch(head.dir){
        case down:
            head.posy += 1;
            break;
        case up:
            head.posy -= 1;
            break;
        case left:
            head.posx -= 1;
            break;
        case right:
            head.posx += 1;
            break;
    }


    //check collisions with itself
    if (head.next != NULL) {
        currentPart = head.next;
        while(currentPart->next != NULL){
            if (head.posx == currentPart->posx && head.posy == currentPart->posy){
                gamerunning = 0;
            }
            currentPart = currentPart->next;
        }
    }

    if (head.posx == gameFood.posx && head.posy == gameFood.posy){
        struct snakePart* newPart = (struct snakePart *) malloc(sizeof(struct snakePart));
        newPart->posx = endPart->posx;
        newPart->posy = endPart->posy;
        newPart->dir = endPart->dir;
        newPart->next = NULL;
        newPart->prev = endPart;
        endPart->next = newPart;

        gameFood.posx = rand() % 50;
        gameFood.posy = rand() % 50;
    }

    if (head.posx > GRID_MAX || head.posx < 0){
        gamerunning = 0;
    }
    if (head.posy > GRID_MAX || head.posy < 0){
        gamerunning = 0;
    }

}

void render(){

    SDL_GetRendererOutputSize(renderer, &wWidth, &wHeight);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    //render food
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    
    SDL_Rect foodRect;
    foodRect.h = wHeight / GRID_MAX;
    foodRect.w = wWidth / GRID_MAX;
    foodRect.x = gameFood.posx * (wWidth/GRID_MAX);
    foodRect.y = gameFood.posy * (wHeight/GRID_MAX);

    const SDL_Rect renderRect = foodRect;

    SDL_RenderDrawRect(renderer, &renderRect);

    //render snake

    struct snakePart *currentPart = &head;

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

    while(1){
        SDL_Rect rect;
        rect.x = currentPart->posx * (wWidth/GRID_MAX);
        rect.y = currentPart->posy * (wHeight/GRID_MAX);
        rect.w = wWidth / GRID_MAX;
        rect.h = wHeight / GRID_MAX;
        const SDL_Rect drawRect = rect;
        SDL_RenderDrawRect(renderer, &drawRect);
        if (currentPart->next == NULL) break;
        currentPart = currentPart->next;
    }

    SDL_RenderPresent(renderer);

}

void mainloop(){

    int frametime = 1000/FRAMERATE;

    TTF_Init();

    TTF_Font *font = TTF_OpenFont("sans.ttf", 20);
    SDL_Color red = {255,0,0};
    SDL_Surface *textsur = TTF_RenderText_Solid(font, "press any key to start", red);
    SDL_Texture *messageStart = SDL_CreateTextureFromSurface(renderer, textsur);
    SDL_FreeSurface(textsur);
    SDL_Surface *textsur2 = TTF_RenderText_Solid(font, "game over", red);
    SDL_Texture *messageEnd = SDL_CreateTextureFromSurface(renderer, textsur2);
    SDL_FreeSurface(textsur2);
    const SDL_Rect rect = {0, 0, wWidth, wHeight};

    SDL_Event evnt;
    while(1){

        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, messageStart, NULL, &rect);
        SDL_RenderPresent(renderer);

        while(SDL_PollEvent(&evnt)){
            switch(evnt.type){

                case SDL_KEYDOWN: {
                    gamerunning = 1;
                    setup();
                    while (gamerunning){
                        uint32_t startTime = SDL_GetTicks();
                        update();
                        render();
                        uint32_t processTime = SDL_GetTicks() - startTime;
                        if (processTime < frametime){
                            SDL_Delay(frametime - processTime);
                        }
                    }


                    SDL_RenderCopy(renderer, messageEnd, NULL, &rect);
                    SDL_RenderPresent(renderer);

                    //free resources
                    struct snakePart *currentPart = &head;

                    while(currentPart->next != NULL){
                        currentPart = currentPart->next;
                    }

                    while(currentPart->prev != NULL){
                        struct snakePart *toFree = currentPart;
                        currentPart = currentPart->prev;
                        free(toFree);
                    }

                    break;
                }

                case SDL_QUIT:
                    return;
            }

        }
    }
}

