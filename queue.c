#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list_sort.h"
#include "queue.h"
#include "sort.c"

/* Compare two elements based on their string values. */
int cmp(const struct list_head *a, const struct list_head *b)
{
    const element_t *ela = list_entry(a, element_t, list);
    const element_t *elb = list_entry(b, element_t, list);

    return strcmp(ela->value, elb->value);
}

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
    if (list_empty(head)) {
        free(head);
        return;
    }
    struct list_head *node, *safe;
    list_for_each_safe(node, safe, head) {
        element_t *current = list_entry(node, element_t, list);
        q_release_element(current);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_elem = malloc(sizeof(element_t));
    if (!new_elem)
        return false;
    new_elem->value = malloc(strlen(s) + 1);
    if (!new_elem->value) {
        q_release_element(new_elem);
        return false;
    }
    strlcpy(new_elem->value, s, strlen(s) + 1);
    list_add(&new_elem->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_elem = malloc(sizeof(element_t));
    if (!new_elem)
        return false;
    new_elem->value = malloc(strlen(s) + 1);
    if (!new_elem->value) {
        q_release_element(new_elem);
        return false;
    }
    strlcpy(new_elem->value, s, strlen(s) + 1);
    list_add_tail(&new_elem->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    struct list_head *rm_node = head->next;
    element_t *rm_elem = list_entry(rm_node, element_t, list);
    strlcpy(sp, rm_elem->value, bufsize);
    list_del(rm_node);
    return rm_elem;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    struct list_head *rm_node = head->prev;
    element_t *rm_elem = list_entry(rm_node, element_t, list);
    strlcpy(sp, rm_elem->value, bufsize);
    list_del(rm_node);
    return rm_elem;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each(li, head)
        len++;
    return len;
}

/* Find the middle node in queue */
struct list_head *q_find_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return NULL;
    struct list_head *access_next = head->next;
    struct list_head *access_prev = head->prev;
    while ((access_next != access_prev) && (access_prev->next != access_next)) {
        access_next = access_next->next;
        access_prev = access_prev->prev;
    }
    return access_next;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    struct list_head *mid = q_find_mid(head);
    list_del(mid);
    q_release_element(list_entry(mid, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;
    struct list_head *cur = head->next, *next, *node;
    while (cur != head) {
        int duplicated = 0;
        next = node = cur->next;
        while (node != head) {
            struct list_head *temp = node->next;
            if (strcmp(list_entry(cur, element_t, list)->value,
                       list_entry(node, element_t, list)->value) == 0) {
                duplicated = 1;
                list_del(node);
                q_release_element(list_entry(node, element_t, list));
            }
            node = temp;
        }
        if (duplicated) {
            next = cur->next;
            list_del(cur);
            q_release_element(list_entry(cur, element_t, list));
        }
        cur = next;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    q_reverseK(head, 2);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *curr, *safe;
    list_for_each_safe(curr, safe, head)
        list_move(curr, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || k == 1)
        return;
    if (k == q_size(head))
        q_reverse(head);
    struct list_head *curr = head->next, *safe, *dummy = head;
    int count = k;
    int group_count = q_size(head) / k;
    while (curr != dummy) {
        while (count > 0) {
            safe = curr->next;
            list_move(curr, dummy);
            curr = safe;
            count--;
        }
        group_count--;
        if (group_count == 0)
            break;
        dummy = curr->prev;
        curr = curr->next;
        count = k - 1;
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    list_sort(head, cmp);
    // list_merge_sort_iterative(head,cmp,q_size);
    if (descend)
        q_reverse(head);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *cur = head->next, *next, *node;
    while (cur != head) {
        int remove = 0;
        next = node = cur->next;
        while (node != head) {
            if (strcmp(list_entry(cur, element_t, list)->value,
                       list_entry(node, element_t, list)->value) > 0) {
                remove = 1;
                break;
            }
            node = node->next;
        }
        if (remove) {
            next = cur->next;
            list_del(cur);
            q_release_element(list_entry(cur, element_t, list));
        }
        cur = next;
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *cur = head->next, *next, *node;
    while (cur != head) {
        int remove = 0;
        next = node = cur->next;
        while (node != head) {
            if (strcmp(list_entry(cur, element_t, list)->value,
                       list_entry(node, element_t, list)->value) < 0) {
                remove = 1;
                break;
            }
            node = node->next;
        }
        if (remove) {
            next = cur->next;
            list_del(cur);
            q_release_element(list_entry(cur, element_t, list));
        }
        cur = next;
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head)
        return 0;
    if (head->next == head)
        return q_size(list_entry(head, queue_contex_t, chain)->q);
    struct list_head *queue = head->next;
    queue_contex_t *curr_1 = list_entry(queue, queue_contex_t, chain);
    while (queue->next != head) {
        queue_contex_t *curr_2 = list_entry(queue->next, queue_contex_t, chain);
        list_splice_tail_init(curr_2->q, curr_1->q);
        queue = queue->next;
    }
    curr_1->size = q_size(curr_1->q);
    q_sort(curr_1->q, descend);
    return curr_1->size;
}
