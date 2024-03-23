#pragma once

#include "RenderInfo.hpp"
#include <SDL.h>

inline void repeatTexture(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Point shift, SDL_Rect dest) {
    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);

    // Calculate the initial starting point considering the shift
    int startX = dest.x + (shift.x % w) - w; // Start before dest.x if shifted
    int startY = dest.y + (shift.y % h) - h; // Start before dest.y if shifted

    // Correct starting point if shift is negative
    if(shift.x < 0) {
        startX += w;
    }
    if(shift.y < 0) {
        startY += h;
    }    

    SDL_Rect srcRect; // Source rectangle from the texture
    SDL_Rect drawRect; // Destination rectangle on the renderer

    // Iterate over the destination rectangle in both dimensions
    for(int y = startY; y < dest.y + dest.h; y += h) {
        for(int x = startX; x < dest.x + dest.w; x += w) {
            // Set up the source rectangle (full texture size)
            srcRect.x = 0;
            srcRect.y = 0;
            srcRect.w = w;
            srcRect.h = h;

            // Set up the drawing rectangle
            drawRect.x = x;
            drawRect.y = y;
            drawRect.w = w;
            drawRect.h = h;

            // Adjust the drawing and source rectangles if they exceed the dest bounds
            if(x < dest.x) {
                srcRect.x += (dest.x - x);
                srcRect.w -= (dest.x - x);
                drawRect.x = dest.x;
                drawRect.w = srcRect.w;
            }
            if(y < dest.y) {
                srcRect.y += (dest.y - y);
                srcRect.h -= (dest.y - y);
                drawRect.y = dest.y;
                drawRect.h = srcRect.h;
            }
            if(drawRect.x + drawRect.w > dest.x + dest.w) {
                srcRect.w -= (drawRect.x + drawRect.w) - (dest.x + dest.w);
                drawRect.w = srcRect.w;
            }
            if(drawRect.y + drawRect.h > dest.y + dest.h) {
                srcRect.h -= (drawRect.y + drawRect.h) - (dest.y + dest.h);
                drawRect.h = srcRect.h;
            }

            // Render the texture segment
            SDL_RenderCopy(renderer, texture, &srcRect, &drawRect);
        }
    }
}

inline void repeatTexture(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Point shift, SDL_Rect dest, float scale) {
    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);

    // Apply scaling to the texture's dimensions
    int scaledW = (int)(w * scale);
    int scaledH = (int)(h * scale);

    // Calculate the initial starting point considering the shift, adjusted for scale
    int startX = dest.x + ((int)(shift.x * scale) % scaledW) - scaledW;
    int startY = dest.y + ((int)(shift.y * scale) % scaledH) - scaledH;

    // Correct starting point if shift is negative
    if(shift.x < 0) {
        startX += scaledW;
    }
    if(shift.y < 0) {
        startY += scaledH;
    }

    SDL_Rect srcRect; // Source rectangle from the texture
    SDL_Rect drawRect; // Destination rectangle on the renderer

    // Iterate over the destination rectangle, adjusted for the scale
    for(int y = startY; y < dest.y + dest.h; y += scaledH) {
        for(int x = startX; x < dest.x + dest.w; x += scaledW) {
            // Set up the source rectangle (full texture size, not affected by scale)
            srcRect.x = 0;
            srcRect.y = 0;
            srcRect.w = w; // Original texture width
            srcRect.h = h; // Original texture height

            // Set up the drawing rectangle, considering the scaling
            drawRect.x = x;
            drawRect.y = y;
            drawRect.w = scaledW;
            drawRect.h = scaledH;

            // Adjust the drawing rectangle if they exceed the dest bounds
            if(x < dest.x) {
                int adjustment = dest.x - x;
                srcRect.x += (int)(adjustment / scale);
                srcRect.w -= (int)(adjustment / scale);
                drawRect.x = dest.x;
                drawRect.w -= adjustment;
            }
            if(y < dest.y) {
                int adjustment = dest.y - y;
                srcRect.y += (int)(adjustment / scale);
                srcRect.h -= (int)(adjustment / scale);
                drawRect.y = dest.y;
                drawRect.h -= adjustment;
            }
            if(drawRect.x + drawRect.w > dest.x + dest.w) {
                int overflow = (drawRect.x + drawRect.w) - (dest.x + dest.w);
                srcRect.w -= (int)(overflow / scale);
                drawRect.w -= overflow;
            }
            if(drawRect.y + drawRect.h > dest.y + dest.h) {
                int overflow = (drawRect.y + drawRect.h) - (dest.y + dest.h);
                srcRect.h -= (int)(overflow / scale);
                drawRect.h -= overflow;
            }

            // Render the texture segment
            SDL_RenderCopy(renderer, texture, &srcRect, &drawRect);
        }
    }
}


inline std::vector<RenderInfo> repeatTexture(int textureID, SDL_Point start, SDL_Point size, Rect dest, int layerID) {
    std::vector<RenderInfo> results;
    // Calculate the initial starting point considering the shift, adjusted for scale
    int startX = start.x % size.x - size.x;
    int startY = start.y % size.y - size.x;
    // Correct starting point if shift is negative
    if(start.x < 0) {
        startX += size.x;
    }
    if(start.y < 0) {
        startY += size.y;
    }

    SDL_Rect srcRect; // Source rectangle from the texture
    Rect drawRect; // Destination rectangle on the renderer

    // Iterate over the destination rectangle, adjusted for the scale
    for(int y = startY; y < dest.y + dest.w; y += size.y) {
        for(int x = startX; x < dest.x + dest.z; x += size.x) {
            // Set up the source rectangle (full texture size, not affected by scale)
            srcRect.x = 0;
            srcRect.y = 0;
            srcRect.w = size.x; // Original texture width
            srcRect.h = size.y; // Original texture height

            // Set up the drawing rectangle, considering the scaling
            drawRect.x = x;
            drawRect.y = y;
            drawRect.z = size.x;
            drawRect.w = size.y;

            int nt = 4;
            int xid = (x-start.x)/size.x % nt;
            int yid = (y-start.y)/size.y % nt;

            if (xid < 0)
            {
                xid += nt;
            }
            if (yid < 0)
            {
                yid += nt;
            }
            
            xid /=2;
            yid /=2;

            //int xid = std::abs(((x-start.x)/(size.x)))/2 % 2;
            //int yid = std::abs(((y-start.y)/(size.y)))/2 % 2;
            /*
            if (x+start.x < 0)
            {
                xid++;
                xid %= 2;
            }

            if (y+start.y < 0)
            {
                yid++;
                yid %= 2;
            }*/

            // Adjust the drawing rectangle if they exceed the dest bounds
            if(x < dest.x) {
                int adjustment = dest.x - x;
                srcRect.x += adjustment;
                srcRect.w -= adjustment;
                drawRect.x = dest.x;
                drawRect.z -= adjustment;
            }
            if(y < dest.y) {
                int adjustment = dest.y - y;
                srcRect.y += adjustment;
                srcRect.h -= adjustment;
                drawRect.y = dest.y;
                drawRect.w -= adjustment;
            }
            if(drawRect.x + drawRect.z > dest.x + dest.z) {
                int overflow = (drawRect.x + drawRect.z) - (dest.x + dest.z);
                srcRect.w -= overflow;
                drawRect.z -= overflow;
            }
            if(drawRect.y + drawRect.w > dest.y + dest.w) {
                int overflow = (drawRect.y + drawRect.w) - (dest.y + dest.w);
                srcRect.h -= overflow;
                drawRect.w -= overflow;
            }
            textureID = xid ^ yid;

            RenderInfo info;
            info.textureID = textureID;
            info.clip = srcRect;
            info.dest = drawRect;
            info.layerID = layerID;
            results.push_back(info);
        }
    }
    return results;
}
