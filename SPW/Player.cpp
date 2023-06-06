#include "Player.h"
#include "Camera.h"
#include "Scene.h"
#include "Collectable.h"
#include "Enemy.h"
#include "Graphics.h"

Player::Player(Scene &scene) :
        GameBody(scene, Layer::PLAYER), m_animator(),
        m_jump(false), m_facingRight(true), m_bounce(false), m_onAirTimer(0.0f), m_onWallTimer(-1.0f), m_hDirection(0.0f),
        m_lifeCount(5), m_fireflyCount(0), m_heartCount(2), m_state(Player::State::IDLE)
{
    m_name = "Player";

    SetToRespawn(true);

    AssetManager &assetsManager = scene.GetAssetManager();
    RE_Atlas *atlas = assetsManager.GetAtlas(AtlasID::PLAYER);
    RE_AtlasPart *part = nullptr;

    // Animation "Idle"
    part = atlas->GetPart("Idle");
    AssertNew(part);
    RE_TexAnim *idleAnim = new RE_TexAnim(
        m_animator, "Idle", part
    );
    idleAnim->SetCycleCount(0);

    // Animation "Falling"
    part = atlas->GetPart("Falling");
    AssertNew(part);
    RE_TexAnim *fallingAnim = new RE_TexAnim(
            m_animator, "Falling", part
    );
    fallingAnim->SetCycleCount(-1);
    fallingAnim->SetCycleTime(0.2f);

    // Animation "Running"
    part = atlas->GetPart("Running");
    AssertNew(part);
    RE_TexAnim *running = new RE_TexAnim(
            m_animator, "Running", part
    );
    running->SetCycleCount(-1);
    running->SetCycleTime(0.15f);

    // Animation "Dying"
    part = atlas->GetPart("Dying");
    AssertNew(part);
    RE_TexAnim *dying = new RE_TexAnim(
            m_animator, "Dying", part
    );
    running->SetCycleCount(0);


    // Couleur des colliders en debug
    m_debugColor.r = 255;
    m_debugColor.g = 0;
    m_debugColor.b = 0;
}

Player::~Player()
{
}

void Player::Start()
{
    // Joue l'animation par d�faut
    m_animator.PlayAnimation("Idle");

    // Cr�e le corps
    PE_World &world = m_scene.GetWorld();
    PE_BodyDef bodyDef;
    bodyDef.type = PE_BodyType::DYNAMIC;
    bodyDef.position = GetStartPosition() + PE_Vec2(0.5f, 0.0f);
    bodyDef.name = (char *)"Player";
    bodyDef.damping.SetZero();
    PE_Body *body = world.CreateBody(bodyDef);
    SetBody(body);

    // Cr�ation du collider
    PE_ColliderDef colliderDef;

    PE_CapsuleShape capsule(PE_Vec2(0.0f, 0.35f), PE_Vec2(0.0f, 0.85f), 0.35f);
    colliderDef.friction = 1.0f;
    colliderDef.filter.categoryBits = CATEGORY_PLAYER;
    colliderDef.shape = &capsule;
    PE_Collider *collider = body->CreateCollider(colliderDef);
}

void Player::Update()
{
    ControlsInput &controls = m_scene.GetInputManager().GetControls();

    // Sauvegarde les contr�les du joueur pour modifier
    // sa physique au prochain FixedUpdate()

    m_hDirection = controls.hAxis;
    if (controls.jumpPressed && (m_state != State::FALLING)) m_jump = true;

    // Diving
    if (controls.goDownDown && (m_state == State::FALLING)) m_dive = true;
}

void Player::Render()
{
    SDL_Renderer *renderer = m_scene.GetRenderer();
    Camera *camera = m_scene.GetActiveCamera();

    // Met � jour les animations du joueur
    m_animator.Update(m_scene.GetTime());

    PE_Vec2 velocity = GetVelocity();
    SDL_RendererFlip flip = m_facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

    float scale = camera->GetWorldToViewScale();
    SDL_FRect rect = { 0 };

    rect.h = 1.375f * scale;
    rect.w = 1.0f * scale;
    camera->WorldToView(GetPosition(), rect.x, rect.y);

    // Dessine l'animateur du joueur
    m_animator.RenderCopyExF(
            &rect, RE_Anchor::SOUTH , 0.0f, Vec2(0.5f, 0.5f), flip
    );
}

void Player::FixedUpdate()
{
    PE_Body *body = GetBody();
    PE_Vec2 position = body->GetPosition();
    PE_Vec2 velocity = body->GetLocalVelocity();
    ControlsInput &controls = m_scene.GetInputManager().GetControls();

    bool noJump {false};

    // R�veille les corps autour du joueur
    WakeUpSurroundings();
    // Update
    UpdateOnGround(position);

    // Tue le joueur s'il tombe dans un trou
    if (position.y < -2.0f){
        m_scene.Respawn();
        return;
    }
    

    if (m_onWallTimer >= 0)
    {
        m_onWallTimer += m_scene.GetFixedTimeStep();
        if (m_onWallTimer > 0.5)
        {
            m_state = State::FALLING;
            m_onWallTimer = -1.0f;
        }
    }

    // FORCE ET DIRECTION
    PE_Vec2 direction = PE_Vec2::right;
    
    PE_Vec2 force = (15.0f * m_hDirection) * direction;
    body->ApplyForce(force);

    float maxHSpeed = 9.0f;
    velocity.x = PE_Clamp(velocity.x, -maxHSpeed, maxHSpeed);

    /* --- STATE --- */
    switch (m_state){
        case State::DYING:{
            if(m_player_dying_counter == 0){
                Kill();
            }
            else{
                m_player_dying_counter--;
                m_animator.PlayAnimation("Dying");
            }
            return;
        } break;
        
        case State::DIVE_LOADING:{
            // TODO: Probably remove the condition
            if(not m_onGround){
                m_animator.PlayAnimation("Idle");
            }
            velocity.y = -DEFAULT_WORLD_GRAVITY_Y/40.f;
            if(!DIVE_LOAD_MODE )velocity.x = 0;
            if(m_dive_load_counter == 0) m_state = State::DIVING;
            else m_dive_load_counter--;
            noJump = true;
        } break;

        case State::CLIMBBING:{
            velocity.x = 0;
            if(m_jump){
                velocity.y = 13.0f;
                // Saute à gauche du mur
                if (m_facingRight) velocity.x = -10.0f;
                // Saute à gauche du mur
                else velocity.x = 10.0f;
                m_facingRight = !m_facingRight;
                m_state = State::FALLING;
            }
            else velocity.y = -1.0f;
        } break;

        case State::FALLING:{
             if(controls.jumpPressed){
                 float gravity = PE_Clamp(body->GetGravityScale()*0.95f, 0.4f, 1.0f);
                 body->SetGravityScale(gravity);
             }
             // Chargement du Dive
             if(m_dive){
                 m_state = State::DIVE_LOADING;
                 m_dive_load_counter = DIVE_LOAD_DURATION;
                 m_dive = false;
             }
        } break;

        case State::DIVING:{
             if(m_onGround) m_state = State::IDLE;
             else {
                 velocity.y = -30;
                 velocity.x = 0;
             }
             noJump = true;
        } break;
    }

    // Jump Additional
    if(m_jump && m_onGround && not noJump) velocity.y = 13.0f;
    m_jump = false;

    // Additional
    if (m_onGround){
        if (m_state != State::IDLE && velocity.x == 0.0f) {
            m_animator.PlayAnimation("Idle");
            m_state = State::IDLE;
        }
        else if (m_state != State::RUNNING && (velocity.x < -3 || velocity.x > 3))
        {
            m_animator.PlayAnimation("Running");
            m_state = State::RUNNING;
        }
        // Réinitialise le timer
        m_onAirTimer = 0.0f;
    }
    else {
        if (m_state != State::FALLING && m_state != State::CLIMBBING && m_state != State::DIVE_LOADING && m_state != State::DIVING)
        {
            m_animator.PlayAnimation("Falling");
            m_state = State::FALLING;
        }
        m_onAirTimer += m_scene.GetFixedTimeStep();
    }

    // Orientation du joueur
    // Utilisez m_hDirection qui vaut :
    // *  0.0f si le joueur n'acc�l�re pas ;
    // * +1.0f si le joueur acc�l�re vers la droite ;
    // * -1.0f si le joueur acc�l�re vers la gauche.

    if (m_hDirection != 0.0f && m_state != State::CLIMBBING) m_facingRight = m_hDirection >= 0.0f;

    

    // TODO : Rebond sur les ennemis

    // Remarques :
    // Le facteur de gravit� peut �tre modifi� avec l'instruction
    // -> body->SetGravityScale(0.5f);
    // pour faire des sauts de hauteurs diff�rentes.
    // La physique peut �tre diff�rente si le joueur touche ou non le sol.

    // D�finit la nouvelle vitesse du corps
    body->SetVelocity(velocity);
}

void Player::OnRespawn()
{
    PE_Body *body = GetBody();
    AssertNew(body);

    body->SetPosition(GetStartPosition() + PE_Vec2(0.5f, 0.0f));
    body->SetVelocity(PE_Vec2::zero);

    m_heartCount = 2;
    m_state = State::IDLE;
    m_hDirection = 0.0f;
    m_facingRight = true;
    m_bounce = false;
    m_jump = false;
    m_dive = false;

    m_animator.StopAnimations();
    m_animator.PlayAnimation("Idle");

    SetToRespawn(true);
}

void Player::DrawGizmos()
{
    SDL_Renderer *renderer = m_scene.GetRenderer();
    Graphics graphics(renderer, *m_scene.GetActiveCamera());
    PE_Vec2 position = GetPosition();
    PE_Vec2 velocity = GetVelocity();

    // Dessine en blanc le vecteur vitesse du joueur
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    graphics.DrawVector(0.5f * velocity, position);

    // Dessine en jaune les rayons pour la d�tection du sol
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    PE_Vec2 originL = position + PE_Vec2(-0.35f, 0.0f);
    PE_Vec2 originR = position + PE_Vec2(+0.35f, 0.0f);
    graphics.DrawVector(0.1f * PE_Vec2::down, originL);
    graphics.DrawVector(0.1f * PE_Vec2::down, originR);
}

void Player::OnCollisionEnter(GameCollision &collision)
{
    const PE_Manifold &manifold = collision.manifold;
    PE_Collider *otherCollider = collision.otherCollider;

    // Réinitialisation de la gravité
    PE_Body *body = GetBody();
    body->SetGravityScale(1.0f);

    // Collision avec un ennemi
    if (otherCollider->CheckCategory(CATEGORY_ENEMY))
    {
        Enemy *enemy = dynamic_cast<Enemy *>(collision.gameBody);
        if (enemy == nullptr)
        {
            assert(false);
            return;
        }

        // Calcule l'angle entre la normale de contact et le vecteur "UP"
        // L'angle vaut :
        // * 0 si le joueur est parfaitement au dessus de l'ennemi,
        // * 90 s'il est � gauche ou � droite
        // * 180 s'il est en dessous
        float angle = PE_AngleDeg(manifold.normal, PE_Vec2::up);
        if (angle < PLAYER_DAMAGE_ANGLE)
        {
            enemy->Damage(this);
            collision.ResolveUp();
        }
        else
        {
            collision.SetEnabled(false);
        }

        return;
    }

    // Collision avec un objet
    if (otherCollider->CheckCategory(CATEGORY_COLLECTABLE))
    {
        Collectable *collectable = dynamic_cast<Collectable *>(collision.gameBody);
        if (collectable == nullptr) return;

        // Collecte l'objet
        // C'est ensuite l'objet qui affecte un bonus au joueur,
        // en appellant AddFirefly() par exemple.
        collectable->Collect(this);
        return;
    }
}

void Player::OnCollisionStay(GameCollision &collision)
{
    const PE_Manifold &manifold = collision.manifold;
    PE_Collider *otherCollider = collision.otherCollider;
    
    if (otherCollider->CheckCategory(CATEGORY_COLLECTABLE))
    {
        // D�sactive la collision avec un objet
        // Evite d'arr�ter le joueur quand il prend un coeur
        collision.SetEnabled(false);
        return;
    }
    else if (otherCollider->CheckCategory(CATEGORY_TERRAIN))
    {
        float angleUp = PE_AngleDeg(manifold.normal, PE_Vec2::up);
        if (angleUp <= 55.0f)
        {
            // R�soud la collision en d�pla�ant le joueur vers le haut
            // Evite de "glisser" sur les pentes si le joueur ne bouge pas
            collision.ResolveUp();
        }

        // Le joueur glisse le long d'un mur
        if (angleUp == 90.0f && m_onAirTimer > 0.3f)
        {
            m_state = State::CLIMBBING;
        }
    }
}

void Player::OnCollisionExit(GameCollision &collision)
{
    const PE_Manifold &manifold = collision.manifold;
    PE_Collider *otherCollider = collision.otherCollider;
    
    

    // Si le joueur quitte un bloc de terrain (un mur), il arrete de grimper
    if (otherCollider->CheckCategory(CATEGORY_TERRAIN) && m_state == State::CLIMBBING)
    {
        m_onWallTimer = 0;
    }
}

void Player::AddFirefly(int count)
{
    m_fireflyCount += count;
}

void Player::AddHeart()
{
    m_heartCount++;
}

void Player::Damage()
{
    // TODO: Gestion de la vie
    // M�thode appel�e par un ennemi qui touche le joueur
    if(m_state != State::DYING){
        m_state = State::DYING;
        m_player_dying_counter = PLAYER_DYING_DURATION;
    }
    
}

void Player::Kill()
{
    m_scene.Respawn();
}

class WakeUpCallback : public PE_QueryCallback
{
public:
    WakeUpCallback() {};
    virtual bool ReportCollider(PE_Collider *collider)
    {
        collider->GetBody()->SetAwake(true);
        return true;
    }
};

void Player::WakeUpSurroundings()
{
    PE_World &world = m_scene.GetWorld();
    PE_Vec2 position = GetBody()->GetPosition();
    PE_AABB aabb(
            position.x - 20.0f, position.y - 10.0f,
            position.x + 20.0f, position.y + 10.0f
    );
    WakeUpCallback callback;
    world.QueryAABB(callback, aabb);
}

PE_Vec2 Player::UpdateOnGround(PE_Vec2 position){
    m_onGround = false;
    PE_Vec2 gndNormal = PE_Vec2::up;

    // Lance deux rayons vers le bas ayant pour origines
    // les coins gauche et droit du bas du collider du joueur
    // Ces deux rayons sont dessin�s en jaune dans DrawGizmos()
    PE_Vec2 originL = position + PE_Vec2(-0.35f, 0.0f);
    PE_Vec2 originR = position + PE_Vec2(+0.35f, 0.0f);

    // Les rayons ne touchent que des colliders solides (non trigger)
    // ayant la cat�gorie FILTER_TERRAIN
    RayHit hitL = m_scene.RayCast(originL, PE_Vec2::down, 0.1f, CATEGORY_TERRAIN, true);
    RayHit hitR = m_scene.RayCast(originR, PE_Vec2::down, 0.1f, CATEGORY_TERRAIN, true);

    if (hitL.collider != NULL)
    {
        // Le rayon gauche � touch� le sol
        m_onGround = true;
        gndNormal = hitL.normal;
    }
    if (hitR.collider != NULL)
    {
        // Le rayon droit � touch� le sol
        m_onGround = true;
        gndNormal = hitR.normal;
    }
    return gndNormal;
}
