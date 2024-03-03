#include "RenderInfo.hpp"
#include "Node.hpp"
#include "TextureInfo.hpp"

RenderInfo createInfo(Node* node, TextureInfo* tinfo)
{
    RenderInfo info;

    info.dest.x = static_cast<int>(node->absolute.position.x) + tinfo->dest.x;
    info.dest.y = static_cast<int>(node->absolute.position.y) + tinfo->dest.y;
    info.dest.w = tinfo->dest.w; 
    info.dest.h = tinfo->dest.h;
    info.textureID = tinfo->textureID;
    info.rotation = fmod(node->absolute.rotation + tinfo->angle, 360.0f); //  TO DO tinfo->dest.x and y should probably be modifed based on the node rotation
    info.flip = static_cast<SDL_RendererFlip>(node->absolute.flip ^ tinfo->flip); // Combine flips

    // Set clip from TextureInfo
    info.clip = tinfo->clip;
    info.layerID = tinfo->layerID;
    // Set other properties as needed, e.g., layerID, visibility

    return info;    
}
