/*
 * This file is part of buxton.
 *
 * Copyright (C) 2013 Intel Corporation
 *
 * buxton is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 */

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <check.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "buxtonsimple.h"
#include "buxtonsimple-internal.h"

#ifdef NDEBUG
#error "re-run configure with --enable-debug"
#endif

#define BUXTON_ROOT_CHECK_ENV "BUXTON_ROOT_CHECK"

/* start libbuxtonsimple test */
START_TEST (buxtond_set_group_check)
{

}
END_TEST

START_TEST (buxtond_set_int32_check)
{

}
END_TEST

START_TEST (buxtond_get_int32_check)
{

}
END_TEST

START_TEST (buxtond_set_string_check)
{

}
END_TEST

START_TEST (buxtond_get_string_check)
{

}
END_TEST

START_TEST (buxtond_set_uint32_check)
{

}
END_TEST

START_TEST (buxtond_get_uint32_check)
{

}
END_TEST

START_TEST (buxtond_set_int64_check)
{

}
END_TEST

START_TEST (buxtond_get_int64_check)
{

}
END_TEST

START_TEST (buxtond_set_uint64_check)
{

}
END_TEST

START_TEST (buxtond_get_uint64_check)
{

}
END_TEST

START_TEST (buxtond_set_float_check)
{

}
END_TEST

START_TEST (buxtond_get_float_check)
{

}
END_TEST

START_TEST (buxtond_set_double_check)
{

}
END_TEST

START_TEST (buxtond_get_double_check)
{

}
END_TEST

START_TEST (buxtond_set_bool_check)
{

}
END_TEST

START_TEST (buxtond_get_bool_check)
{

}
END_TEST

START_TEST (buxtond_remove_group_check)
{

}
END_TEST

/* Start buxtonsimple-internal tests */
START_TEST (save_errno_check)
{

}
END_TEST

START_TEST (sbuxton_open_check)
{

}
END_TEST

START_TEST (sbuxton_close_check)
{

}
END_TEST

START_TEST (client_connection_check)
{

}
END_TEST

START_TEST (client_disconnect_check)
{

}
END_TEST

START_TEST (cg_cb_check)
{

}
END_TEST

START_TEST (bs_print_check)
{

}
END_TEST

START_TEST (bs_cb_check)
{

}
END_TEST

START_TEST (bg_cb_check)
{

}
END_TEST

START_TEST (buxton_group_create_check)
{

}
END_TEST

START_TEST (rg_cb_check)
{

}
END_TEST
