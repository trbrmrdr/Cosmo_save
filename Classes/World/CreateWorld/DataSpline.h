#pragma once
#include "stdafx.h"
#include "Box2D/Box2D.h"
#include "WorldSpline/TSpline.h"

extern int velocityIterations;
extern int positionIterations;
extern float timeStep;
extern b2World* m_b2world;

#define to_b2Vec2(vec2) (b2Vec2((vec2).x,(vec2).y))
#define to_Vec2(vec2) (Vec2((vec2).x,(vec2).y))

enum  DataSplineType {
    DSType_EndBox = 0,
    DSType_Stataic,

    DSType_Bubble,
    DSType_Dynamic,

    DSType_Water,
};

const uint16 k_BybbleC = 0x0001;
const uint16 k_TempC = 0x0002;

const uint16 k_defMask = 0xFFFF;
const uint16 k_neededMask = 0xFFFF ^ k_TempC;

typedef std::vector<b2Body*> t_sector;
class PhysicsObject;

class DataSpline: public Ref {

//    friend class PhysicsObject;

public:
    b2Body* getBody() { return m_body; }

    int test_check = 0;
    //=1; в центре крутимся по кругу
    //=2; в низу двигаемся на верх

    //______ test from smooth body
    b2Body *innerBody = NULL;

    float springiness = 2.6f;//2.5
    float damp_rat = .5f;

    float m_radiys_j = .5f;

    int m_count_w = 1;
    int m_count_h = 1;

    float m_sc_r = .6f;
    Vec2 d_c = Vec2(.0f, -8.5f);
    std::vector<t_sector> m_sectors;
    //____________	

    void setParent(PhysicsObject* parent) { m_parent = parent; }
    PhysicsObject* getParent() { return m_parent; }
private:
    bool m_not_spline = false;
    bool m_enable_physics = true;
    float m_gscale;
    float m_restriction = .0f;
    float m_dencity = 1.f;

    TSpline* m_parentSpline = NULL;

    PhysicsObject* m_parent = NULL;

    //______________

    b2Body* m_body = NULL;

    DataSplineType m_type;

    b2MassData m_massData;
    b2Filter m_filter;
    Vec2 m_pos;
public:

    int m_di = 1;
    void setInt() { m_di = RandomHelper::random_int(0, 2) > 1 ? -1 : +1; }

    static b2Filter getFilter_out_b() {
        b2Filter _filter;
        _filter.categoryBits = k_TempC;
        _filter.maskBits = k_defMask;
        return _filter;
    }

    static b2Filter getFilter_b() {
        b2Filter _filter;
        _filter.categoryBits = k_BybbleC;
        _filter.maskBits = k_neededMask;
        return _filter;
    }

    void setFilter_def() {
        b2Filter _filter;
        _filter.categoryBits = k_BybbleC;
        _filter.maskBits = k_defMask;
        setFilter(_filter);
    }

    void setFilter_b() {
        setFilter(getFilter_b());
    }

    void setFilter_out_b() {
        setFilter(getFilter_out_b());
    }


    b2Filter getFilter() { return m_filter; }

    void setFilter(const b2Filter& filter) {
        if (m_filter.categoryBits == filter.categoryBits &&
            m_filter.groupIndex == filter.groupIndex&&
            m_filter.maskBits == filter.maskBits)
            return;
        m_filter = filter;

        //todo 
        if (NULL != m_body) {
            b2Fixture* t_fixture = m_body->GetFixtureList();
            while (NULL != t_fixture) {
                t_fixture->SetFilterData(m_filter);
                t_fixture = t_fixture->GetNext();
            }
        }
        //setDataInWorld();
    }

    void rmBody() {
        m_b2world->DestroyBody(m_body);
        m_body = NULL;
    }
    //_____________


    DataSpline(const Vec2& pos, const Vec2& rect, bool isSensor) {
        m_not_spline = true;
        init(Vec2::ZERO,
             Vec2::ZERO,
             isSensor ?
             DSType_Water :
             DSType_Stataic, .0f, .0f, .0f, .0f);
        test_check = 1;
        createBody();

        if (isSensor) {
            b2Vec2 verts[4] = {
                b2Vec2(.0f, .0f),
                b2Vec2(.0f, rect.y),
                b2Vec2(rect.x, rect.y),
                b2Vec2(rect.x, .0f)};

            b2PolygonShape shape;
            shape.Set(verts, 4);

            b2FixtureDef fd;
            fd.shape = &shape;
            fd.restitution = .02f;
            //fd.filter = getFilter_b();
            fd.isSensor = isSensor;
            m_body->CreateFixture(&fd);
        }
        else {

#define CREATE_EDGE(V1,V2)                \
        {                                 \
                b2EdgeShape edge;         \
                edge.Set(V1, V2);         \
                b2FixtureDef fd;          \
                fd.shape = &edge;         \
                fd.restitution = .02f;     \
                fd.filter = getFilter_b();\
                fd.isSensor = isSensor;   \
                m_body->CreateFixture(&fd);}

            CREATE_EDGE(b2Vec2(.0f, .0f), b2Vec2(.0f, rect.y));
            CREATE_EDGE(b2Vec2(.0f, rect.y), b2Vec2(rect.x, rect.y));
            CREATE_EDGE(b2Vec2(rect.x, rect.y), b2Vec2(rect.x, .0f));
            CREATE_EDGE(b2Vec2(rect.x, .0f), b2Vec2(.0f, .0f));
        }

        m_body->SetTransform(to_b2Vec2(pos), .0f);

    }

    TSpline* getParentSpline() { return m_parentSpline; }

    DataSpline(TSpline* spline):
        m_parentSpline(spline) {
        init(Vec2::ZERO, spline->getControl()[0]->getPos(), DataSplineType::DSType_Stataic);
        createBody();
    }

    DataSpline(TSpline* spline, __Dictionary* dictInSplines, const Vec2& d_pos, const float& scale):
        m_parentSpline(spline) {
        load(dictInSplines, scale, d_pos);
        createBody();
        setInt();
    }

    DataSpline(TSpline* spline, __Dictionary* dictInSplines):
        m_parentSpline(spline) {
        load(dictInSplines);
        createBody();
    }

    void setEnablePhysics(bool enable) { m_enable_physics = enable; }
    void createBody() {
        if (!m_enable_physics)return;
        b2BodyDef bodyDef;
        //bodyDef.position.Set(0, 0); // bottom-left corner
                                    //bodyDef.allowSleep = false;
        switch (m_type) {
            case DSType_EndBox:
            case DSType_Stataic:bodyDef.type = b2_staticBody; break;

            case DSType_Bubble:
            case DSType_Dynamic:bodyDef.type = b2_dynamicBody;
                //centr in Vec2::Zero -=> pos change  relaticity centr
                //bodyDef.position = to_b2Vec2(m_pos);
                break;

            case DSType_Water:bodyDef.type = b2_staticBody; break;
        }
        setBody(m_b2world->CreateBody(&bodyDef));
        //m_body = m_b2world->CreateBody(&bodyDef);
        //m_body->SetUserData(this);
    }

    void setBody(b2Body* body) {
        m_body = body;
        m_body->SetUserData(this);
    }

    static void createShapes(std::vector<Vertex*> splines, b2Body* body, b2FixtureDef fd, const Vec2& centr);

    static b2Fixture* createFixture(b2Vec2* t_verts, size_t t_vI, b2FixtureDef fd, b2Body* m_body);

    static std::vector<std::vector<b2Vec2>> createFixtres(const b2Vec2& centr, const std::vector<Vertex*>& splines, const Vec2& d_pos = Vec2::ZERO);

    void create_soft_body(std::vector<Vertex*> splines);

    void createFixture(std::vector<Vertex*> splines);

    void createWhell(float radius);

    void destroy_physics() {
        if (NULL != m_body && NULL != m_b2world) {
            m_b2world->DestroyBody(m_body);
            m_body = NULL;
        }
    }

    virtual ~DataSpline() {
        if (m_not_spline)return;
        destroy_physics();
    }

    bool m_enableW = true;
    bool isEnableW() { return m_enableW; }

    void enableWater(bool val) {
        m_enableW = val;
    }

    DataSplineType getType() { return m_type; }
    void setType(DataSplineType type, bool isSetData = true) {
        m_type = type;
        if (m_type == DSType_Dynamic) {
            Vec2 t_c;
            //todo mass data center -> from visual center only
            if (NULL != m_parentSpline)
                t_c = m_parentSpline->getControl()[0]->getPos() + Vec2(20, 20);
            m_massData.center.Set(t_c.x, t_c.y);
        }
        if (isSetData)
            setDataInWorld();
    }

    float getDencity() { return m_dencity; }
    void setDencity(const float dencity) {
        m_dencity = dencity;
        setDataInWorld();
    }

    float getRestriction() { return m_restriction; }
    void setRestriction(const float restriction) {
        m_restriction = restriction;
        setDataInWorld();
    }

    b2MassData getMassData() { return m_massData; }
    void setMass(const float mass) {
        m_massData.mass = mass;
        setDataInWorld();
    }

    float getGScale() { return m_gscale; }
    void setGscale(float scale) {
        m_gscale = scale;
        setDataInWorld();
    }

    //not saved err centr

    Vec2 getWorldCenter() { return m_pos + getLocalCenter(); }

    Vec2 getLocalCenter() { return to_Vec2(m_massData.center); }

    void setSPos(const Vec2& pos) { m_pos = pos; }

    const Vec2& getSPos() { return m_pos; }

    void setLocalCentr(const Vec2& centr) {
        m_massData.center = to_b2Vec2(centr);
        setDataInWorld();
    }

    void save(__Dictionary* dictInSplines, const float& scale = 1.f, const Vec2& d_pos = Vec2::ZERO) {
        dictInSplines->setObject(__Integer::create(m_type), "spline_type");
        //dictInSplines->setObject(__Float::create(( m_massData.x - d_pos.x ) / scale), "spline_centr.x");
        //dictInSplines->setObject(__Float::create(( m_center.y - d_pos.y ) / scale), "spline_centr.y");
        dictInSplines->setObject(__Float::create(m_gscale), "spline_gscale");
        dictInSplines->setObject(__Float::create(m_massData.mass), "spline_mass");
        dictInSplines->setObject(__Float::create(m_dencity), "spline_dencity");
        dictInSplines->setObject(__Float::create(m_restriction), "spline_restriction");
    }

    void load(__Dictionary* dictInSplines, const float& scale = 1.f, const Vec2& d_pos = Vec2::ZERO) {
        float c_x = ( (__String*) dictInSplines->valueForKey("spline_centr.x") )->floatValue();
        float c_y = ( (__String*) dictInSplines->valueForKey("spline_centr.y") )->floatValue();
        int type = ( (__String*) dictInSplines->valueForKey("spline_type") )->intValue();
        float gscale = ( (__String*) dictInSplines->valueForKey("spline_gscale") )->floatValue();
        float mass = ( (__String*) dictInSplines->valueForKey("spline_mass") )->floatValue();
        float dencity = ( (__String*) dictInSplines->valueForKey("spline_dencity") )->floatValue();
        float restriction = ( (__String*) dictInSplines->valueForKey("spline_restriction") )->floatValue();
        //init(Vec2(c_x, c_y)*scale + d_pos, (DataSplineType) type, gscale, mass, dencity, restriction);
        init(Vec2(c_x, c_y)*scale, d_pos, (DataSplineType) type, gscale, mass, dencity, restriction);
    }

    //only debug editor
    void setDataInWorld(bool hasFixture = true) {
        if (NULL != m_body) {

            //m_body->SetTransform(to_b2Vec2(m_pos - getLocalCenter()), .0);
            m_body->SetTransform(to_b2Vec2(m_pos), .0);

            if (hasFixture) {
                b2Fixture* t_fixture = m_body->GetFixtureList();
                while (NULL != t_fixture) {
                    t_fixture->SetDensity(m_dencity);
                    t_fixture->SetSensor(hasSensor());
                    t_fixture->SetRestitution(m_restriction);

                    t_fixture->SetFilterData(m_filter);

                    t_fixture = t_fixture->GetNext();
                }
            }
            m_body->SetMassData(&m_massData);
            m_body->SetGravityScale(m_gscale);
        }
    }

    void init(const Vec2 & centr,
              const Vec2 & pos,
              DataSplineType type,
              float gscale = 1.,
              float mass = .0,
              float dencity = 1,
              float restriction = .0) {
        m_type = type;

        m_massData.center = to_b2Vec2(centr);
        m_pos = pos;

        m_massData.I = .0;
        m_massData.mass = mass;
        m_gscale = gscale;
        m_dencity = dencity;
        m_restriction = restriction;
    }

    bool hasSensor() {
        return m_type == DSType_EndBox || m_type == DSType_Water;
    }

};
