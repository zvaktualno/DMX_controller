

#ifndef _MY_MENU_H_
#define _MY_MENU_H_

#define MENU_MAX_ITEMS 17

enum VAR_TYPE {
    TYPE_UINT8,  TYPE_UINT16,  TYPE_UINT32,  TYPE_FLOAT, TYPE_MENU, TYPE_BAR, TYPE_PRESET, TYPE_ENUM, TYPE_DMX_CH, TYPE_SAVE, TYPE_CONFIRM,
};

typedef struct {
    enum VAR_TYPE type;
    char name[11];
    char units [3];
    void *variable;
    char val_str[9];
    float val_min;
    float val_max;
    void (*getter)(char *dest, uint8_t mode);
} menu_item;

typedef struct {
    menu_item items[MENU_MAX_ITEMS];
    uint8_t num_of_items;
    uint8_t curr_pos;
    uint8_t curr_window_pos;
} MENU;



void menu_init(void);
void menu_add_item(MENU *m, menu_item item);
void increment_menu_position(MENU *m);
void decrement_menu_position(MENU *m);
uint8_t get_menu_position(MENU *m);
menu_item *get_p_to_item(MENU *m);
void menu_get_item_string(MENU *m, char *str, uint8_t n);
void menu_whole_string(MENU *m, char *s, STATE state);
void menu_create_item(menu_item *item, const char *short_name, enum VAR_TYPE typ, const char *units, void *p_variable, float min_val, float max_val, void *getter);
void menu_increment_item(MENU *m);
void menu_decrement_item(MENU *m);
void menu_swap(MENU **dest, MENU *src);
void menu_whole_solo_string(MENU *m, char *s);

#endif