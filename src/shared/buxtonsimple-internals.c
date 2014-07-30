/*
 * This file is part of buxton.
 *
 * Copyright (C) 2014 Intel Corporation
 *
 * buxton is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the license, or (at your option) any later version.
 */
 
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buxton.h"
#include "buxtonsimple-internals.h"
#include "log.h"

BuxtonClient client=NULL;

/* Open client connection */
void _sbuxton_open(void)
{
	if ((buxton_open(&client)) <0 ) {
		buxton_debug("Couldn't connect.\n");
		return;
	}
	buxton_debug("Connection successful.\n");
}

/* Close client connection */
void _sbuxton_close(void)
{ 
	buxton_close(client);
	buxton_debug("Connection closed\n");
}

/* Make sure client connection is open */
void _client_connection(void)
{
	/* Check if client connection is open */
	if (!client) {
		/* Open connection if needed */
		_sbuxton_open();
	}
}

/* Close an open client connection */
void _client_disconnect(void)
{
	/* Only attempt to close the client if it != NULL */
	if (client) {
		/* Close the connection */
		_sbuxton_close();
		client = NULL;
	}
}

/* Create group callback */
void _cg_cb(BuxtonResponse response, void *data)
{
	if (buxton_response_status(response) != 0) {
		buxton_debug("Failed to create group (may already exist).\n");
	} else {
		buxton_debug("Created group.\n");
	}
}
void _bs_print(vstatus *data, BuxtonResponse response)
{
	switch (data->type) {
		case STRING:
		{
			char * val = data->val.sval;
			buxton_debug("Success: value has been set: %s(string). ", val);
			break;
		}
		case INT32:
		{
			int32_t val = data->val.i32val;
			buxton_debug("Success: value has been set: %d(int32_t). ", val);
			break;
		}
		case UINT32:
		{
			uint32_t val = data->val.ui32val;
			buxton_debug("Success: value has been set: %d(uint32_t). ", val);
			break;
		}
		case INT64:
		{
			int64_t val = data->val.i64val;
			buxton_debug("Success: value has been set: ""%"PRId64"(int64_t). ", val);
			break;
		}
		case UINT64:
		{
			uint64_t val = data->val.ui64val;
			buxton_debug("Success: value has been set: ""%"PRIu64"(uint64_t). ", val);
			break;
		}
		case FLOAT:
		{
			float val = data->val.fval;
			buxton_debug("Success: value has been set: %f(float). ", val);
			break;
		}
		case DOUBLE:
		{
			double val = data->val.dval;
			buxton_debug("Success: value has been set: %e(double). ", val);
			break;
		}
		case BOOLEAN:
		{
			bool val = data->val.bval;
			buxton_debug("Success: value has been set: %i(bool). ", val);
			break;
		}
		default:
		{
			buxton_debug("Data type not found\n");
			break;
		}
	}
	BuxtonKey k = buxton_response_key(response);
	buxton_debug("Key: %s, Group: %s, Layer: %s.\n", buxton_key_get_name(k), buxton_key_get_group(k), buxton_key_get_layer(k));
	buxton_key_free(k);	
}

/* buxton_set_value callback for all buxton data types */
void _bs_cb(BuxtonResponse response, void *data){
	vstatus *ret = (vstatus*)data;
	ret->status = 0;
	/* check response before switch */
	if (buxton_response_status(response)) {
		buxton_debug("Failed to set value.\n");
		return;
	}
	ret->status =1;
	_bs_print(ret, response);

}

void _bg_cb(BuxtonResponse response, void *data)
{
	char *type;
	vstatus *ret= (vstatus *)data;
	switch (ret->type) {
		case STRING:
		{
			ret->val.sval = *(char**)buxton_response_value(response);
			type = "string";
			break;
		}
		case INT32:
		{
			ret->val.i32val = *(int32_t*)buxton_response_value(response);
			type = "int32_t";
			break;
		}
		case UINT32:
		{
			ret->val.ui32val = *(uint32_t*)buxton_response_value(response);
			type = "uint32_t";
			break;
		}
		case INT64:
		{
			ret->val.i64val = *(int64_t*)buxton_response_value(response);
			type = "int64_t";
			break;
		}
		case UINT64:
		{
			ret->val.ui64val = *(uint64_t*)buxton_response_value(response);
			type = "uint64_t";
			break;
		}
		case FLOAT:
		{
			ret->val.fval = *(float*)buxton_response_value(response);
			type = "float";
			break;
		}
		case DOUBLE:
		{
			ret->val.dval = *(double*)buxton_response_value(response);
			type = "double";
			break;
		}
		case BOOLEAN:
		{
			ret->val.bval = *(bool*)buxton_response_value(response);
			type = "bool";
			break;
		}
		default:
		{
			type = "unknown";
			break;
		}
	}
	ret->status = 0;
	if (buxton_response_status(response)) {
		buxton_debug("Failed to get %s. \n", type);
		return;
	}
	ret->status = 1;
}
/* create a client side group TODO: create BuxtonGroup type
	probably not really needed */
BuxtonKey _buxton_group_create(char *name, char *layer)
{
	BuxtonKey ret = buxton_key_create(name, NULL, layer, STRING);
	return ret;
}

/* buxton_remove_group callback and function */
void _rg_cb(BuxtonResponse response, void *data)
{
	if (buxton_response_status(response) != 0) {
		buxton_debug("Failed to remove group.\n");
	} else {
		buxton_debug("Removed group.\n");
	}
}

/*
 * Editor modelines  -	http://www.wireshark.org/tools/modelines.html
 *
 * Local variables:
 * c-basic-offset: 8
 * tab-width: 8
 * indent-tabs-mode: t
 * End:
 *
 * vi: set shiftwidth=8 tabstop=8 noexpandtab:
 * :indentSize=8:tabSize=8:noTabs=false:
 */


