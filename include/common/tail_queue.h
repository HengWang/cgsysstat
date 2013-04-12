/* Copyright (c) 2012, Heng.Wang. All rights reserved.

 This program is aimed to monitor the cgroup like iostat for each group,
 if you have more effective processing methods or some ideas to solve
 the problem, thanks for your sharing with the developers. It's pleasure
 for you to contact me.

 @Author: Heng.Wang
 @Date  : 11/07/2012
 @Email : heng.wang@qunar.com
 wangheng.king@gmail.com
 king_wangheng@163.com
 @Github: https://github.com/HengWang/
 @Blog  : http://hengwang.blog.chinaunix.net
 */

#ifndef __TAIL_QUEUE_H
#define __TAIL_QUEUE_H

/*
 * Tail queue functions.
 */
#ifndef TAILQ_ENTRY
#define TAILQ_ENTRY(type)						\
struct {								\
struct type *tqe_next;	/* next element */			\
struct type **tqe_prev;	/* address of previous next element */	\
}
#endif /* !TAILQ_ENTRY */

#ifndef TAILQ_HEAD
#define TAILQ_HEAD(name, type)			\
struct name {					\
struct type *tqh_first;		/* first element */		\
struct type **tqh_last;		/* addr of last next element */	\
int count; /* count of the elements. */ \
}
#endif /* !TAIL_HEAD */

#ifndef TAILQ_FIRST
#define	TAILQ_FIRST(head)		((head)->tqh_first)
#endif /* !TAILQ_FIRST */

#ifndef TAILQ_END
#define	TAILQ_END(head)			NULL
#endif /* !TAILQ_END */

#ifndef TAILQ_NEXT
#define	TAILQ_NEXT(elm, field)		((elm)->field.tqe_next)
#endif  /* !TAILQ_NEXT */

#ifndef TAILQ_LAST
#define TAILQ_LAST(head, headname)					\
  (*(((struct headname *)((head)->tqh_last))->tqh_last))
#endif  /* !TAILQ_LAST */

#ifndef TAILQ_PREV
#define TAILQ_PREV(elm, headname, field)				\
  (*(((struct headname *)((elm)->field.tqe_prev))->tqh_last))
#endif /* !TAILQ_PREV */

#ifndef TAILQ_EMPTY
#define	TAILQ_EMPTY(head)						\
  (TAILQ_FIRST(head) == TAILQ_END(head))
#endif  /* !TAILQ_EMPTY */

#ifndef TAILQ_FOREACH
#define TAILQ_FOREACH(var, head, field)					\
  for ((var) = TAILQ_FIRST(head);					\
  (var) != TAILQ_END(head);					\
  (var) = TAILQ_NEXT(var, field))
#endif /* !TAILQ_FOREACH */

#ifndef TAILQ_FOREACH_REVERSE
#define TAILQ_FOREACH_REVERSE(var, head, headname, field)		\
  for((var) = TAILQ_LAST(head, headname);				\
  (var) != TAILQ_END(head);					\
  (var) = TAILQ_PREV(var, headname, field))
#endif  /* !TAILQ_FOREACH_REVERSE */

#ifndef TAILQ_INIT
#define	TAILQ_INIT(head) do {						\
  (head)->tqh_first = NULL;					\
  (head)->tqh_last = &(head)->tqh_first;				\
  (head)->count = 0;      \
} while (0)
#endif  /* !TAILQ_INIT */

#ifndef TAILQ_INSERT_HEAD
#define TAILQ_INSERT_HEAD(head, elm, field) do {			\
  if (((elm)->field.tqe_next = (head)->tqh_first) != NULL)	\
  (head)->tqh_first->field.tqe_prev =			\
  &(elm)->field.tqe_next;				\
  else								\
  (head)->tqh_last = &(elm)->field.tqe_next;		\
  (head)->tqh_first = (elm);					\
  (elm)->field.tqe_prev = &(head)->tqh_first;			\
} while (0)
#endif  /* !TAILQ_INSERT_HEAD */

#ifndef TAILQ_INSERT_TAIL
#define TAILQ_INSERT_TAIL(head, elm, field) do {			\
  (elm)->field.tqe_next = NULL;					\
  (elm)->field.tqe_prev = (head)->tqh_last;			\
  *(head)->tqh_last = (elm);					\
  (head)->tqh_last = &(elm)->field.tqe_next;			\
} while (0)
#endif /* !TAILQ_INSERT_TAIL */

#ifndef TAILQ_INSERT_AFTER
#define TAILQ_INSERT_AFTER(head, listelm, elm, field) do {		\
  if (((elm)->field.tqe_next = (listelm)->field.tqe_next) != NULL)\
  (elm)->field.tqe_next->field.tqe_prev =			\
  &(elm)->field.tqe_next;				\
  else								\
  (head)->tqh_last = &(elm)->field.tqe_next;		\
  (listelm)->field.tqe_next = (elm);				\
  (elm)->field.tqe_prev = &(listelm)->field.tqe_next;		\
} while (0)
#endif  /* !TAILQ_INSERT_AFTER */

#ifndef TAILQ_INSERT_BEFORE
#define	TAILQ_INSERT_BEFORE(listelm, elm, field) do {			\
  (elm)->field.tqe_prev = (listelm)->field.tqe_prev;		\
  (elm)->field.tqe_next = (listelm);				\
  *(listelm)->field.tqe_prev = (elm);				\
  (listelm)->field.tqe_prev = &(elm)->field.tqe_next;		\
} while (0)
#endif /* !TAILQ_INSERT_BEFORE */

#ifndef TAILQ_REMOVE
#define TAILQ_REMOVE(head, elm, field) do {				\
  if (((elm)->field.tqe_next) != NULL)				\
  (elm)->field.tqe_next->field.tqe_prev =			\
  (elm)->field.tqe_prev;				\
  else								\
  (head)->tqh_last = (elm)->field.tqe_prev;		\
  *(elm)->field.tqe_prev = (elm)->field.tqe_next;			\
} while (0)
#endif /* !TAILQ_REMOVE */

#ifndef TAILQ_REPLACE
#define TAILQ_REPLACE(head, elm, elm2, field) do {			\
  if (((elm2)->field.tqe_next = (elm)->field.tqe_next) != NULL)	\
  (elm2)->field.tqe_next->field.tqe_prev =		\
  &(elm2)->field.tqe_next;				\
  else								\
  (head)->tqh_last = &(elm2)->field.tqe_next;		\
  (elm2)->field.tqe_prev = (elm)->field.tqe_prev;			\
  *(elm2)->field.tqe_prev = (elm2);				\
} while (0)
#endif /* !TAILQ_REPLACE */

#endif /* __TAIL_QUEUE_H */
