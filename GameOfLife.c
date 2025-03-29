#include <SDL3/SDL.h>

#define w 1700
#define h 900

#define black 0x000000
#define white 0xffffff
#define gray 0x111111

#define l 20
#define lw 2
#define columns w/l
#define rows h/l

void drawCell(SDL_Surface* surface, int cx, int cy, Uint32 color) {
    SDL_Rect cell = (SDL_Rect) {cx * l, cy * l, l, l};
    SDL_FillSurfaceRect(surface, &cell, color);
}

void drawMatrix(SDL_Surface* surface, int r, int c, int arr[r][c]) {
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            if (arr[i][j] == 1) {
                drawCell(surface, j, i, white);
            } else {
                drawCell(surface, j, i, black);
            }
        }
    }
}

void drawGrid(SDL_Surface* surface, Uint32 color) {
    for (int i = l; i < w; i += l) {
        SDL_Rect column = (SDL_Rect) {i, 0, lw, h};
        SDL_FillSurfaceRect(surface, &column, color);
    }
    for (int i = l; i < h; i += l) {
        SDL_Rect row = (SDL_Rect) {0, i, w, lw};
        SDL_FillSurfaceRect(surface, &row, color);
    }
}

void clear(SDL_Surface* surface, int r, int c, int arr[r][c]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            arr[i][j] = 0;
        }
    }
    drawMatrix(surface, r, c, arr);
}

void updateMatrix(SDL_Surface* surface, float mx, float my, int arr[rows][columns], int alive) {
    int mr = my / l;
    int mc = mx / l;

    arr[mr][mc] = alive;
    drawMatrix(surface, rows, columns, arr);
}

int neighbors(int i, int j, int r, int c, int arr[r][c]) {
    int count = 0;

    for (int di = -1; di <= 1; di++) {
        for (int dj = -1; dj <= 1; dj++) {
            if (di == 0 && dj == 0) continue;

            int ni = i + di;
            int nj = j + dj;

            // Ensure the neighbor is within bounds
            if (ni >= 0 && ni < r && nj >= 0 && nj < c) {
                count += arr[ni][nj];
            }
        }
    }

    return count;
}

void simStep(int r, int c, int arr[r][c]) {
    int newArr[r][c];
    
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            int neighborCount = neighbors(i, j, r, c, arr);
            
            if (arr[i][j] == 1 && neighborCount < 2) {
                newArr[i][j] = 0;
            } else if (arr[i][j] == 1 && (neighborCount == 2 || neighborCount == 3)) {
                newArr[i][j] = 1;
            } else if (arr[i][j] == 1 && neighborCount > 3) {
                newArr[i][j] = 0;
            } else if (arr[i][j] == 0 && neighborCount == 3) {
                newArr[i][j] = 1;
            } else {
                newArr[i][j] = arr[i][j];
            }
        }
    }

    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            arr[i][j] = newArr[i][j];
        }
    }
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Game Of Life", w, h, 0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);

    int arr[rows][columns];
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            arr[i][j] = 0; 
        }
    }
    drawGrid(surface, gray);

    SDL_Event event;
    float x, y;
    int mouseDownLeft = 0;
    int mouseDownRight = 0;
    int pause = 1;
    int running = 1;
    int speed = 100;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE) {
                    running = 0;
                }
                
                if (event.key.key == SDLK_SPACE) {
                    pause = !pause;
                } else if (event.key.key == SDLK_BACKSPACE) {
                    clear(surface, rows, columns, arr);
                    drawGrid(surface, gray);
                } else if (event.key.key == SDLK_UP && speed > 5) {
                    speed -= 5;
                } else if (event.key.key == SDLK_DOWN && speed < 1000) {
                    speed += 5;
                } 
            } 
            
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    mouseDownLeft = 1;
                    if (pause != 1) {
                        pause = 1;
                    }
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    mouseDownRight = 1;
                }
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    mouseDownLeft = 0;
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    mouseDownRight = 0;
                }
            } else if (event.type == SDL_EVENT_MOUSE_MOTION) { 
                SDL_GetMouseState(&x, &y);
                
                if (mouseDownLeft) {
                    updateMatrix(surface, x, y, arr, 1);
                    drawGrid(surface, gray);
                } else if (mouseDownRight) {
                    updateMatrix(surface, x, y, arr, 0);
                    drawGrid(surface, gray);
                } 
            }
            SDL_UpdateWindowSurface(window);      
        }
        if (pause == 0) {
            drawMatrix(surface, rows, columns, arr);
            drawGrid(surface, gray);
            simStep(rows, columns, arr);
            SDL_UpdateWindowSurface(window);
            SDL_Delay(speed);
        }
    }
}
