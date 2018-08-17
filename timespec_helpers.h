/*
 * Helpers to POSIX time structures
 * Copyright (C) 2018  Ricardo Biehl Pasquali
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * 14/08/2018
 *
 * helpers to POSIX timespec structure
 *
 * - timespec_normalize()
 * - timespec_equal()
 * - timespec_compare()
 * - timespec_is_valid()
 * - timespec_to_ns()
 * - timespec_from_ns()
 * - timespec_add()
 * - timespec_sub()
 * - timespec_add_ns()
 * - timespec_sub_ns()
 */

#ifndef TIME_HELPERS_H
#define TIME_HELPERS_H

/*
 * check for C99
 *
 * see timespec_normalize() below
 */
#if __STDC_VERSION__ < 199901L
#error "Needs support to C99 standard for the sign of \
the result of the remainder operation to be the same as \
the dividend's one"
#endif

#include <time.h>

#define MSEC_PER_SEC  1000
#define USEC_PER_SEC  1000000
#define NSEC_PER_SEC  1000000000

/*
 * set seconds and nanoseconds fields of a timespec
 * variable and normalize to the timespec storage format
 *
 * A normalized timespec is defined by:
 *   0 <= tv_nsec < NSEC_PER_SEC
 * For negative values only the tv_sec field is negative!
 *
 * Note regarding the remainder operation done in this
 * function:
 * According to C99 standard, "6.5.5 Multiplicative
 * operators", the sign of the result of the remainder
 * operation is the same as the dividend's one. This is
 * inferred from: (a / b) * b + a % b = a
 */
static inline struct timespec
timespec_normalize(time_t sec, int64_t nsec)
{
	sec += nsec / NSEC_PER_SEC;
	nsec = nsec % NSEC_PER_SEC;

	/* assure nsec >= 0 */
	if (nsec < 0) {
		nsec += NSEC_PER_SEC;
		--sec;
	}

	return (struct timespec) {sec, nsec};
}

/* Return 1 if equal, otherwise 0. */
static inline int
timespec_equal(const struct timespec *a, const struct timespec *b)
{
	return (a->tv_sec == b->tv_sec) && (a->tv_nsec == b->tv_nsec);
}

/*
 * a < b:  return <0
 * a == b: return 0
 * a > b:  return >0
 */
static inline int
timespec_compare(const struct timespec *a, const struct timespec *b)
{
	if (a->tv_sec < b->tv_sec)
		return -1;
	if (a->tv_sec > b->tv_sec)
		return 1;
	return a->tv_nsec - b->tv_nsec;
}

/* Return 1 if timespec is valid, 0 otherwise. */
static inline int
timespec_is_valid(const struct timespec *t)
{
	/* second must be greater or equal zero */
	if (t->tv_sec < 0)
		return 0;

	/* nanosecond must be less than a second */
	if ((unsigned long) t->tv_nsec >= NSEC_PER_SEC)
		return 0;

	return 1;
}

static inline int64_t
timespec_to_ns(const struct timespec *t)
{
	return ((int64_t) t->tv_sec * NSEC_PER_SEC) + t->tv_nsec;
}

static inline struct timespec
timespec_from_ns(const int64_t nsec)
{
	return timespec_normalize(0, nsec);
}

static inline struct timespec
timespec_add(struct timespec a, struct timespec b)
{
	return timespec_normalize(a.tv_sec + b.tv_sec, a.tv_nsec + b.tv_nsec);
}

/* return a - b */
static inline struct timespec
timespec_sub(struct timespec a, struct timespec b)
{
	return timespec_normalize(a.tv_sec - b.tv_sec, a.tv_nsec - b.tv_nsec);
}

static inline void
timespec_add_ns(struct timespec *t, uint64_t nsec)
{
	*t = timespec_normalize(t->tv_sec, t->tv_nsec + nsec);
}

static inline void
timespec_sub_ns(struct timespec *t, uint64_t nsec)
{
	*t = timespec_normalize(t->tv_sec, t->tv_nsec - nsec);
}

#endif /* TIME_HELPERS_H */
