#pragma once

#include "Settings.h"
#include "GameBody.h"

struct EditorTile
{
    enum class Type : int
    {
        EMPTY, GROUND, WOOD, ONE_WAY, SPIKE,
        STEEP_SLOPE_L, STEEP_SLOPE_R,
        GENTLE_SLOPE_L1, GENTLE_SLOPE_L2, GENTLE_SLOPE_R1, GENTLE_SLOPE_R2,
        FAKE_FLAG, FAKE_NUT, FAKE_FIREFLY, SPAWN_POINT
    };
    Type type;
    int partIdx;
    PE_Collider *collider;
};

class EditorMap : public GameBody
{
public:
    EditorMap(Scene &scene, int width, int height);
    virtual ~EditorMap();

    void SetTile(int x, int y, EditorTile::Type type);
    void InitTiles();

    virtual void Update() override;
    virtual void Render() override;
    virtual void Start() override;
    virtual void OnCollisionStay(GameCollision &collision) override;

    int GetRealWidth();
    int GetRealHeight();

    EditorTile::Type GetTileType(int x, int y) const;

private:

    RE_AtlasPart *m_woodPart;
    RE_AtlasPart *m_oneWayPart;
    RE_AtlasPart *m_terrainPart;
    RE_AtlasPart *m_spikePart;
    RE_AtlasPart *m_fakePart;
    RE_AtlasPart *m_fakeNutPart;
    RE_AtlasPart *m_fakeFireflyPart;
    RE_AtlasPart *m_spawnPointPart;

    EditorTile **m_tiles;
    int m_width;
    int m_height;
    int m_realWidth;
    int m_realHeight;
    
    bool IsGround(int x, int y) const;
};

inline void EditorMap::Update()
{
    SetVisible(true);
}

inline int EditorMap::GetRealWidth()
{
    return m_realWidth;
}

inline int EditorMap::GetRealHeight()
{
    return m_realHeight;
}