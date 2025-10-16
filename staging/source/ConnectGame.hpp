#pragma once
#include <BaseModule.hpp>

class ConnectGame : public BaseModule
{
public:
    void tick(float dt, std::span<const EventData> ev) override;
    void extract( RenderSnapshots2D& s) override;
    void onInit() override;
    
private:
    RenderSnapshot2D rs;
    ConnectModel model;
    ConnectGui gui;
};
