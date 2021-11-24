#include "object.hpp"

using namespace LVGL;

Object::Object(Object& parent)
{
    lv_obj = lv_obj_create(parent.lv_obj);
    lv_obj_add_event_cb(lv_obj, &on_delete_handler, LV_EVENT_DELETE, this);
    delayed_delete = DeletionMode::HeapAllocated;
    lv_obj_set_user_data(lv_obj, this);
}


Object::Object()
{
    lv_obj = lv_obj_create(lv_scr_act());
    lv_obj_add_event_cb(lv_obj, &on_delete_handler, LV_EVENT_DELETE, this);
    delayed_delete = DeletionMode::HeapAllocated;
    lv_obj_set_user_data(lv_obj, this);
}


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
    switch (delayed_delete)
    {
        case DeletionMode::HeapDeleting:
        case DeletionMode::HeapDeleted: // ? ? ?
#ifndef LVGL_IGNORE_DELAYED_DOUBLE_DELETE_ERROR
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wexceptions"
            throw std::logic_error("Destructing an LVGL::Object too soon! (Should you have used new instead of making it a local object?)");
#pragma GCC diagnostic pop
#else
            lv_obj_set_user_data(lv_obj, nullptr);
#endif /* LVGL_IGNORE_DELAYED_DOUBLE_DELETE */
            break;
        case DeletionMode::HeapAllocated:
            delayed_delete = DeletionMode::HeapDeleted;
            lv_obj_del(lv_obj);
            break;
        case DeletionMode::ManualDuration:
            // Do nothing
            break;
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
        return (*static_cast<std::function<lv_obj_tree_walk_res_t(Object&)> const*>(data))(*obj);
    return LV_OBJ_TREE_WALK_NEXT;
}


void Object::Refresh() noexcept
{
    TreeWalk(
        [](Object& target)
        {
            lv_event_send(target, LV_EVENT_REFRESH, nullptr);
            return LV_OBJ_TREE_WALK_NEXT;
        }
    );
}


#define LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(FUNCTOR_TYPE, CALLBACK) Object::EventHandlerID Object::AddEventHandler(FUNCTOR_TYPE func, lv_event_code_t event_id) noexcept\
{\
    EventHandlerID id = functors_list.size();\
    functors_list.push_back({CALLBACK, event_id, func, true});\
    lv_obj_add_event_cb(lv_obj, &CALLBACK, event_id, reinterpret_cast<void*>(id));\
    return id;\
}


LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(UnboundEventHandler, generic_event_handler)
LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(UnboundImpliedEventHandler, generic_implied_event_handler)
LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(UnboundOrphanEventHandler, generic_orphan_event_handler)
LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(UnboundOrphanImpliedEventHandler, generic_orphan_implied_event_handler)
LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(BoundEventHandler, less_generic_event_handler)
LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(BoundImpliedEventHandler, less_generic_implied_event_handler)
LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(BoundOrphanEventHandler, less_generic_orphan_event_handler)
LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(BoundOrphanImpliedEventHandler, less_generic_orphan_implied_event_handler)
LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(ParamlessUnboundEventHandler, generic_paramless_event_handler)
LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(ParamlessUnboundImpliedEventHandler, generic_paramless_implied_event_handler)
LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(ParamlessUnboundOrphanEventHandler, generic_paramless_orphan_event_handler)
LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(ParamlessUnboundOrphanImpliedEventHandler, generic_paramless_orphan_implied_event_handler)
LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(ParamlessBoundEventHandler, less_generic_paramless_event_handler)
LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(ParamlessBoundImpliedEventHandler, less_generic_paramless_implied_event_handler)
LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(ParamlessBoundOrphanEventHandler, less_generic_paramless_orphan_event_handler)
LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(ParamlessBoundOrphanImpliedEventHandler, less_generic_paramless_orphan_implied_event_handler)
LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(MemberEventHandler, member_event_handler)
LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(MemberImpliedEventHandler, member_implied_event_handler)
LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(MemberOrphanEventHandler, member_orphan_event_handler)
LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(MemberOrphanImpliedEventHandler, member_orphan_implied_event_handler)
LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(ParamlessMemberEventHandler, member_paramless_event_handler)
LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(ParamlessImpliedMemberEventHandler, member_paramless_implied_event_handler)
LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(ParamlessOrphanMemberEventHandler, member_paramless_orphan_event_handler)
LVGL_OBJECT_IMPLEMENT_ADD_EVENT_HANDLER(ParamlessOrphanImpliedMemberEventHandler, member_paramless_orphan_implied_event_handler)


void Object::generic_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    std::get<UnboundEventHandler>(
        target->functors_list[
            reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))
        ].handler)
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
    std::get<UnboundImpliedEventHandler>(
        target->functors_list[
            reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))
        ].handler)
    (
        *target, GetWrapper(lv_event_get_target(event)), 
        lv_event_get_param(event)
    );
}


void Object::generic_orphan_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    std::get<UnboundOrphanEventHandler>(
        target->functors_list[
            reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))
        ].handler)
    (*target, lv_event_get_code(event), lv_event_get_param(event));
}


void Object::generic_orphan_implied_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    std::get<UnboundOrphanImpliedEventHandler>(
        target->functors_list[
            reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))
        ].handler)
    (*target, lv_event_get_param(event));
}


void Object::less_generic_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    std::get<BoundEventHandler>(
        target->functors_list[
            reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))
        ].handler)
    (
        GetWrapper(lv_event_get_target(event)),
        lv_event_get_code(event), lv_event_get_param(event)
    );
}


void Object::less_generic_implied_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    std::get<BoundImpliedEventHandler>(
        target->functors_list[
            reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))
        ].handler)
    (GetWrapper(lv_event_get_target(event)), lv_event_get_param(event));
}


void Object::less_generic_orphan_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    std::get<BoundOrphanEventHandler>(
        target->functors_list[
            reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))
        ].handler)
    (lv_event_get_code(event), lv_event_get_param(event));
}


void Object::less_generic_orphan_implied_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    std::get<BoundOrphanImpliedEventHandler>(
        target->functors_list[
            reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))
        ].handler)
    (lv_event_get_param(event));
}


void Object::generic_paramless_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    std::get<ParamlessUnboundEventHandler>(
        target->functors_list[
            reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))
        ].handler)
    (
        *target, GetWrapper(lv_event_get_target(event)),
        lv_event_get_code(event)
    );
}


void Object::generic_paramless_implied_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    std::get<ParamlessUnboundImpliedEventHandler>(
        target->functors_list[
            reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))
        ].handler)
    (*target, GetWrapper(lv_event_get_target(event)));
}


void Object::generic_paramless_orphan_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    std::get<ParamlessUnboundOrphanEventHandler>(
        target->functors_list[
            reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))
        ].handler)
    (*target, lv_event_get_code(event));
}


void Object::generic_paramless_orphan_implied_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    std::get<ParamlessUnboundOrphanImpliedEventHandler>(
        target->functors_list[
            reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))
        ].handler)
    (*target);
}


void Object::less_generic_paramless_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    std::get<ParamlessBoundEventHandler>(
        target->functors_list[
            reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))
        ].handler)
    (
        GetWrapper(lv_event_get_target(event)),
        lv_event_get_code(event)
    );
}


void Object::less_generic_paramless_implied_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    std::get<ParamlessBoundImpliedEventHandler>(
        target->functors_list[
            reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))
        ].handler)
    (GetWrapper(lv_event_get_target(event)));
}


void Object::less_generic_paramless_orphan_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    std::get<ParamlessBoundOrphanEventHandler>(
        target->functors_list[
            reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))
        ].handler)
    (lv_event_get_code(event));
}


void Object::less_generic_paramless_orphan_implied_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    std::get<ParamlessBoundOrphanImpliedEventHandler>(
        target->functors_list[
            reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))
        ].handler)
    ();
}


void Object::member_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    std::invoke(
        std::get<MemberEventHandler>(
            target->functors_list[
                reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))
            ].handler),
        *target,
        GetWrapper(lv_event_get_target(event)), lv_event_get_code(event), lv_event_get_param(event)
    );
}


void Object::member_implied_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    std::invoke(
        std::get<MemberImpliedEventHandler>(
            target->functors_list[
                reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))
            ].handler),
        *target,
        GetWrapper(lv_event_get_target(event)), lv_event_get_param(event)
    );
}


void Object::member_orphan_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    std::invoke(
        std::get<MemberOrphanEventHandler>(
            target->functors_list[
                reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))
            ].handler),
        *target,
        lv_event_get_code(event), lv_event_get_param(event)
    );
}


void Object::member_orphan_implied_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    std::invoke(
        std::get<MemberOrphanImpliedEventHandler>(
            target->functors_list[
                reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))
            ].handler),
        *target,
        lv_event_get_param(event)
    );
}


void Object::member_paramless_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    std::invoke(
        std::get<ParamlessMemberEventHandler>(
            target->functors_list[
                reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))
            ].handler),
        *target,
        GetWrapper(lv_event_get_target(event)), lv_event_get_code(event)
    );
}


void Object::member_paramless_implied_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    std::invoke(
        std::get<ParamlessImpliedMemberEventHandler>(
            target->functors_list[
                reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))
            ].handler),
        *target,
        GetWrapper(lv_event_get_target(event))
    );
}


void Object::member_paramless_orphan_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    std::invoke(
        std::get<ParamlessOrphanMemberEventHandler>(
            target->functors_list[
                reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))
            ].handler),
        *target,
        lv_event_get_code(event)
    );
}


void Object::member_paramless_orphan_implied_event_handler(lv_event_t* event)
{
    Object* target = GetWrapper(lv_event_get_current_target(event));
    if (!target)
        return;
    std::invoke(
        std::get<ParamlessOrphanImpliedMemberEventHandler>(
            target->functors_list[
                reinterpret_cast<uintptr_t>(lv_event_get_user_data(event))
            ].handler),
        *target
        // No parameters
    );
}


#ifndef LVGL_GET_WRAPPER_NOT_PARANOID
Screen& Screen::Active()
{
    lv_obj_t* object = lv_scr_act();
    Object* screen = GetWrapper(object);
    if (screen != nullptr)
        return *((Screen*)screen);
    return *(new Screen(object));
}
#endif
