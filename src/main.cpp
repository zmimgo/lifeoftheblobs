#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <vector>
#include <cmath>
#include <unistd.h>
#include <time.h>
#include <iostream>

// General variables
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
int blobMaxAge = 1200;
bool showBlobAmountMessage = true;
int blobAmountMessageDelay = 100;
bool includeYPC = true;
bool showMajoritytMessage = true;
int blobMajorityMessageDelay = 100;
int universalBlobSpeed = 5;
bool extraRandomness = false;

// Blob data structures
struct blob {
    int x;
    int y;
    int radius;
    int r;
    int g;
    int b;
    int a;
    int direction;
    int speed = universalBlobSpeed;
    int invincible = 0;
    int age = 0;
};

std::vector<blob> blobs;

// Blob manipulation functions
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
                    if(extraRandomness){
                        addBlob(blobs[i].x, blobs[i].y, blobs[i].radius, newR, newG, newB, newA, blobs[i].direction + 90 + rand() % 70 - 35);
                    } else {
                        addBlob(blobs[i].x, blobs[i].y, blobs[i].radius, newR, newG, newB, newA, blobs[i].direction + 90);
                    }
                    
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

void addStarterBlobs(){
    addBlob(rand() % window.width, rand() % window.height, 10, 255, 0, 0, 127, rand() % 360 + 15);
    addBlob(rand() % window.width, rand() % window.height, 10, 0, 255, 0, 127, rand() % 360 + 15);
    addBlob(rand() % window.width, rand() % window.height, 10, 0, 0, 255, 127, rand() % 360 + 15);
    if(includeYPC){
        addBlob(rand() % window.width, rand() % window.height, 10, 255, 255, 0, 127, rand() % 360 + 15);
        addBlob(rand() % window.width, rand() % window.height, 10, 255, 0, 255, 127, rand() % 360 + 15);
        addBlob(rand() % window.width, rand() % window.height, 10, 0, 255, 255, 127, rand() % 360 + 15);
    }
}

// Main function
int main(int argc, char *argv[]) {
    // Generate a random seed for the random number generator
    srand(time(NULL));

    // Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }

    // Create an SDL window
    SDL_Window* win = SDL_CreateWindow("Life of the Blobs", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window.width, window.height, 0);
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);

    // Set up the frame rate
    int close = 0;

    // Set up the blobs
    addStarterBlobs();

    // Main loop
    while (!close) {
        // Advance frame counter
        frame++;

        // Check for events
        SDL_Event event;
        if(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                close = 1;
            }
        }

        // Set the draw color to black
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);

        // Run one blob cycle
        moveBlobs();
        bounceBlobs();
        drawBlobs(rend);
        splitBlobs();
        if(allowBlobAging) {
            killOldBlobs();
        }

        // If there are too many or too few blobs, reset the blobs
        if(blobs.size() > maxBlobs || blobs.size() < 2) {
            SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
            SDL_RenderClear(rend);
            blobs.clear();
            addStarterBlobs();
        }

        // Print blob amount and majority color, if chosen
        if(frame % blobAmountMessageDelay == 0 && showBlobAmountMessage) {
        printf("Blobs: %d\n", blobs.size());
        }

        if(frame % blobMajorityMessageDelay == 0 && showMajoritytMessage) {
            // Find the amount of each color
            int red = 0;
            int green = 0;
            int blue = 0;
            int yellow = 0;
            int purple = 0;
            int cyan = 0;
            for (int i = 0; i < blobs.size(); i++) {
                if(blobs[i].r > blobs[i].g && blobs[i].r > blobs[i].b) {
                    red++;
                } else if(blobs[i].g > blobs[i].r && blobs[i].g > blobs[i].b) {
                    green++;
                } else if(blobs[i].b > blobs[i].r && blobs[i].b > blobs[i].g) {
                    blue++;
                }
                if(includeYPC){
                    if(blobs[i].r > blobs[i].g && blobs[i].b > blobs[i].g) {
                        yellow++;
                    } else if(blobs[i].r > blobs[i].b && blobs[i].g > blobs[i].b) {
                        purple++;
                    } else if(blobs[i].g > blobs[i].r && blobs[i].b > blobs[i].r) {
                        cyan++;
                    }
                }
            }

            // Find the majority color
            int max = red;
            std::string color = "Red";
            if(green > max) {
                max = green;
                color = "Green";
            }
            if(blue > max) {
                max = blue;
                color = "Blue";
            }
            if(includeYPC){
                if(yellow > max) {
                    max = yellow;
                    color = "Yellow";
                }
                if(purple > max) {
                    max = purple;
                    color = "Purple";
                }
                if(cyan > max) {
                    max = cyan;
                    color = "Cyan";
                }

                // Print the majority color
                printf("Majority [%s] (RGBYPC): %d, %d, %d, %d, %d, %d\n", color.c_str(), red, green, blue, yellow, purple, cyan);
            } else {
                printf("Majority [%s] (RGB): %d, %d, %d\n", color.c_str(), red, green, blue);
            }
        }

        // Render the frame
        SDL_RenderPresent(rend);

        // Delay to maintain frame rate
        SDL_Delay(1000 / 60);
    }

    // Destroy renderer
    SDL_DestroyRenderer(rend);

    // Destroy window
    SDL_DestroyWindow(win);

    // Close SDL
    SDL_Quit();

    return 0;
}
