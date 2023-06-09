#include "EditorMap.h"
#include "Camera.h"
#include "EditorScene.h"
#include "Player.h"


EditorMap::EditorMap(Scene &scene, int width, int height) :
    GameBody(scene, Layer::TERRAIN_BACKGROUND), m_width(width), m_height(height), m_realWidth(1), m_realHeight(1), m_editorScene(scene)
{
    m_name = "EditorMap";

    m_tiles = new EditorTile*[width];
    for (int x = 0; x < width; x++)
    {
        m_tiles[x] = new EditorTile[height];
        for (int y = 0; y < height; y++)
        {
            EditorTile &tile = m_tiles[x][y];
            tile.collider = nullptr;
            tile.partIdx = 0;
            tile.type = EditorTile::Type::EMPTY;
        }
    }

    RE_Atlas *atlas = scene.GetAssetManager().GetAtlas(AtlasID::TERRAIN);
    RE_Atlas *ennemyAtlas = scene.GetAssetManager().GetAtlas(AtlasID::ENEMY);
    RE_Atlas *uiAtlas = scene.GetAssetManager().GetAtlas(AtlasID::UI);
    RE_Atlas *leverAtlas = scene.GetAssetManager().GetAtlas(AtlasID::LEVER);
    RE_Atlas *gridAtlas = scene.GetAssetManager().GetAtlas(AtlasID::GRID);

    m_woodPart = atlas->GetPart("Wood");
    AssertNew(m_woodPart);

    m_oneWayPart = atlas->GetPart("OneWay");
    AssertNew(m_oneWayPart);

    m_terrainPart = atlas->GetPart("Terrain");
    AssertNew(m_terrainPart);

    m_spikePart = atlas->GetPart("Spike");
    AssertNew(m_spikePart);

    //TODO: ADD FAKE TILES HERE
    m_fakePart = atlas->GetPart("Fake");
    AssertNew(m_fakePart);

    m_fakeNutPart = ennemyAtlas->GetPart("NutIdle");
    AssertNew(m_fakeNutPart);

    m_fakeFireflyPart = uiAtlas->GetPart("Firefly");
    AssertNew(m_fakeNutPart);

    m_spawnPointPart = uiAtlas->GetPart("Life");
    AssertNew(m_spawnPointPart);

    m_checkpointPart = leverAtlas->GetPart("LeverOff");
    AssertNew(m_checkpointPart);

    m_fakeFlappyPart = ennemyAtlas->GetPart("FlappyIdle");
    AssertNew(m_fakeFlappyPart);
    m_brickPart = atlas->GetPart("Brick");
    AssertNew(m_brickPart);


    m_gridPart = gridAtlas->GetPart("Grid");
    AssertNew(m_gridPart);

    // Couleur des colliders en debug
    m_debugColor.r = 255;
    m_debugColor.g = 200;
    m_debugColor.b = 0;
}

EditorMap::~EditorMap()
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

void EditorMap::SetTile(int x, int y, EditorTile::Type type, int partIdx, bool extendGroup)
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
    {
        return;
        // TODO: EXTEND THE MATRIX HERE
    }

    if(x >= m_realWidth)
        m_realWidth = x + 1;
    if(y >= m_realHeight)
        m_realHeight = y + 1;

    /* CHECK FOR DETACHED HEAD */
    if(m_commit_head != (int)m_commits.size()-1)
    {
        while(m_commit_head != (int)m_commits.size()-1)
        {
            m_commits.pop_back();
        }
        while(m_group_head != (int)m_commit_groups.size()-1)
        {
            m_commit_groups.pop_back();
        }
    }

    EditorTile &tile = m_tiles[x][y];
    Commit commit {x, y, tile.type, type, tile.partIdx, partIdx};
    m_commits.push_back(commit); m_commit_head+=1;
    tile.partIdx = partIdx;
    tile.type = type;
    if(extendGroup) m_commit_groups.back()+=1;
    else
    {
        m_commit_groups.push_back(1); m_group_head+=1;
        // DEBUG
        // printf("EXTENDING GROUP HEAD TO %d\n", m_group_head);
    }
}

void EditorMap::InitTiles()
{
    for (int x = 0; x < m_realWidth; x++)
    {
        for (int y = 0; y < m_realHeight; y++)
        {
            EditorTile &tile = m_tiles[x][y];
            EditorTile::Type type = GetTileType(x, y);

            switch (type)
            {
            case EditorTile::Type::GROUND:
                if(GetTileType(x, y + 1) == EditorTile::Type::STEEP_SLOPE_L or GetTileType(x, y + 1) == EditorTile::Type::GENTLE_SLOPE_L1)
                {
                    tile.partIdx = 14;
                }
                else if(GetTileType(x, y + 1) == EditorTile::Type::STEEP_SLOPE_R or GetTileType(x, y + 1) == EditorTile::Type::GENTLE_SLOPE_R1)
                {
                    tile.partIdx = 17;
                }
                else if(GetTileType(x, y + 1) == EditorTile::Type::GENTLE_SLOPE_L2 or GetTileType(x, y + 1) == EditorTile::Type::GENTLE_SLOPE_R2)
                {
                    tile.partIdx = 4;
                }
                else if (IsGround(x, y + 1))
                {
                    tile.partIdx = 4;
                }
                else if (GetTileType(x-1, y) == EditorTile::Type::EMPTY)
                {
                    tile.partIdx = 0;
                }
                else if (GetTileType(x+1, y) == EditorTile::Type::EMPTY)
                {
                    tile.partIdx = 2;
                }
                else
                {
                    tile.partIdx = 1;
                }
                break;
            case EditorTile::Type::STEEP_SLOPE_L:
                tile.partIdx = 9;
                break;
            case EditorTile::Type::STEEP_SLOPE_R:
                tile.partIdx = 10;
                break;
            case EditorTile::Type::GENTLE_SLOPE_L1:
                tile.partIdx = 13;
                break;
            case EditorTile::Type::GENTLE_SLOPE_L2:
                tile.partIdx = 12;
                break;
            case EditorTile::Type::GENTLE_SLOPE_R1:
                tile.partIdx = 15;
                break;  
            case EditorTile::Type::GENTLE_SLOPE_R2:
                tile.partIdx = 16;
                break;

            case EditorTile::Type::ROOF:
                if(GetTileType(x, y - 1) == EditorTile::Type::STEEP_ROOF_L or GetTileType(x, y - 1) == EditorTile::Type::GENTLE_ROOF_L1)
                {
                    tile.partIdx = 14;
                }
                else if(GetTileType(x, y - 1) == EditorTile::Type::STEEP_ROOF_R or GetTileType(x, y - 1) == EditorTile::Type::GENTLE_ROOF_R1)
                {
                    tile.partIdx = 17;
                }
                else if (IsDirt(x, y - 1))
                {
                    tile.partIdx = 4;
                }
                else if (GetTileType(x-1, y) == EditorTile::Type::EMPTY)
                {
                    tile.partIdx = 0;
                }
                else if (GetTileType(x+1, y) == EditorTile::Type::EMPTY)
                {
                    tile.partIdx = 2;
                }
                else
                {
                    tile.partIdx = 1;
                }
                break;
            case EditorTile::Type::STEEP_ROOF_L:
                tile.partIdx = 9;
                break;
            case EditorTile::Type::STEEP_ROOF_R:
                tile.partIdx = 10;
                break;
            case EditorTile::Type::GENTLE_ROOF_L1:
                tile.partIdx = 13;
                break;
            case EditorTile::Type::GENTLE_ROOF_L2:
                tile.partIdx = 12;
                break;
            case EditorTile::Type::GENTLE_ROOF_R1:
                tile.partIdx = 15;
                break;  
            case EditorTile::Type::GENTLE_ROOF_R2:
                tile.partIdx = 16;
                break;

            default:
                tile.partIdx = 0;
                break;
            }
        }
    }
}

void EditorMap::Render()
{
    SDL_Renderer *renderer = m_scene.GetRenderer();
    Camera *camera = m_scene.GetActiveCamera();
    
    float scale = camera->GetWorldToViewScale();
    scale*=m_viewFactor;
    
    PE_Vec2 position {0, 0};
    ViewToWorld(0, 0, position);
    int x0 = (int)position.x;
    int y1 = (int)position.y;
    ViewToWorld((float)camera->GetWidth(), (float)camera->GetHeight(), position);
    int x1 = (int)position.x + 1;
    int y0 = (int)position.y + 1;

    x0-=2;
    y0-=2;
    x1+=2;
    y1+=2;
    x0 = PE_Max(x0, 0);
    y0 = PE_Max(y0, 0);
    x1 = PE_Min(x1, m_width);
    y1 = PE_Min(y1, m_width);
    
    for (int x = x0; x < x1; ++x)
    {
        for (int y = y0; y < y1; ++y)
        {
            EditorTile &tile = m_tiles[x][y];
            PE_Collider *collider = tile.collider;

            position.x = (float)x; position.y = (float)y;
            SDL_FRect dst = { 0 };

            WorldToView(position, dst.x, dst.y, m_viewFactor);
            dst.w = scale;
            dst.h = scale;

            switch (tile.type)
            {
            case EditorTile::Type::GROUND:
            case EditorTile::Type::STEEP_SLOPE_L:
            case EditorTile::Type::STEEP_SLOPE_R:
            case EditorTile::Type::GENTLE_SLOPE_L1:
            case EditorTile::Type::GENTLE_SLOPE_L2:
            case EditorTile::Type::GENTLE_SLOPE_R1:
            case EditorTile::Type::GENTLE_SLOPE_R2:
                m_terrainPart->RenderCopyF(tile.partIdx, &dst, RE_Anchor::SOUTH_WEST);
                break;
            case EditorTile::Type::ROOF:
            case EditorTile::Type::STEEP_ROOF_L:
            case EditorTile::Type::STEEP_ROOF_R:
            case EditorTile::Type::GENTLE_ROOF_L1:
            case EditorTile::Type::GENTLE_ROOF_L2:
            case EditorTile::Type::GENTLE_ROOF_R1:
            case EditorTile::Type::GENTLE_ROOF_R2:
                m_terrainPart->RenderCopyExF(tile.partIdx, &dst, RE_Anchor::SOUTH_WEST, 0.0, Vec2(0.0f, 0.0f), SDL_FLIP_VERTICAL);
                break;
            case EditorTile::Type::FAKE_FLAG:
                m_fakePart->RenderCopyF(0, &dst, RE_Anchor::SOUTH_WEST);
                break;
            case EditorTile::Type::FAKE_NUT:
                m_fakeNutPart->RenderCopyF(0, &dst, RE_Anchor::SOUTH_WEST);
                break;
            case EditorTile::Type::FAKE_FIREFLY:
                m_fakeFireflyPart->RenderCopyF(0, &dst, RE_Anchor::SOUTH_WEST);
                break;
            case EditorTile::Type::WOOD:
                m_woodPart->RenderCopyF(0, &dst, RE_Anchor::SOUTH_WEST);
                break;
            case EditorTile::Type::ONE_WAY:
                m_oneWayPart->RenderCopyF(0, &dst, RE_Anchor::SOUTH_WEST);
                break;
            case EditorTile::Type::SPIKE:
                m_spikePart->RenderCopyF(0, &dst, RE_Anchor::SOUTH_WEST);
                break;
            case EditorTile::Type::SPAWN_POINT:
                m_spawnPointPart->RenderCopyF(0, &dst, RE_Anchor::SOUTH_WEST);
                break;
            case EditorTile::Type::CHECKPOINT:
                m_checkpointPart->RenderCopyF(0, &dst, RE_Anchor::SOUTH_WEST);
                break;
            case EditorTile::Type::FAKE_FLAPPY:
                m_fakeFlappyPart->RenderCopyF(0, &dst, RE_Anchor::SOUTH_WEST);
                break;
            case EditorTile::Type::BRICK:
                m_brickPart->RenderCopyF(0, &dst, RE_Anchor::SOUTH_WEST);
                break;
            default:
                break;
            }
            m_gridPart->RenderCopyF(0, &dst, RE_Anchor::SOUTH_WEST);

        }
    }
}

void EditorMap::Start()
{
}

void EditorMap::OnCollisionStay(GameCollision &collision)
{
}

EditorTile::Type EditorMap::GetTileType(int x, int y) const
{
    if (x < 0 || x >= m_width || y < 0)
        return EditorTile::Type::GROUND;
    else if (y >= m_height)
        return EditorTile::Type::EMPTY;
    else
        return m_tiles[x][y].type;
}

bool EditorMap::IsGround(int x, int y) const
{
    switch (GetTileType(x, y))
    {
    case EditorTile::Type::GROUND:
    case EditorTile::Type::STEEP_SLOPE_L:
    case EditorTile::Type::STEEP_SLOPE_R:
    case EditorTile::Type::GENTLE_SLOPE_L1:
    case EditorTile::Type::GENTLE_SLOPE_L2:
    case EditorTile::Type::GENTLE_SLOPE_R1:
    case EditorTile::Type::GENTLE_SLOPE_R2:
        return true;
    default:
        return false;
    }
}

bool EditorMap::IsRoof(int x, int y) const
{
    switch (GetTileType(x, y))
    {
    case EditorTile::Type::ROOF:
    case EditorTile::Type::STEEP_ROOF_L:
    case EditorTile::Type::STEEP_ROOF_R:
    case EditorTile::Type::GENTLE_ROOF_L1:
    case EditorTile::Type::GENTLE_ROOF_L2:
    case EditorTile::Type::GENTLE_ROOF_R1:
    case EditorTile::Type::GENTLE_ROOF_R2:
        return true;
    default:
        return false;
    }
}

bool EditorMap::IsDirt(int x, int y) const
{
    return IsGround(x, y) or IsRoof(x, y);
}

void EditorMap::Forward(int n)
{
        
    for(int i=0; i<n; i++)
    {
        m_commit_head++;
        Commit &commit = m_commits[m_commit_head];
        m_tiles[commit.x][commit.y].type = commit.toType;
        m_tiles[commit.x][commit.y].partIdx = commit.toPartIdx;
        if(commit.toType == EditorTile::Type::SPAWN_POINT) ((EditorScene&)m_editorScene).SetSpawnSet(true);
    }
}

void EditorMap::ForwardGroup()
{
    if(m_group_head == (int)m_commit_groups.size()-1) return;
    m_group_head++;
    int gs = m_commit_groups[m_group_head];
    Forward(gs);
    InitTiles();
}


void EditorMap::Rollback(int n)
{
    if(n>(int)m_commits.size())n=(int)m_commits.size();
        
    for(int i=0; i<n; i++)
    {
        Commit &commit = m_commits[m_commit_head];
        m_tiles[commit.x][commit.y].type = commit.fromType;
        m_tiles[commit.x][commit.y].partIdx = commit.fromPartIdx;
        if(commit.toType == EditorTile::Type::SPAWN_POINT) ((EditorScene&)m_editorScene).SetSpawnSet(false);
        m_commit_head--;
    }
}

void EditorMap::RollbackGroup()
{
    if(m_group_head == -1) return;
    int gs = m_commit_groups[m_group_head]; m_group_head--;
    Rollback(gs);
    InitTiles();
}

void EditorMap::WorldToView(PE_Vec2 position, float& x, float& y, float factor)
{
    Camera *camera = m_scene.GetActiveCamera();
    float w = camera->GetWorldView().GetWidth();
    float h = camera->GetWorldView().GetHeight();
    w/=factor; h/=factor;
    float xScale = (float)camera->GetWidth()  / w;
    float yScale = (float)camera->GetHeight() / h;
    x = (position.x - camera->GetWorldView().lower.x) * xScale;
    y = (position.y - camera->GetWorldView().lower.y) * yScale;
    y = (float)camera->GetHeight() - y;
}

void EditorMap::ViewToWorld(float x, float y, PE_Vec2 &position)
{
    Camera *camera = m_scene.GetActiveCamera();
    y = (float)camera->GetHeight() - y;
    float w = camera->GetWorldView().GetWidth();
    float h = camera->GetWorldView().GetHeight();
    w/=m_viewFactor; h/=m_viewFactor;
    float xRatio = x / (float)camera->GetWidth();
    float yRatio = y / (float)camera->GetHeight();
    position.x = camera->GetWorldView().lower.x + xRatio * w;
    position.y = camera->GetWorldView().lower.y + yRatio * h;
}