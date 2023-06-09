#pragma once

#include "Settings.h"
#include "InputGroup.h"

class ControlsInput : public InputGroup
{
public:
    ControlsInput();

    virtual void OnPreEventProcess() override;
    virtual void OnEventProcess(SDL_Event evt) override;
    virtual void Reset() override;

    float hAxis;
    bool jumpDown;
    bool jumpPressed;
    bool goDownDown;  // used for the dive
    bool goUpDown;
    bool goRightDown;
    bool goLeftDown;

    bool savePressed;
    bool areaDown;
    bool areaReleased;

    bool fillDown;

    bool crouchDown;
    bool crouchReleased;

    bool copyPressed;
    bool pastePressed;

    bool rotatePressed;
    bool flipVPressed;
    bool flipHPressed;
};
