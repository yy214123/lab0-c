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
    if (!head)
        return false;
    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;

    new->value = strdup(s);
    if (!new->value) {
        free(new);
        new = NULL;
        return false;
    }
    list_add(&new->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;

    new->value = strdup(s);
    if (!new->value) {
        free(new);
        new = NULL;
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

    element_t *now, *safe_now;
    bool dup = false;
    list_for_each_entry_safe (now, safe_now, head, list) {
        if (now->list.next != head &&
            strcmp(now->value, safe_now->value) == 0) {
            list_del(&now->list);
            q_release_element(now);
            dup = true;
        } else if (dup) {
            list_del(&now->list);
            q_release_element(now);
            dup = false;
        }
    }

    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }
    struct list_head *node = head->next;
    struct list_head *tmp = head;

    while (node != head && node->next != head) {
        struct list_head *next_node = node->next;
        list_move(next_node, tmp);
        tmp = node;
        node = node->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    }
    struct list_head *current = head->next;
    while (current != head) {
        struct list_head *tmp = current->next;
        list_move(current, head);
        current = tmp;
    }
}

/* Reverse the nodes of the list k at a time */
struct list_head *sub_q_reverse(struct list_head *head, int k)
{
    struct list_head *current = head->next;
    while (k > 0) {
        struct list_head *tmp = current->next;
        list_move(current, head);
        current = tmp;
        k--;
    }
    return current->prev;
}
void q_reverseK(struct list_head *head, int k)
{
    int count = (q_size(head)) / k;
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || list_is_singular(head) || k < 2)
        return;
    struct list_head *tmp = head;
    while (count > 0) {
        tmp = sub_q_reverse(tmp, k);
        count--;
    }
}
static int q_merge_two(struct list_head *first,
                       struct list_head *second,
                       bool descend)
{
    if (!first || !second)
        return 0;

    int count = 0;
    LIST_HEAD(tmp);
    while (!list_empty(first) && !list_empty(second)) {
        element_t *f = list_first_entry(first, element_t, list);
        element_t *s = list_first_entry(second, element_t, list);
        int cmp = strcmp(f->value, s->value);
        if (descend)
            cmp = -cmp;
        if (cmp <= 0)
            list_move_tail(&f->list, &tmp);
        else
            list_move_tail(&s->list, &tmp);
        count++;
    }
    count += q_size(first) + q_size(second);
    list_splice(&tmp, first);
    list_splice_tail_init(second, first);

    return count;
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head, bool descend)
{
    /* Try to use merge sort*/
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    /* Find middle point */
    struct list_head *mid, *left, *right;
    left = right = head;
    do {
        left = left->next;
        right = right->prev;
    } while (left != right && left->next != right);
    mid = left;

    /* Divide into two part */
    LIST_HEAD(second);
    list_cut_position(&second, mid, head->prev);

    /* Conquer */
    q_sort(head, descend);
    q_sort(&second, descend);

    /* Merge */
    q_merge_two(head, &second, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head) || list_is_singular(head))
        return 0;

    struct list_head *pos = head->prev;
    element_t *entry = list_entry(pos, element_t, list);
    char *min = entry->value;
    int count = 1;
    pos = pos->prev;

    while (pos != head) {
        entry = list_entry(pos, element_t, list);
        if (strcmp(entry->value, min) <= 0) {
            min = entry->value;
            count++;
        } else {
            struct list_head *tmp = pos->prev;
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
    char *max = entry->value;
    int count = 1;
    pos = pos->prev;

    while (pos != head) {
        entry = list_entry(pos, element_t, list);
        if (strcmp(entry->value, max) >= 0) {
            max = entry->value;
            count++;
        } else {
            struct list_head *tmp = pos->prev;
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
    if (!head || list_empty(head)) {
        return 0;
    }
    queue_contex_t *first_ctx = list_entry(head->next, queue_contex_t, chain);
    if (list_is_singular(head))
        return first_ctx->size;
    queue_contex_t *ctx = NULL;
    list_for_each_entry (ctx, head, chain) {
        if (ctx == first_ctx)
            continue;
        list_splice_tail_init(ctx->q, first_ctx->q);
    }
    first_ctx->size = q_size(first_ctx->q);
    q_sort(first_ctx->q, descend);
    return first_ctx->size;
}
