#include <SDL.h>
#include <stdlib.h> // For rand() and srand()
#include <time.h>   // For time()

SDL_Texture* createRandomClippedTexture(SDL_Renderer* renderer, SDL_Texture* originalTexture,
                                        SDL_Point clipSize, SDL_Point resultTextureSize) {
    // Seed the random number generator
    srand(time(NULL));

    // Create a target texture that we can render to
    SDL_Texture* resultTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                                   SDL_TEXTUREACCESS_TARGET, resultTextureSize.x, resultTextureSize.y);
    if (!resultTexture) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        return NULL;
    }

    // Set the target texture as the rendering target
    SDL_SetRenderTarget(renderer, resultTexture);

    // Clear the target texture
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // Get the dimensions of the original texture
    int originalWidth, originalHeight;
    SDL_QueryTexture(originalTexture, NULL, NULL, &originalWidth, &originalHeight);

    // Calculate how many clips we need to cover the resultTextureSize
    int cols = (resultTextureSize.x + clipSize.x - 1) / clipSize.x;
    int rows = (resultTextureSize.y + clipSize.y - 1) / clipSize.y;
    SDL_Point center = {clipSize.x/2, clipSize.y/2};
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            // Choose a random position in the original texture for the clip
            int clipX = rand() % (originalWidth - clipSize.x);
            int clipY = rand() % (originalHeight - clipSize.y);
            int angleDex = rand() % 4;
            int flipDex = rand() % 4;
            SDL_RendererFlip flip = SDL_FLIP_NONE;
            switch (flipDex)
            {
                case 1:
                    flip = SDL_FLIP_VERTICAL;
                    break;
                case 2:
                    flip = SDL_FLIP_HORIZONTAL;
                    break;
                case 3:
                    flip = SDL_RendererFlip(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
                    break;
            }

            SDL_Rect srcRect = {clipX, clipY, clipSize.x, clipSize.y};
            SDL_Rect dstRect = {x * clipSize.x, y * clipSize.y, clipSize.x, clipSize.y};
            float angle = 90*angleDex;
        
            // Render the clip onto the resultTexture
            SDL_RenderCopyEx(renderer, originalTexture, &srcRect, &dstRect, angle, &center, flip);
        }
    }

    // Reset the rendering target to the default (the window)
    SDL_SetRenderTarget(renderer, NULL);

    return resultTexture;
}