/* libcoap unit tests
 *
 * Copyright (C) 2012,2015 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include "test_pdu.h"
#include "../inc/ucoap.h"
#include "../src/coap_type.h"
#include "../src/coap_pkt.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PKT_PARSE_TEST(s,t) if (!CU_ADD_TEST(s,t)) {fprintf(stderr, "W: cannot add p_pkt parser test (%s)\n",	CU_get_error_msg());}
#define PKT_BUILD_TEST(s,t) if (!CU_ADD_TEST(s,t)) {fprintf(stderr, "W: cannot add p_pkt encoder test (%s)\n", CU_get_error_msg());}

static void test_parse_pkt1_minimum_size()
{
	uint8 teststr[] = {  0x40, 0x01, 0x93, 0x34 };
	coap_pkt_t * p_pkt = coap_pkt_parse((unsigned char *)teststr, sizeof(teststr));
	
	CU_ASSERT_PTR_NOT_NULL(p_pkt);
	CU_ASSERT(p_pkt->packet_length == sizeof(teststr));
	CU_ASSERT(p_pkt->hdr.version == 1);
	CU_ASSERT(p_pkt->hdr.type == COAP_PKT_TYPE_CON);
	CU_ASSERT(p_pkt->hdr.token_length == 0);
	CU_ASSERT(p_pkt->hdr.code == COAP_PKT_CODE_GET);
	CU_ASSERT(memcmp(&p_pkt->hdr.message_id, teststr + 2, 2) == 0);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);

	coap_pkt_free(p_pkt);
}

static void test_parse_pkt2_token_is_correct() 
{
	uint8 teststr[] = {  0x55, 0x69, 0x12, 0x34, 't', 'o', 'k', 'e', 'n' };
	coap_pkt_t * p_pkt = coap_pkt_parse((unsigned char *)teststr, sizeof(teststr));
	
	CU_ASSERT_PTR_NOT_NULL(p_pkt);
	CU_ASSERT(p_pkt->packet_length == sizeof(teststr));
	CU_ASSERT(p_pkt->hdr.version == 1);
	CU_ASSERT(p_pkt->hdr.type == COAP_PKT_TYPE_NON);
	CU_ASSERT(p_pkt->hdr.token_length == 5);
	CU_ASSERT(p_pkt->hdr.code == 0x69);
	CU_ASSERT(memcmp(&p_pkt->hdr.message_id, teststr + 2, 2) == 0);
	CU_ASSERT(memcmp(&p_pkt->hdr.token, teststr + 4, 5) == 0);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);

	coap_pkt_free(p_pkt);
}

static void test_parse_pkt3_token_data_exceed() 
{
	uint8 teststr[] = {  0x53, 0x69, 0x12, 0x34, 't', 'o', 'k', 'e', 'n' };
	coap_pkt_t * p_pkt = coap_pkt_parse((unsigned char *)teststr, sizeof(teststr));
	CU_ASSERT_PTR_NULL(p_pkt);
}

static void test_parse_pkt4_token_length_exceed()
{
	uint8 teststr[] = {  0x59, 0x69, 0x12, 0x34, 't', 'o', 'k', 'e', 'n', '1', '2', '3', '4' };
	coap_pkt_t * p_pkt = coap_pkt_parse((unsigned char *)teststr, sizeof(teststr));
	CU_ASSERT_PTR_NULL(p_pkt);
	
	teststr[0] = 0x5f;
	p_pkt = coap_pkt_parse((unsigned char *)teststr, sizeof(teststr));
	CU_ASSERT_PTR_NULL(p_pkt);
}

static void test_parse_pkt5_with_options() 
{
	uint8 teststr[] = {0x55, 0x73, 0x12, 0x34, 't', 'o', 'k', 'e', 'n',  0x00, 0xc1, 0x00};
	coap_pkt_t * p_pkt = coap_pkt_parse((unsigned char *)teststr, sizeof(teststr));
	
	CU_ASSERT_PTR_NOT_NULL(p_pkt);
	CU_ASSERT(p_pkt->packet_length == sizeof(teststr));
	CU_ASSERT(p_pkt->hdr.version == 1);
	CU_ASSERT(p_pkt->hdr.type == COAP_PKT_TYPE_NON);
	CU_ASSERT(p_pkt->hdr.token_length == 5);
	CU_ASSERT(p_pkt->hdr.code == 0x73);
	CU_ASSERT(memcmp(&p_pkt->hdr.message_id, teststr + 2, 2) == 0);
	CU_ASSERT(memcmp(&p_pkt->hdr.token, teststr + 4, 5) == 0);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);

	coap_pkt_free(p_pkt);
}

static void test_parse_pkt6_option_length_exceed()
{
	uint8 teststr[] = {0x55, 0x73, 0x12, 0x34, 't', 'o', 'k', 'e', 'n',  0x00, 0xc1, 0x00, 0xae, 0xf0, 0x03};
	coap_pkt_t * p_pkt = coap_pkt_parse((unsigned char *)teststr, sizeof(teststr));
	CU_ASSERT_PTR_NULL(p_pkt);
}

static void test_parse_pkt7_with_options_and_payload() 
{
	uint8 teststr[] = {0x55, 0x73, 0x12, 0x34, 't', 'o', 'k', 'e', 'n',  0x00, 0xc1, 0x00, 0xff, 'p', 'a', 'y', 'l', 'o', 'a', 'd'};
	coap_pkt_t * p_pkt = coap_pkt_parse((unsigned char *)teststr, sizeof(teststr));
	
	CU_ASSERT_PTR_NOT_NULL(p_pkt);
	CU_ASSERT(p_pkt->packet_length == sizeof(teststr));
	CU_ASSERT(p_pkt->hdr.version == 1);
	CU_ASSERT(p_pkt->hdr.type == COAP_PKT_TYPE_NON);
	CU_ASSERT(p_pkt->hdr.token_length == 5);
	CU_ASSERT(p_pkt->hdr.code == 0x73);
	CU_ASSERT(memcmp(&p_pkt->hdr.message_id, teststr + 2, 2) == 0);
	CU_ASSERT(memcmp(&p_pkt->hdr.token, teststr + 4, 5) == 0);
	CU_ASSERT(p_pkt->p_data == (unsigned char *)&p_pkt->hdr + 13);
	CU_ASSERT(memcmp(p_pkt->p_data, teststr + 13, 7) == 0);

	coap_pkt_free(p_pkt);
}

static void test_parse_pkt8_without_options_with_payload() 
{
	uint8 teststr[] = {  0x50, 0x73, 0x12, 0x34, 0xff, 'p', 'a', 'y', 'l', 'o', 'a', 'd'};
	coap_pkt_t * p_pkt = coap_pkt_parse((unsigned char *)teststr, sizeof(teststr));

	CU_ASSERT_PTR_NOT_NULL(p_pkt);	
	CU_ASSERT(p_pkt->packet_length == sizeof(teststr));
	CU_ASSERT(p_pkt->hdr.version == 1);
	CU_ASSERT(p_pkt->hdr.type == COAP_PKT_TYPE_NON);
	CU_ASSERT(p_pkt->hdr.token_length == 0);
	CU_ASSERT(p_pkt->hdr.code == 0x73);
	CU_ASSERT(memcmp(&p_pkt->hdr.message_id, teststr + 2, 2) == 0);
	CU_ASSERT(p_pkt->p_data == (unsigned char *)&p_pkt->hdr + 5);
	CU_ASSERT(memcmp(p_pkt->p_data, teststr + 5, 7) == 0);

	coap_pkt_free(p_pkt);
}

static void test_parse_pkt9_without_options_and_payload_but_with_payload_marker() 
{
	uint8 teststr[] = {  0x70, 0x00, 0x12, 0x34, 0xff };
	coap_pkt_t * p_pkt = coap_pkt_parse((unsigned char *)teststr, sizeof(teststr));
	CU_ASSERT_PTR_NULL(p_pkt);
}

static void test_parse_pkt10_with_options_without_payload_but_with_payload_marker() 
{
	uint8 teststr[] = {0x53, 0x73, 0x12, 0x34, 't', 'o', 'k', 0x30, 0xc1, 0x00, 0xff};
	
	coap_pkt_t * p_pkt = coap_pkt_parse((unsigned char *)teststr, sizeof(teststr));
	CU_ASSERT_PTR_NULL(p_pkt);
}

static void test_parse_pkt11_code_zero_dot_zero() 
{
	uint8 teststr[] = {0x60, 0x00, 0x12, 0x34 };	
	coap_pkt_t * p_pkt = coap_pkt_parse((unsigned char *)teststr, sizeof(teststr));

	CU_ASSERT_PTR_NOT_NULL(p_pkt);
	CU_ASSERT(p_pkt->packet_length == sizeof(teststr));
	CU_ASSERT(p_pkt->hdr.version == 1);
	CU_ASSERT(p_pkt->hdr.type == COAP_PKT_TYPE_ACK);
	CU_ASSERT(p_pkt->hdr.token_length == 0);
	CU_ASSERT(p_pkt->hdr.code == 0);
	CU_ASSERT(memcmp(&p_pkt->hdr.message_id, teststr + 2, 2) == 0);

	coap_pkt_free(p_pkt);
}

static void test_parse_pkt12_reset() 
{
	uint8 teststr[] = {0x70, 0x00, 0x12, 0x34};
	coap_pkt_t * p_pkt = coap_pkt_parse((unsigned char *)teststr, sizeof(teststr));

	CU_ASSERT_PTR_NOT_NULL(p_pkt);
	CU_ASSERT(p_pkt->packet_length == sizeof(teststr));
	CU_ASSERT(p_pkt->hdr.version == 1);
	CU_ASSERT(p_pkt->hdr.type == COAP_PKT_TYPE_RST);
	CU_ASSERT(p_pkt->hdr.token_length == 0);
	CU_ASSERT(p_pkt->hdr.code == 0);
	CU_ASSERT(memcmp(&p_pkt->hdr.message_id, teststr + 2, 2) == 0);

	coap_pkt_free(p_pkt);
}

static void test_parse_pkt13_reset_with_content()
{
	uint8 teststr[] = {0x70, 0x00, 0x12, 0x34, 0xff, 'c', 'o', 'n', 't', 'e', 'n', 't'};
	
	coap_pkt_t * p_pkt = coap_pkt_parse((unsigned char *)teststr, sizeof(teststr));
	CU_ASSERT_PTR_NULL(p_pkt);
}

static void test_parse_pkt14_ack_with_content() 
{
	uint8 teststr[] = {0x60, 0x00, 0x12, 0x34, 0xff, 'c', 'o', 'n', 't', 'e', 'n', 't'};
	
	coap_pkt_t * p_pkt = coap_pkt_parse((unsigned char *)teststr, sizeof(teststr));
	CU_ASSERT_PTR_NULL(p_pkt);
}

static void test_build_pkt1_with_token()
{
	int result = -1;
	uint8 teststr[] = { 0x45, 0x01, 0x12, 0x34, 't', 'o', 'k', 'e', 'n' };
	coap_pkt_t * p_pkt = coap_pkt_alloc(COAP_PKT_MAX_SIZE);

	result = coap_pkt_add_header(p_pkt, COAP_PKT_TYPE_CON, COAP_PKT_CODE_GET, 0x3412);
	result = coap_pkt_add_token(p_pkt, (unsigned char *)"token", 5);
	CU_ASSERT(result == 0);
	CU_ASSERT(p_pkt->packet_length = sizeof(teststr));
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	CU_ASSERT(memcmp(&p_pkt->hdr, teststr, sizeof(teststr)) == 0);

	coap_pkt_free(p_pkt);
}

static void test_build_pkt2_with_token() 
{
	int result = 0;	
	coap_pkt_t * p_pkt = coap_pkt_alloc(7);
	result = coap_pkt_add_header(p_pkt, COAP_PKT_TYPE_CON, COAP_PKT_CODE_GET, 0x3412);
	result = coap_pkt_add_token(p_pkt, (unsigned char *)"token", 5);
	
	CU_ASSERT_PTR_NOT_NULL(p_pkt);
	CU_ASSERT(result < 0);

	coap_pkt_free(p_pkt);
}

static void test_build_pkt3_without_header_with_token()
{
	int result = 0;
	coap_pkt_t * p_pkt = coap_pkt_alloc(COAP_PKT_MAX_SIZE);
	result = coap_pkt_add_token(p_pkt, (unsigned char *)"123456789", 9);
	
	CU_ASSERT_PTR_NOT_NULL(p_pkt);
	CU_ASSERT(result < 0);

	coap_pkt_free(p_pkt);
}

static void
test_build_pkt4_with_options() {
	uint8 teststr[] = 
	{
		0x60, 0x99, 0x12, 0x34, 0x3d, 0x05, 0x66, 0x61,
		0x6e, 0x63, 0x79, 0x70, 0x72, 0x6f, 0x78, 0x79,
		0x2e, 0x63, 0x6f, 0x61, 0x70, 0x2e, 0x6d, 0x65,
		0x84, 0x70, 0x61, 0x74, 0x68, 0x00, 0xe8, 0x1e,
		0x28, 0x66, 0x61, 0x6e, 0x63, 0x79, 0x6f, 0x70,
		0x74
	};
	int result;
	coap_pkt_t * p_pkt = coap_pkt_alloc(COAP_PKT_MAX_SIZE);
	
	result = coap_pkt_add_header(p_pkt, COAP_PKT_TYPE_ACK, 0x99, 0x3412);
	
	CU_ASSERT(p_pkt->packet_length == 4);
	
	result = coap_pkt_add_option(p_pkt, COAP_PKT_OPTION_URI_HOST, (unsigned char *)"fancyproxy.coap.me", 18);
	
	CU_ASSERT(result == 0);
	CU_ASSERT(p_pkt->max_delta == 3);
	CU_ASSERT(p_pkt->packet_length == 24);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	result = coap_pkt_add_option(p_pkt, COAP_PKT_OPTION_URI_PATH, (unsigned char *)"path", 4);
	
	CU_ASSERT(result == 0);
	CU_ASSERT(p_pkt->max_delta == 11);
	CU_ASSERT(p_pkt->packet_length == 29);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	result = coap_pkt_add_option(p_pkt, COAP_PKT_OPTION_URI_PATH, NULL, 0);
	
	CU_ASSERT(result == 0);
	CU_ASSERT(p_pkt->max_delta == 11);
	CU_ASSERT(p_pkt->packet_length == 30);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	result = coap_pkt_add_option(p_pkt, 8000, (unsigned char *)"fancyopt", 8);
	
	CU_ASSERT(result == 0);
	CU_ASSERT(p_pkt->max_delta == 8000);
	CU_ASSERT(p_pkt->packet_length == 41);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	CU_ASSERT(p_pkt->packet_length == sizeof(teststr));
	CU_ASSERT(memcmp(&p_pkt->hdr, teststr, sizeof(teststr)) == 0);

	coap_pkt_free(p_pkt);
}

static void test_build_pkt5_with_token_and_options()
{
	uint8 teststr[] =
	{
		0x68, 0x84, 0x12, 0x34, '1',  '2',  '3',  '4',
		'5',  '6',  '7',  '8',  0x18, 0x41, 0x42, 0x43,
		0x44, 0x45, 0x46, 0x47, 0x48, 0xd1, 0x03, 0x12
	};
	int result;
	
	coap_pkt_t * p_pkt = coap_pkt_alloc(COAP_PKT_MAX_SIZE);
	
	result = coap_pkt_add_header(p_pkt, COAP_PKT_TYPE_ACK, COAP_PKT_MAKE_CODE(4,4), 0x3412);
	
	CU_ASSERT(p_pkt->packet_length == 4);
	
	result = coap_pkt_add_token(p_pkt, (unsigned char *)"12345678", 8);
	
	CU_ASSERT(p_pkt->packet_length == 12);
	
	result = coap_pkt_add_option(p_pkt, COAP_PKT_OPTION_IF_MATCH, (unsigned char *)"ABCDEFGH", 8);
	
	CU_ASSERT(result == 0);
	CU_ASSERT(p_pkt->max_delta == 1);
	CU_ASSERT(p_pkt->packet_length == 21);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	result = coap_pkt_add_option(p_pkt, COAP_PKT_OPTION_ACCEPT, (unsigned char *)"\x12", 1);
	
	CU_ASSERT(result == 0);
	CU_ASSERT(p_pkt->max_delta == 17);
	CU_ASSERT(p_pkt->packet_length == 24);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	CU_ASSERT(p_pkt->packet_length == sizeof(teststr));
	CU_ASSERT(memcmp(&p_pkt->hdr, teststr, sizeof(teststr)) == 0);

	coap_pkt_free(p_pkt);
}

static void test_build_pkt6_with_data()
{
	uint8 teststr[] = {0x50, 0x02, 0x12, 0x34, 0xff, '1',  '2',  '3', '4', '5',  '6',  '7',  '8'};
	coap_pkt_t * p_pkt = coap_pkt_alloc(COAP_PKT_MAX_SIZE);
	
	coap_pkt_add_header(p_pkt, COAP_PKT_TYPE_NON, COAP_PKT_CODE_POST, 0x3412);
	
	CU_ASSERT(p_pkt->packet_length == 4);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	coap_pkt_add_data(p_pkt, (unsigned char *)"12345678", 8);
	
	CU_ASSERT(p_pkt->packet_length == sizeof(teststr));
	CU_ASSERT(memcmp(&p_pkt->hdr, teststr, sizeof(teststr)) == 0);

	coap_pkt_free(p_pkt);
}

static void test_build_pkt7_with_empty_data()
{
	uint8 teststr[] = { 0x40, 0x43, 0x12, 0x34 };
	int result;
	coap_pkt_t * p_pkt = coap_pkt_alloc(COAP_PKT_MAX_SIZE);	/* clear p_pkt */

	coap_pkt_add_header(p_pkt, COAP_PKT_TYPE_CON, COAP_PKT_MAKE_CODE(2, 3), 0x3412);

	CU_ASSERT(p_pkt->packet_length == 4);
	
	result = coap_pkt_add_data(p_pkt, NULL, 0);
	
	CU_ASSERT_PTR_NOT_NULL(p_pkt);
	CU_ASSERT(p_pkt->packet_length == 4);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	CU_ASSERT(p_pkt->packet_length == sizeof(teststr));
	CU_ASSERT(memcmp(&p_pkt->hdr, teststr, sizeof(teststr)) == 0);

	coap_pkt_free(p_pkt);
}

static void test_build_pkt8_with_token_and_data()
{
	uint8 teststr[] = { 0x42, 0x43, 0x12, 0x34, 0x00, 0x01, 0xff, 0x00 };
	int result;
	coap_pkt_t * p_pkt = coap_pkt_alloc(COAP_PKT_MAX_SIZE);
	
	coap_pkt_add_header(p_pkt, COAP_PKT_TYPE_CON, COAP_PKT_MAKE_CODE(2, 3), 0x3412);
	
	CU_ASSERT(p_pkt->packet_length == 4);
	
	result = coap_pkt_add_token(p_pkt, (unsigned char *)"\x00\x01", 2);
	
	CU_ASSERT_PTR_NOT_NULL(p_pkt);
	
	result = coap_pkt_add_data(p_pkt, (unsigned char *)"\0", 1);
	
	CU_ASSERT_PTR_NOT_NULL(p_pkt);
	CU_ASSERT(p_pkt->packet_length == 8);
	CU_ASSERT(p_pkt->p_data == (unsigned char *)&p_pkt->hdr + 7);
	
	CU_ASSERT(p_pkt->packet_length == sizeof(teststr));
	CU_ASSERT(memcmp(&p_pkt->hdr, teststr, sizeof(teststr)) == 0);

	coap_pkt_free(p_pkt);
}

static void test_build_pkt9_with_options_and_data()
{
	uint8 teststr[] = 
	{
		0x60, 0x44, 0x12, 0x34, 0x48, 's',  'o',  'm',
		'e',  'e',  't',  'a',  'g',  0x10, 0xdd, 0x11,
		0x04, 's',  'o',  'm',  'e',  'r',  'a',  't',
		'h',  'e',  'r',  'l',  'o',  'n',  'g',  'u',
		'r',  'i',  0xff, 'd',  'a',  't',  'a'
	};
	int result;
	
	coap_pkt_t * p_pkt = coap_pkt_alloc(COAP_PKT_MAX_SIZE);
	
	coap_pkt_add_header(p_pkt, COAP_PKT_TYPE_ACK, COAP_PKT_MAKE_CODE(2, 4), 0x3412);
	
	CU_ASSERT(p_pkt->packet_length == 4);
	
	result = coap_pkt_add_option(p_pkt, COAP_PKT_OPTION_ETAG, (unsigned char *)"someetag", 8);
	
	CU_ASSERT(result == 0);
	CU_ASSERT(p_pkt->max_delta == 4);
	CU_ASSERT(p_pkt->packet_length == 13);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	result = coap_pkt_add_option(p_pkt, COAP_PKT_OPTION_IF_NONE_MATCH, NULL, 0);
	
	CU_ASSERT(result == 0);
	CU_ASSERT(p_pkt->max_delta == 5);
	CU_ASSERT(p_pkt->packet_length == 14);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	result = coap_pkt_add_option(p_pkt, COAP_PKT_OPTION_PROXY_URI, (unsigned char *)"someratherlonguri", 17);
	
	CU_ASSERT(result == 0);
	CU_ASSERT(p_pkt->max_delta == 35);
	CU_ASSERT(p_pkt->packet_length == 34);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	result = coap_pkt_add_data(p_pkt, (unsigned char *)"data", 4);
	
	CU_ASSERT_PTR_NOT_NULL(p_pkt);
	CU_ASSERT(p_pkt->packet_length == 39);
	CU_ASSERT(p_pkt->p_data == (unsigned char *)&p_pkt->hdr + 35);
	
	CU_ASSERT(p_pkt->packet_length == sizeof(teststr));
	CU_ASSERT(memcmp(&p_pkt->hdr, teststr, sizeof(teststr)) == 0);

	coap_pkt_free(p_pkt);
}

static void test_build_pkt10_with_token_options_data()
{
	uint8 teststr[] = 
	{
		0x62, 0x44, 0x12, 0x34, 0x00, 0x00, 0x8d, 0xf2,
		'c',  'o',  'a',  'p',  ':',  '/',  '/',  'e',
		'x',  'a',  'm',  'p',  'l',  'e',  '.',  'c',
		'o',  'm',  '/',  '1',  '2',  '3',  '4',  '5',
		'/',  '%',  '3',  'F',  'x',  'y',  'z',  '/',
		'3',  '0',  '4',  '8',  '2',  '3',  '4',  '2',
		'3',  '4',  '/',  '2',  '3',  '4',  '0',  '2',
		'3',  '4',  '8',  '2',  '3',  '4',  '/',  '2',
		'3',  '9',  '0',  '8',  '4',  '2',  '3',  '4',
		'-',  '2',  '3',  '/',  '%',  'A',  'B',  '%',
		'3',  '0',  '%',  'a',  'f',  '/',  '+',  '1',
		'2',  '3',  '/',  'h',  'f',  'k',  's',  'd',
		'h',  '/',  '2',  '3',  '4',  '8',  '0',  '-',
		'2',  '3',  '4',  '-',  '9',  '8',  '2',  '3',
		'5',  '/',  '1',  '2',  '0',  '4',  '/',  '2',
		'4',  '3',  '5',  '4',  '6',  '3',  '4',  '5',
		'3',  '4',  '5',  '2',  '4',  '3',  '/',  '0',
		'1',  '9',  '8',  's',  'd',  'n',  '3',  '-',
		'a',  '-',  '3',  '/',  '/',  '/',  'a',  'f',
		'f',  '0',  '9',  '3',  '4',  '/',  '9',  '7',
		'u',  '2',  '1',  '4',  '1',  '/',  '0',  '0',
		'0',  '2',  '/',  '3',  '9',  '3',  '2',  '4',
		'2',  '3',  '5',  '3',  '2',  '/',  '5',  '6',
		'2',  '3',  '4',  '0',  '2',  '3',  '/',  '-',
		'-',  '-',  '-',  '/',  '=',  '1',  '2',  '3',
		'4',  '=',  '/',  '0',  '9',  '8',  '1',  '4',
		'1',  '-',  '9',  '5',  '6',  '4',  '6',  '4',
		'3',  '/',  '2',  '1',  '9',  '7',  '0',  '-',
		'-',  '-',  '-',  '-',  '/',  '8',  '2',  '3',
		'6',  '4',  '9',  '2',  '3',  '4',  '7',  '2',
		'w',  'e',  'r',  'e',  'r',  'e',  'w',  'r',
		'0',  '-',  '9',  '2',  '1',  '-',  '3',  '9',
		'1',  '2',  '3',  '-',  '3',  '4',  '/',  0x0d,
		0x01, '/',  '/',  '4',  '9',  '2',  '4',  '0',
		'3',  '-',  '-',  '0',  '9',  '8',  '/',  0xc1,
		'*',  0xff, 'd',  'a',  't',  'a'
	};
	int result;
	
	coap_pkt_t * p_pkt = coap_pkt_alloc(COAP_PKT_MAX_SIZE);
	
	coap_pkt_add_header(p_pkt, COAP_PKT_TYPE_ACK, COAP_PKT_MAKE_CODE(2, 4), 0x3412);
	
	CU_ASSERT(p_pkt->packet_length == 4);
	
	result = coap_pkt_add_token(p_pkt, (unsigned char *)"\0\0", 2);
	
	CU_ASSERT_PTR_NOT_NULL(p_pkt);
	result = coap_pkt_add_option(p_pkt, COAP_PKT_OPTION_LOCATION_PATH, (unsigned char *)"coap://example.com/12345/%3Fxyz/3048234234/23402348234/239084234-23/%AB%30%af/+123/hfksdh/23480-234-98235/1204/243546345345243/0198sdn3-a-3///aff0934/97u2141/0002/3932423532/56234023/----/=1234=/098141-9564643/21970-----/82364923472wererewr0-921-39123-34/", 255);
	
	CU_ASSERT(result == 0);
	CU_ASSERT(p_pkt->max_delta == 8);
	CU_ASSERT(p_pkt->packet_length == 263);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	result = coap_pkt_add_option(p_pkt, COAP_PKT_OPTION_LOCATION_PATH, (unsigned char *)"//492403--098/", 14);
	
	CU_ASSERT(result == 0);
	CU_ASSERT(p_pkt->max_delta == 8);
	CU_ASSERT(p_pkt->packet_length == 279);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	result = coap_pkt_add_option(p_pkt, COAP_PKT_OPTION_LOCATION_QUERY, (unsigned char *)"*", 1);
	
	CU_ASSERT(result == 0);
	CU_ASSERT(p_pkt->max_delta == 20);
	CU_ASSERT(p_pkt->packet_length == 281);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	result = coap_pkt_add_data(p_pkt, (unsigned char *)"data", 4);
	
	CU_ASSERT_PTR_NOT_NULL(p_pkt);
	CU_ASSERT(p_pkt->packet_length == 286);
	CU_ASSERT(p_pkt->p_data == (unsigned char *)&p_pkt->hdr + 282);
	
	CU_ASSERT(p_pkt->packet_length == sizeof(teststr));
	CU_ASSERT(memcmp(&p_pkt->hdr, teststr, sizeof(teststr)) == 0);

	coap_pkt_free(p_pkt);
}

static void test_build_pkt11_packet_size_too_small_for_data()
{
	int result;
	
	coap_pkt_t * p_pkt = coap_pkt_alloc(8);
	
	CU_ASSERT(p_pkt->p_data == NULL);
	
	result = coap_pkt_add_data(p_pkt, (unsigned char *)"0123456789", 10);
	
	CU_ASSERT(result < 0);
	CU_ASSERT(p_pkt->p_data == NULL);
	
	coap_pkt_free(p_pkt);
}

static int t_pkt_tests_create(void) 
{
	return 0;
}

static int t_pkt_tests_remove(void)
{
	return 0;
}

CU_pSuite t_init_pdu_tests() 
{
	CU_pSuite suite[2];
	
	suite[0] = CU_add_suite("p_pkt parser", t_pkt_tests_create, t_pkt_tests_remove);
	if (!suite[0]) 
	{
		fprintf(stderr, "W: cannot add p_pkt parser test suite (%s)\n", CU_get_error_msg());
		return NULL;
	}
	
	PKT_PARSE_TEST(suite[0], test_parse_pkt1_minimum_size);
	PKT_PARSE_TEST(suite[0], test_parse_pkt2_token_is_correct);
	PKT_PARSE_TEST(suite[0], test_parse_pkt3_token_data_exceed);
	PKT_PARSE_TEST(suite[0], test_parse_pkt4_token_length_exceed);
	PKT_PARSE_TEST(suite[0], test_parse_pkt5_with_options);
	PKT_PARSE_TEST(suite[0], test_parse_pkt6_option_length_exceed);
	PKT_PARSE_TEST(suite[0], test_parse_pkt7_with_options_and_payload);
	PKT_PARSE_TEST(suite[0], test_parse_pkt8_without_options_with_payload);
	PKT_PARSE_TEST(suite[0], test_parse_pkt9_without_options_and_payload_but_with_payload_marker);
	PKT_PARSE_TEST(suite[0], test_parse_pkt10_with_options_without_payload_but_with_payload_marker);
	PKT_PARSE_TEST(suite[0], test_parse_pkt11_code_zero_dot_zero);
	PKT_PARSE_TEST(suite[0], test_parse_pkt12_reset);
	PKT_PARSE_TEST(suite[0], test_parse_pkt13_reset_with_content);
	PKT_PARSE_TEST(suite[0], test_parse_pkt14_ack_with_content);
	
	suite[1] = CU_add_suite("p_pkt encoder", t_pkt_tests_create, t_pkt_tests_remove);
	if (!suite[1]) 
	{
		fprintf(stderr, "W: cannot add p_pkt parser test suite (%s)\n", CU_get_error_msg());
		return NULL;
	}
	
	PKT_BUILD_TEST(suite[1], test_build_pkt1_with_token);
	PKT_BUILD_TEST(suite[1], test_build_pkt2_with_token);
	PKT_BUILD_TEST(suite[1], test_build_pkt3_without_header_with_token);
	PKT_BUILD_TEST(suite[1], test_build_pkt4_with_options);
	PKT_BUILD_TEST(suite[1], test_build_pkt5_with_token_and_options);
	PKT_BUILD_TEST(suite[1], test_build_pkt6_with_data);
	PKT_BUILD_TEST(suite[1], test_build_pkt7_with_empty_data);
	PKT_BUILD_TEST(suite[1], test_build_pkt8_with_token_and_data);
	PKT_BUILD_TEST(suite[1], test_build_pkt9_with_options_and_data);
	PKT_BUILD_TEST(suite[1], test_build_pkt10_with_token_options_data);
/*	PKT_BUILD_TEST(suite[1], test_build_pkt11_packet_size_too_small_for_data);
*/	
	return suite[0];
}

