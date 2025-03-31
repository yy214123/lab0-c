#include "list.h"
#include "list_sort.h"

static struct list_head *merge_two_lists(struct list_head *l1,
                                         struct list_head *l2,
                                         list_cmp_func_t cmp)
{
    struct list_head dummy;
    struct list_head *tail = &dummy;
    dummy.next = NULL;
    while (l1 && l2) {
        if (cmp(l1, l2) <= 0) {
            tail->next = l1;
            l1->prev = tail;
            l1 = l1->next;
        } else {
            tail->next = l2;
            l2->prev = tail;
            l2 = l2->next;
        }
        tail = tail->next;
    }
    tail->next = (l1 ? l1 : l2);
    if (tail->next)
        tail->next->prev = tail;
    return dummy.next;
}

void list_merge_sort_iterative(struct list_head *head,
                               list_cmp_func_t cmp,
                               int (*q_size)(struct list_head *))
{
    if (list_empty(head) || list_is_singular(head))
        return;
    int n = q_size(head);
    struct list_head *first = head->next;
    first->prev = NULL;
    head->prev->next = NULL;

    int step = 1;
    while (step < n) {
        struct list_head dummy;
        dummy.next = NULL;
        struct list_head *tail = &dummy;
        struct list_head *curr = first;

        while (curr) {
            struct list_head *left = curr;
            int i;
            for (i = 1; i < step && curr->next; i++)
                curr = curr->next;
            struct list_head *right = curr->next;
            curr->next = NULL;
            curr = right;
            for (i = 1; i < step && curr && curr->next; i++)
                curr = curr->next;
            struct list_head *next = NULL;
            if (curr) {
                next = curr->next;
                curr->next = NULL;
            }
            struct list_head *merged = merge_two_lists(left, right, cmp);
            tail->next = merged;
            while (tail->next)
                tail = tail->next;
            curr = next;
        }
        first = dummy.next;
        step *= 2;
    }
    INIT_LIST_HEAD(head);
    struct list_head *iter = first;
    while (iter) {
        struct list_head *next = iter->next;
        list_add_tail(iter, head);
        iter = next;
    }
}