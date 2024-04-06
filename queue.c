#include "queue.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

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
    q_insert_head(head->prev, s);
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

struct list_head *get_midpoint(struct list_head *head)
{
    struct list_head *node1 = head->next, *node2 = head->prev;

    while (node1->next != node2 && node1 != node2) {
        node1 = node1->next;
        node2 = node2->prev;
    }

    return node2;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    struct list_head *current = get_midpoint(head);
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
    q_reverseK(head, 2);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    }
    // struct list_head *current = head->next;
    // while (current != head) {
    //     struct list_head *tmp = current->next;
    //     list_move(current, head);
    //     current = tmp;
    // }
    struct list_head *node = NULL;
    struct list_head *safe = NULL;
    list_for_each_safe (node, safe, head) {
        list_move(node, head);
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

static int q_merge_two(struct list_head *first, struct list_head *second)
{
    if (!first || !second)
        return 0;

    int count = 0;
    LIST_HEAD(tmp);
    while (!list_empty(first) && !list_empty(second)) {
        element_t *f = list_first_entry(first, element_t, list);
        element_t *s = list_first_entry(second, element_t, list);
        int _cmp = strcmp(f->value, s->value);
        if (_cmp <= 0)
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

void merge_sort(struct list_head *head)
{
    /* Try to use merge sort*/
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    /* Find middle point */
    struct list_head *mid;
    mid = get_midpoint(head);

    /* Divide into two part */
    LIST_HEAD(second);
    list_cut_position(&second, mid->prev, head->prev);

    /* Conquer */
    merge_sort(head);
    merge_sort(&second);

    /* Merge */
    q_merge_two(head, &second);
}

/* lib/list_sort.c */

static int cmp(const struct list_head *a, const struct list_head *b)
{
    element_t *ela = list_entry(a, element_t, list);
    element_t *elb = list_entry(b, element_t, list);

    return strcmp(ela->value, elb->value);
}

typedef int (*list_cmp_func_t)(const struct list_head *,
                               const struct list_head *);

static struct list_head *merge(list_cmp_func_t cmp,
                               struct list_head *a,
                               struct list_head *b)
{
    struct list_head *head = NULL, **tail = &head;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (cmp(a, b) <= 0) {
            *tail = a;
            tail = &a->next;
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            tail = &b->next;
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    return head;
}

static void merge_final(list_cmp_func_t cmp,
                        struct list_head *head,
                        struct list_head *a,
                        struct list_head *b)
{
    struct list_head *tail = head;
    uint8_t count = 0;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (cmp(a, b) <= 0) {
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;
            if (!a)
                break;
        } else {
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
            if (!b) {
                b = a;
                break;
            }
        }
    }

    /* Finish linking remainder of list b on to tail */
    tail->next = b;
    do {
        /*
         * If the merge is highly unbalanced (e.g. the input is
         * already sorted), this loop may run many iterations.
         * Continue callbacks to the client even though no
         * element comparison is needed, so the client's cmp()
         * routine can invoke cond_resched() periodically.
         */
        if (unlikely(!++count))
            cmp(b, b);
        b->prev = tail;
        tail = b;
        b = b->next;
    } while (b);

    /* And the final links to make a circular doubly-linked list */
    tail->next = head;
    head->prev = tail;
}
void list_sort(struct list_head *head, list_cmp_func_t cmp)
{
    struct list_head *list = head->next, *pending = NULL;
    size_t count = 0; /* Count of pending */

    if (list == head->prev) /* Zero or one elements */
        return;

    /* Convert to a null-terminated singly-linked list. */
    head->prev->next = NULL;

    /*
     * Data structure invariants:
     * - All lists are singly linked and null-terminated; prev
     *   pointers are not maintained.
     * - pending is a prev-linked "list of lists" of sorted
     *   sublists awaiting further merging.
     * - Each of the sorted sublists is power-of-two in size.
     * - Sublists are sorted by size and age, smallest & newest at front.
     * - There are zero to two sublists of each size.
     * - A pair of pending sublists are merged as soon as the number
     *   of following pending elements equals their size (i.e.
     *   each time count reaches an odd multiple of that size).
     *   That ensures each later final merge will be at worst 2:1.
     * - Each round consists of:
     *   - Merging the two sublists selected by the highest bit
     *     which flips when count is incremented, and
     *   - Adding an element from the input as a size-1 sublist.
     */
    do {
        size_t bits;
        struct list_head **tail = &pending;

        /* Find the least-significant clear bit in count */
        for (bits = count; bits & 1; bits >>= 1)
            tail = &(*tail)->prev;
        /* Do the indicated merge */
        if (likely(bits)) {
            struct list_head *a = *tail, *b = a->prev;

            a = merge(cmp, b, a);
            /* Install the merged result in place of the inputs */
            a->prev = b->prev;
            *tail = a;
        }

        /* Move one element from input list to pending */
        list->prev = pending;
        pending = list;
        list = list->next;
        pending->next = NULL;
        count++;
    } while (list);

    /* End of input; merge together all the pending lists. */
    list = pending;
    pending = pending->prev;
    for (;;) {
        struct list_head *next = pending->prev;

        if (!next)
            break;
        list = merge(cmp, pending, list);
        pending = next;
    }
    /* The final merge, rebuilding prev links */
    merge_final(cmp, head, pending, list);
}

static inline size_t run_size(struct list_head *head)
{
    if (!head)
        return 0;
    if (!head->next)
        return 1;
    return (size_t) (head->next->prev);
}

struct pair {
    struct list_head *head, *next;
};

static size_t stk_size;

static void build_prev_link(struct list_head *head,
                            struct list_head *tail,
                            struct list_head *list)
{
    tail->next = list;
    do {
        list->prev = tail;
        tail = list;
        list = list->next;
    } while (list);

    /* The final links to make a circular doubly-linked list */
    tail->next = head;
    head->prev = tail;
}


static struct pair find_run(struct list_head *list, list_cmp_func_t cmp)

{
    size_t len = 1;
    struct list_head *next = list->next, *head = list;
    struct pair result;

    if (!next) {
        result.head = head, result.next = next;
        return result;
    }

    if (cmp(list, next) > 0) {
        /* decending run, also reverse the list */
        struct list_head *prev = NULL;
        do {
            len++;
            list->next = prev;
            prev = list;
            list = next;
            next = list->next;
            head = list;
        } while (next && cmp(list, next) > 0);
        list->next = prev;
    } else {
        do {
            len++;
            list = next;
            next = list->next;
        } while (next && cmp(list, next) <= 0);
        list->next = NULL;
    }
    head->prev = NULL;
    head->next->prev = (struct list_head *) len;
    result.head = head, result.next = next;
    return result;
}

static struct list_head *merge_at(list_cmp_func_t cmp, struct list_head *at)

{
    size_t len = run_size(at) + run_size(at->prev);
    struct list_head *prev = at->prev->prev;
    struct list_head *list = merge(cmp, at->prev, at);
    list->prev = prev;
    list->next->prev = (struct list_head *) len;
    --stk_size;
    return list;
}

static struct list_head *merge_force_collapse(list_cmp_func_t cmp,
                                              struct list_head *tp)

{
    while (stk_size >= 3) {
        if (run_size(tp->prev->prev) < run_size(tp)) {
            tp->prev = merge_at(cmp, tp->prev);
        } else {
            tp = merge_at(cmp, tp);
        }
    }
    return tp;
}

static struct list_head *merge_collapse(list_cmp_func_t cmp,
                                        struct list_head *tp)

{
    int n;
    while ((n = stk_size) >= 2) {
        if ((n >= 3 &&
             run_size(tp->prev->prev) <= run_size(tp->prev) + run_size(tp)) ||
            (n >= 4 && run_size(tp->prev->prev->prev) <=
                           run_size(tp->prev->prev) + run_size(tp->prev))) {
            if (run_size(tp->prev->prev) < run_size(tp)) {
                tp->prev = merge_at(cmp, tp->prev);
            } else {
                tp = merge_at(cmp, tp);
            }
        } else if (run_size(tp->prev) <= run_size(tp)) {
            tp = merge_at(cmp, tp);
        } else {
            break;
        }
    }

    return tp;
}

void timsort(struct list_head *head, list_cmp_func_t cmp)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    stk_size = 0;

    struct list_head *list = head->next, *tp = NULL;

    /* Convert to a null-terminated singly-linked list. */
    head->prev->next = NULL;

    do {
        /* Find next run */
        struct pair result = find_run(list, cmp);
        result.head->prev = tp;
        tp = result.head;
        list = result.next;
        stk_size++;
        tp = merge_collapse(cmp, tp);
    } while (list);

    /* End of input; merge together all the runs. */
    tp = merge_force_collapse(cmp, tp);

    /* The final merge; rebuild prev links */
    struct list_head *stk0 = tp, *stk1 = stk0->prev;
    while (stk1 && stk1->prev)
        stk0 = stk0->prev, stk1 = stk1->prev;
    if (stk_size <= 1) {
        build_prev_link(head, head, stk0);
        return;
    }
    merge_final(cmp, head, stk1, stk0);
}

/* Sort elements of queue in ascending order */
extern int sort_type;
void q_sort(struct list_head *head, bool descend)
{
    switch (sort_type) {
    case 0:
        merge_sort(head);
        break;
    case 1:
        list_sort(head, cmp);
        break;
    case 2:
        timsort(head, cmp);
        break;
    default:
        printf("Unknown sort type.\n");
        break;
    }

    if (descend)
        q_reverse(head);
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
