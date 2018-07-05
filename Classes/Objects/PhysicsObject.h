#pragma once

#include "stdafx.h"
#include "Soul.h"
#include "WorldSpline/WorldSpline_and_Physics.h"
#include "Effects/EffectRenderTexture.h"
#include "ExtendedNode/DrawNode2.h"

struct FromSpline {
    Vec2 _c1;
    Vec2 _c2;
    Vec2 _pos;
    FromSpline(const Vec2& pos, const Vec2& c1, const Vec2& c2):_pos(pos), _c1(c1), _c2(c2) {};
};

class PhysicsObject:public Soul {
private:
    DrawNode2* drawNode = NULL;
public:
    PhysicsObject(Layer* layer);
    virtual ~PhysicsObject();
    virtual SoulType getType() { return SoulType_PhysicsObject; };
    virtual void Load(SettingUtil* setting);
    virtual SettingUtil* Save();

    virtual bool mouseDown(const Vec2& mousePos);
    virtual bool mouseUp(const Vec2& mousePos);
    virtual bool mouseMovie(const Vec2& mousePos);

    virtual bool select(bool isParent = false);
    virtual void deselect();
    virtual void refresh();

    virtual void update(float delta);
    void updateWind(float delta);

    void drawRope(float delta);

    virtual void draw();

    virtual void setPos(const Vec2 & pos);

    void createPhysics(bool recreate);
    //-----------------------------------------------------------

    virtual void setSize(float width, float height) {
        Soul::setSize(width, height);
        _image->setScale(width / _image->getContentSize().width,
                         height / _image->getContentSize().height);
        _image_debug->setScale(_image->getScaleX(), _image->getScaleY());
    }

    virtual void setWidth(float width) {
        Soul::setWidth(width);
        setSize(_width, _height);
    }
    virtual void setHeight(float height) {
        Soul::setHeight(height);
        setSize(_width, _height);
    }

    virtual float getWidth() {
        return _image->getContentSize().width *  _image->getScaleX();
    }

    virtual float getHeight() {
        return _image->getContentSize().height * _image->getScaleY();
    }


    const float getScale() { return _scale; }

    void setScale(const float& scale);

    Vec2 getDPosCircle() { return _d_pos_circle; }
    void setDPosCircle();

    virtual void setZ(int val);


    DataSpline* getDataSpline() {
        if (_spline->getPolygons().empty()) return NULL;
        return TO_DATA_SPLINE(_spline->getPolygons()[0]->getData());
    }

    WorldSpline* getSpline() { return _spline; }

    void drawFromPhysics();

    void setParam();
    void setBubble(const std::string& let = "Рђ", const Color4B& color_let = Color4B(16, 200, 146, 255), const std::string& image_name = "0CFB8F");

    typedef std::function<void(PhysicsObject* obj)> on_press_release;
    void active_to_button(const on_press_release& callback);

    void setEndBox(PhysicsObject* let);
    void toEnd(PhysicsObject* end_box, bool success);

    void setTemplate(bool hasActive, float delay);

    Sprite* getImage() { return _image; }

    bool hasDel() { return _hasTemp && _stop_physics; }

    bool hasCheckOn() { return _active_check_on; }
    void setAStopPhysics(bool stop);

    void check_action_physics(b2Body* body);

    bool hasSelect() { return _has_select; }
    PhysicsObject* getLetObj() { return _let_obj; }

    void play_anim_hide(float max_delay);

    void play_anim_prev() { play_anim_translate(true); }
    void play_anim_next() { play_anim_translate(false); }
    void play_anim_translate(bool left);

    void play_anim(float delay, bool start);
    bool hasFinish() { return _has_finish; }


    const std::string& getLet() { return _let; }

    bool hasTemp() { return _hasTemp; }
protected:
    //__________________________
    //все переменные если callback не NULL
    Vec2 s_def;
    bool _has_press;
    bool _join_len_activate;
    on_press_release _callback_press;
    //________________________________
    bool _has_playing_anim;
    bool _has_finish;
    bool _has_select;

    EffectRenderTexture* _rt_rope;
    Sprite* _rt_rope_Sprite;
    float _rt_scale;

    Vec2 _rt_size;
    Vec2 _d_pos_rt;
    //___________
    bool _active_to_stop;
    bool _active_check_on;
    float _template_delay;
    bool _hasTemp;

    bool _enable_wind;
    float _delay_g = 2.f;
    float _max_d_g = .15f;
    float delay_change_gravity;
    float d_gravity;

    FromSpline drawSmootLine(const Vec2& p0, const Vec2& curr, const Vec2& n0);
    float _smoot_percent = .5f;

    //todo 
    bool _stop_physics;
    bool _recreate_physics;

    PhysicsObject* _let_obj;
    std::list<b2Body*> _body_rope;
    std::list<b2Joint*> _join_rope;

    std::string _let;
    Color4B _color_let;

    float _circle_r;
    Vec2 _d_pos_circle;

    float _scale;
    Sprite* _image;
    Sprite* _image_circle;
    Label* _node_text;
    Sprite* _image_debug;
    WorldSpline_and_Physics* _spline;
    bool _spline_down;

    bool _pos_spline;

    Vec2 _pos_start_unit;
    int _count_unit;
    float _d_unit;
    Vec2 _size_unit;

    float _density_unit;
    float _friction_unit;
    float _d_angle_unit;

    float _frequencyHz;
    float _dampingRatio;

	DrawNode* draw_rope;


    void setParamUnit(const Vec2& pos, int count, float d, const Vec2& size, bool recreate = false);
};
