#ifndef LVGL_OBJECT_HPP
#define LVGL_OBJECT_HPP

#include "lvgl.hpp"
#include <stdexcept>
#include <functional>
#include <iterator>

namespace LVGL
{

#ifndef LVGL_GET_WRAPPER_NOT_PARANOID
/**
 * This is a random number from random.org.  It has no meaning.
 */
#define LVGL_OBJECT_MAGIC_VALUE 0x1545d4b4747ee87a
#endif /* LVGL_GET_WRAPPER_NOT_PARANOID */

/**
 * C++ wrapper for lv_obj_t.
 * 
 * @note Some LVGL calls return lv_obj_t pointers which need to be translated
 * back to C++ objects.  To facilitate this, the user_data field is reserved for
 * use by this wrapper class.
 * @warning This wrapper attempts to detect when user_data does not point to a
 * valid C++ wrapper object, but C++ provides no way to guarantee this can
 * reliably be detected.  Just don't use user_data!
 * 
 * @todo void *lv_obj_get_group(const lv_obj_t *obj)
 * void lv_obj_allocate_spec_attr(lv_obj_t *obj)
 * bool lv_obj_check_type(const lv_obj_t *obj, const lv_obj_class_t *class_p)
 * bool lv_obj_has_class(const lv_obj_t *obj, const lv_obj_class_t *class_p)
 * const lv_obj_class_t *lv_obj_get_class(const lv_obj_t *obj)
 * static inline lv_coord_t lv_obj_dpx(const lv_obj_t *obj, lv_coord_t n)
 * lv_disp_t* GetDisplayNONRAW()
 */
class Object
{
    /* NOTA BENE: 
     *   The little bit of implementation code not found here is in lvgl.cpp. */

    public:
        ////////////////////////////////////////////////////////////////////////
        ////// CONSTRUCTION & DESTRUCTION //////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////

        Object(lv_obj_t& parent, bool automatic_duration = false)
        {
            lv_obj = lv_obj_create(&parent);
            if (!automatic_duration)
            {
                lv_obj_add_event_cb(lv_obj, &on_delete_handler, LV_EVENT_DELETE, this);
                delayed_delete = DeletionMode::HeapAllocated;
            }
            else
                delayed_delete = DeletionMode::AutomaticDuration;
        }

        /**
         * Attempts to get a C++ Object pointer given an lv_obj_t*.
         * @return nullptr if the lv_obj_t doesn't specify a C++ wrapper
         * @warning This is not 100 % reliable.
         */
        static Object* GetWrapper(lv_obj_t* obj)
#ifdef LVGL_GET_WRAPPER_NOT_PARANOID
        { return (Object*)lv_obj_get_user_data(obj); }
#else
        ;
#endif /* LVGL_GET_WRAPPER_NOT_PARANOID */
        
        /**
         * Immediate destructor.
         */
        virtual ~Object();
        
        /**
         * Delete an object after some delay
         * @note When LVGL deletes the underlying LVGL object, the C++ wrapper's
         * destructor will also run before LVGL deletes the LVGL object.
         * @note If you subsequently attempt to manually delete this object, or
         * the object's destructor runs due to exiting scope, the destructor
         * will throw an std::logic_error which will likely kill your program.
         * This is intentional.
         * @param delay_ms  time to wait before delete in milliseconds
         */
        void DeleteDelayed(uint32_t delay_ms)
        {
            delayed_delete = DeletionMode::HeapDeleting;
            lv_obj_del_delayed(lv_obj, delay_ms);
        }

        /**
         * Helper function for asynchronously deleting objects.
         * Useful for cases where you can't delete an object directly in an `LV_EVENT_DELETE` handler (i.e. parent).
         * @note When LVGL deletes the underlying LVGL object, the C++ wrapper's
         * destructor will also run before LVGL deletes the LVGL object.
         * @note If you subsequently attempt to manually delete this object, or
         * the object's destructor runs due to exiting scope, the destructor
         * will throw an std::logic_error which will likely kill your program.
         * This is intentional.
         * @see lv_async_call
         */
        void DeleteAsync()
        {
            delayed_delete = DeletionMode::HeapDeleting;
            lv_obj_del_async(lv_obj);
        }

        /**
         * Check if any object is still "alive".
         */
        bool IsValid() const noexcept { return lv_obj_is_valid(lv_obj); }

        /**
         * Allows passing Object* directly to LVGL C routines.
         */
        operator lv_obj_t*() const noexcept { return lv_obj; }


        ////////////////////////////////////////////////////////////////////////
        ////// FLAGS AND STATE /////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        
        /**
         * Sets one or more flags.
         */
        Object& AddFlag(lv_obj_flag_t flag) noexcept { lv_obj_add_flag(lv_obj, flag); return *this; }
        
        /**
         * Clears one or more flags.
         */
        Object& ClearFlag(lv_obj_flag_t flag) noexcept { lv_obj_clear_flag(lv_obj, flag); return *this; }
        
        /**
         * Check if a given flag or ALL the given flags are set on an object.
         * @note Wraps lv_obj_has_flag
         * @return true: all flags are set; false: not all flags are set
         */
        bool HasFlags(lv_obj_flag_t flag) const noexcept { return lv_obj_has_flag(lv_obj, flag); }
        
        /**
         * Check if a given flag or ANY of the flags are set on an object.
         * @note Wraps lv_obj_has_flag_any
         * @return true: at lest one flag flag is set; false: none of the flags
         * are set
         */
        bool HasFlag(lv_obj_flag_t flag) const noexcept { return lv_obj_has_flag_any(lv_obj, flag); }
        
        /**
         * Add one or more states to the object. The other state bits will
         * remain unchanged. If specified in the styles, transition animation
         * will be started from the previous state to the current.
         * @param state The states to add.
         */
        Object& AddState(lv_state_t state) noexcept { lv_obj_add_state(lv_obj, state); return *this; }
        
        /**
         * Remove one or more states to the object. The other state bits will
         * remain unchanged. If specified in the styles, transition animation
         * will be started from the previous state to the current.
         */
        Object& ClearState(lv_state_t state) noexcept { lv_obj_clear_state(lv_obj, state); return *this; }
        
        /**
         * Get the state of an object
         */
        lv_state_t GetState() const noexcept { return lv_obj_get_state(lv_obj); }
        
        /**
         * Check if the object is in a given state or not.
         * @param state a state or combination of states to check
         * @return true: obj is in state; false: obj is not in state
         */
        bool HasState(lv_state_t state) const noexcept { return lv_obj_has_state(lv_obj, state); }
        

        ////////////////////////////////////////////////////////////////////////
        ////// CONTAINER ///////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        
        /**
         * Move the parent of an object. The relative coordinates will be kept.
         *
         * @param parent pointer to the new parent
         */
        Object& SetParent(lv_obj_t* parent) noexcept { lv_obj_set_parent(lv_obj, parent); return *this; }
        
        /**
         * Swap the positions of two objects.
         * When used in listboxes, it can be used to sort the listbox items.
         * @param obj2  pointer to the second object
         */
        Object& Swap(lv_obj_t* obj2) noexcept { lv_obj_swap(lv_obj, obj2); return *this; }
        
        /**
         * moves the object to the given index in its parent.
         * When used in listboxes, it can be used to sort the listbox items.
         * @param index  new index in parent.
         * @note to move to the foreground: MoveToIndex(0)
         * @note to move forward (up): MoveToIndex(GetIndex() - 1)
         */
        Object& MoveToIndex(int32_t index) noexcept { lv_obj_move_to_index(lv_obj, index); return *this; }
        
        /**
         * Move the object to the foreground.
         * It will look like if it was created as the last child of its parent.
         * It also means it can cover any of the siblings.
         */
        Object& MoveToForeground() noexcept { lv_obj_move_foreground(lv_obj); return *this; }
        
        /**
         * Move the object to the background.
         * It will look like if it was created as the first child of its parent.
         * It also means any of the siblings can cover the object.
         */
        Object& MoveToBackground() noexcept { lv_obj_move_background(lv_obj); return *this; }

        /**
         * Get the C++ screen of an object
         * @return          pointer to the object's screen or nullptr if none or no C++ object
         */
        Object* GetScreen() const noexcept { return GetWrapper(lv_obj_get_screen(lv_obj)); }
        
        /**
         * Get the C++ screen of an object
         * @return          pointer to the object's screen
         */
        lv_obj_t* GetScreenRaw() const noexcept { return lv_obj_get_screen(lv_obj); }
        
        /**
         * Get the display of the object
         * @return          pointer to the object's display
         */
        lv_disp_t* GetDisplayRaw() const noexcept { return lv_obj_get_disp(lv_obj); }
        
        /**
         * Get the parent of an object
         * @return          the parent of the object. (nullptr if `obj` was a screen or no C++ object)
         */
        Object* GetParent() const noexcept { return GetWrapper(lv_obj_get_parent(lv_obj)); }
        
        /**
         * Get the parent of an object
         * @return          the parent of the object. (NULL if `obj` was a screen)
         */
        lv_obj_t* GetParentRaw() const noexcept { return lv_obj_get_parent(lv_obj); }
        
        /**
         * Get the child of an object by the child's index.
         * @param id        the index of the child.
         *                  0: the oldest (firstly created) child
         *                  1: the second oldest
         *                  child count-1: the youngest
         *                  -1: the youngest
         *                  -2: the second youngest
         * @return          pointer to the child or NULL if the index was invalid or no C++ object
         */
        Object* GetChild(int32_t id) const noexcept { return GetWrapper(lv_obj_get_child(lv_obj, id)); }
        
        /**
         * Get the child of an object by the child's index.
         * @param id        the index of the child.
         *                  0: the oldest (firstly created) child
         *                  1: the second oldest
         *                  child count-1: the youngest
         *                  -1: the youngest
         *                  -2: the second youngest
         * @return          pointer to the child or NULL if the index was invalid
         */
        lv_obj_t* GetChildRaw(int32_t id) const noexcept { return lv_obj_get_child(lv_obj, id); }
        
        /**
         * Get the number of children
         * @param obj       pointer to an object
         * @return          the number of children
         */
        uint32_t GetChildCount() const noexcept { return lv_obj_get_child_cnt(lv_obj); }
        
        /**
         * Get the index of a child.
         * @return          the child index of the object.
         *                  E.g. 0: the oldest (firstly created child)
         */
        uint32_t GetIndex() const noexcept { return lv_obj_get_index(lv_obj); }
        
        /**
         * Iterate through all children of any object.
         * @param cb            call this callback on the objects
         * @param user_data     pointer to any user related data (will be passed to `cb`)
         */
        Object& TreeWalk(lv_obj_tree_walk_cb_t cb, void* user_data) noexcept { lv_obj_tree_walk(lv_obj, cb, user_data); return *this; }
        
        /**
         * Iterate through all children of any object.
         * @param func          This is invoked for each object processed.
         *                      If no C++ object is attached, then the callback
         *                      is skipped and LV_OBJ_TREE_WALK_NEXT is assumed.
         */
        Object& TreeWalk(std::function<lv_obj_tree_walk_res_t(Object&)>& func) noexcept { lv_obj_tree_walk(lv_obj, &tree_walk_handler, &func); return *this; }
    
        
        ////////////////////////////////////////////////////////////////////////
        ////// EVENTS //////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////

        /**
         * A functor that is unbound to a specific Object.
         * @param target A reference to the object getting the event
         * @param original_target The original object that got the event
         *        (namely, a child object that bubbled an event up to its parent)
         *        This would be nullptr only if somebody was unkind enough to
         *        introduce an unwrapped object.                 
         * @param event_id The event ID code of the event happening.
         * @param param Some kind of arbitrary data associated with the specific event.
         */
        typedef std::function<void(Object& target, Object* original_target, lv_event_code_t event_id, void* param)> UnboundEventHandler;
        
        /**
         * Registers an event handler.
         * @param func Some kind of callable thing to be invoked
         * @param event_id an event code (e.g. `LV_EVENT_CLICKED`) on which the event should be called. `LV_EVENT_ALL` can be used the receive all the events.
         * @see std::mem_fn
         * @warning I'm not sure what happens if you try to register more than one event handler using the exact same functor.
         */
        struct _lv_event_dsc_t* AddEventHandler(UnboundEventHandler& func, lv_event_code_t event_id) noexcept
        {
            return lv_obj_add_event_cb(lv_obj, &generic_event_handler, event_id, &func);
        }
        
        /**
         * Registers an event handler.
         * @param func Some kind of callable thing to be invoked
         * @param event_id an event code (e.g. `LV_EVENT_CLICKED`) on which the event should be called. `LV_EVENT_ALL` can be used the receive all the events.
         * @see std::mem_fn
         * @warning I'm not sure what happens if you try to register more than one event handler using the exact same functor.
         */
        Object& OnEvent(UnboundEventHandler& func, lv_event_code_t event_id) noexcept { AddEventHandler(func, event_id); return *this; }

        /**
         * A functor that is unbound to a specific Object and in which the event ID is implied.
         * @param target A reference to the object getting the event
         * @param original_target The original object that got the event
         *        (namely, a child object that bubbled an event up to its parent)
         *        This would be nullptr only if somebody was unkind enough to
         *        introduce an unwrapped object.                 
         * @param param Some kind of arbitrary data associated with the specific event.
         */
        typedef std::function<void(Object& target, Object* original_target, void* param)> UnboundImpliedEventHandler;
        
        /**
         * Registers an event handler.
         * @param func Some kind of callable thing to be invoked
         * @param event_id an event code (e.g. `LV_EVENT_CLICKED`) on which the event should be called. `LV_EVENT_ALL` can be used the receive all the events.
         * @see std::mem_fn
         * @warning I'm not sure what happens if you try to register more than one event handler using the exact same functor.
         */
        struct _lv_event_dsc_t* AddEventHandler(UnboundImpliedEventHandler& func, lv_event_code_t event_id) noexcept
        {
            return lv_obj_add_event_cb(lv_obj, &generic_implied_event_handler, event_id, &func);
        }
        
        /**
         * Registers an event handler.
         * @param func Some kind of callable thing to be invoked
         * @param event_id an event code (e.g. `LV_EVENT_CLICKED`) on which the event should be called. `LV_EVENT_ALL` can be used the receive all the events.
         * @see std::mem_fn
         * @warning I'm not sure what happens if you try to register more than one event handler using the exact same functor.
         */
        Object& OnEvent(UnboundImpliedEventHandler& func, lv_event_code_t event_id) noexcept { AddEventHandler(func, event_id); return *this; }

        /**
         * A functor that is bound to a specific Object (i.e. the functor has
         * additional state such that the target Object parameter is already
         * implied)
         * @param original_target The original object that got the event
         *        (namely, a child object that bubbled an event up to its parent)
         *        This would be nullptr only if somebody was unkind enough to
         *        introduce an unwrapped object.                 
         * @param event_id The event ID code of the event happening.
         * @param param Some kind of arbitrary data associated with the specific event.
         */
        typedef std::function<void(Object* original_target, lv_event_code_t event_id, void* param)> BoundEventHandler;

        /**
         * Registers an event handler.
         * @param func Some kind of callable thing to be invoked
         * @param event_id an event code (e.g. `LV_EVENT_CLICKED`) on which the event should be called. `LV_EVENT_ALL` can be used the receive all the events.
         * @warning I'm not sure what happens if you try to register more than one event handler using the exact same functor.
         */
        struct _lv_event_dsc_t* AddEventHandler(BoundEventHandler& func, lv_event_code_t event_id) noexcept
        {
            return lv_obj_add_event_cb(lv_obj, &less_generic_event_handler, event_id, &func);
        }

        /**
         * Registers an event handler.
         * @param func Some kind of callable thing to be invoked
         * @param event_id an event code (e.g. `LV_EVENT_CLICKED`) on which the event should be called. `LV_EVENT_ALL` can be used the receive all the events.
         * @warning I'm not sure what happens if you try to register more than one event handler using the exact same functor.
         */
        Object& OnEvent(BoundEventHandler& func, lv_event_code_t event_id) noexcept { AddEventHandler(func, event_id); return *this; }
        
        /**
         * A functor that is bound to a specific Object (i.e. the functor has
         * additional state such that the target Object parameter is already
         * implied) and in which the event ID is implied.
         * @param original_target The original object that got the event
         *        (namely, a child object that bubbled an event up to its parent)
         *        This would be nullptr only if somebody was unkind enough to
         *        introduce an unwrapped object.                 
         * @param param Some kind of arbitrary data associated with the specific event.
         */
        typedef std::function<void(Object* original_target, void* param)> BoundImpliedEventHandler;

        /**
         * Registers an event handler.
         * @param func Some kind of callable thing to be invoked
         * @param event_id an event code (e.g. `LV_EVENT_CLICKED`) on which the event should be called. `LV_EVENT_ALL` can be used the receive all the events.
         * @warning I'm not sure what happens if you try to register more than one event handler using the exact same functor.
         */
        struct _lv_event_dsc_t* AddEventHandler(BoundImpliedEventHandler& func, lv_event_code_t event_id) noexcept
        {
            return lv_obj_add_event_cb(lv_obj, &less_generic_implied_event_handler, event_id, &func);
        }

        /**
         * Registers an event handler.
         * @param func Some kind of callable thing to be invoked
         * @param event_id an event code (e.g. `LV_EVENT_CLICKED`) on which the event should be called. `LV_EVENT_ALL` can be used the receive all the events.
         * @warning I'm not sure what happens if you try to register more than one event handler using the exact same functor.
         */
        Object& OnEvent(BoundImpliedEventHandler& func, lv_event_code_t event_id) noexcept { AddEventHandler(func, event_id); return *this; }

        /**
         * Remove an event handler function for an object.
         * @param event_cb  the event function to remove
         * @return          true if any event handlers were removed
         */
        bool RemoveEventHandler(UnboundEventHandler& func) noexcept { return lv_obj_remove_event_cb_with_user_data(lv_obj, &generic_event_handler, &func); }

        /**
         * Remove an event handler function for an object.
         * @param event_cb  the event function to remove
         */
        Object& OffEvent(UnboundEventHandler& func) noexcept { RemoveEventHandler(func); return *this; }

        /**
         * Remove an event handler function for an object.
         * @param event_cb  the event function to remove
         * @return          true if any event handlers were removed
         */
        bool RemoveEventHandler(UnboundImpliedEventHandler& func) noexcept { return lv_obj_remove_event_cb_with_user_data(lv_obj, &generic_implied_event_handler, &func); }

        /**
         * Remove an event handler function for an object.
         * @param event_cb  the event function to remove
         */
        Object& OffEvent(UnboundImpliedEventHandler& func) noexcept { RemoveEventHandler(func); return *this; }

        /**
         * Remove an event handler function for an object.
         * @param event_cb  the event function to remove
         * @return          true if any event handlers were removed
         */
        bool RemoveEventHandler(BoundEventHandler& func) noexcept { return lv_obj_remove_event_cb_with_user_data(lv_obj, &less_generic_event_handler, &func); }

        /**
         * Remove an event handler function for an object.
         * @param event_cb  the event function to remove
         */
        Object& OffEvent(BoundEventHandler& func) noexcept { RemoveEventHandler(func); return *this; }

        /**
         * Remove an event handler function for an object.
         * @param event_cb  the event function to remove
         * @return          true if any event handlers were removed
         */
        bool RemoveEventHandler(BoundImpliedEventHandler& func) noexcept { return lv_obj_remove_event_cb_with_user_data(lv_obj, &less_generic_implied_event_handler, &func); }

        /**
         * Remove an event handler function for an object.
         * @param event_cb  the event function to remove
         */
        Object& OffEvent(BoundImpliedEventHandler& func) noexcept { RemoveEventHandler(func); return *this; }

    protected:
        /**
         * Default constructor---use only by deriving classes!
         * This is for subclasses that use a different LVGL object creation
         * routine.
         * @warning Remember to initialize lv_obj!
         */
        Object(bool automatic_duration)
        {
            if (!automatic_duration)
                lv_obj_add_event_cb(lv_obj, &on_delete_handler, LV_EVENT_DELETE, this);
        }
        
        /**
         * Actual lv_obj_t
         */
        lv_obj_t* lv_obj;


        ////////////////////////////////////////////////////////////////////////
        ////// HOOKS ///////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////

    private:

        enum class DeletionMode : uint_fast8_t
        {
            AutomaticDuration,
            HeapAllocated,
            HeapDeleting,
            HeapDeleted
        };

#ifndef LVGL_GET_WRAPPER_NOT_PARANOID
        /**
         * Attempt to detect if a user_data pointer points to a valid Object.
         */
        volatile uint64_t magic = LVGL_OBJECT_MAGIC_VALUE;
#endif

        /**
         * Set when delayed deletion is requested.
         */
        DeletionMode delayed_delete;
        
        /**
         * Translate deletion to C++
         */
        static void on_delete_handler(lv_event_t* event);
        
        /**
         * Translate TreeWalk std::function<> to C.
         */
        static lv_obj_tree_walk_res_t tree_walk_handler(lv_obj_t* target, void* data);

        /**
         * Generic event handler; translates LVGL events back to C++.
         */
        static void generic_event_handler(lv_event_t* event);
        
        /**
         * Generic event handler; translates LVGL events back to C++.
         */
        static void generic_implied_event_handler(lv_event_t* event);

        /**
         * Generic event handler; translates LVGL events back to C++.
         */
        static void less_generic_event_handler(lv_event_t* event);

        /**
         * Generic event handler; translates LVGL events back to C++.
         */
        static void less_generic_implied_event_handler(lv_event_t* event);
};


/**
 * This is the standard top-level container.
 * 
 * @todo This just assumes single-display operation; for multiple displays,
 * the displays need additional code to set up the pointer to their default
 * screen.
 */
class Screen : public Object
{
    public:
        /**
         * Nothing special.
         */
        Screen(bool automatic_duration = false) : Object(automatic_duration) { lv_obj = lv_obj_create(nullptr); }
    
        friend class Global;
    
    private:
        /**
         * For use by Global to initialize the default screen.
         */
        Screen(lv_obj_t* actual) : Object(false) { lv_obj = actual; }
};

} /* namespace LVGL */

#endif /* LVGL_OBJECT_HPP */
