#include "cunit/cunit.h"
#include "xmalloc.h"
#include "strarray.h"

static void test_auto(void)
{
    strarray_t sa = STRARRAY_INITIALIZER;
    char *s1;
    char *s2;

    CU_ASSERT_EQUAL(sa.count, 0);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NULL(strarray_nth(&sa, 0));
    CU_ASSERT_PTR_NULL(strarray_nth(&sa, -1));

    s1 = xstrdup("lorem ipsum");
    strarray_appendm(&sa, s1);
    CU_ASSERT_EQUAL(sa.count, 1);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_PTR_EQUAL((char *)strarray_nth(&sa, 0), s1);
    CU_ASSERT_PTR_EQUAL((char *)strarray_nth(&sa, -1), s1);

    s2 = xstrdup("dolor sit");
    strarray_appendm(&sa, s2);
    CU_ASSERT_EQUAL(sa.count, 2);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_PTR_EQUAL((char *)strarray_nth(&sa, 0), s1);
    CU_ASSERT_PTR_EQUAL((char *)strarray_nth(&sa, 1), s2);
    CU_ASSERT_PTR_EQUAL((char *)strarray_nth(&sa, -1), s2);

    strarray_fini(&sa);
    CU_ASSERT_EQUAL(sa.count, 0);
    CU_ASSERT_EQUAL(sa.alloc, 0);
    CU_ASSERT_PTR_NULL(sa.data);
}

static void test_heap(void)
{
    strarray_t *sa = strarray_new();
    char *s1;
    char *s2;

    CU_ASSERT_EQUAL(sa->count, 0);
    CU_ASSERT(sa->alloc >= sa->count);
    CU_ASSERT_PTR_NULL((char *)strarray_nth(sa, 0));
    CU_ASSERT_PTR_NULL((char *)strarray_nth(sa, -1));

    s1 = xstrdup("lorem ipsum");
    strarray_appendm(sa, s1);
    CU_ASSERT_EQUAL(sa->count, 1);
    CU_ASSERT(sa->alloc >= sa->count);
    CU_ASSERT_PTR_NOT_NULL(sa->data);
    CU_ASSERT_PTR_EQUAL((char *)strarray_nth(sa, 0), s1);
    CU_ASSERT_PTR_EQUAL((char *)strarray_nth(sa, -1), s1);

    s2 = xstrdup("dolor sit");
    strarray_appendm(sa, s2);
    CU_ASSERT_EQUAL(sa->count, 2);
    CU_ASSERT(sa->alloc >= sa->count);
    CU_ASSERT_PTR_NOT_NULL(sa->data);
    CU_ASSERT_PTR_EQUAL((char *)strarray_nth(sa, 0), s1);
    CU_ASSERT_PTR_EQUAL((char *)strarray_nth(sa, 1), s2);
    CU_ASSERT_PTR_EQUAL((char *)strarray_nth(sa, -1), s2);

    strarray_free(sa);
}

static void test_set(void)
{
    strarray_t sa = STRARRAY_INITIALIZER;
#define WORD0	"lorem"
#define WORD0REP "DeLorean"
#define WORD0REP2 "Jiggawatts"
#define WORD1	"ipsum"
#define WORD2	"dolor"
#define WORD2REP "DOLORES"
#define WORD3	"sit"
#define WORD4	"amet"

    CU_ASSERT_EQUAL(sa.count, 0);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NULL(strarray_nth(&sa, 0));
    CU_ASSERT_PTR_NULL(strarray_nth(&sa, -1));

    strarray_append(&sa, WORD0);
    CU_ASSERT_EQUAL(sa.count, 1);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD0);

    strarray_set(&sa, 0, WORD0REP);
    CU_ASSERT_EQUAL(sa.count, 1);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD0REP);

    strarray_set(&sa, -1, WORD0REP2);
    CU_ASSERT_EQUAL(sa.count, 1);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD0REP2);

    strarray_append(&sa, WORD1);
    strarray_append(&sa, WORD2);
    strarray_append(&sa, WORD3);
    strarray_append(&sa, WORD4);
    CU_ASSERT_EQUAL(sa.count, 5);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD0REP2);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 2), WORD2);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 3), WORD3);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 4), WORD4);

    strarray_set(&sa, 2, WORD2REP);
    CU_ASSERT_EQUAL(sa.count, 5);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD0REP2);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 2), WORD2REP);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 3), WORD3);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 4), WORD4);

    strarray_fini(&sa);
#undef WORD0
#undef WORD0REP
#undef WORD0REP2
#undef WORD1
#undef WORD2
#undef WORD2REP
#undef WORD3
#undef WORD4
}

static void test_setm(void)
{
    strarray_t sa = STRARRAY_INITIALIZER;
#define WORD0	"lorem"
#define WORD0REP "DeLorean"
#define WORD0REP2 "Jiggawatts"
#define WORD1	"ipsum"
#define WORD2	"dolor"
#define WORD2REP "DOLORES"
#define WORD3	"sit"
#define WORD4	"amet"

    CU_ASSERT_EQUAL(sa.count, 0);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NULL(strarray_nth(&sa, 0));
    CU_ASSERT_PTR_NULL(strarray_nth(&sa, -1));

    strarray_appendm(&sa, xstrdup(WORD0));
    CU_ASSERT_EQUAL(sa.count, 1);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD0);

    strarray_setm(&sa, 0, xstrdup(WORD0REP));
    CU_ASSERT_EQUAL(sa.count, 1);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD0REP);

    strarray_setm(&sa, -1, xstrdup(WORD0REP2));
    CU_ASSERT_EQUAL(sa.count, 1);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD0REP2);

    strarray_appendm(&sa, xstrdup(WORD1));
    strarray_appendm(&sa, xstrdup(WORD2));
    strarray_appendm(&sa, xstrdup(WORD3));
    strarray_appendm(&sa, xstrdup(WORD4));
    CU_ASSERT_EQUAL(sa.count, 5);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD0REP2);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 2), WORD2);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 3), WORD3);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 4), WORD4);

    strarray_setm(&sa, 2, xstrdup(WORD2REP));
    CU_ASSERT_EQUAL(sa.count, 5);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD0REP2);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 2), WORD2REP);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 3), WORD3);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 4), WORD4);

    strarray_fini(&sa);
#undef WORD0
#undef WORD0REP
#undef WORD0REP2
#undef WORD1
#undef WORD2
#undef WORD2REP
#undef WORD3
#undef WORD4
}

static void test_insert(void)
{
    strarray_t sa = STRARRAY_INITIALIZER;
#define WORD0	"lorem"
#define WORD1	"ipsum"
#define WORD2	"dolor"
#define WORD3	"sit"
#define WORD4	"amet"

    CU_ASSERT_EQUAL(sa.count, 0);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NULL(strarray_nth(&sa, 0));
    CU_ASSERT_PTR_NULL(strarray_nth(&sa, -1));

    strarray_insert(&sa, 0, WORD0);
    CU_ASSERT_EQUAL(sa.count, 1);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD0);

    strarray_insert(&sa, -1, WORD1);
    CU_ASSERT_EQUAL(sa.count, 2);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 1), WORD0);

    strarray_insert(&sa, 0, WORD2);
    CU_ASSERT_EQUAL(sa.count, 3);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD2);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 2), WORD0);

    strarray_insert(&sa, -1, WORD3);
    CU_ASSERT_EQUAL(sa.count, 4);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD2);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 2), WORD3);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 3), WORD0);

    strarray_insert(&sa, 2, WORD4);
    CU_ASSERT_EQUAL(sa.count, 5);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD2);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 2), WORD4);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 3), WORD3);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 4), WORD0);

    strarray_fini(&sa);
#undef WORD0
#undef WORD1
#undef WORD2
#undef WORD3
#undef WORD4
}

static void test_insertm(void)
{
    strarray_t sa = STRARRAY_INITIALIZER;
#define WORD0	"lorem"
#define WORD1	"ipsum"
#define WORD2	"dolor"
#define WORD3	"sit"
#define WORD4	"amet"

    CU_ASSERT_EQUAL(sa.count, 0);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NULL(strarray_nth(&sa, 0));
    CU_ASSERT_PTR_NULL(strarray_nth(&sa, -1));

    strarray_insertm(&sa, 0, xstrdup(WORD0));
    CU_ASSERT_EQUAL(sa.count, 1);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD0);

    strarray_insertm(&sa, -1, xstrdup(WORD1));
    CU_ASSERT_EQUAL(sa.count, 2);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 1), WORD0);

    strarray_insertm(&sa, 0, xstrdup(WORD2));
    CU_ASSERT_EQUAL(sa.count, 3);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD2);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 2), WORD0);

    strarray_insertm(&sa, -1, xstrdup(WORD3));
    CU_ASSERT_EQUAL(sa.count, 4);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD2);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 2), WORD3);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 3), WORD0);

    strarray_insertm(&sa, 2, xstrdup(WORD4));
    CU_ASSERT_EQUAL(sa.count, 5);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD2);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 2), WORD4);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 3), WORD3);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 4), WORD0);

    strarray_fini(&sa);
#undef WORD0
#undef WORD1
#undef WORD2
#undef WORD3
#undef WORD4
}

static void test_join(void)
{
    strarray_t sa = STRARRAY_INITIALIZER;
    char *s;
#define WORD0	"lorem"
#define WORD1	"ipsum"
#define WORD2	"dolor"
#define WORD3	"sit"
#define WORD4	"amet"

    CU_ASSERT_EQUAL(sa.count, 0);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NULL(strarray_nth(&sa, 0));
    CU_ASSERT_PTR_NULL(strarray_nth(&sa, -1));

    s = strarray_join(&sa, NULL);
    CU_ASSERT_PTR_NULL(s);

    s = strarray_join(&sa, " ");
    CU_ASSERT_PTR_NULL(s);

    strarray_append(&sa, WORD0);
    strarray_append(&sa, WORD1);
    strarray_append(&sa, WORD2);
    strarray_append(&sa, WORD3);
    strarray_append(&sa, WORD4);
    CU_ASSERT_EQUAL(sa.count, 5);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD0);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 2), WORD2);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 3), WORD3);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 4), WORD4);

    s = strarray_join(&sa, NULL);
    CU_ASSERT_STRING_EQUAL(s, WORD0""WORD1""WORD2""WORD3""WORD4);
    free(s);

    s = strarray_join(&sa, " ");
    CU_ASSERT_STRING_EQUAL(s, WORD0" "WORD1" "WORD2" "WORD3" "WORD4);
    free(s);

    s = strarray_join(&sa, "-X-");
    CU_ASSERT_STRING_EQUAL(s, WORD0"-X-"WORD1"-X-"WORD2"-X-"WORD3"-X-"WORD4);
    free(s);

    /* check that sa is unharmed by the join */
    CU_ASSERT_EQUAL(sa.count, 5);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD0);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 2), WORD2);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 3), WORD3);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 4), WORD4);

    strarray_fini(&sa);
#undef WORD0
#undef WORD1
#undef WORD2
#undef WORD3
#undef WORD4
}

static void test_split(void)
{
    strarray_t *sa;
#define WORD0	"lorem"
#define WORD1	"ipsum"
#define WORD2	"dolor"
#define WORD3	"sit"
#define WORD4	"amet"

    /* 5 words, space separator */
    sa = strarray_split(WORD0" "WORD1" "WORD2" "WORD3" "WORD4, " ");
    CU_ASSERT_PTR_NOT_NULL(sa);
    CU_ASSERT_EQUAL(sa->count, 5);
    CU_ASSERT(sa->alloc >= sa->count);
    CU_ASSERT_PTR_NOT_NULL(sa->data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 0), WORD0);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 2), WORD2);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 3), WORD3);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 4), WORD4);
    strarray_free(sa);

    /* 5 words, NULL separator (whitespace) */
    sa = strarray_split(WORD0" "WORD1"\t"WORD2"\r"WORD3"\n"WORD4, NULL);
    CU_ASSERT_PTR_NOT_NULL(sa);
    CU_ASSERT_EQUAL(sa->count, 5);
    CU_ASSERT(sa->alloc >= sa->count);
    CU_ASSERT_PTR_NOT_NULL(sa->data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 0), WORD0);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 2), WORD2);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 3), WORD3);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 4), WORD4);
    strarray_free(sa);

    /* 5 words, several separators */
    sa = strarray_split(WORD0"("WORD1")"WORD2"["WORD3"]"WORD4, "[]()");
    CU_ASSERT_PTR_NOT_NULL(sa);
    CU_ASSERT_EQUAL(sa->count, 5);
    CU_ASSERT(sa->alloc >= sa->count);
    CU_ASSERT_PTR_NOT_NULL(sa->data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 0), WORD0);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 2), WORD2);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 3), WORD3);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 4), WORD4);
    strarray_free(sa);

    /* splitm - takes ownership of a strdup()d argument */
    sa = strarray_splitm(xstrdup(WORD0" "WORD1" "WORD2" "WORD3" "WORD4), " ");
    CU_ASSERT_PTR_NOT_NULL(sa);
    CU_ASSERT_EQUAL(sa->count, 5);
    CU_ASSERT(sa->alloc >= sa->count);
    CU_ASSERT_PTR_NOT_NULL(sa->data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 0), WORD0);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 2), WORD2);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 3), WORD3);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 4), WORD4);
    strarray_free(sa);

    /* nsplit - specify a byte range to copy and split */
    sa = strarray_nsplit(WORD0" "WORD1" "WORD2" "WORD3" "WORD4,
			 sizeof(WORD0)+sizeof(WORD1)+sizeof(WORD2), " ");
    CU_ASSERT_PTR_NOT_NULL(sa);
    CU_ASSERT_EQUAL(sa->count, 3);
    CU_ASSERT(sa->alloc >= sa->count);
    CU_ASSERT_PTR_NOT_NULL(sa->data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 0), WORD0);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 2), WORD2);
    strarray_free(sa);

#undef WORD0
#undef WORD1
#undef WORD2
#undef WORD3
#undef WORD4
}

static void test_remove(void)
{
    strarray_t sa = STRARRAY_INITIALIZER;
    char *s;
#define WORD0	"lorem"
#define WORD1	"ipsum"
#define WORD2	"dolor"
#define WORD3	"sit"
#define WORD4	"amet"

    CU_ASSERT_EQUAL(sa.count, 0);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NULL(strarray_nth(&sa, 0));
    CU_ASSERT_PTR_NULL(strarray_nth(&sa, -1));

    strarray_append(&sa, WORD0);
    strarray_append(&sa, WORD1);
    strarray_append(&sa, WORD2);
    strarray_append(&sa, WORD3);
    strarray_append(&sa, WORD4);
    CU_ASSERT_EQUAL(sa.count, 5);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD0);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 2), WORD2);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 3), WORD3);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 4), WORD4);

    s = strarray_remove(&sa, 2);
    CU_ASSERT_STRING_EQUAL(s, WORD2);
    free(s);
    CU_ASSERT_EQUAL(sa.count, 4);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD0);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 2), WORD3);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 3), WORD4);

    s = strarray_remove(&sa, 0);
    CU_ASSERT_STRING_EQUAL(s, WORD0);
    free(s);
    CU_ASSERT_EQUAL(sa.count, 3);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 1), WORD3);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 2), WORD4);

    s = strarray_remove(&sa, -1);
    CU_ASSERT_STRING_EQUAL(s, WORD4);
    free(s);
    CU_ASSERT_EQUAL(sa.count, 2);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 1), WORD3);

    s = strarray_remove(&sa, 1);
    CU_ASSERT_STRING_EQUAL(s, WORD3);
    free(s);
    CU_ASSERT_EQUAL(sa.count, 1);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD1);

    s = strarray_remove(&sa, 0);
    CU_ASSERT_STRING_EQUAL(s, WORD1);
    free(s);
    CU_ASSERT_EQUAL(sa.count, 0);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_PTR_NULL(strarray_nth(&sa, 0));

    s = strarray_remove(&sa, 0);
    CU_ASSERT_PTR_NULL(s);

    strarray_fini(&sa);
#undef WORD0
#undef WORD1
#undef WORD2
#undef WORD3
#undef WORD4
}

static void test_takevf(void)
{
    strarray_t *sa;
    char **ss;
#define WORD0	"lorem"
#define WORD1	"ipsum"
#define WORD2	"dolor"
#define WORD3	"sit"
#define WORD4	"amet"

    /* 5 words, space separator */
    sa = strarray_split(WORD0" "WORD1" "WORD2" "WORD3" "WORD4, " ");
    CU_ASSERT_PTR_NOT_NULL(sa);
    CU_ASSERT_EQUAL(sa->count, 5);
    CU_ASSERT(sa->alloc >= sa->count);
    CU_ASSERT_PTR_NOT_NULL(sa->data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 0), WORD0);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 2), WORD2);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 3), WORD3);
    CU_ASSERT_STRING_EQUAL(strarray_nth(sa, 4), WORD4);

    ss = strarray_takevf(sa);
    /* note: takevf frees the strarray itself */
    CU_ASSERT_PTR_NOT_NULL(ss);
    CU_ASSERT_STRING_EQUAL(ss[0], WORD0);
    CU_ASSERT_STRING_EQUAL(ss[1], WORD1);
    CU_ASSERT_STRING_EQUAL(ss[2], WORD2);
    CU_ASSERT_STRING_EQUAL(ss[3], WORD3);
    CU_ASSERT_STRING_EQUAL(ss[4], WORD4);
    CU_ASSERT_PTR_NULL(ss[5]);

    free(ss[0]);
    free(ss[1]);
    free(ss[2]);
    free(ss[3]);
    free(ss[4]);
    free(ss);

#undef WORD0
#undef WORD1
#undef WORD2
#undef WORD3
#undef WORD4
}

static void test_truncate(void)
{
    strarray_t sa = STRARRAY_INITIALIZER;
#define WORD0	"lorem"
#define WORD1	"ipsum"
#define WORD2	"dolor"
#define WORD3	"sit"
#define WORD4	"amet"

    CU_ASSERT_EQUAL(sa.count, 0);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NULL(strarray_nth(&sa, 0));
    CU_ASSERT_PTR_NULL(strarray_nth(&sa, -1));

    strarray_append(&sa, WORD0);
    strarray_append(&sa, WORD1);
    strarray_append(&sa, WORD2);
    strarray_append(&sa, WORD3);
    strarray_append(&sa, WORD4);
    CU_ASSERT_EQUAL(sa.count, 5);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD0);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 2), WORD2);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 3), WORD3);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 4), WORD4);

    /* expand the array */
    strarray_truncate(&sa, 7);
    CU_ASSERT_EQUAL(sa.count, 7);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD0);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 2), WORD2);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 3), WORD3);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 4), WORD4);
    CU_ASSERT_PTR_NULL(strarray_nth(&sa, 5));
    CU_ASSERT_PTR_NULL(strarray_nth(&sa, 6));

    /* shrink the array */
    strarray_truncate(&sa, 4);
    CU_ASSERT_EQUAL(sa.count, 4);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD0);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 2), WORD2);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 3), WORD3);

    /* shrink the array harder */
    strarray_truncate(&sa, 3);
    CU_ASSERT_EQUAL(sa.count, 3);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD0);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 2), WORD2);

    /* shrink the array to nothing */
    strarray_truncate(&sa, 0);
    CU_ASSERT_EQUAL(sa.count, 0);
    CU_ASSERT(sa.alloc >= sa.count);
    /* whether sa.data is NULL is undefined at this time */
    CU_ASSERT_PTR_NULL(strarray_nth(&sa, 0));
    CU_ASSERT_PTR_NULL(strarray_nth(&sa, -1));

    strarray_fini(&sa);
#undef WORD0
#undef WORD1
#undef WORD2
#undef WORD3
#undef WORD4
}

static void test_find(void)
{
    strarray_t sa = STRARRAY_INITIALIZER;
    int i;
#define WORD0	"lorem"
#define WORD1	"ipsum"
#define WORD2	"dolor"
#define WORD3	"sit"
#define WORD4	"amet"

    CU_ASSERT_EQUAL(sa.count, 0);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NULL(strarray_nth(&sa, 0));
    CU_ASSERT_PTR_NULL(strarray_nth(&sa, -1));

    strarray_append(&sa, WORD0);
    strarray_append(&sa, WORD1);
    strarray_append(&sa, WORD2);
    strarray_append(&sa, WORD3);
    strarray_append(&sa, WORD0);
    strarray_append(&sa, WORD4);
    CU_ASSERT_EQUAL(sa.count, 6);
    CU_ASSERT(sa.alloc >= sa.count);
    CU_ASSERT_PTR_NOT_NULL(sa.data);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 0), WORD0);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 1), WORD1);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 2), WORD2);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 3), WORD3);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 4), WORD0);
    CU_ASSERT_STRING_EQUAL(strarray_nth(&sa, 5), WORD4);

    /* search for something which isn't there */
    i = strarray_find(&sa, "NotHere", 0);
    CU_ASSERT_EQUAL(i, -1);

    /* search for something which isn't there, starting off the end */
    i = strarray_find(&sa, "NotHere", 7);
    CU_ASSERT_EQUAL(i, -1);

    /* search for something which is there */
    i = strarray_find(&sa, WORD1, 0);
    CU_ASSERT_EQUAL(i, 1);
    i = strarray_find(&sa, WORD1, i+1);
    CU_ASSERT_EQUAL(i, -1);

    /* search for something which is there, starting off the end */
    i = strarray_find(&sa, WORD1, 7);
    CU_ASSERT_EQUAL(i, -1);

    /* search for something which is there multiple times */
    i = strarray_find(&sa, WORD0, 0);
    CU_ASSERT_EQUAL(i, 0);
    i = strarray_find(&sa, WORD0, i+1);
    CU_ASSERT_EQUAL(i, 4);
    i = strarray_find(&sa, WORD0, i+1);
    CU_ASSERT_EQUAL(i, -1);

    strarray_fini(&sa);
#undef WORD0
#undef WORD1
#undef WORD2
#undef WORD3
#undef WORD4
}

