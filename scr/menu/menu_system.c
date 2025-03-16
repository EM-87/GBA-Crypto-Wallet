/**
 * @file menu_system.c
 * @brief Menu system implementation for GBA Cryptocurrency QR Code Generator
 * 
 * This file provides the implementation of the menu system, handling:
 * - Menu navigation and selection
 * - Text rendering
 * - Cursor animation
 * - Input processing
 * 
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

 #include "menu_system.h"
 #include "menu_sprite.h"
 
 // =====================================================================
 // VARIABLES GLOBALES
 // =====================================================================
 
 // Global menu system state
 MenuSystem g_menu;
 
 // Configuration variables
 bool fullscreen_enabled = false;
 int volume_level = 7;
 
 // =====================================================================
 // TEXT SYSTEM IMPLEMENTATION
 // =====================================================================
 
 // Forward declarations for internal text system functions
 static void text_init(TextLayerSystem *self);
 static void text_clear_area(TextLayerSystem *self, int x, int y, int width, int height);
 static void text_mark_dirty(TextLayerSystem *self, int x, int y, int width, int height);
 static void text_update_dirty_areas(TextLayerSystem *self);
 static void text_render_text(TextLayerSystem *self, const char *text, int x, int y, u16 color, TextStyleFlags style);
 static void text_render_text_aligned(TextLayerSystem *self, const char *text, int x, int y, int width, TextAlignment align, u16 color);
 static int text_get_text_width(TextLayerSystem *self, const char *text);
 static void text_set_color(TextLayerSystem *self, u16 color);
 static void text_set_style(TextLayerSystem *self, TextStyleFlags style);
 static void text_set_alignment(TextLayerSystem *self, TextAlignment alignment);
 
 // Forward declarations for other functions
 static void menu_process_input(MenuSystem *menu);
 static void menu_update_cursor_position(MenuSystem *menu);
 static void menu_animate_cursor(MenuSystem *menu);
 
 // Global text system instance
 static TextLayerSystem text_system = {
     .x = 0,
     .y = 0,
     .width = SCREEN_WIDTH,
     .height = SCREEN_HEIGHT,
     .scroll_x = 0,
     .scroll_y = 0,
     .needs_full_update = true,
     .current_color = CLR_WHITE,
     .current_style = TEXT_STYLE_NORMAL,
     .current_alignment = TEXT_ALIGN_LEFT,
     .init = text_init,
     .clear_area = text_clear_area,
     .mark_dirty = text_mark_dirty,
     .update_dirty_areas = text_update_dirty_areas,
     .render_text = text_render_text,
     .render_text_aligned = text_render_text_aligned,
     .get_text_width = text_get_text_width,
     .set_color = text_set_color,
     .set_style = text_set_style,
     .set_alignment = text_set_alignment
 };
 
 /**
  * @brief Initialize the text system
  */
 static void text_init(TextLayerSystem *self) {
     // Initialize Tonc Text Engine
     tte_init_se(TEXT_LAYER_BG, 
                 BG_CBB(TEXT_CHAR_BLOCK) | BG_SBB(TEXT_SCREEN_BLOCK), 
                 TEXT_PALETTE_BANK, 
                 CLR_WHITE, 0, NULL, NULL);
     tte_init_con();
     
     // Store pointers to character and map bases
     self->char_base = (u16*)MEM_VRAM_ADDR(TEXT_CHAR_BLOCK * 0x4000);
     self->map_base = (u16*)MEM_VRAM_ADDR(0x800 * TEXT_SCREEN_BLOCK);
     
     // Initialize dirty areas
     memset(self->dirty_areas, 0, sizeof(self->dirty_areas));
 }
 
 /**
  * @brief Clear a specific text area
  */
 static void text_clear_area(TextLayerSystem *self, int x, int y, int width, int height) {
     // Save current margins
     int old_left, old_top, old_right, old_bottom;
     tte_get_margins(&old_left, &old_top, &old_right, &old_bottom);
     
     // Set new margins for the area to clear
     tte_set_margins(x, y, x + width, y + height);
     
     // Clear the area with spaces
     int num_chars = (width / 8) + 1;
     if (num_chars > MAX_TEXT_BUFFER - 1) {
         num_chars = MAX_TEXT_BUFFER - 1;
     }
     
     char spaces[MAX_TEXT_BUFFER];
     memset(spaces, ' ', num_chars);
     spaces[num_chars] = '\0';
     
     int num_lines = (height / 8) + 1;
     for (int i = 0; i < num_lines; i++) {
         tte_set_pos(x, y + i * 8);
         tte_write(spaces);
     }
     
     // Restore original margins
     tte_set_margins(old_left, old_top, old_right, old_bottom);
     
     // Mark the area as dirty
     text_mark_dirty(self, x, y, width, height);
 }
 
 /**
  * @brief Mark a text area as dirty for updating
  */
 static void text_mark_dirty(TextLayerSystem *self, int x, int y, int width, int height) {
     int startTileX = menu_clamp(x / 8, 0, SCREEN_WIDTH / 8 - 1);
     int startTileY = menu_clamp(y / 8, 0, SCREEN_HEIGHT / 8 - 1);
     int endTileX = menu_clamp((x + width - 1) / 8, 0, SCREEN_WIDTH / 8 - 1);
     int endTileY = menu_clamp((y + height - 1) / 8, 0, SCREEN_HEIGHT / 8 - 1);
     
     // Mark tiles as dirty
     for (int ty = startTileY; ty <= endTileY; ty++) {
         for (int tx = startTileX; tx <= endTileX; tx++) {
             self->dirty_areas[ty][tx] = true;
         }
     }
 }
 
 /**
  * @brief Update all areas marked as dirty
  */
 static void text_update_dirty_areas(TextLayerSystem *self) {
     // This is a simplified implementation. In a real system,
     // only the tiles marked as dirty would be updated in VRAM.
     
     // If a full update is needed, redefine the character map
     if (self->needs_full_update) {
         // Reimport the full map if needed
         self->needs_full_update = false;
         memset(self->dirty_areas, 0, sizeof(self->dirty_areas));
         return;
     }
     
     // Go through the tile map and update only the dirty ones
     for (int y = 0; y < SCREEN_HEIGHT / 8; y++) {
         for (int x = 0; x < SCREEN_WIDTH / 8; x++) {
             if (self->dirty_areas[y][x]) {
                 // In a real system, here we would update the specific tile in VRAM
                 self->dirty_areas[y][x] = false;
             }
         }
     }
 }
 
 /**
  * @brief Render text with specific style
  */
 static void text_render_text(TextLayerSystem *self, const char *text, int x, int y, u16 color, TextStyleFlags style) {
     if (!text) return;
     
     // Save current configuration
     u16 old_color = tte_get_ink();
     tte_set_ink(color);
     
     // Basic style implementation
     if (style & TEXT_STYLE_SHADOW) {
         // Shadow
         tte_set_ink(CLR_BLACK);
         tte_set_pos(x + 1, y + 1);
         tte_write(text);
         tte_set_ink(color);
     }
     
     if (style & TEXT_STYLE_BOLD) {
         // Simulate bold by duplicating text with a small offset
         tte_set_pos(x, y);
         tte_write(text);
         tte_set_pos(x + 1, y);
         tte_write(text);
     } else {
         // Normal style
         tte_set_pos(x, y);
         tte_write(text);
     }
     
     // Underline (simple implementation)
     if (style & TEXT_STYLE_UNDERLINE) {
         int text_width = text_get_text_width(self, text);
         int underline_y = y + 8; // Adjust according to font size
         
         // Draw underline
         for (int i = 0; i < text_width; i++) {
             tte_plot(x + i, underline_y, color);
         }
     }
     
     // Restore original color
     tte_set_ink(old_color);
     
     // Mark the area as dirty
     int text_width = text_get_text_width(self, text);
     text_mark_dirty(self, x, y, text_width, 10); // Approximate height to include underline
 }
 
 /**
  * @brief Render text with specific alignment
  */
 static void text_render_text_aligned(TextLayerSystem *self, const char *text, int x, int y, int width, TextAlignment align, u16 color) {
     if (!text) return;
     
     int text_width = text_get_text_width(self, text);
     int pos_x = x;
     
     switch (align) {
         case TEXT_ALIGN_CENTER:
             pos_x = x + (width - text_width) / 2;
             if (pos_x < x) pos_x = x;
             break;
             
         case TEXT_ALIGN_RIGHT:
             pos_x = x + width - text_width;
             if (pos_x < x) pos_x = x;
             break;
             
         case TEXT_ALIGN_LEFT:
         default:
             pos_x = x;
             break;
     }
     
     // Render the text at the calculated position
     text_render_text(self, text, pos_x, y, color, self->current_style);
 }
 
 /**
  * @brief Get the approximate width of text
  */
 static int text_get_text_width(TextLayerSystem *self, const char *text) {
     if (!text) return 0;
     
     // In this simplified implementation, we assume a monospaced 8-pixel font
     // In a real system, we would calculate the width based on variable font
     return strlen(text) * 8;
 }
 
 /**
  * @brief Set the current text color
  */
 static void text_set_color(TextLayerSystem *self, u16 color) {
     self->current_color = color;
 }
 
 /**
  * @brief Set the current text style
  */
 static void text_set_style(TextLayerSystem *self, TextStyleFlags style) {
     self->current_style = style;
 }
 
 /**
  * @brief Set the current text alignment
  */
 static void text_set_alignment(TextLayerSystem *self, TextAlignment alignment) {
     self->current_alignment = alignment;
 }
 
 /**
  * @brief Helper function to clamp a value to a range
  */
 int menu_clamp(int value, int min, int max) {
     if (value < min) return min;
     if (value > max) return max;
     return value;
 }
 
 // =====================================================================
 // MENU SYSTEM IMPLEMENTATION
 // =====================================================================
 
 /**
  * @brief Get the global menu system instance
  */
 MenuSystem* menu_system_get_instance() {
     return &g_menu;
 }
 
 /**
  * @brief Initialize the menu system
  */
 void menu_system_init(MenuSystem *menu) {
     if (!menu) return;
     
     // Initialize menu system variables
     menu->current_menu = NULL;
     menu->cursor_position = 0;
     menu->cursor_x = 0;
     menu->cursor_y = 0;
     menu->cursor_target_x = 0;
     menu->cursor_target_y = 0;
     menu->cursor_visible = true;
     menu->cursor_blink_counter = 0;
     menu->menu_stack_size = 0;
     
     // Initialize text system
     text_system.init(&text_system);
     
     // Initialize menu graphics
     menu_init_graphics();
 }
 
 /**
  * @brief Initialize the menu graphics
  */
 void menu_init_graphics() {
     // Configure video mode
     REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_OBJ | DCNT_OBJ_1D;
     
     // Initialize sprite system for cursor
     oam_init(obj_buffer, 128);
     
     // Load cursor sprite data
     memcpy(&tile_mem[4][0], cursor_sprite_data, sizeof(cursor_sprite_data));
     
     // Load cursor palette
     memcpy(pal_obj_mem, cursor_sprite_pal, sizeof(cursor_sprite_pal));
     
     // Initialize backgrounds for menu
     REG_BG0CNT = BG_CBB(0) | BG_SBB(30) | BG_4BPP | BG_REG_32x32 | BG_PRIO(2);
     REG_BG1CNT = BG_CBB(1) | BG_SBB(29) | BG_4BPP | BG_REG_32x32 | BG_PRIO(1);
 }
 
 /**
  * @brief Set the currently active menu
  */
 void menu_system_set_active_menu(MenuSystem *menu, MenuItem *item) {
     if (!menu || !item) return;
     
     // Save previous menu in the stack if menu changes
     if (menu->current_menu != item && menu->current_menu != NULL) {
         // Avoid stack overflow
         if (menu->menu_stack_size < MAX_MENU_STACK_SIZE) {
             menu->menu_stack[menu->menu_stack_size++] = menu->current_menu;
         }
     }
     
     // Set new active menu
     menu->current_menu = item;
     menu->cursor_position = 0;
     
     // Calculate initial cursor position
     if (item->num_options > 0) {
         menu->cursor_target_x = item->options[0].x - CURSOR_OFFSET_X;
         menu->cursor_target_y = item->options[0].y;
         menu->cursor_x = menu->cursor_target_x;
         menu->cursor_y = menu->cursor_target_y;
     }
 }
 
 /**
  * @brief Return to the previous menu in the stack
  */
 void menu_system_return_to_previous(MenuSystem *menu) {
     if (!menu || menu->menu_stack_size <= 0) return;
     
     // Retrieve previous menu from stack
     MenuItem *previous_menu = menu->menu_stack[--menu->menu_stack_size];
     
     // Set as active menu
     menu->current_menu = previous_menu;
     menu->cursor_position = 0;
     
     // Calculate initial cursor position
     if (previous_menu->num_options > 0) {
         menu->cursor_target_x = previous_menu->options[0].x - CURSOR_OFFSET_X;
         menu->cursor_target_y = previous_menu->options[0].y;
         menu->cursor_x = menu->cursor_target_x;
         menu->cursor_y = menu->cursor_target_y;
     }
 }
 
 /**
  * @brief Update the menu system logic
  */
 void menu_system_update(MenuSystem *menu) {
     if (!menu || !menu->current_menu) return;
     
     // Process user input
     menu_process_input(menu);
     
     // Update cursor position
     menu_update_cursor_position(menu);
     
     // Animate cursor
     menu_animate_cursor(menu);
 }
 
 /**
  * @brief Process user input for the menu
  */
 static void menu_process_input(MenuSystem *menu) {
     if (!menu || !menu->current_menu || menu->current_menu->num_options <= 0) return;
     
     // References to simplify code
     MenuItem *current_menu = menu->current_menu;
     int current_position = menu->cursor_position;
     
     // Vertical navigation (up/down)
     if (key_hit(KEY_UP)) {
         // Find previous enabled option
         int new_position = current_position;
         do {
             new_position = (new_position - 1 + current_menu->num_options) % current_menu->num_options;
             
             // Avoid infinite loop if no options are enabled
             if (new_position == current_position) break;
         } while (current_menu->options[new_position].type == MENU_ITEM_DISABLED);
         
         // Update cursor position
         menu->cursor_position = new_position;
         menu->cursor_target_x = current_menu->options[new_position].x - CURSOR_OFFSET_X;
         menu->cursor_target_y = current_menu->options[new_position].y;
     }
     else if (key_hit(KEY_DOWN)) {
         // Find next enabled option
         int new_position = current_position;
         do {
             new_position = (new_position + 1) % current_menu->num_options;
             
             // Avoid infinite loop if no options are enabled
             if (new_position == current_position) break;
         } while (current_menu->options[new_position].type == MENU_ITEM_DISABLED);
         
         // Update cursor position
         menu->cursor_position = new_position;
         menu->cursor_target_x = current_menu->options[new_position].x - CURSOR_OFFSET_X;
         menu->cursor_target_y = current_menu->options[new_position].y;
     }
     
     // Option selection (A button)
     if (key_hit(KEY_A)) {
         MenuOption *selected_option = &current_menu->options[current_position];
         
         switch (selected_option->type) {
             case MENU_ITEM_ACTION:
                 // Execute associated action
                 if (selected_option->action.callback) {
                     selected_option->action.callback(selected_option->action.user_data);
                 }
                 break;
                 
             case MENU_ITEM_SUBMENU:
                 // Change to submenu
                 if (selected_option->submenu.submenu) {
                     menu_system_set_active_menu(menu, selected_option->submenu.submenu);
                 }
                 break;
                 
             case MENU_ITEM_TOGGLE:
                 // Change boolean value
                 if (selected_option->toggle.value_ptr) {
                     *selected_option->toggle.value_ptr = !(*selected_option->toggle.value_ptr);
                     
                     // Execute callback if it exists
                     if (selected_option->toggle.on_change) {
                         selected_option->toggle.on_change(*selected_option->toggle.value_ptr);
                     }
                 }
                 break;
                 
             case MENU_ITEM_VALUE:
                 // Increment value
                 if (selected_option->value.value_ptr) {
                     *selected_option->value.value_ptr += selected_option->value.step;
                     
                     // Apply limits
                     if (*selected_option->value.value_ptr > selected_option->value.max_value) {
                         *selected_option->value.value_ptr = selected_option->value.min_value;
                     }
                     
                     // Execute callback if it exists
                     if (selected_option->value.on_change) {
                         selected_option->value.on_change(*selected_option->value.value_ptr);
                     }
                 }
                 break;
                 
             default:
                 break;
         }
     }
     
     // Horizontal navigation (left/right) for values
     if (current_menu->options[current_position].type == MENU_ITEM_VALUE) {
         MenuOption *selected_option = &current_menu->options[current_position];
         
         if (key_hit(KEY_LEFT)) {
             // Decrement value
             if (selected_option->value.value_ptr) {
                 *selected_option->value.value_ptr -= selected_option->value.step;
                 
                 // Apply limits
                 if (*selected_option->value.value_ptr < selected_option->value.min_value) {
                     *selected_option->value.value_ptr = selected_option->value.max_value;
                 }
                 
                 // Execute callback if it exists
                 if (selected_option->value.on_change) {
                     selected_option->value.on_change(*selected_option->value.value_ptr);
                 }
             }
         }
         else if (key_hit(KEY_RIGHT)) {
             // Increment value
             if (selected_option->value.value_ptr) {
                 *selected_option->value.value_ptr += selected_option->value.step;
                 
                 // Apply limits
                 if (*selected_option->value.value_ptr > selected_option->value.max_value) {
                     *selected_option->value.value_ptr = selected_option->value.min_value;
                 }
                 
                 // Execute callback if it exists
                 if (selected_option->value.on_change) {
                     selected_option->value.on_change(*selected_option->value.value_ptr);
                 }
             }
         }
     }
     
     // Return (B button)
     if (key_hit(KEY_B)) {
         // Special case: "Return" option with null callback is handled here
         if (current_position < current_menu->num_options && 
             current_menu->options[current_position].type == MENU_ITEM_ACTION &&
             current_menu->options[current_position].action.callback == NULL &&
             strcmp(current_menu->options[current_position].text, "Volver") == 0) {
             // If there's a parent menu defined, go to it
             if (current_menu->parent) {
                 menu_system_set_active_menu(menu, current_menu->parent);
             }
         }
         // Normal case: return to previous menu in stack
         else if (current_menu->parent) {
             menu_system_set_active_menu(menu, current_menu->parent);
         }
     }
 }
 
 /**
  * @brief Update cursor position with smooth animation
  */
 static void menu_update_cursor_position(MenuSystem *menu) {
     if (!menu) return;
     
     // Calculate new position with smooth movement
     menu->cursor_x += (menu->cursor_target_x - menu->cursor_x) / 4;
     menu->cursor_y += (menu->cursor_target_y - menu->cursor_y) / 4;
 }
 
 /**
  * @brief Animate the cursor (blinking)
  */
 static void menu_animate_cursor(MenuSystem *menu) {
     if (!menu) return;
     
     // Blink counter
     menu->cursor_blink_counter = (menu->cursor_blink_counter + 1) % CURSOR_BLINK_RATE;
     
     // Toggle visibility based on counter
     menu->cursor_visible = (menu->cursor_blink_counter < CURSOR_BLINK_RATE / 2);
 }
 
 /**
  * @brief Render the menu system
  */
 void menu_system_render(MenuSystem *menu) {
     if (!menu || !menu->current_menu) return;
     
     // Clear screen
     tte_erase_screen();
     
     // Draw menu title
     if (menu->current_menu->title) {
         text_system.render_text_aligned(&text_system, 
                                        menu->current_menu->title, 
                                        0, MENU_TITLE_Y, 
                                        SCREEN_WIDTH, 
                                        TEXT_ALIGN_CENTER, 
                                        RGB15(31,31,0));
     }
     
     // Draw menu options
     for (int i = 0; i < menu->current_menu->num_options; i++) {
         MenuOption *option = &menu->current_menu->options[i];
         
         // Select color based on state
         u16 color = RGB15(31,31,31); // Default color
         
         if (i == menu->cursor_position) {
             color = RGB15(31,31,0); // Yellow for selected
         }
         
         if (option->type == MENU_ITEM_DISABLED) {
             color = RGB15(15,15,15); // Gray for disabled
         }
         
         // Draw option text
         text_system.render_text(&text_system, 
                                option->text, 
                                option->x, option->y, 
                                color, 
                                TEXT_STYLE_NORMAL);
         
         // Special rendering for specific types
         switch (option->type) {
             case MENU_ITEM_TOGGLE:
                 // Show ON/OFF state
                 if (option->toggle.value_ptr) {
                     const char *state_text = *option->toggle.value_ptr ? "ON" : "OFF";
                     text_system.render_text(&text_system, 
                                            state_text, 
                                            option->x + option->width - MENU_VALUE_MARGIN, 
                                            option->y, 
                                            color, 
                                            TEXT_STYLE_NORMAL);
                 }
                 break;
                 
             case MENU_ITEM_VALUE:
                 // Show numeric value
                 if (option->value.value_ptr) {
                     char value_text[16];
                     sprintf(value_text, "%d", *option->value.value_ptr);
                     text_system.render_text(&text_system, 
                                            value_text, 
                                            option->x + option->width - MENU_VALUE_MARGIN, 
                                            option->y, 
                                            color, 
                                            TEXT_STYLE_NORMAL);
                 }
                 break;
                 
             default:
                 break;
         }
     }
     
     // Draw help text
     if (menu->current_menu->help_text) {
         text_system.render_text_aligned(&text_system, 
                                        menu->current_menu->help_text, 
                                        0, 
                                        SCREEN_HEIGHT - MENU_HELP_MARGIN, 
                                        SCREEN_WIDTH, 
                                        TEXT_ALIGN_CENTER, 
                                        RGB15(20,20,20));
     }
     
     // Update dirty areas of text system
     text_system.update_dirty_areas(&text_system);
     
     // Render cursor
     if (menu->cursor_visible && menu->current_menu->num_options > 0) {
         // Configure cursor sprite
         obj_set_attr(&obj_buffer[0], 
                     ATTR0_SQUARE | ATTR0_Y((int)menu->cursor_y), 
                     ATTR1_SIZE_16 | ATTR1_X((int)menu->cursor_x), 
                     ATTR2_PALBANK(0) | 0);
         
         // Update OAM
         oam_copy(oam_mem, obj_buffer, 1);
     } else {
         // Hide cursor
         obj_set_attr(&obj_buffer[0], 
                     ATTR0_HIDE, 
                     0, 
                     0);
         
         // Update OAM
         oam_copy(oam_mem, obj_buffer, 1);
     }
 }