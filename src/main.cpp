#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <vector>
#include <cmath>
#include <unistd.h>
#include <time.h>

// Global variables
struct blob {
    int x;
    int y;
    int radius;
    int r;
    int g;
    int b;
    int a;
    int direction;
    int speed = 5;
    int invincible = 0;
    int age = 0;
};

std::vector<blob> blobs;

struct windowTemplate {
    int width;
    int height;
};

unsigned int frame = 0;

// Settings

windowTemplate window = {800, 800};
int maxBlobs = 500;
int blobInvincibility = 100;
bool allowBlobAging = true;
int blobMaxAge = 10000;
bool showBlobAmountMessage = true;
int blobAmountMessageDelay = 100;

// Functions
void drawBlobs(SDL_Renderer* rend) {
    for (int i = 0; i < blobs.size(); i++) {
        filledCircleRGBA(rend, blobs[i].x, blobs[i].y, blobs[i].radius, blobs[i].r, blobs[i].g, blobs[i].b, blobs[i].a);
    }
}

void addBlob(int ix, int iy, int iradius, int ir, int ig, int ib, int ia, int idirection) {
    blob b;
    b.x = ix;
    b.y = iy;
    b.radius = iradius;
    b.r = ir;
    b.g = ig;
    b.b = ib;
    b.a = ia;
    b.direction = idirection;
    b.invincible = blobInvincibility;
    blobs.push_back(b);
}

void moveBlobs() {
    for (int i = 0; i < blobs.size(); i++) {
        if(blobs[i].invincible > 0) {
            blobs[i].invincible--;
        }
        blobs[i].age++;
        blobs[i].x += blobs[i].speed * cos(blobs[i].direction * M_PI / 180);
        blobs[i].y += blobs[i].speed * sin(blobs[i].direction * M_PI / 180);
    }
}

void bounceBlobs(){
    // If out of bounds, bounce blobs
    for (int i = 0; i < blobs.size(); i++) {
        if (blobs[i].x > window.width || blobs[i].x < 0) {
            blobs[i].direction = 180 - blobs[i].direction;
        }
        if (blobs[i].y > window.height || blobs[i].y < 0) {
            blobs[i].direction = 360 - blobs[i].direction;
        }
    }
}

void splitBlobs(){
    // If blobs collide, split them
    for (int i = 0; i < blobs.size(); i++) {
        for (int j = 0; j < blobs.size(); j++) {
            if (i != j) {
                int distance = sqrt(pow(blobs[i].x - blobs[j].x, 2) + pow(blobs[i].y - blobs[j].y, 2));
                if (distance < blobs[i].radius + blobs[j].radius) {
                    if(blobs[i].invincible > 0 || blobs[j].invincible > 0) {
                        goto skip;
                    }
                    // Split blobs
                    blobs[i].direction + 90;
                    blobs[j].direction + 90;
                    blobs[i].invincible += blobInvincibility;
                    blobs[j].invincible += blobInvincibility;
                    
                    // Make new blob with combined RGB values
                    int newR = (blobs[i].r + blobs[j].r) / 2;
                    int newG = (blobs[i].g + blobs[j].g) / 2;
                    int newB = (blobs[i].b + blobs[j].b) / 2;
                    int newA = (blobs[i].a + blobs[j].a) / 2;
                    addBlob(blobs[i].x, blobs[i].y, blobs[i].radius, newR, newG, newB, newA, blobs[i].direction + 90 + rand() % 70 - 35);
                }
            }
        }
    }
    skip:
    return;
}

void killOldBlobs(){
    // If blobs are too old, kill them
    for (int i = 0; i < blobs.size(); i++) {
        if (blobs[i].age > blobMaxAge) {
            blobs.erase(blobs.begin() + i);
        }
    }
}

// Main function
int main(int argc, char *argv[]) {
    srand(time(NULL));
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }

    SDL_Window* win = SDL_CreateWindow("Life of the Blobs", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window.width, window.height, 0);
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);

    // controls animation loop
    int close = 0;

    // Generate first gen blobs
    addBlob(rand() % window.width, rand() % window.height, 10, 255, 0, 0, 127, rand() % 360 + 15);
    addBlob(rand() % window.width, rand() % window.height, 10, 0, 255, 0, 127, rand() % 360 + 15);
    addBlob(rand() % window.width, rand() % window.height, 10, 0, 0, 255, 127, rand() % 360 + 15);

    // animation loop
    while (!close) {
        frame++;

        SDL_Event event;
        if(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                close = 1;
            }
        }

        // Set the draw color to black
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);

        // Let the blobs do their thing
        moveBlobs();
        bounceBlobs();
        drawBlobs(rend);
        splitBlobs();
        if(allowBlobAging) {
            killOldBlobs();
        }

        // If too many or too few, restart
        if(blobs.size() > maxBlobs || blobs.size() < 2) {
            SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
            SDL_RenderClear(rend);
            blobs.clear();
            addBlob(rand() % window.width, rand() % window.height, 10, 255, 0, 0, 127, rand() % 360);
            addBlob(rand() % window.width, rand() % window.height, 10, 0, 255, 0, 127, rand() % 360);
            addBlob(rand() % window.width, rand() % window.height, 10, 0, 0, 255, 127, rand() % 360);
        }

        if(frame % blobAmountMessageDelay == 0 && showBlobAmountMessage) {
        printf("Blobs: %d\n", blobs.size());
        }

        // triggers the double buffers
        // for multiple rendering
        SDL_RenderPresent(rend);

        // calculates to 60 fps
        SDL_Delay(1000 / 60);
    }

    // destroy renderer
    SDL_DestroyRenderer(rend);

    // destroy window
    SDL_DestroyWindow(win);

    // close SDL
    SDL_Quit();

    return 0;
}
