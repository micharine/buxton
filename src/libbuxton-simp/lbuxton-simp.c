#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "buxton.h"

static BuxtonClient client=NULL;
static int fd =-1;
/*remove global and do local open and close*/
static char _layer[256];
static char _group[256];
typedef struct vstatus {
	int status;
	BuxtonDataType type;
	union {
		char * sval;
		int32_t i32val;
		uint32_t ui32val;
		int64_t i64val;
		uint64_t ui64val;
		float fval;
		double dval;
		bool bval;
	};
} vstatus;
/*Save errno*/
int saved_errno;
void save_errno(void)
{
	saved_errno = errno;
}
/*Open client connection*/
void sbuxton_open(void)
{
	if ((fd= buxton_open(&client)) <0 ){
		printf("Couldn't connect.\n");
		return;
	}
	printf("Connection successful.\n");
}
/*Make sure client connection is open*/
void client_connection(void)
{
	/*Check if client connection is open*/
	if(!client){
		/*Open connection if needed*/
		sbuxton_open();
	}
}
/*Create group callback*/
void cg_cb(BuxtonResponse response, void *data)
{
	int *status = (int *)data;
	*status = 0;
	if (buxton_response_status(response) != 0){
		printf("Failed to create group.\n");
	} else {
		printf("Created group.\n");
		*status = 1;
	}
}
/*Initialization of group*/
void buxtond_set_group(char *group, char *layer)
{
	client_connection();
	save_errno();
	if(_layer){
		free(_layer);
	}
	if(_group){
		free(_group);
	}
	_layer = strcpy(layer);
	/*add cleanup for strdups*/
	if(!_layer){
		printf("Layer assignment failed. Aborting operation.\n");
		return;
	}
	_group = strcpy(group);
	if(!_group){
		printf("Group assignment failed. Aborting operation.\n");
		return;
	}
	BuxtonKey g = buxton_key_create(_group, NULL, _layer, STRING);
	int status;
	if (buxton_create_group(client, g, cg_cb, &status, true)){
		printf("Create group call failed.\n");
	} else {
		printf("Switched to group: %s, layer: %s.\n", buxton_key_get_group(g), buxton_key_get_layer(g));
		errno = saved_errno;
	}
	buxton_key_free(g);
}
/*buxton_set_value callback for all buxton data types*/
void bs_cb(BuxtonResponse response, void *data){
	vstatus *ret = (vstatus*)data;
	char * type;
	ret->status = 0;
	/*check response before switch*/
	if (buxton_response_status(response)){
		printf("Failed to set value.\n");
		return;
	}
	ret->status =1;
	switch(ret->type){
		case STRING:
		{
			char * val = ret->sval;
			type = "string";
			printf("Success: value has been set: %s(%s). ", val, type);
			break;
		}
		case INT32:
		{
			int32_t val = ret->i32val;
			type = "int32_t";
			printf("Success: value has been set: %i(%s). ", val, type);
			break;
		}
		case UINT32:
		{
			uint32_t val = ret->ui32val;
			type = "uint32_t";
			printf("Success: value has been set: %i(%s). ", val, type);
			break;
		}
		case INT64:
		{
			int64_t val = ret->i64val;
			type = "int64_t";
			printf("Success: value has been set: ""%"PRId64"(%s). ", val, type);
			break;
		}
		case UINT64:
		{
			uint64_t val = ret->ui64val;
			type = "uint64_t";
			printf("Success: value has been set: ""%"PRIu64"(%s). ", val, type);
			break;
		}
		case FLOAT:
		{
			float val = ret->fval;
			type = "float";
			printf("Success: value has been set: %f(%s). ", val, type);
			break;
		}
		case DOUBLE:
		{
			double val = ret->dval;
			type = "double";
			printf("Success: value has been set: %e(%s). ", val, type);
			break;
		}
		case BOOLEAN:
		{
			bool val = ret->bval;
			type = "bool";
			printf("Success: value has been set: %i(%s). ", val, type);
			break;
		}
	}
	BuxtonKey k = buxton_response_key(response);
	printf("Key: %s, Group: %s, Layer: %s.\n", buxton_key_get_name(k), buxton_key_get_group(k), buxton_key_get_layer(k));
	buxton_key_free(k);	

}
void bg_cb(BuxtonResponse response, void *data)
{
	vstatus *ret= (vstatus *)data;
	vstatus get = *ret;
	char *type;
	switch (get.type){
		case STRING:
		{
			ret->sval = *(char**)buxton_response_value(response);
			type = "string";
			break;
		}
		case INT32:
		{
			ret->i32val = *(int32_t*)buxton_response_value(response);
			type = "int32_t";
			break;
		}
		case UINT32:
		{
			ret->ui32val = *(uint32_t*)buxton_response_value(response);
			type = "uint32_t";
			break;
		}
		case INT64:
		{
			ret->i64val = *(int64_t*)buxton_response_value(response);
			type = "int64_t";
			break;
		}
		case UINT64:
		{
			ret->ui64val = *(uint64_t*)buxton_response_value(response);
			type = "uint64_t";
			break;
		}
		case FLOAT:
		{
			ret->fval = *(float*)buxton_response_value(response);
			type = "float";
			break;
		}
		case DOUBLE:
		{
			ret->dval = *(double*)buxton_response_value(response);
			type = "double";
			break;
		}
		case BOOLEAN:
		{
			ret->bval = *(bool*)buxton_response_value(response);
			type = "bool";
			break;
		}
	}
	ret->status = 0;
	if (buxton_response_status(response)){
		printf("Failed to get %s. \n", type);
		return;
	}
	ret->status = 1;
}
/*buxton_set_value callback and function for int32_t*/
void bsi32_cb(BuxtonResponse response, void *data)
{
	int32_t *ret=(int32_t*)data;
	if (buxton_response_status(response)){
		printf("Failed to set int32_t.\n");
		return;
	}
	printf("Value has been set: %i(int32_t)\n", *ret);
	BuxtonKey k = buxton_response_key(response);
	printf("Key: %s, Group: %s, Layer: %s.\n", buxton_key_get_name(k), buxton_key_get_group(k), 
		buxton_key_get_layer(k));	
}
void buxtond_set_int32(char *key, int32_t value)
{
	/*make sure client connection is open*/
	client_connection();
	/*check if a key has been created*/
	/*create key */
	BuxtonKey _key = buxton_key_create(_group, strdup(key), _layer, INT32);
	/*Return value and status*/
	vstatus ret;
	ret.type = INT32;
	ret.i32val = value;
	save_errno();
	/*call buxton_set_value for type INT32*/
	if (buxton_set_value(client, _key, &value, bs_cb, &ret, true)){
		printf("Set int32_t call failed.\n");
		return;
	}
	if (!ret.status){
		errno = EACCES;
	} else {
		errno = saved_errno;
	}
	buxton_key_free(_key);
}
/*buxton_get_value callback and function for int32_t*/
void bgi32_cb(BuxtonResponse response, void *data)
{
	vstatus *ret = (vstatus*)data;
	ret->status=0;
	if (buxton_response_status(response)){
		printf("Failed to get int32_t.\n");
		return;
	}
	ret->status=1;
	ret->i32val = *(int32_t*)buxton_response_value(response);
}
int32_t buxtond_get_int32(char *key)
{
	/*make sure client connection is open*/
	client_connection();
	/*create key*/
	BuxtonKey _key = buxton_key_create(_group, strdup(key), _layer, INT32);
	/*return value*/
	vstatus ret;
	ret.type = INT32;
	save_errno();
	/*get value*/
	if (buxton_get_value(client, _key, bg_cb, &ret, true)){
		printf("Get int32_t call failed.\n");
	}
	if (!ret.status){
		errno = EACCES;
	} else {
		errno = saved_errno;
	}
	buxton_key_free(_key);
	return ret.i32val;
}
/*buxton_set_value callback and function for string*/
void bss_cb(BuxtonResponse response, void *data)
{
	char **ret=(char**)data;
	if (buxton_response_status(response)){
		printf("Failed to set string.\n");
		return;
	}
	printf("Value has been set: %s(string)\n", *ret);
	BuxtonKey k = buxton_response_key(response);
	printf("Key: %s, Group: %s, Layer: %s.\n", buxton_key_get_name(k), buxton_key_get_group(k),
		buxton_key_get_layer(k));
}
void buxtond_set_string(char *key, char *value )
{
	/*make sure client connection is open*/
	client_connection();
	/*create key*/
	BuxtonKey _key = buxton_key_create(_group, strdup(key), _layer, STRING);
	/*Return value and status*/
	vstatus ret;
	ret.type = STRING;
	ret.sval = value;
	save_errno();
	/*set value*/
	if (buxton_set_value(client, _key, &value, bs_cb, &ret, true)){
		printf("Set string call failed.\n");
	}
	if (!ret.status){
		errno = EACCES;
	} else {
		errno = saved_errno;
	}
	buxton_key_free(_key);
}
/*buxton_get_value callback and function for string*/
void bgs_cb(BuxtonResponse response, void *data)
{
	vstatus *ret = (vstatus*)data;
	ret->status=0;
	if (buxton_response_status(response)){
		printf("Failed to get string.\n");
		return;
	}
	ret->status=1;
	ret->sval = *(char**)buxton_response_value(response);
}
char* buxtond_get_string(char *key)
{
	/*make sure client connection is open*/
	client_connection();
	/*create key*/
	BuxtonKey _key = buxton_key_create(_group, strdup(key), _layer, STRING);
	/*return value*/
	vstatus ret;
	ret.type = STRING;
	save_errno();
	/*get value*/
	if (buxton_get_value(client, _key, bg_cb, &ret, true)){
		printf("Get string call failed.\n");
	}
	if (!ret.status){
		errno = EACCES;
	} else {
		errno = saved_errno;
	}
	buxton_key_free(_key);
	return ret.sval;
}
/*buxton_set_value callback and function for uint32_t*/
void bsui32_cb(BuxtonResponse response, void *data)
{
	uint32_t *ret=(uint32_t*)data;
	if (buxton_response_status(response)){
		printf("Failed to set uint32_t.\n");
		return;
	}
	printf("Value has been set: %i(uint32_t)\n", *ret);
	BuxtonKey k = buxton_response_key(response);
	printf("Key: %s, Group: %s, Layer: %s.\n", buxton_key_get_name(k), buxton_key_get_group(k),
		buxton_key_get_layer(k));
}
void buxtond_set_uint32(char *key, uint32_t value)
{
	/*make sure client connection is open*/
	client_connection();
	/*create key*/
	BuxtonKey _key = buxton_key_create(_group, strdup(key), _layer, UINT32);
	/*Return value and status*/
	vstatus ret;
	ret.type = UINT32;
	ret.ui32val = value;
	save_errno();
	if (buxton_set_value(client,_key, &value, bs_cb, &ret, true)){
		printf("Set uint32_t call failed.\n");
	}
	if (!ret.status){
		errno = EACCES;
	} else {
		errno = saved_errno;
	}
	buxton_key_free(_key);
}
/*buxton_get_value callback and function for uint32_t*/
void bgui32_cb(BuxtonResponse response, void *data)
{
	vstatus *ret = (vstatus*)data;
	ret->status=0;
	if (buxton_response_status(response)){
		printf("Failed to get uint32_t.\n");
		return;
	}		
	ret->status = 1;
	ret->i32val = *(uint32_t*)buxton_response_value(response);
}
uint32_t buxtond_get_uint32(char *key)
{
	/*make sure client connection is open*/
	client_connection();
	/*create key*/
	BuxtonKey _key = buxton_key_create(_group, strdup(key), _layer, UINT32);
	/*return value*/
	vstatus ret;
	ret.type = UINT32;
	save_errno();
	/*get value*/
	if (buxton_get_value(client, _key, bg_cb, &ret, true)){
		printf("Get uint32_t call failed.\n");
	}
	if (!ret.status){
		errno = EACCES;
	} else {
		errno = saved_errno;
	}
	buxton_key_free(_key);
	return ret.ui32val;
}
/*buxton_set_value callback and function for int64_t*/
void bsi64_cb(BuxtonResponse response, void *data)
{
	int64_t *ret = (int64_t*)data;
	if (buxton_response_status(response)){
		printf("Failed to set int64_t.\n");
		return;
	}
	printf("Value has been set: ""%"PRId64"(int64_t)\n", *ret);
	BuxtonKey k = buxton_response_key(response);
	printf("Key: %s, Group: %s, Layer: %s.\n", buxton_key_get_name(k), buxton_key_get_group(k),
		buxton_key_get_layer(k));
}
void buxtond_set_int64(char *key, int64_t value)
{
	/*make sure client connection is open*/
	client_connection();
	/*create key*/
	BuxtonKey _key = buxton_key_create(_group, strdup(key), _layer, INT64);
	/*Return value and status*/
	vstatus ret;
	ret.type = INT64;
	ret.i64val = value;
	save_errno();
	if(buxton_set_value(client, _key, &value, bs_cb, &ret, true)){
		printf("Set int64_t call failed.\n");
	}
	if (!ret.status){
		errno = EACCES;
	} else {
		errno = saved_errno;
	}
	buxton_key_free(_key);
}
/*buxton_get_value callback and function for int64_t*/
void bgi64_cb(BuxtonResponse response, void *data)
{
	vstatus *ret = (vstatus*)data;
	ret->status=0;
	if (buxton_response_status(response)){
		printf("Failed to get int64_t.\n");
		return;
	}
	ret->status = 1;
	ret->i64val = *(int64_t*)buxton_response_value(response);
}
int64_t buxtond_get_int64(char *key)
{
	/*make sure client connection is open*/
	client_connection();
	/*create key*/
	BuxtonKey _key = buxton_key_create(_group, strdup(key), _layer, INT64);
	/*return value*/
	vstatus ret;
	ret.type = INT64;
	save_errno();
	/*get value*/
	if (buxton_get_value(client, _key, bg_cb, &ret, true)){
		printf("Get int64_t call failed.\n");
	}
	if (!ret.status){
		errno = EACCES;
	} else {
		errno = saved_errno;
	}
	buxton_key_free(_key);
	return ret.i64val;
}
/*buxton_set_value callback and function for uint64_t*/
void bsui64_cb(BuxtonResponse response, void *data)
{
	uint64_t *ret = (uint64_t*)data;
	if (buxton_response_status(response)){
		printf("Failed to set uint64_t.\n");
		return;
	}
	printf("Value has been set: ""%"PRIu64"(uint64_t)\n", *ret);
	BuxtonKey k = buxton_response_key(response);
	printf("Key: %s, Group: %s, Layer: %s.\n", buxton_key_get_name(k), buxton_key_get_group(k),
		buxton_key_get_layer(k));
}
void buxtond_set_uint64(char *key, uint64_t value)
{
	/*make sure client connection is open*/
	client_connection();
	/*create key*/
	BuxtonKey _key = buxton_key_create(_group, strdup(key), _layer, UINT64);
	/*Return value and status*/
	vstatus ret;
	ret.type = UINT64;
	ret.ui64val = value;
	save_errno();
	if(buxton_set_value(client, _key, &value, bs_cb, &ret, true)){
		printf("Set uint64_t call failed.\n");
	}
	if (!ret.status){
		errno = EACCES;
	} else {
		errno = saved_errno;
	}
	buxton_key_free(_key);
}
/*buxton_get_value callback and function for uint64_t*/
void bgui64_cb(BuxtonResponse response, void *data)
{
	vstatus *ret = (vstatus*)data;
	ret->status=0;
	if (buxton_response_status(response)){
		printf("Failed to get uint64_t.\n");
		return;
	}
	ret->status = 1;
	ret->ui64val = *(uint64_t*)buxton_response_value(response);
}
uint64_t buxtond_get_uint64(char *key)
{
	/*make sure client connection is open*/
	client_connection();
	/*create key*/
	BuxtonKey _key = buxton_key_create(_group, strdup(key), _layer, UINT64);
	/*return value*/
	vstatus ret;
	ret.type = UINT64;
	save_errno();
	/*get value*/
	if (buxton_get_value(client, _key, bg_cb, &ret, true)){
		printf("Get uint64_t call failed.\n");
	}
	if (!ret.status){
		errno = EACCES;
	} else {
		errno = saved_errno;
	}
	buxton_key_free(_key);
	return ret.ui64val;
}
/*buxton_set_value callback and function for float*/
void bsf_cb(BuxtonResponse response, void *data)
{
	float *ret = (float*)data;
	if (buxton_response_status(response)){
		printf("Failed to set float.\n");
		return;
	}
	printf("Value has been set: %f(float)\n", *ret);
	BuxtonKey k = buxton_response_key(response);
	printf("Key: %s, Group: %s, Layer: %s.\n", buxton_key_get_name(k), buxton_key_get_group(k),
		buxton_key_get_layer(k));
}
void buxtond_set_float(char *key, float value)
{
	/*make sure client connection is open*/
	client_connection();
	/*create key*/
	BuxtonKey _key = buxton_key_create(_group, strdup(key), _layer, FLOAT);
	/*Return value and status*/
	vstatus ret;
	ret.type = FLOAT;
	ret.fval = value;
	save_errno();
	if(buxton_set_value(client, _key, &value, bs_cb, &ret, true)){
		printf("Set float call failed.\n");
	}
	if (!ret.status){
		errno = EACCES;
	} else {
		errno = saved_errno;
	}
	buxton_key_free(_key);
}
/*buxton_get_value callback and function for float*/
void bgf_cb(BuxtonResponse response, void *data)
{
	vstatus *ret = (vstatus*)data;
	ret->status=0;
	if (buxton_response_status(response)){
		printf("Failed to get float.\n");
		return;
	}
	ret->status = 1;
	ret->fval = *(float*)buxton_response_value(response);
}
float buxtond_get_float(char *key)
{
	/*make sure client connection is open*/
	client_connection();
	/*create key*/
	BuxtonKey _key = buxton_key_create(_group, strdup(key), _layer, FLOAT);
	/*return value*/
	vstatus ret;
	ret.type = FLOAT;
	save_errno();
	/*get value*/
	if (buxton_get_value(client, _key, bg_cb, &ret, true)){
		printf("Get float call failed.\n");
	}
	if (!ret.status){
		errno = EACCES;
	} else {
		errno = saved_errno;
	}
	buxton_key_free(_key);
	return ret.fval;
}
/*buxton_set_value callback and function for double*/
void bsd_cb(BuxtonResponse response, void *data)
{
	double *ret = (double*)data;
	if (buxton_response_status(response)){
		printf("Failed to set double.\n");
		return;
	}
	printf("Value has been set: %e(double)\n", *ret);
	BuxtonKey k = buxton_response_key(response);
	printf("Key: %s, Group: %s, Layer: %s.\n", buxton_key_get_name(k), buxton_key_get_group(k),
		buxton_key_get_layer(k));
}
void buxtond_set_double(char *key, double value)
{
	/*make sure client connection is open*/
	client_connection();
	/*create key*/
	BuxtonKey _key = buxton_key_create(_group, strdup(key), _layer, DOUBLE);
	/*Return value and status*/
	vstatus ret;
	ret.type = DOUBLE;
	ret.dval = value;
	save_errno();
	if(buxton_set_value(client, _key, &value, bs_cb, &ret, true)){
		printf("Set double call failed.\n");
	}
	if (!ret.status){
		errno = EACCES;
	} else {
		errno = saved_errno;
	}
	buxton_key_free(_key);
}
/*buxton_get_value callback and function for double*/
void bgd_cb(BuxtonResponse response, void *data)
{
	vstatus *ret = (vstatus*)data;
	ret->status=0;
	if (buxton_response_status(response)){
		printf("Failed to get double.\n");
		return;
	}
	ret->status = 1;
	ret->dval = *(double*)buxton_response_value(response);
}
double buxtond_get_double(char *key)
{
	/*make sure client connection is open*/
	client_connection();
	/*create key*/
	BuxtonKey _key = buxton_key_create(_group, strdup(key), _layer, DOUBLE);
	/*return value*/
	vstatus ret;
	ret.type = DOUBLE;
	save_errno();
	/*get value*/
	if (buxton_get_value(client, _key, bg_cb, &ret, true)){
		printf("Get double call failed.\n");
	}
	if (!ret.status){
		errno = EACCES;
	} else {
		errno = saved_errno;
	}
	buxton_key_free(_key);
	return ret.dval;
}
/*buxton_set_value callback and function for boolean*/
void bsb_cb(BuxtonResponse response, void *data)
{
	bool *ret = (bool*)data;
	if(buxton_response_status(response)){
		printf("Failed to set boolean.\n");
		return;
	}
	printf("Value has been set: %i(bool)\n", *ret);
	BuxtonKey k = buxton_response_key(response);
	printf("Key: %s, Group: %s, Layer: %s.\n", buxton_key_get_name(k), buxton_key_get_group(k),
		buxton_key_get_layer(k));
}
void buxtond_set_bool(char *key, bool value)
{
	/*make sure client connection is open*/
	client_connection();
	/*create key*/
	BuxtonKey _key = buxton_key_create(_group, strdup(key), _layer, BOOLEAN);
	/*Return value and status*/
	vstatus ret;
	ret.type = BOOLEAN;
	ret.bval = value;
	save_errno();
	if(buxton_set_value(client, _key, &value, bs_cb, &ret, true)){
		printf("Set bool call failed.\n");
	}
	if (!ret.status){
		errno = EACCES;
	} else {
		errno = saved_errno;
	}
	buxton_key_free(_key);
}
/*buxton_get_value callback and function for boolean*/
void bgb_cb(BuxtonResponse response, void *data)
{
	vstatus *ret = (vstatus*)data;
	ret->status=0;
	if (buxton_response_status(response)){
		printf("Failed to get boolean.\n");
		return;
	}
	ret->status = 1;
	ret->bval = *(bool*)buxton_response_value(response);
}
bool buxtond_get_bool(char *key)
{
	/*make sure client connection is open*/
	client_connection();
	/*create key*/
	BuxtonKey _key = buxton_key_create(_group, strdup(key), _layer, BOOLEAN);
	/*return value*/
	vstatus ret;
	ret.type = BOOLEAN;
	save_errno();
	/*get value*/
	if (buxton_get_value(client, _key, bgb_cb, &ret, true)){
		printf("Get bool call failed.\n");
	}
	if (!ret.status){
		errno = EACCES;
	} else {
		errno = saved_errno;
	}
	buxton_key_free(_key);
	return ret.bval;
}
/*create a client side group TODO: create BuxtonGroup type*/
BuxtonKey buxton_group_create(char *name, char *layer)
{
	client_connection();
	BuxtonKey ret = buxton_key_create(name, NULL, layer, STRING);
	return ret;
}
/*create a buxtond side group*/
void buxtond_create_group(BuxtonKey group)
{
	client_connection();
	if (buxton_create_group(client, group, NULL, NULL, true)){
		printf("Create group call failed.\n");
		buxton_key_free(group);
		return;
	}
	/*TODO*/
	//buxton_key_free(group);
}
BuxtonKey buxtond_create_group2(char *group_name, char *layer)
{
	client_connection();
	BuxtonKey group = buxton_key_create(group_name, NULL, layer, STRING);
	if (buxton_create_group(client, group, NULL, NULL, true)){
		printf("Create group call failed.\n");
		buxton_key_free(group);
		return;
	}
	return group;
	/*Buxton free key? TODO*/
}
/*buxton_remove_group callback and function*/
void rg_cb(BuxtonResponse response, void *data)
{
	if (buxton_response_status(response) != 0){
		printf("Failed to remove group.\n");
	} else {
		printf("Removed group.\n");
	}
}
void buxtond_remove_group(BuxtonKey group)
{
	client_connection();
	if (buxton_remove_group(client, group, rg_cb, NULL, true)){
		printf("Remove group call failed.\n");
	}
}
void buxtond_remove_group2(char *group_name, char *layer)
{
	client_connection();
	BuxtonKey group = buxton_group_create(group_name, layer);
	if (buxton_remove_group(client, group, rg_cb, NULL, true)){
		printf("Remove group call failed.\n");
	}
	buxton_key_free(group);
}
/*buxton_key_free insert char key name and type*/
void buxtond_key_free(char * key_name, BuxtonDataType type)
{
	BuxtonKey k = buxton_key_create(_group, strdup(key_name), _layer, type);
	buxton_key_free(k);
}

