#include <ctype.h>
#include <czmq.h>

#include "zwshandshake.h"

typedef enum
{
	initial = 0,
	request_line_G,
	request_line_GE,
	request_line_GET,
	request_line_GET_space,
	request_line_resource,
	request_line_resource_space,
	request_line_H,
	request_line_HT,
	request_line_HTT,
	request_line_HTTP,
	request_line_HTTP_slash,
	request_line_HTTP_slash_1,
	request_line_HTTP_slash_1_dot,
	request_line_HTTP_slash_1_dot_1,
	request_line_cr,
	header_field_begin_name,
	header_field_name,
	header_field_colon,
	header_field_value_trailing_space,
	header_field_value,
	header_field_cr,
	end_line_cr,
	complete,	

	error = -1
} state_t;

struct _zwshandshake_t
{
	state_t state;
	zhash_t* header_fields;
};

bool zwshandshake_validate(zwshandshake_t *self);

zwshandshake_t* zwshandshake_new()
{
	zwshandshake_t* self = (zwshandshake_t*)zmalloc(sizeof(zwshandshake_t));
	self->state = initial;
	self->header_fields = zhash_new();

	return self;
}

void s_free_item(void *data)
{
	free(data);
}

void zwshandshake_destroy(zwshandshake_t **self_p)
{
	zwshandshake_t *self = *self_p;

	zhash_destroy(&self->header_fields);

	free(self);
	*self_p = NULL;
}

bool zwshandshake_parse_request(zwshandshake_t *self, zframe_t* data)
{
	// the parse method is not fully implemented and therefore not secured.
	// for the purpose of ZWS prototyoe only the request-line, upgrade header and Sec-WebSocket-Key are validated.

	char *request = zframe_strdup(data);
	int length = (int)strlen(request);

	char *field_name;
	char *field_value;

	size_t name_begin, name_length, value_begin, value_length;

	for (size_t i = 0; i < length; i++)
	{
		char c = request[i];

		switch (self->state)
		{
		case initial:
			if (c == 'G')
				self->state = request_line_G;
			else
				self->state = error;
			break;
		case request_line_G:
			if (c == 'E')
				self->state = request_line_GE;
			else
				self->state = error;
			break;
		case request_line_GE:
			if (c == 'T')
				self->state = request_line_GET;
			else
				self->state = error;
			break;
		case request_line_GET:
			if (c == ' ')
				self->state = request_line_GET_space;
			else
				self->state = error;
			break;
		case request_line_GET_space:
			if (c == '\r' || c == '\n')
				self->state = error;
			// TODO: instead of check what is not allowed check what is allowed
			if (c != ' ')
				self->state = request_line_resource;
			else
				self->state = request_line_GET_space;
			break;
		case request_line_resource:
			if (c == '\r' || c == '\n')
				self->state = error;
			else if (c == ' ')
				self->state = request_line_resource_space;
			else
				self->state = request_line_resource;
			break;
		case request_line_resource_space:
			if (c == 'H')
				self->state = request_line_H;
			else
				self->state = error;
			break;
		case request_line_H:
			if (c == 'T')
				self->state = request_line_HT;
			else
				self->state = error;
			break;
		case request_line_HT:
			if (c == 'T')
				self->state = request_line_HTT;
			else
				self->state = error;
			break;
		case request_line_HTT:
			if (c == 'P')
				self->state = request_line_HTTP;
			else
				self->state = error;
			break;
		case request_line_HTTP:
			if (c == '/')
				self->state = request_line_HTTP_slash;
			else
				self->state = error;
			break;
		case request_line_HTTP_slash:
			if (c == '1')
				self->state = request_line_HTTP_slash_1;
			else
				self->state = error;
			break;
		case request_line_HTTP_slash_1:
			if (c == '.')
				self->state = request_line_HTTP_slash_1_dot;
			else
				self->state = error;
			break;
		case request_line_HTTP_slash_1_dot:
			if (c == '1')
				self->state = request_line_HTTP_slash_1_dot_1;
			else
				self->state = error;
			break;
		case request_line_HTTP_slash_1_dot_1:
			if (c == '\r')
				self->state = request_line_cr;
			else
				self->state = error;
			break;
		case request_line_cr:
			if (c == '\n')
				self->state = header_field_begin_name;
			else
				self->state = error;
			break;
		case header_field_begin_name:
			switch (c)
			{
			case '\r':
				self->state = end_line_cr;
				break;
			case '\n':
				self->state = error;
				break;
			default:
				name_begin = i;
				self->state = header_field_name;
				break;
			}
			break;
		case header_field_name:
			if (c == '\r' || c == '\n')
				self->state = error;
			else if (c == ':')
			{
				name_length = i - name_begin;
				self->state = header_field_colon;
			}
			else
				self->state = header_field_name;
			break;
		case header_field_colon:
		case header_field_value_trailing_space:
			if (c == '\n')
				self->state = error;
			else if (c == '\r')
				self->state = header_field_cr;
			else if (c == ' ')
				self->state = header_field_value_trailing_space;
			else
			{
				value_begin = i;
				self->state = header_field_value;
			}
			break;
		case header_field_value:
			if (c == '\n')
				self->state = error;
			else if (c == '\r')
			{
				value_length = i - value_begin;

				field_name = (char*)zmalloc(sizeof(char) * (name_length + 1));
				field_value = (char*)zmalloc(sizeof(char) * (value_length + 1));

				strncpy(field_name, request + name_begin, name_length);
				strncpy(field_value, request + value_begin, value_length);

				field_name[name_length] = '\0';
				field_value[value_length] = '\0';

				for (size_t j = 0; j < name_length; j++)
				{
					field_name[j] = tolower(field_name[j]);
				}

				zhash_insert(self->header_fields, field_name, field_value);
				zhash_freefn(self->header_fields, field_name, &s_free_item);

				free(field_name);

				self->state = header_field_cr;
			}
			else
				self->state = header_field_value;
			break;
		case header_field_cr:
			if (c == '\n')
				self->state = header_field_begin_name;
			else
				self->state = error;
			break;
		case end_line_cr:
			if (c == '\n')
			{
				self->state = complete;
				//free(request);
				zstr_free(&request);
				return zwshandshake_validate(self);
			}
			break;		
		case error:
			zstr_free(&request);//free(request);
			return false;
		default:
			assert(false);
			zstr_free(&request);//free(request);
			return false;
		}
	}

	zstr_free(&request);//free(request);
	return false;
}

bool zwshandshake_validate(zwshandshake_t *self)
{
	// TODO: validate that the request is valid
	return true;
}

int encode_base64(uint8_t *in, int in_len, char* out, int out_len)
{
	static const uint8_t base64enc_tab[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	unsigned ii, io;
	uint_least32_t v;
	unsigned rem;

	for (io = 0, ii = 0, v = 0, rem = 0; ii<in_len; ii++) {
		uint8_t ch;
		ch = in[ii];
		v = (v << 8) | ch;
		rem += 8;
		while (rem >= 6) {
			rem -= 6;
			if (io >= out_len) return -1; /* truncation is failure */
			out[io++] = base64enc_tab[(v >> rem) & 63];
		}
	}
	if (rem) {
		v <<= (6 - rem);
		if (io >= out_len) return -1; /* truncation is failure */
		out[io++] = base64enc_tab[v & 63];
	}
	while (io & 3) {
		if (io >= out_len) return -1; /* truncation is failure */
		out[io++] = '=';
	}
	if (io >= out_len) return -1; /* no room for null terminator */
	out[io] = 0;
	return io;

}

zframe_t* zwshandshake_get_response(zwshandshake_t *self)
{
	const char * sec_websocket_key_name = "sec-websocket-key";
	const char * magic_string = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

	char * key = (char*)zhash_lookup(self->header_fields, sec_websocket_key_name);

	if (key) {
		int len = strlen(key) + strlen(magic_string);

		char plain[150];

		strcpy(plain, key);
		strcat(plain, magic_string);

		zdigest_t* digest = zdigest_new();
		zdigest_update(digest, (byte *)plain, len);

		byte* hash = zdigest_data(digest);

		char accept_key[150];

		int accept_key_len = encode_base64(hash, zdigest_size(digest), accept_key, 150);

		int response_len = strlen("HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: websocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: \r\n"
			"Sec-WebSocket-Protocol: WSNetMQ\r\n\r\n") + accept_key_len;

		char* response = (char*)zmalloc(sizeof(char) * (response_len + 1));

		strcpy(response, "HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: websocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: ");
		strncat(response, accept_key, accept_key_len);
		strcat(response, "\r\nSec-WebSocket-Protocol: WSNetMQ\r\n\r\n");

		return zframe_new(response, response_len);
	}
	else {
		const char* err = "HTTP/1.1 Not Found Switching Protocols\r\nContent - Type: text / plain; charset = utf - 8\r\n\r\n";
		char* response = (char*)zmalloc(sizeof(char) * (strlen(err) + 1));
		strcpy(response, "HTTP/1.1 Not Found Switching Protocols\r\nContent - Type: text / plain; charset = utf - 8\r\n\r\n");
		return zframe_new(response, strlen(err));
	}

	
}

