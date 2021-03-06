/* cyr_expire.c -- Program to expire deliver.db entries and messages
 *
 * Copyright (c) 1994-2008 Carnegie Mellon University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The name "Carnegie Mellon University" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For permission or any legal
 *    details, please contact
 *      Carnegie Mellon University
 *      Center for Technology Transfer and Enterprise Creation
 *      4615 Forbes Avenue
 *      Suite 302
 *      Pittsburgh, PA  15213
 *      (412) 268-7393, fax: (412) 268-7395
 *      innovation@andrew.cmu.edu
 *
 * 4. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by Computing Services
 *     at Carnegie Mellon University (http://www.cmu.edu/computing/)."
 *
 * CARNEGIE MELLON UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE
 * FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * $Id: cyr_expire.c,v 1.25 2010/01/06 17:01:31 murch Exp $
 */

#include <config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <syslog.h>
#include <errno.h>
#include <signal.h>

#include <sasl/sasl.h>

#include "annotate.h"
#include "cyrusdb.h"
#include "duplicate.h"
#include "exitcodes.h"
#include "global.h"
#include "hash.h"
#include "libcyr_cfg.h"
#include "mboxlist.h"
#include "util.h"
#include "xmalloc.h"
#include "xstrlcpy.h"
#include "xstrlcat.h"

/* global state */
const int config_need_data = 0;
static int sigquit = 0;
static int verbose = 0;

void usage(void)
{
    fprintf(stderr,
	    "cyr_expire [-C <altconfig>] -E <days> [-X <expunge-days>] [-p prefix] [-a] [-v]\n");
    exit(-1);
}

struct expire_rock {
    struct hash_table table;
    time_t expire_mark;
    time_t expunge_mark;
    unsigned long mailboxes_seen;
    unsigned long messages_seen;
    unsigned long messages_expired;
    unsigned long messages_expunged;
    int skip_annotate;
};

struct delete_node {
    struct delete_node *next;
    char *name;
};

struct delete_rock {
    time_t delete_mark;
    struct delete_node *head;
    struct delete_node *tail;
};

/*
 * Parse a string as an unsigned decimal integer.
 * Note: returned value is signed to allow the caller to
 * use negative values as an error case.
 * Returns 1 if successful and *@valp is filled in,
 *  or 0 on error.
 * gnb:TODO should return bool
 */
static int parse_uint(const char *s, int *valp)
{
    char *end = NULL;
    int val;

    if (*s == '-')
	return 0;
    val = (int)strtoul(s, &end, 10);
    if (end == NULL || *end || end == s)
	return 0;
    *valp = val;
    return 1;
}

/*
 * mailbox_expunge() callback to expunge expired articles.
 */
static unsigned expire_cb(struct mailbox *mailbox __attribute__((unused)), 
			  struct index_record *record, 
			  void *rock)
{
    struct expire_rock *erock = (struct expire_rock *) rock;

    /* otherwise, we're expiring messages by sent date */
    if (record->gmtime < erock->expire_mark) {
	erock->messages_expired++;
	return 1;
    }

    return 0;
}


/*
 * mboxlist_findall() callback function to:
 * - expire messages from mailboxes,
 * - build a hash table of mailboxes in which we expired messages,
 * - and perform a cleanup of expunged messages
 */
static int expire(char *name, int matchlen __attribute__((unused)),
	          int maycreate __attribute__((unused)), void *rock)
{
    struct mboxlist_entry *mbentry = NULL;
    struct expire_rock *erock = (struct expire_rock *) rock;
    char *buf;
    struct annotation_data attrib;
    int r;
    struct mailbox *mailbox = NULL;
    unsigned numexpunged = 0;
    int expire_days;

    if (sigquit) {
	return 1;
    }
    /* Skip remote mailboxes */
    r = mboxlist_lookup(name, &mbentry, NULL);
    if (r) {
	if (verbose) {
	    printf("error looking up %s: %s\n", name, error_message(r));
	}
	return 1;
    }

    if (mbentry->mbtype & MBTYPE_REMOTE) {
	mboxlist_entry_free(&mbentry);
	return 0;
    }
    mboxlist_entry_free(&mbentry);

    buf = xstrdup(name);

    /* see if we need to expire messages.
     * since mailboxes inherit /vendor/cmu/cyrus-imapd/expire,
     * we need to iterate all the way up to "" (server entry)
     */
    if (erock->skip_annotate) {
      /* we don't want to check for annotations, so we didn't find any */
      attrib.value = 0;
    }
    else {
        do {
	    r = annotatemore_lookup(buf, "/vendor/cmu/cyrus-imapd/expire", "",
				    &attrib);

	    if (r ||				/* error */
	        attrib.value)			/* found an entry */
	        break;

	} while (mboxname_make_parent(buf));
    }
    free(buf);

    r = mailbox_open_iwl(name, &mailbox);
    if (r) {
	/* mailbox corrupt/nonexistent -- skip it */
	syslog(LOG_WARNING, "unable to open mailbox %s", name);
	return 0;
    }

    if (attrib.value && parse_uint(attrib.value, &expire_days)) {
	/* add mailbox to table */
	erock->expire_mark = expire_days ?
	    time(0) - (expire_days * 60 * 60 * 24) : 0 /* never */ ;
	hash_insert(name,
		    xmemdup(&erock->expire_mark, sizeof(erock->expire_mark)),
		    &erock->table);

	if (expire_days) {
	    if (verbose) {
		fprintf(stderr,
			"expiring messages in %s older than %d days\n",
			name, expire_days);
	    }

	    r = mailbox_expunge(mailbox, expire_cb, erock, NULL);
	    if (r)
		syslog(LOG_ERR, "failed to expire old messages: %s", mailbox->name);
	}
    }

    erock->messages_seen += mailbox->i.num_records;

    r = mailbox_expunge_cleanup(mailbox, erock->expunge_mark, &numexpunged);

    erock->messages_expunged += numexpunged;
    erock->mailboxes_seen++;

    mailbox_close(&mailbox);

    if (r) {
	syslog(LOG_WARNING, "failure expiring %s: %s", name, error_message(r));
    }

    /* Even if we had a problem with one mailbox, continue with the others */
    return 0;
}

static int delete(char *name,
	          int matchlen __attribute__((unused)),
	          int maycreate __attribute__((unused)),
	          void *rock)
{
    struct mboxlist_entry *mbentry = NULL;
    struct delete_rock *drock = (struct delete_rock *) rock;
    int r;
    struct delete_node *node;
    time_t timestamp;

    if (sigquit) {
	return 1;
    }

    /* check if this is a mailbox we want to examine */
    if (!mboxname_isdeletedmailbox(name, &timestamp))
	return 0;

    /* Skip remote mailboxes */
    r = mboxlist_lookup(name, &mbentry, NULL);
    if (r) {
	if (verbose) {
	    printf("error looking up %s: %s\n", name, error_message(r));
	}
	return 1;
    }

    if (mbentry->mbtype & MBTYPE_REMOTE) {
	mboxlist_entry_free(&mbentry);
	return 0;
    }
    mboxlist_entry_free(&mbentry);

    if ((timestamp == 0) || (timestamp > drock->delete_mark))
        return 0;

    /* Add this mailbox to list of mailboxes to delete */
    node = xmalloc(sizeof(struct delete_node));
    node->next = NULL;
    node->name = xstrdup(name);

    if (drock->tail) {
	drock->tail->next = node;
	drock->tail = node;
    } else {
	drock->head = drock->tail = node;
    }
    return(0);
}
static void sighandler (int sig __attribute((unused)))
{
    sigquit = 1;
    return;
}

int main(int argc, char *argv[])
{
    extern char *optarg;
    int opt, r = 0;
    int expire_days = 0;
    int expunge_days = -1;
    int delete_days = -1;
    int do_expunge = 1;	/* gnb:TODO bool */
    char *alt_config = NULL;
    const char *find_prefix = "*";
    struct expire_rock erock;
    struct delete_rock drock;
    struct sigaction action;

    if ((geteuid()) == 0 && (become_cyrus() != 0)) {
	fatal("must run as the Cyrus user", EC_USAGE);
    }

    /* zero the expire_rock & delete_rock */
    memset(&erock, 0, sizeof(erock));
    construct_hash_table(&erock.table, 10000, 1);
    memset(&drock, 0, sizeof(drock));

    while ((opt = getopt(argc, argv, "C:D:E:X:p:vax")) != EOF) {
	switch (opt) {
	case 'C': /* alt config file */
	    alt_config = optarg;
	    break;

	case 'D':
	    if (delete_days >= 0) usage();
	    if (!parse_uint(optarg, &delete_days)) usage();
	    break;

	case 'E':
	    if (expire_days) usage();
	    if (!parse_uint(optarg, &expire_days)) usage();
	    break;

	case 'X':
	    if (expunge_days >= 0) usage();
	    if (!parse_uint(optarg, &expunge_days)) usage();
	    break;

	case 'x':
	    if (!do_expunge) usage();
	    do_expunge = 0;
	    break;

	case 'p':
	    find_prefix = optarg;
	    break;

	case 'v':
	    verbose++;
	    break;

	case 'a':
	    erock.skip_annotate = 1;
	    break;

	default:
	    usage();
	    break;
	}
    }

    if (!expire_days) usage();

    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = sighandler;
    if (sigaction(SIGQUIT, &action, NULL) < 0) {
        fatal("unable to install signal handler for %d: %m", SIGQUIT);
    }

    cyrus_init(alt_config, "cyr_expire", 0);
    global_sasl_init(1, 0, NULL);

    annotatemore_init(0, NULL, NULL);
    annotatemore_open(NULL);

    mboxlist_init(0);
    mboxlist_open(NULL);

    /* open the quota db, we'll need it for expunge */
    quotadb_init(0);
    quotadb_open(NULL);

    if (duplicate_init(NULL, 0) != 0) {
	fprintf(stderr, 
		"cyr_expire: unable to init duplicate delivery database\n");
	exit(1);
    }

    if (do_expunge) {
	/* xxx better way to determine a size for this table? */

	/* expire messages from mailboxes,
	 * build a hash table of mailboxes in which we expired messages,
	 * and perform a cleanup of expunged messages
	 */
	if (expunge_days == -1) {
	    erock.expunge_mark = 0;
	} else {
	    erock.expunge_mark = time(0) - (expunge_days * 60 * 60 * 24);

	    if (verbose) {
		fprintf(stderr,
			"Expunging deleted messages in mailboxes older than %d days\n",
			expunge_days);
	    }
	}

	mboxlist_findall(NULL, find_prefix, 1, 0, 0, expire, &erock);

	syslog(LOG_NOTICE, "Expired %lu and expunged %lu out of %lu "
			    "messages from %lu mailboxes",
			   erock.messages_expired,
			   erock.messages_expunged,
			   erock.messages_seen,
			   erock.mailboxes_seen);
	if (verbose) {
	    fprintf(stderr, "\nExpired %lu and expunged %lu out of %lu "
			    "messages from %lu mailboxes\n",
			   erock.messages_expired,
			   erock.messages_expunged,
			   erock.messages_seen,
			   erock.mailboxes_seen);
	}
    }
    if (sigquit) {
	goto finish;
    }

    if ((delete_days != -1) && mboxlist_delayed_delete_isenabled() &&
	config_getstring(IMAPOPT_DELETEDPREFIX)) {
        struct delete_node *node;
        int count = 0;
        
        if (verbose) {
            fprintf(stderr,
                    "Removing deleted mailboxes older than %d days\n",
                    delete_days);
        }

        drock.delete_mark = time(0) - (delete_days * 60 * 60 * 24);
        drock.head = NULL;
        drock.tail = NULL;

        mboxlist_findall(NULL, find_prefix, 1, 0, 0, delete, &drock);

        for (node = drock.head ; node ; node = node->next) {
	    if (sigquit) {
		goto finish;
	    }
            if (verbose) {
                fprintf(stderr, "Removing: %s\n", node->name);
            }
            r = mboxlist_deletemailbox(node->name, 1, NULL, NULL, 0, 0, 0);
            count++;
        }

        if (verbose) {
            if (count != 1) {
                fprintf(stderr, "Removed %d deleted mailboxes\n", count);
            } else {
                fprintf(stderr, "Removed 1 deleted mailbox\n");
            }
        }
        syslog(LOG_NOTICE, "Removed %d deleted mailboxes", count);
    }
    if (sigquit) {
	goto finish;
    }

    /* purge deliver.db entries of expired messages */
    r = duplicate_prune(expire_days, &erock.table);

finish:
    free_hash_table(&erock.table, free);

    quotadb_close();
    quotadb_done();
    mboxlist_close();
    mboxlist_done();
    annotatemore_close();
    annotatemore_done();
    duplicate_done();
    sasl_done();
    cyrus_done();

    exit(r);
}
