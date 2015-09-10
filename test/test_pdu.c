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

#define PKT_TEST(s,t) if (!CU_ADD_TEST(s,t)) {fprintf(stderr, "W: cannot add p_pkt parser test (%s)\n",	CU_get_error_msg());}
#define pkt_ENCODER_TEST(s,t) if (!CU_ADD_TEST(s,t)) {fprintf(stderr, "W: cannot add p_pkt encoder test (%s)\n", CU_get_error_msg());}

static void tect_pkt1_minimum_size()
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

	free(p_pkt);
}

static void test_pkt2_token_is_correct() 
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
	CU_ASSERT(memcmp(p_pkt->hdr.token, teststr + 4, 5) == 0);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
}

static void test_pkt3_token_data_exceed() 
{
	uint8 teststr[] = {  0x53, 0x69, 0x12, 0x34, 't', 'o', 'k', 'e', 'n' };
	coap_pkt_t * p_pkt = coap_pkt_parse((unsigned char *)teststr, sizeof(teststr));
	CU_ASSERT_PTR_NULL(p_pkt);
}

static void test_pkt4_token_length_exceed()
{
	uint8 teststr[] = {  0x59, 0x69, 0x12, 0x34, 't', 'o', 'k', 'e', 'n', '1', '2', '3', '4' };
	coap_pkt_t * p_pkt = coap_pkt_parse((unsigned char *)teststr, sizeof(teststr));
	CU_ASSERT_PTR_NULL(p_pkt);
	
	teststr[0] = 0x5f;
	p_pkt = coap_pkt_parse((unsigned char *)teststr, sizeof(teststr));
	CU_ASSERT_PTR_NULL(p_pkt);
}

static void test_pkt5_with_options() 
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
	CU_ASSERT(memcmp(p_pkt->hdr.token, teststr + 4, 5) == 0);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
}

static void test_pkt6_option_length_exceed()
{
	uint8 teststr[] = {0x55, 0x73, 0x12, 0x34, 't', 'o', 'k', 'e', 'n',  0x00, 0xc1, 0x00, 0xae, 0xf0, 0x03};
	coap_pkt_t * p_pkt = coap_pkt_parse((unsigned char *)teststr, sizeof(teststr));
	CU_ASSERT_PTR_NULL(p_pkt);
}

static void test_pkt7_with_options_and_payload() 
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
	CU_ASSERT(memcmp(p_pkt->hdr.token, teststr + 4, 5) == 0);
	CU_ASSERT(p_pkt->p_data == (unsigned char *)&p_pkt->hdr + 13);
	CU_ASSERT(memcmp(p_pkt->p_data, teststr + 13, 7) == 0);
}

static void test_pkt8_without_options_with_payload() 
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
}

static void test_pkt9_without_options_and_payload_but_with_payload_marker() 
{
	uint8 teststr[] = {  0x70, 0x00, 0x12, 0x34, 0xff };
	coap_pkt_t * p_pkt = coap_pkt_parse((unsigned char *)teststr, sizeof(teststr));
	CU_ASSERT_PTR_NULL(p_pkt);
}

static void test_pkt10_with_options_without_payload_but_with_payload_marker() 
{
	uint8 teststr[] = {0x53, 0x73, 0x12, 0x34, 't', 'o', 'k', 0x30, 0xc1, 0x00, 0xff};
	
	coap_pkt_t * p_pkt = coap_pkt_parse((unsigned char *)teststr, sizeof(teststr));
	CU_ASSERT_PTR_NULL(p_pkt);
}

static void test_pkt11_code_zero_dot_zero() 
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
}

static void test_pkt12_reset(void) 
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
}

#if 0
static void
t_parse_pkt13(void) {
	/* RST with content */
	uint8 teststr[] = {  0x70, 0x00, 0x12, 0x34,
		      0xff, 'c', 'o', 'n', 't', 'e', 'n', 't'
	};
	int result;
	
	coap_pkt_t * p_pkt = coap_pkt_parse((unsigned char *)teststr, sizeof(teststr));
	CU_ASSERT_PTR_NULL(p_pkt);
}

static void
t_parse_pkt14(void) {
	/* ACK with content */
	uint8 teststr[] = {  0x60, 0x00, 0x12, 0x34,
		      0xff, 'c', 'o', 'n', 't', 'e', 'n', 't'
	};
	int result;
	
	coap_pkt_t * p_pkt = coap_pkt_parse((unsigned char *)teststr, sizeof(teststr));
	CU_ASSERT_PTR_NULL(p_pkt);
}

/************************************************************************
** p_pkt encoder
************************************************************************/

static void
t_encode_pkt1(void) {
	uint8 teststr[] = { 0x45, 0x01, 0x12, 0x34, 't', 'o', 'k', 'e', 'n' };
	int result;
	
	coap_pkt_clear(p_pkt, p_pkt->max_size);
	p_pkt->hdr.type = COAP_PKT_TYPE_CON;
	p_pkt->hdr.code = COAP_PKT_CODE_GET;
	p_pkt->hdr.message_id = htons(0x1234);
	
	result = coap_add_token(p_pkt, 5, (unsigned char *)"token");
	
	CU_ASSERT(result == 1);
	CU_ASSERT(p_pkt->packet_length = sizeof(teststr));
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	CU_ASSERT(memcmp(p_pkt->hdr, teststr, sizeof(teststr)) == 0);
}

static void
t_encode_pkt2(void) {
	size_t old_max = p_pkt->max_size;
	int result;
	
	coap_pkt_clear(p_pkt, 7);	/* set very small p_pkt size */
	
	p_pkt->hdr.type = COAP_PKT_TYPE_CON;
	p_pkt->hdr.code = COAP_PKT_CODE_GET;
	p_pkt->hdr.message_id = htons(0x1234);
	
	result = coap_add_token(p_pkt, 5, (unsigned char *)"token");
	
	CU_ASSERT_PTR_NULL(p_pkt);
	
	coap_pkt_clear(p_pkt, old_max);	/* restore p_pkt size */
}

static void
t_encode_pkt3(void) {
	int result;
	
	result = coap_add_token(p_pkt, 9, (unsigned char *)"123456789");
	
	CU_ASSERT_PTR_NULL(p_pkt);
}

static void
t_encode_pkt4(void) {
	/* p_pkt with options */
	uint8 teststr[] = { 0x60, 0x99, 0x12, 0x34, 0x3d, 0x05, 0x66, 0x61,
		0x6e, 0x63, 0x79, 0x70, 0x72, 0x6f, 0x78, 0x79,
		0x2e, 0x63, 0x6f, 0x61, 0x70, 0x2e, 0x6d, 0x65,
		0x84, 0x70, 0x61, 0x74, 0x68, 0x00, 0xe8, 0x1e,
		0x28, 0x66, 0x61, 0x6e, 0x63, 0x79, 0x6f, 0x70,
		0x74
	};
	int result;
	
	coap_pkt_clear(p_pkt, p_pkt->max_size);	/* clear p_pkt */
	
	p_pkt->hdr.type = COAP_PKT_TYPE_ACK;
	p_pkt->hdr.code = 0x99;
	p_pkt->hdr.message_id = htons(0x1234);
	
	CU_ASSERT(p_pkt->packet_length == 4);
	
	result = coap_add_option(p_pkt, COAP_OPTION_URI_HOST,
		18, (unsigned char *)"fancyproxy.coap.me");
	
	CU_ASSERT(result == 20);
	CU_ASSERT(p_pkt->max_delta == 3);
	CU_ASSERT(p_pkt->packet_length == 24);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	result = coap_add_option(p_pkt, COAP_OPTION_URI_PATH,
		4, (unsigned char *)"path");
	
	CU_ASSERT(result == 5);
	CU_ASSERT(p_pkt->max_delta == 11);
	CU_ASSERT(p_pkt->packet_length == 29);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	result = coap_add_option(p_pkt, COAP_OPTION_URI_PATH, 0, NULL);
	
	CU_ASSERT(result == 1);
	CU_ASSERT(p_pkt->max_delta == 11);
	CU_ASSERT(p_pkt->packet_length == 30);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	result = coap_add_option(p_pkt, 8000, 8, (unsigned char *)"fancyopt");
	
	CU_ASSERT(result == 11);
	CU_ASSERT(p_pkt->max_delta == 8000);
	CU_ASSERT(p_pkt->packet_length == 41);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	CU_ASSERT(p_pkt->packet_length == sizeof(teststr));
	CU_ASSERT(memcmp(p_pkt->hdr, teststr, sizeof(teststr)) == 0);
}

static void
t_encode_pkt5(void) {
	/* p_pkt with token and options */
	uint8 teststr[] = { 0x68, 0x84, 0x12, 0x34, '1',  '2',  '3',  '4',
		'5',  '6',  '7',  '8',  0x18, 0x41, 0x42, 0x43,
		0x44, 0x45, 0x46, 0x47, 0x48, 0xd1, 0x03, 0x12
	};
	int result;
	
	coap_pkt_clear(p_pkt, p_pkt->max_size);	/* clear p_pkt */
	
	p_pkt->hdr.type = COAP_PKT_TYPE_ACK;
	p_pkt->hdr.code = COAP_RESPONSE_CODE(404);
	p_pkt->hdr.message_id = htons(0x1234);
	
	CU_ASSERT(p_pkt->packet_length == 4);
	
	result = coap_add_token(p_pkt, 8, (unsigned char *)"12345678");
	
	CU_ASSERT(p_pkt->packet_length == 12);
	
	result = coap_add_option(p_pkt, COAP_OPTION_IF_MATCH,
		8, (unsigned char *)"ABCDEFGH");
	
	CU_ASSERT(result == 9);
	CU_ASSERT(p_pkt->max_delta == 1);
	CU_ASSERT(p_pkt->packet_length == 21);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	result = coap_add_option(p_pkt, COAP_OPTION_ACCEPT,
		1, (unsigned char *)"\x12");
	
	CU_ASSERT(result == 3);
	CU_ASSERT(p_pkt->max_delta == 17);
	CU_ASSERT(p_pkt->packet_length == 24);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	CU_ASSERT(p_pkt->packet_length == sizeof(teststr));
	CU_ASSERT(memcmp(p_pkt->hdr, teststr, sizeof(teststr)) == 0);
}

static void
t_encode_pkt6(void) {
	/* p_pkt with data */
	uint8 teststr[] = { 0x50, 0x02, 0x12, 0x34, 0xff, '1',  '2',  '3',
		'4', '5',  '6',  '7',  '8'
	};
	coap_pkt_clear(p_pkt, p_pkt->max_size);	/* clear p_pkt */
	
	p_pkt->hdr.type = COAP_PKT_TYPE_NON;
	p_pkt->hdr.code = COAP_PKT_CODE_POST;
	p_pkt->hdr.message_id = htons(0x1234);
	
	CU_ASSERT(p_pkt->packet_length == 4);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	coap_add_data(p_pkt, 8, (unsigned char *)"12345678");
	
	CU_ASSERT(p_pkt->packet_length == sizeof(teststr));
	CU_ASSERT(memcmp(p_pkt->hdr, teststr, sizeof(teststr)) == 0);
}

static void
t_encode_pkt7(void) {
	/* p_pkt with empty data */
	uint8 teststr[] = { 0x40, 0x43, 0x12, 0x34 };
	int result;
	coap_pkt_clear(p_pkt, p_pkt->max_size);	/* clear p_pkt */
	
	p_pkt->hdr.type = COAP_PKT_TYPE_CON;
	p_pkt->hdr.code = COAP_RESPONSE_CODE(203);
	p_pkt->hdr.message_id = htons(0x1234);
	
	CU_ASSERT(p_pkt->packet_length == 4);
	
	result = coap_add_data(p_pkt, 0, NULL);
	
	CU_ASSERT_PTR_NOT_NULL(p_pkt);
	CU_ASSERT(p_pkt->packet_length == 4);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	CU_ASSERT(p_pkt->packet_length == sizeof(teststr));
	CU_ASSERT(memcmp(p_pkt->hdr, teststr, sizeof(teststr)) == 0);
}

static void
t_encode_pkt8(void) {
	/* p_pkt with token and data */
	uint8 teststr[] = { 0x42, 0x43, 0x12, 0x34, 0x00, 0x01, 0xff, 0x00 };
	int result;
	coap_pkt_clear(p_pkt, p_pkt->max_size);	/* clear p_pkt */
	
	p_pkt->hdr.type = COAP_PKT_TYPE_CON;
	p_pkt->hdr.code = COAP_RESPONSE_CODE(203);
	p_pkt->hdr.message_id = htons(0x1234);
	
	CU_ASSERT(p_pkt->packet_length == 4);
	
	result = coap_add_token(p_pkt, 2, (unsigned char *)"\x00\x01");
	
	CU_ASSERT_PTR_NOT_NULL(p_pkt);
	
	result = coap_add_data(p_pkt, 1, (unsigned char *)"\0");
	
	CU_ASSERT_PTR_NOT_NULL(p_pkt);
	CU_ASSERT(p_pkt->packet_length == 8);
	CU_ASSERT(p_pkt->p_data == (unsigned char *)p_pkt->hdr + 7);
	
	CU_ASSERT(p_pkt->packet_length == sizeof(teststr));
	CU_ASSERT(memcmp(p_pkt->hdr, teststr, sizeof(teststr)) == 0);
}

static void
t_encode_pkt9(void) {
	/* p_pkt with options and data */
	uint8 teststr[] = { 0x60, 0x44, 0x12, 0x34, 0x48, 's',  'o',  'm',
		'e',  'e',  't',  'a',  'g',  0x10, 0xdd, 0x11,
		0x04, 's',  'o',  'm',  'e',  'r',  'a',  't',
		'h',  'e',  'r',  'l',  'o',  'n',  'g',  'u',
		'r',  'i',  0xff, 'd',  'a',  't',  'a'
	};
	int result;
	
	coap_pkt_clear(p_pkt, p_pkt->max_size);	/* clear p_pkt */
	
	p_pkt->hdr.type = COAP_PKT_TYPE_ACK;
	p_pkt->hdr.code = COAP_RESPONSE_CODE(204);
	p_pkt->hdr.message_id = htons(0x1234);
	
	CU_ASSERT(p_pkt->packet_length == 4);
	
	result = coap_add_option(p_pkt, COAP_OPTION_ETAG, 8, (unsigned char *)"someetag");
	
	CU_ASSERT(result == 9);
	CU_ASSERT(p_pkt->max_delta == 4);
	CU_ASSERT(p_pkt->packet_length == 13);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	result = coap_add_option(p_pkt, COAP_OPTION_IF_NONE_MATCH, 0, NULL);
	
	CU_ASSERT(result == 1);
	CU_ASSERT(p_pkt->max_delta == 5);
	CU_ASSERT(p_pkt->packet_length == 14);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	result = coap_add_option(p_pkt, COAP_OPTION_PROXY_URI,
		17, (unsigned char *)"someratherlonguri");
	
	CU_ASSERT(result == 20);
	CU_ASSERT(p_pkt->max_delta == 35);
	CU_ASSERT(p_pkt->packet_length == 34);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	result = coap_add_data(p_pkt, 4, (unsigned char *)"data");
	
	CU_ASSERT_PTR_NOT_NULL(p_pkt);
	CU_ASSERT(p_pkt->packet_length == 39);
	CU_ASSERT(p_pkt->p_data == (unsigned char *)p_pkt->hdr + 35);
	
	CU_ASSERT(p_pkt->packet_length == sizeof(teststr));
	CU_ASSERT(memcmp(p_pkt->hdr, teststr, sizeof(teststr)) == 0);
}

static void
t_encode_pkt10(void) {
	/* p_pkt with token, options and data */
	uint8 teststr[] = { 0x62, 0x44, 0x12, 0x34, 0x00, 0x00, 0x8d, 0xf2,
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
	
	coap_pkt_clear(p_pkt, p_pkt->max_size);	/* clear p_pkt */
	
	p_pkt->hdr.type = COAP_PKT_TYPE_ACK;
	p_pkt->hdr.code = COAP_RESPONSE_CODE(204);
	p_pkt->hdr.message_id = htons(0x1234);
	
	CU_ASSERT(p_pkt->packet_length == 4);
	
	result = coap_add_token(p_pkt, 2, (unsigned char *)"\0\0");
	
	CU_ASSERT_PTR_NOT_NULL(p_pkt);
	result = coap_add_option(p_pkt, COAP_OPTION_LOCATION_PATH, 255,
		(unsigned char *)"coap://example.com/12345/%3Fxyz/3048234234/23402348234/239084234-23/%AB%30%af/+123/hfksdh/23480-234-98235/1204/243546345345243/0198sdn3-a-3///aff0934/97u2141/0002/3932423532/56234023/----/=1234=/098141-9564643/21970-----/82364923472wererewr0-921-39123-34/");
	
	CU_ASSERT(result == 257);
	CU_ASSERT(p_pkt->max_delta == 8);
	CU_ASSERT(p_pkt->packet_length == 263);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	result = coap_add_option(p_pkt, COAP_OPTION_LOCATION_PATH, 14,
		(unsigned char *)"//492403--098/");
	
	CU_ASSERT(result == 16);
	CU_ASSERT(p_pkt->max_delta == 8);
	CU_ASSERT(p_pkt->packet_length == 279);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	result = coap_add_option(p_pkt, COAP_OPTION_LOCATION_QUERY,
		1, (unsigned char *)"*");
	
	CU_ASSERT(result == 2);
	CU_ASSERT(p_pkt->max_delta == 20);
	CU_ASSERT(p_pkt->packet_length == 281);
	CU_ASSERT_PTR_NULL(p_pkt->p_data);
	
	result = coap_add_data(p_pkt, 4, (unsigned char *)"data");
	
	CU_ASSERT_PTR_NOT_NULL(p_pkt);
	CU_ASSERT(p_pkt->packet_length == 286);
	CU_ASSERT(p_pkt->p_data == (unsigned char *)p_pkt->hdr + 282);
	
	CU_ASSERT(p_pkt->packet_length == sizeof(teststr));
	CU_ASSERT(memcmp(p_pkt->hdr, teststr, sizeof(teststr)) == 0);
}

static void
t_encode_pkt11(void) {
	coap_log_t level = coap_get_log_level();
	/* data too long for p_pkt */
	size_t old_max = p_pkt->max_size;
	int result;
	
	coap_pkt_clear(p_pkt, 8);	/* clear p_pkt, with small maximum */
	
	CU_ASSERT(p_pkt->p_data == NULL);
	coap_set_log_level(LOG_CRIT);
	result = coap_add_data(p_pkt, 10, (unsigned char *)"0123456789");
	coap_set_log_level(level);
	
	CU_ASSERT_PTR_NULL(p_pkt);
	CU_ASSERT(p_pkt->p_data == NULL);
	
	p_pkt->max_size = old_max;
}


#endif

static int t_pkt_tests_create(void) {
	return 0;
}

static int t_pkt_tests_remove(void) {
	return 0;
}

CU_pSuite t_init_pdu_tests(void) {
	CU_pSuite suite[2];
	
	suite[0] = CU_add_suite("p_pkt parser", t_pkt_tests_create, t_pkt_tests_remove);
	if (!suite[0]) {			/* signal error */
		fprintf(stderr, "W: cannot add p_pkt parser test suite (%s)\n", CU_get_error_msg());
		
		return NULL;
	}
	
	PKT_TEST(suite[0], tect_pkt1_minimum_size);
	PKT_TEST(suite[0], test_pkt2_token_is_correct);
	PKT_TEST(suite[0], test_pkt3_token_data_exceed);
	PKT_TEST(suite[0], test_pkt4_token_length_exceed);
	PKT_TEST(suite[0], test_pkt5_with_options);
	PKT_TEST(suite[0], test_pkt6_option_length_exceed);
	PKT_TEST(suite[0], test_pkt7_with_options_and_payload);
	PKT_TEST(suite[0], test_pkt8_without_options_with_payload);
	PKT_TEST(suite[0], test_pkt9_without_options_and_payload_but_with_payload_marker);
	PKT_TEST(suite[0], test_pkt10_with_options_without_payload_but_with_payload_marker);
	PKT_TEST(suite[0], test_pkt11_code_zero_dot_zero);
	PKT_TEST(suite[0], test_pkt12_reset);
/*	PKT_TEST(suite[0], t_parse_pkt13);
	PKT_TEST(suite[0], t_parse_pkt14);
*/	
	suite[1] = CU_add_suite("p_pkt encoder", t_pkt_tests_create, t_pkt_tests_remove);
	if (suite[1]) {

/*		pkt_ENCODER_TEST(suite[1], t_encode_pkt1);
		pkt_ENCODER_TEST(suite[1], t_encode_pkt2);
		pkt_ENCODER_TEST(suite[1], t_encode_pkt3);
		pkt_ENCODER_TEST(suite[1], t_encode_pkt4);
		pkt_ENCODER_TEST(suite[1], t_encode_pkt5);
		pkt_ENCODER_TEST(suite[1], t_encode_pkt6);
		pkt_ENCODER_TEST(suite[1], t_encode_pkt7);
		pkt_ENCODER_TEST(suite[1], t_encode_pkt8);
		pkt_ENCODER_TEST(suite[1], t_encode_pkt9);
		pkt_ENCODER_TEST(suite[1], t_encode_pkt10);
		pkt_ENCODER_TEST(suite[1], t_encode_pkt11);
*/		
	} else 			/* signal error */
		fprintf(stderr, "W: cannot add p_pkt parser test suite (%s)\n",
		CU_get_error_msg());
	
	return suite[0];
}

