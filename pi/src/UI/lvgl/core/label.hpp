#ifndef LVGL_LABEL_HPP
#define LVGL_LABEL_HPP

#include "lvgl.hpp"
#include "object.hpp"
#include <cstdarg>

namespace LVGL
{

class Label : public Object
{
    public:
        /**
         * Basic constructor
         */
        Label(lv_obj_t& parent) { lv_obj = lv_label_create(&parent); }
        
        /**
         * Set a new text for a label. Memory will be allocated to store the text by the label.
         * @param text          '\0' terminated character string. NULL to refresh with the current text.
         */
        Label& SetText(const char* text) noexcept { lv_label_set_text(lv_obj, text); return *this; }
        
        /**
         * Set a new formatted text for a label. Memory will be allocated to store the text by the label.
         * @param fmt           `printf`-like format
         * @example lv_label_set_text_fmt(label1, "%d user", user_num);
         */
        template<class... args_list> Label& SetText(const char* fmt, args_list&&... args) noexcept
            { lv_label_set_text_fmt(lv_obj, fmt, std::forward<args_list>(args)...); return *this; }
        
        /**
         * Set a static text. It will not be saved by the label so the 'text' variable
         * has to be 'alive' while the label exists.
         * @param label         pointer to a label object
         * @param text          pointer to a text. NULL to refresh with the current text.
         */
        Label& SetTextStatic(const char* text) { lv_label_set_text_static(lv_obj, text); return *this; }
        
        /**
         * Get the text of a label
         * @return          the text of the label
         */
        const char* GetText() const noexcept { return lv_label_get_text(lv_obj); }
        
        /**
         * Insert a text to a label. The label text can not be static.
         * @param pos       character index to insert. Expressed in character index and not byte index.
         *                  0: before first char. LV_LABEL_POS_LAST: after last char.
         * @param txt       pointer to the text to insert
         */
        Label& InsertText(uint32_t pos, const char* txt) noexcept { lv_label_ins_text(lv_obj, pos, txt); return *this; }

        /**
         * Delete characters from a label. The label text can not be static.
         * @param pos       character index from where to cut. Expressed in character index and not byte index.
         *                  0: start in from of the first character
         * @param cnt       number of characters to cut
         */
        Label& DeleteText(uint32_t pos, uint32_t cnt) noexcept { lv_label_cut_text(lv_obj, pos, cnt); return *this; }

        /**
         * Set the behavior of the label with longer text then the object size
         * @param long_mode     the new mode from 'lv_label_long_mode' enum.
         *                      In LV_LONG_WRAP/DOT/SCROLL/SCROLL_CIRC the size of the label should be set AFTER this function
         */
        Label& SetLongMode(lv_label_long_mode_t long_mode) noexcept { lv_label_set_long_mode(lv_obj, long_mode); return *this; }

        /**
         * Get the long mode of a label
         * @return          the current long mode
         */
        lv_label_long_mode_t GetLongMode() const noexcept { return lv_label_get_long_mode(lv_obj); }

        /**
         * Enable the recoloring by in-line commands
         * @param en            true: enable recoloring, false: disable
         * @example "This is a #ff0000 red# word"
         * @see EnableRecolor() DisableRecolor()
         */
        Label& SetRecolor(bool en) noexcept { lv_label_set_recolor(lv_obj, en); return *this; }

        /**
         * Enable the recoloring by in-line commands
         * @example "This is a #ff0000 red# word"
         * @see SetRecolor()
         */
        Label& EnableRecolor() noexcept { lv_label_set_recolor(lv_obj, true); return *this; }

        /**
         * Disable the recoloring by in-line commands
         * @see SetRecolor()
         */
        Label& DisableRecolor() noexcept { lv_label_set_recolor(lv_obj, false); return *this; }

        /**
         * Get the recoloring attribute
         * @return          true: recoloring is enabled, false: disable
         */
        bool GetRecolor() const noexcept { return lv_label_get_recolor(lv_obj); }

        /**
         * Set where text selection should start
         * @param index     character index from where selection should start. `LV_LABEL_TEXT_SELECTION_OFF` for no selection
         */
        Label& SetTextSelectionStart(uint32_t index) noexcept { lv_label_set_text_sel_start(lv_obj, index); return *this; }

        /**
         * @brief Get the selection start index.
         * @return          selection start index. `LV_LABEL_TEXT_SELECTION_OFF` if nothing is selected.
         */
        uint32_t GetTextSelectionStart() const noexcept { return lv_label_get_text_selection_start(lv_obj); }

        /**
         * Set where text selection should end
         * @param index     character index where selection should end.  `LV_LABEL_TEXT_SELECTION_OFF` for no selection
         */
        Label& SetTextSelectionEnd(uint32_t index) noexcept { lv_label_set_text_sel_end(lv_obj, index); return *this; }

        /**
         * @brief Get the selection end index.
         * @return          selection end index. `LV_LABEL_TXT_SEL_OFF` if nothing is selected.
         */
        uint32_t GetTextSelectionEnd() const noexcept { return lv_label_get_text_selection_end(lv_obj); }

        /**
         * Get the relative x and y coordinates of a letter
         * @param index     index of the character [0 ... text length - 1].
         *                  Expressed in character index, not byte index (different in UTF-8)
         */
        lv_point_t GetLetterPosition(uint32_t char_id) const noexcept
        {
            lv_point_t p;
            lv_label_get_letter_pos(lv_obj, char_id, &p);
            return p;
        }

        /**
         * Get the relative x and y coordinates of a letter
         * @param index     index of the character [0 ... text length - 1].
         *                  Expressed in character index, not byte index (different in UTF-8)
         * @param pos       store the result here (E.g. index = 0 gives 0;0 coordinates if the text if aligned to the left)
         */
        const Label& GetLetterPosition(uint32_t char_id, lv_point_t& pos) const noexcept { lv_label_get_letter_pos(lv_obj, char_id, &pos); return *this; }

        /**
         * Get the index of letter on a relative point of a label.
         * @param pos       pointer to point with coordinates on a the label
         * @return          The index of the letter on the 'pos_p' point (E.g. on 0;0 is the 0. letter if aligned to the left)
         *                  Expressed in character index and not byte index (different in UTF-8)
         */
        uint32_t GetLetterAt(lv_point_t& pos) const noexcept { return lv_label_get_letter_on(lv_obj, &pos); }
        
        /**
         * Check if a character is drawn under a point.
         * @param pos Point to check for character under
         * @return whether a character is drawn under the point
         */
        bool IsACharThere(lv_point_t& pos) const noexcept { return lv_label_is_char_under_pos(lv_obj, &pos); }
};

} /* namespace LVGL */

#endif /* LVGL_LABEL_HPP */
