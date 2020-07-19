

#ifndef _MY_MENU_H_
#define _MY_MENU_H_



enum VAR_TYPE {
    UINT8, UINT16, UINT32, FLOAT,TYPE_MENU,TYPE_BAR
};

typedef struct {
    enum VAR_TYPE type;
    char name[10];
    char units [5];
    void *variable;
    char val_str[8];
} menu_item;

typedef struct {
    menu_item items[MENU_MAX_ITEMS];
    uint8_t num_of_items;
    uint8_t curr_pos;
    uint8_t curr_window_pos;
} MENU;



void menu_init(void);
void menu_add_item(MENU *m, menu_item item);
void create_bar_chars(void);
void increment_menu_position(MENU *m);
void decrement_menu_position(MENU *m);
uint8_t get_menu_position(MENU *m);
menu_item *get_p_to_item(MENU *m);
void menu_get_item_string(MENU *m, char *str, uint8_t n);
void menu_whole_string(MENU *m, char *s, STATE state);
void menu_create_item(menu_item *m, const char * name, enum VAR_TYPE t,const char *units, void *p_variable);
void menu_increment_item(MENU *m);
void menu_decrement_item(MENU *m);
void menu_swap(MENU **dest, MENU *src);
void menu_whole_solo_string(MENU *m, char *s);

#endif