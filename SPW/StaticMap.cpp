#include "StaticMap.h"
#include "Camera.h"
#include "Player.h"

StaticMap::StaticMap(Scene &scene, int width, int height) :
    GameBody(scene, Layer::TERRAIN_BACKGROUND), m_width(width), m_height(height)
{
    m_name = "StaticMap";

    m_tiles = new Tile*[width];
    for (int x = 0; x < width; x++)
    {
        m_tiles[x] = new Tile[height];
        for (int y = 0; y < height; y++)
        {
            Tile &tile = m_tiles[x][y];
            tile.collider = nullptr;
            tile.partIdx = 0;
            tile.type = Tile::Type::EMPTY;
        }
    }

    RE_Atlas *atlas = scene.GetAssetManager().GetAtlas(AtlasID::TERRAIN);
    RE_Atlas *ennemyAtlas = scene.GetAssetManager().GetAtlas(AtlasID::ENEMY);

    m_woodPart = atlas->GetPart("Wood");
    AssertNew(m_woodPart);

    m_oneWayPart = atlas->GetPart("OneWay");
    AssertNew(m_oneWayPart);

    m_terrainPart = atlas->GetPart("Terrain");
    AssertNew(m_terrainPart);

    m_spikePart = atlas->GetPart("Spike");
    AssertNew(m_spikePart);

    // Couleur des colliders en debug
    m_debugColor.r = 255;
    m_debugColor.g = 200;
    m_debugColor.b = 0;
}

StaticMap::~StaticMap()
{
    if (m_tiles)
    {
        for (int x = 0; x < m_width; x++)
        {
            delete[] m_tiles[x];
        }
        delete[] m_tiles;
    }
}

void StaticMap::SetTile(int x, int y, Tile::Type type)
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
    {
        assert(false);
        return;
    }

    Tile &tile = m_tiles[x][y];
    tile.partIdx = 0;
    tile.type = type;
}

void StaticMap::InitTiles()
{
    for (int x = 0; x < m_width; x++)
    {
        for (int y = 0; y < m_height; y++)
        {
            Tile &tile = m_tiles[x][y];
            Tile::Type type = GetTileType(x, y);

            switch (type)
            {
            case Tile::Type::GROUND:
                if(GetTileType(x, y + 1) == Tile::Type::STEEP_SLOPE_L or GetTileType(x, y + 1) == Tile::Type::GENTLE_SLOPE_L1)
                {
                    tile.partIdx = 14;
                }
                else if(GetTileType(x, y + 1) == Tile::Type::STEEP_SLOPE_R or GetTileType(x, y + 1) == Tile::Type::GENTLE_SLOPE_R1)
                {
                    tile.partIdx = 17;
                }
                else if (IsDirt(x, y + 1))
                {
                    tile.partIdx = 4;
                }
                else if (GetTileType(x-1, y) == Tile::Type::EMPTY)
                {
                    tile.partIdx = 0;
                }
                else if (GetTileType(x+1, y) == Tile::Type::EMPTY)
                {
                    tile.partIdx = 2;
                }
                else
                {
                    tile.partIdx = 1;
                }
                break;
            case Tile::Type::STEEP_SLOPE_L:
                tile.partIdx = 9;
                break;
            case Tile::Type::STEEP_SLOPE_R:
                tile.partIdx = 10;
                break;
            case Tile::Type::GENTLE_SLOPE_L1:
                tile.partIdx = 13;
                break;
            case Tile::Type::GENTLE_SLOPE_L2:
                tile.partIdx = 12;
                break;
            case Tile::Type::GENTLE_SLOPE_R1:
                tile.partIdx = 15;
                break;  
            case Tile::Type::GENTLE_SLOPE_R2:
                tile.partIdx = 16;
                break;

            case Tile::Type::ROOF:
                if(GetTileType(x, y - 1) == Tile::Type::STEEP_ROOF_L or GetTileType(x, y - 1) == Tile::Type::GENTLE_ROOF_L1)
                {
                    tile.partIdx = 14;
                }
                else if(GetTileType(x, y - 1) == Tile::Type::STEEP_ROOF_R or GetTileType(x, y - 1) == Tile::Type::GENTLE_ROOF_R1)
                {
                    tile.partIdx = 17;
                }
                else if (IsDirt(x, y - 1))
                {
                    tile.partIdx = 4;
                }
                else if (GetTileType(x-1, y) == Tile::Type::EMPTY)
                {
                    tile.partIdx = 0;
                }
                else if (GetTileType(x+1, y) == Tile::Type::EMPTY)
                {
                    tile.partIdx = 2;
                }
                else
                {
                    tile.partIdx = 1;
                }
                break;
            case Tile::Type::STEEP_ROOF_L:
                tile.partIdx = 9;
                break;
            case Tile::Type::STEEP_ROOF_R:
                tile.partIdx = 10;
                break;
            case Tile::Type::GENTLE_ROOF_L1:
                tile.partIdx = 13;
                break;
            case Tile::Type::GENTLE_ROOF_L2:
                tile.partIdx = 12;
                break;
            case Tile::Type::GENTLE_ROOF_R1:
                tile.partIdx = 15;
                break;  
            case Tile::Type::GENTLE_ROOF_R2:
                tile.partIdx = 16;
                break;

                

            default:
                tile.partIdx = 0;
                break;
            }
        }
    }
}

void StaticMap::Render()
{
    SDL_Renderer *renderer = m_scene.GetRenderer();
    Camera *camera = m_scene.GetActiveCamera();

    PE_AABB view = camera->GetWorldView();
    int x0 = (int)view.lower.x - 1;
    int y0 = (int)view.lower.y - 1;
    int x1 = (int)view.upper.x + 2;
    int y1 = (int)view.upper.y + 2;

    x0 = PE_Max(x0, 0);
    y0 = PE_Max(y0, 0);
    x1 = PE_Min(x1, m_width);
    y1 = PE_Min(y1, m_height);

    for (int x = x0; x < x1; ++x)
    {
        for (int y = y0; y < y1; ++y)
        {
            Tile &tile = m_tiles[x][y];
            PE_Collider *collider = tile.collider;

            PE_Vec2 position((float)x, (float)y);
            SDL_FRect dst = { 0 };

            camera->WorldToView(position, dst.x, dst.y);
            float scale = camera->GetWorldToViewScale();
            dst.w = scale * 1.0f;
            dst.h = scale * 1.0f;

            switch (tile.type)
            {
            case Tile::Type::GROUND:
            case Tile::Type::STEEP_SLOPE_L:
            case Tile::Type::STEEP_SLOPE_R:
            case Tile::Type::GENTLE_SLOPE_L1:
            case Tile::Type::GENTLE_SLOPE_L2:
            case Tile::Type::GENTLE_SLOPE_R1:
            case Tile::Type::GENTLE_SLOPE_R2:
                m_terrainPart->RenderCopyF(tile.partIdx, &dst, RE_Anchor::SOUTH_WEST);
                break;
                
            case Tile::Type::ROOF:
            case Tile::Type::STEEP_ROOF_L:
            case Tile::Type::STEEP_ROOF_R:
            case Tile::Type::GENTLE_ROOF_L1:
            case Tile::Type::GENTLE_ROOF_L2:
            case Tile::Type::GENTLE_ROOF_R1:
            case Tile::Type::GENTLE_ROOF_R2:
                m_terrainPart->RenderCopyExF(tile.partIdx, &dst, RE_Anchor::SOUTH_WEST, 0.0, Vec2(0.0f, 0.0f), SDL_FLIP_VERTICAL);
                break;
                
            case Tile::Type::WOOD:
                m_woodPart->RenderCopyF(0, &dst, RE_Anchor::SOUTH_WEST);
                break;
            case Tile::Type::SPIKE:
                m_spikePart->RenderCopyF(0, &dst, RE_Anchor::SOUTH_WEST);
                break;
            default:
                break;
            }
        }
    }
}

void StaticMap::Start()
{
    PE_World &world = m_scene.GetWorld();
    PE_Body *body = NULL;

    // Crée le corps
    PE_BodyDef bodyDef;
    bodyDef.type = PE_BodyType::STATIC;
    bodyDef.position.SetZero();
    bodyDef.name = (char *)"StaticMap";
    body = world.CreateBody(bodyDef);
    AssertNew(body);
    SetBody(body);

    // Crée les colliders
    PE_Vec2 vertices[3];
    PE_Vec2 lvertices[4];
    PE_PolygonShape polygon;
    PE_ColliderDef colliderDef;

    for (int x = 0; x < m_width; ++x)
    {
        for (int y = 0; y < m_height; ++y)
        {
            Tile &tile = m_tiles[x][y];
            if (tile.type == Tile::Type::EMPTY)
            {
                continue;
            }

            PE_Vec2 position((float)x, (float)y);
            bool newCollider = true;
            colliderDef.SetDefault();
            colliderDef.shape = &polygon;
            colliderDef.friction = 0.5f;
            colliderDef.filter.categoryBits = CATEGORY_TERRAIN;
            colliderDef.userData.id = 0;

            switch (tile.type)
            {
            case Tile::Type::GROUND:
            case Tile::Type::ROOF:
            case Tile::Type::WOOD:
                polygon.SetAsBox(PE_AABB(position, position + PE_Vec2(1.0f, 1.0f)));
                break;

            case Tile::Type::SPIKE:
                colliderDef.userData.id = 1;

                vertices[0] = position + PE_Vec2(0.1f, 0.0f);
                vertices[1] = position + PE_Vec2(0.9f, 0.0f);
                vertices[2] = position + PE_Vec2(0.5f, 0.8f);
                polygon.SetVertices(vertices, 3);
                break;
            /* SLOPES */
            case Tile::Type::GENTLE_SLOPE_R1:
                vertices[0] = position + PE_Vec2(0.0f, 0.0f);
                vertices[1] = position + PE_Vec2(1.0f, 0.0f);
                vertices[2] = position + PE_Vec2(1.0f, 0.5f);
                polygon.SetVertices(vertices, 3);
                colliderDef.filter.categoryBits = CATEGORY_TERRAIN | CATEGORY_SLOPE;
                break;
            case Tile::Type::GENTLE_SLOPE_R2:
                lvertices[0] = position + PE_Vec2(0.0f, 0.5f);
                lvertices[1] = position + PE_Vec2(0.0f, 0.0f);
                lvertices[2] = position + PE_Vec2(1.0f, 0.0f);
                lvertices[3] = position + PE_Vec2(1.0f, 1.0f);
                polygon.SetVertices(lvertices, 4);
                colliderDef.filter.categoryBits = CATEGORY_TERRAIN | CATEGORY_SLOPE;
                break;
            case Tile::Type::GENTLE_SLOPE_L1:
                vertices[0] = position + PE_Vec2(0.0f, 0.0f);
                vertices[1] = position + PE_Vec2(1.0f, 0.0f);
                vertices[2] = position + PE_Vec2(0.0f, 0.5f);
                polygon.SetVertices(vertices, 3);
                colliderDef.filter.categoryBits = CATEGORY_TERRAIN | CATEGORY_SLOPE;
                break;
            case Tile::Type::GENTLE_SLOPE_L2:
                lvertices[0] = position + PE_Vec2(0.0f, 1.0f);
                lvertices[1] = position + PE_Vec2(0.0f, 0.0f);
                lvertices[2] = position + PE_Vec2(1.0f, 0.0f);
                lvertices[3] = position + PE_Vec2(1.0f, 0.5f);
                polygon.SetVertices(lvertices, 4);
                colliderDef.filter.categoryBits = CATEGORY_TERRAIN | CATEGORY_SLOPE;
                break;
            case Tile::Type::STEEP_SLOPE_L:
                vertices[0] = position + PE_Vec2(0.0f, 0.0f);
                vertices[1] = position + PE_Vec2(1.0f, 0.0f);
                vertices[2] = position + PE_Vec2(0.0f, 1.0f);
                polygon.SetVertices(vertices, 3);
                colliderDef.filter.categoryBits = CATEGORY_TERRAIN | CATEGORY_SLOPE;
                break;
            case Tile::Type::STEEP_SLOPE_R:
                vertices[0] = position + PE_Vec2(0.0f, 0.0f);
                vertices[1] = position + PE_Vec2(1.0f, 0.0f);
                vertices[2] = position + PE_Vec2(1.0f, 1.0f);
                polygon.SetVertices(vertices, 3);
                colliderDef.filter.categoryBits = CATEGORY_TERRAIN | CATEGORY_SLOPE;
                break;
            /* ROOFS */
            case Tile::Type::GENTLE_ROOF_R1:
                vertices[0] = position + PE_Vec2(0.0f, 1.0f);      // Bottom left vertex
                vertices[1] = position + PE_Vec2(1.0f, 0.5f);      // Bottom right vertex
                vertices[2] = position + PE_Vec2(1.0f, 1.0f);     // Top right vertex
                polygon.SetVertices(vertices, 3);
                colliderDef.filter.categoryBits = CATEGORY_TERRAIN | CATEGORY_SLOPE;
                break;
            case Tile::Type::GENTLE_ROOF_R2:
                lvertices[0] = position + PE_Vec2(0.0f, 1.0f);
                lvertices[1] = position + PE_Vec2(0.0f, 0.5f);
                lvertices[2] = position + PE_Vec2(1.0f, 0.0f);
                lvertices[3] = position + PE_Vec2(1.0f, 1.0f);
                polygon.SetVertices(lvertices, 4);
                colliderDef.filter.categoryBits = CATEGORY_TERRAIN | CATEGORY_SLOPE;
                break;
            case Tile::Type::GENTLE_ROOF_L1:
                vertices[0] = position + PE_Vec2(0.0f, 1.0f);
                vertices[1] = position + PE_Vec2(0.0f, 0.5f);
                vertices[2] = position + PE_Vec2(1.0f, 1.0f);
                polygon.SetVertices(vertices, 3);
                colliderDef.filter.categoryBits = CATEGORY_TERRAIN | CATEGORY_SLOPE;
                break;
            case Tile::Type::GENTLE_ROOF_L2:
                lvertices[0] = position + PE_Vec2(0.0f, 1.0f);
                lvertices[1] = position + PE_Vec2(0.0f, 0.0f);
                lvertices[2] = position + PE_Vec2(1.0f, 0.5f);
                lvertices[3] = position + PE_Vec2(1.0f, 1.0f);
                polygon.SetVertices(lvertices, 4);
                colliderDef.filter.categoryBits = CATEGORY_TERRAIN | CATEGORY_SLOPE;
                break;
            case Tile::Type::STEEP_ROOF_L:
                vertices[0] = position + PE_Vec2(0.0f, 1.0f);
                vertices[1] = position + PE_Vec2(0.0f, 0.0f);
                vertices[2] = position + PE_Vec2(1.0f, 1.0f);
                polygon.SetVertices(vertices, 3);
                colliderDef.filter.categoryBits = CATEGORY_TERRAIN | CATEGORY_SLOPE;
                break;
            case Tile::Type::STEEP_ROOF_R:
                vertices[0] = position + PE_Vec2(0.0f, 1.0f);
                vertices[1] = position + PE_Vec2(1.0f, 0.0f);
                vertices[2] = position + PE_Vec2(1.0f, 1.0f);
                polygon.SetVertices(vertices, 3);
                colliderDef.filter.categoryBits = CATEGORY_TERRAIN | CATEGORY_SLOPE;
                break;
           

            default:
                newCollider = false;
                break;
            }
            if (newCollider)
            {
                tile.collider = body->CreateCollider(colliderDef);
                AssertNew(tile.collider);
            }
            else
            {
                tile.collider = nullptr;
            }
        }
    }

    // Limite à gauche du monde
    polygon.SetAsBox(-1.0f, -2.0f, 0.0f, (float)m_height + 10.0f);
    colliderDef.SetDefault();
    colliderDef.friction = 0.0f;
    colliderDef.filter.categoryBits = CATEGORY_TERRAIN;
    colliderDef.shape = &polygon;
    body->CreateCollider(colliderDef);

    // Limite à droite du monde
    polygon.SetAsBox((float)m_width, -2.0f, (float)m_width + 1.0f, (float)m_height + 10.0f);
    colliderDef.SetDefault();
    colliderDef.friction = 0.0f;
    colliderDef.filter.categoryBits = CATEGORY_TERRAIN;
    colliderDef.shape = &polygon;
    body->CreateCollider(colliderDef);
}

void StaticMap::OnCollisionStay(GameCollision &collision)
{
    
    // On v�rifie que la collision concerne un pique
    if (collision.collider->GetUserData().id == 1)
    {
        if (collision.otherCollider->CheckCategory(CATEGORY_PLAYER))
        {
            Player *player = dynamic_cast<Player *>(collision.gameBody);
            if (player == nullptr)
            {
                assert(false);
                return;
            }

            player->Kill();
        }
    }
}

Tile::Type StaticMap::GetTileType(int x, int y) const
{
    if (x < 0 || x >= m_width || y < 0)
        return Tile::Type::GROUND;
    else if (y >= m_height)
        return Tile::Type::EMPTY;
    else
        return m_tiles[x][y].type;
}

bool StaticMap::IsGround(int x, int y) const
{
    switch (GetTileType(x, y))
    {
    case Tile::Type::GROUND:
    case Tile::Type::STEEP_SLOPE_L:
    case Tile::Type::STEEP_SLOPE_R:
    case Tile::Type::GENTLE_SLOPE_L1:
    case Tile::Type::GENTLE_SLOPE_L2:
    case Tile::Type::GENTLE_SLOPE_R1:
    case Tile::Type::GENTLE_SLOPE_R2:
        return true;
    default:
        return false;
    }
}

bool StaticMap::IsRoof(int x, int y) const
{
    switch (GetTileType(x, y))
    {
    case Tile::Type::ROOF:
    case Tile::Type::STEEP_ROOF_L:
    case Tile::Type::STEEP_ROOF_R:
    case Tile::Type::GENTLE_ROOF_L1:
    case Tile::Type::GENTLE_ROOF_L2:
    case Tile::Type::GENTLE_ROOF_R1:
    case Tile::Type::GENTLE_ROOF_R2:
        return true;
    default:
        return false;
    }
}

bool StaticMap::IsDirt(int x, int y) const
{
    return IsGround(x, y) or IsRoof(x, y);
}