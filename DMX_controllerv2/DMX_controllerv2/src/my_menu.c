#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "config.h"
#include "my_menu.h"
//#include "functions.h"


void menu_add_item(MENU *m, menu_item item)
{
    if(m->num_of_items == MENU_MAX_ITEMS)
        return;
    m->items[m->num_of_items] = item;
    m->num_of_items++;
    m->curr_pos = 0;
    m->curr_window_pos = 0;
}

void increment_menu_position(MENU *m)
{
    if(m->curr_pos < m->num_of_items - 1)
        m->curr_pos++;
    if((m->curr_pos - m->curr_window_pos) == 4)
        m->curr_window_pos++;
}

void decrement_menu_position(MENU *m)
{
    if(m->curr_pos > 0)
        m->curr_pos--;
    if(m->curr_pos < m->curr_window_pos)
        m->curr_window_pos--;
}

uint8_t get_menu_position(MENU *m)
{
    return m->curr_pos;
}

menu_item *get_p_to_item(MENU *m)
{
    return &(m->items[m->curr_pos]);
}

void ftoe(char *s, float val)
{
    uint32_t whole = (int) val;
    float fract = val-whole;
    uint32_t whole_fract= fract*1000;
    //ne dela v primeru 4.05
    sprintf(s, "%3lu.%03lu", whole,whole_fract);
}

void menu_get_item_string(MENU *m, char *str, uint8_t n)
{
    if(n >= m->num_of_items)
        return;
    menu_item *tmp = m->items + n;
    switch(tmp->type) {
        case UINT8:
            sprintf(tmp->val_str, "%6d", *(uint8_t *)tmp->variable);
            break;
        case UINT16:
            sprintf(tmp->val_str, "%6d", *(uint16_t *)tmp->variable);
            break;
        case UINT32:
            sprintf(tmp->val_str, "%6lu", *(uint32_t *)tmp->variable);
            break;
        case TYPE_MENU:
            sprintf(tmp->val_str, "       ");
            break;
        case FLOAT:
            ftoe(tmp->val_str, *(float *)tmp->variable);
            break;
        default:
            strcpy(tmp->val_str, "ERROR99");
            break;
    }
    *(tmp->val_str + 7) = 0;
    char test[21];
    if(n == m->curr_pos) {
        sprintf(test, "%c%s%s%s", '~', tmp->name, tmp->val_str + 1, tmp->units);
    }
    else
        sprintf(test, "%s%s%s", tmp->name, tmp->val_str, tmp->units);
    memcpy(str, test, 20);

}

void menu_whole_solo_string(MENU *m, char *s)
{
    char tmp_string[21];
    menu_item *p_to_item = get_p_to_item(m);
    menu_get_item_string(m, tmp_string, m->curr_pos);

    uint8_t string_length = strlen(p_to_item->name);
    uint8_t value_length = strlen(p_to_item->val_str);

    p_to_item->name[sizeof(p_to_item->name) - 1] = 0;
    for(uint8_t i = 0; i < 20; i++)
        tmp_string[i] = ' ';
    tmp_string[20] = 0;
    for(uint8_t i = 0; i < 4; i++)
        memcpy(s + 21 * i, tmp_string, 20);

    p_to_item->name[sizeof(p_to_item->name) - 1] = 0;
    p_to_item->val_str[sizeof(p_to_item->val_str) - 1] = 0;

    memcpy(s + 21, p_to_item->name, string_length);
    memcpy(s + 42, p_to_item->val_str, value_length);
    memcpy(s + 42 + value_length + 1, p_to_item->units, strlen(p_to_item->units));
}

void menu_whole_string(MENU *m, char *s, STATE state)
{
    if(state == EDIT) {
        menu_whole_solo_string(m, s);
    }
    else {
        uint8_t j = 0;
        for(uint8_t i = (m->curr_window_pos); i < (4 + m->curr_window_pos) && i < m->num_of_items; i++) {
            menu_get_item_string(m, s + 21 * (j++), i);
        }
        for(; j < 4; j++)
            for(uint8_t i = 0; i < 20; i++)
                *(s + 21 * j) = ' ';
    }

}

void menu_create_item(menu_item *item, const char *name, enum VAR_TYPE typ, const char *units, void *p_variable, float min_val, float max_val)
{
    strcpy(item->name, "         ");
    strcpy(item->name, name);
    strcpy(item->units, "    ");
    strcpy(item->units, units);
    strcpy(item->val_str,"       ");
    item->type = typ;
    item->variable = p_variable;
    item->val_max = max_val;
    item->val_min = min_val;
}

void menu_increment_item(MENU *m)
{
    switch(m->items[m->curr_pos].type) {
        case UINT8:
            if((*(uint8_t *)m->items[m->curr_pos].variable) < (m->items[m->curr_pos].val_max))
                (*(uint8_t *)m->items[m->curr_pos].variable)++;
            break;
        case UINT16:
            if((*(uint16_t *)m->items[m->curr_pos].variable) < (m->items[m->curr_pos].val_max))
                (*(uint16_t *)m->items[m->curr_pos].variable)++;
            break;
        case UINT32:
            if((*(uint32_t *)m->items[m->curr_pos].variable) < (m->items[m->curr_pos].val_max))
                (*(uint32_t *)m->items[m->curr_pos].variable)++;
            break;
        case FLOAT:
            if((*(float *)m->items[m->curr_pos].variable) < (m->items[m->curr_pos].val_max))
                (*(float *)m->items[m->curr_pos].variable) += 0.1;
            break;
        default:
            break;
    }
}
void menu_decrement_item(MENU *m)
{
    switch(m->items[m->curr_pos].type) {
        case UINT8:
            if((*(uint8_t *)m->items[m->curr_pos].variable) > (m->items[m->curr_pos].val_min))
                (*(uint8_t *)m->items[m->curr_pos].variable)--;
            break;
        case UINT16:
            if((*(uint16_t *)m->items[m->curr_pos].variable) > (m->items[m->curr_pos].val_min))
                (*(uint16_t *)m->items[m->curr_pos].variable)--;
            break;
        case UINT32:
            if((*(uint32_t *)m->items[m->curr_pos].variable) > (m->items[m->curr_pos].val_min))
                (*(uint32_t *)m->items[m->curr_pos].variable)--;
            break;
        case FLOAT:
            if((*(float *)m->items[m->curr_pos].variable) > (m->items[m->curr_pos].val_min))
                (*(float *)m->items[m->curr_pos].variable) -= 0.1;
            break;
        default:
            break;
    }
}
void menu_swap(MENU **dest, MENU *src)
{
    *dest = src;
}