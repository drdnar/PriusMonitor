#include "lvgl.hpp"
#include "object.hpp"

using namespace LVGL;

Global* Global::instance = nullptr;


Global& Global::GetInstance()
{
    if (instance)
        return *instance;
    else
        return *(instance = new Global());
}


Global::Global()
{
    lv_init();
    // TODO: This should really be handled by the display system.
    new Screen(lv_scr_act());
}


Global::~Global( )
{
    lv_deinit();
    delete instance;
}


////////////////////////////////////////////////////////////////////////////////
////// object.hpp stuff ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef LVGL_GET_WRAPPER_NOT_PARANOID
Object* Object::GetWrapper(lv_obj_t* obj)
{
    void* data = lv_obj_get_user_data(obj);
    if (!data)
        return nullptr;
    Object* object = static_cast<Object*>(data);
    if (object->magic != LVGL_OBJECT_MAGIC_VALUE)
        return nullptr;
    return object;
}
#endif /* LVGL_GET_WRAPPER_NOT_PARANOID */


Object::~Object()
{
    if (delayed_delete == DeletionMode::HeapDeleting)
#ifndef LVGL_IGNORE_DELAYED_DOUBLE_DELETE_ERROR
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wexceptions"
        throw std::logic_error("Destructing an LVGL::Object too soon! (Should you have used new instead of making it a local object?)");
#pragma GCC diagnostic pop
#else
        lv_obj_set_user_data(lv_obj, nullptr);
#endif /* LVGL_IGNORE_DELAYED_DOUBLE_DELETE */
    if (delayed_delete != DeletionMode::AutomaticDuration)
    {
        delayed_delete = DeletionMode::HeapDeleted;
        lv_obj_del(lv_obj);
    }
}


void Object::on_delete_handler(lv_event_t* event)
{
    auto& self = *(Object*)lv_event_get_user_data(event);
    if (self.delayed_delete == DeletionMode::HeapDeleting)
    {
        self.delayed_delete = DeletionMode::HeapDeleted;
        delete self;
    }
}


lv_obj_tree_walk_res_t Object::tree_walk_handler(lv_obj_t* target, void* data)
{
    Object* obj = GetWrapper(target);
    if (obj)
        return ((std::function<lv_obj_tree_walk_res_t(Object&)>&)data)(*obj);
    return LV_OBJ_TREE_WALK_NEXT;
}


void Object::generic_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    void* functor_ptr = lv_event_get_user_data(event);
    ((UnboundEventHandler&)functor_ptr)
    (
        *target, GetWrapper(lv_event_get_target(event)),
        lv_event_get_code(event), lv_event_get_param(event)
    );
}


void Object::generic_implied_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    void* functor_ptr = lv_event_get_user_data(event);
    ((UnboundImpliedEventHandler&)functor_ptr)
    (
        *target, GetWrapper(lv_event_get_target(event)), lv_event_get_param(event)
    );
}


void Object::less_generic_event_handler(lv_event_t* event)
{
    void* functor_ptr = lv_event_get_user_data(event);
    ((BoundEventHandler&)functor_ptr)
    (
        GetWrapper(lv_event_get_target(event)),
        lv_event_get_code(event), lv_event_get_param(event)
    );
}


void Object::less_generic_implied_event_handler(lv_event_t* event)
{
    void* functor_ptr = lv_event_get_user_data(event);
    ((BoundImpliedEventHandler&)functor_ptr)
    (
        GetWrapper(lv_event_get_target(event)),
        lv_event_get_param(event)
    );
}
