#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *pos, *tmp;
    list_for_each_safe (pos, tmp, head) {
        element_t *current_pos = list_entry(pos, element_t, list);
        q_release_element(current_pos);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;

    new->value = strdup(s);
    if (!new->value) {
        free(new);
        return false;
    }
    list_add(&new->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;

    new->value = strdup(s);
    if (!new->value) {
        free(new);
        return false;
    }
    list_add_tail(&new->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *q_head = list_entry(head->next, element_t, list);
    if (sp) {
        strncpy(sp, q_head->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(head->next);
    return q_head;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *q_head = list_entry(head->prev, element_t, list);
    if (sp) {
        strncpy(sp, q_head->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(head->prev);
    return q_head;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    int mid = q_size(head) / 2;
    struct list_head *current = NULL;
    int i = 0;
    list_for_each (current, head) {
        if (i == mid)
            break;
        i++;
    }
    element_t *mid_node = list_entry(current, element_t, list);
    list_del(current);
    q_release_element(mid_node);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;
    struct list_head *node1, *node2;
    list_for_each_safe (node1, node2, head) {
        element_t *elem1 = list_entry(node1, element_t, list);
        element_t *elem2 = list_entry(node2, element_t, list);
        if (!strcmp(elem1->value, elem2->value)) {
            list_del(node1);
            q_release_element(elem1);
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    struct list_head *current = head->next;

    while (current != head && current->next != head) {
        struct list_head *next = current->next;
        struct list_head *next_next = next->next;
        next->prev = current->prev;
        current->prev->next = next;
        current->next = next_next;
        next_next->prev = current;
        current->prev = next;
        next->next = current;
        current = next_next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    struct list_head *curr = head;
    struct list_head *prev = NULL;
    struct list_head *next = NULL;

    do {
        next = curr->next;
        curr->next = prev;
        curr->prev = next;
        prev = curr;
        curr = next;
    } while (curr != head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head) || list_is_singular(head))
        return 0;

    struct list_head *pos = head->prev;
    element_t *entry = list_entry(pos, element_t, list);
    int max = int(entry->value);
    int count = 1;
    pos = pos->prev;

    while (pos != head) {
        entry = list_entry(pos, element_t, list);
        if (entry->value <= max) {
            count++;
        } else {
            struct list_head *tmp = head->prev;
            list_del(pos);
            q_release_element(entry);
            pos = tmp;
            continue;
        }
        pos = pos->prev;
    }

    return count;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head) || list_is_singular(head))
        return 0;

    struct list_head *pos = head->prev;
    element_t *entry = list_entry(pos, element_t, list);
    int max = int(entry->value);
    int count = 1;
    pos = pos->prev;

    while (pos != head) {
        entry = list_entry(pos, element_t, list);
        if (entry->value >= max) {
            count++;
        } else {
            struct list_head *tmp = head->prev;
            list_del(pos);
            q_release_element(entry);
            pos = tmp;
            continue;
        }
        pos = pos->prev;
    }

    return count;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
