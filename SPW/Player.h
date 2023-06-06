#pragma once

#include "Settings.h"
#include "GameBody.h"
#include "Constants.h"

#define PLAYER_DAMAGE_ANGLE 55.0f

class Player : public GameBody
{
public:
    Player(Scene &scene);
    virtual ~Player();

    virtual void Start() override;
    virtual void Update() override;
    virtual void Render() override;
    virtual void FixedUpdate() override;
    virtual void OnRespawn() override;
    virtual void DrawGizmos() override;

    virtual void OnCollisionEnter(GameCollision &collision) override;
    virtual void OnCollisionStay(GameCollision &collision) override;
    virtual void OnCollisionExit(GameCollision &collision) override;

    int GetFireflyCount() const;
    int GetHeartCount() const;
    int GetLifeCount() const;
    void AddFirefly(int count);
    void AddHeart();
    void Damage();
    void Kill();
    void Bounce();

private:
    void WakeUpSurroundings();
    PE_Vec2 UpdateOnGround(PE_Vec2 position);

    enum class State
    {
        IDLE, WALKING, RUNNING, SKIDDING, FALLING, DYING, DEAD, CLIMBBING, DIVE_LOADING, DIVING
    };
    State m_state;

    RE_Animator m_animator;

    float m_onAirTimer;
    float m_onWallTimer;
    float m_hDirection;
    bool m_jump;
    bool m_onGround;
    bool m_bounce;
    bool m_facingRight;
    bool m_climb;

    int m_lifeCount;
    int m_heartCount;
    int m_fireflyCount;

    int m_player_dying_counter;

    // Diving attack
    bool m_dive;
    int m_dive_load_counter;
};


inline void Player::Bounce()
{
    m_bounce = true;
}

inline int Player::GetFireflyCount() const
{
    return m_fireflyCount;
}

inline int Player::GetHeartCount() const
{
    return m_heartCount;
}

inline int Player::GetLifeCount() const
{
    return m_lifeCount;
}
