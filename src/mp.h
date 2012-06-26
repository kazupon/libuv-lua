/*
 * lua-messagepack-native
 * copyright Kengo Nakajima.
 * Apache License Version 2.0
 * [ https://github.com/kengonakajima/lua-msgpack-native ]
 */

#ifndef MP_H
#define MP_H

#include "lua.h"
#include "lauxlib.h"


#define ntohll(x) ( ( (uint64_t)(ntohl( (uint32_t)((x << 32) >> 32) )) << 32) | ntohl( ((uint32_t)(x >> 32)) ) )
#define htonll(x) ntohll(x)
#define elementof(x) ( sizeof(x) / sizeof(x[0]))

#ifdef DEBUG
#define MPDEBUGPRINT(...) fprintf( stderr, __VA_ARGS__ )
#else
#define MPDEBUGPRINT(...) 
#endif /* DEBUG */

#define ERRORBIT_BUFNOLEFT 1
#define ERRORBIT_STRINGLEN 2
#define ERRORBIT_TYPE_LIGHTUSERDATA 4
#define ERRORBIT_TYPE_FUNCTION 8
#define ERRORBIT_TYPE_USERDATA 16
#define ERRORBIT_TYPE_THREAD 32
#define ERRORBIT_TYPE_UNKNOWN 64


typedef struct {
    unsigned char data[1024*1024];
    size_t used; // start from zero
    size_t capacity;
    int err;
} mpwbuf_t;

typedef struct {
    unsigned char *data; // read buffer dont have to allocate buffer.
    size_t ofs;
    size_t len;
    int err;
} mprbuf_t;

typedef enum {
    // containers without size bytes
    MPCT_FIXARRAY,
    MPCT_FIXMAP,
    // containers with size bytes
    MPCT_ARRAY16,
    MPCT_MAP16,
    MPCT_ARRAY32,
    MPCT_MAP32,
    // direct values with size bytes
    MPCT_RAW16,
    MPCT_RAW32,    
    // direct values without size bytes
    MPCT_RAW,
    MPCT_FLOAT,
    MPCT_DOUBLE,
    MPCT_UINT8,
    MPCT_UINT16,
    MPCT_UINT32,
    MPCT_UINT64,
    MPCT_INT8,
    MPCT_INT16,
    MPCT_INT32,
    MPCT_INT64,
} MP_CONTAINER_TYPE;

typedef struct {
    MP_CONTAINER_TYPE t;
    size_t expect;
    size_t sofar;
    // for containers with size bytes
    char sizebytes[4];
    size_t sizesize; // array16,map16,raw16:2 array32,map32,raw32:4    
    size_t sizesofar;
} mpstackent_t;

typedef struct {
    char *buf;
    size_t size;
    size_t used;
    mpstackent_t stack[256];
    size_t nstacked;
    size_t resultnum;
} unpacker_t;


void mpwbuf_init (mpwbuf_t *b);
void mprbuf_init (mprbuf_t *b, const unsigned char *p, size_t len);
size_t mpwbuf_left (mpwbuf_t *b);
size_t mprbuf_left (mprbuf_t *b);
void mp_rcopy (unsigned char *dest, unsigned char *from, size_t l);
size_t mpwbuf_append (mpwbuf_t *b, const unsigned char *toadd, size_t l);
size_t mpwbuf_pack_nil (mpwbuf_t *b);
size_t mpwbuf_pack_boolean (mpwbuf_t *b, int i);
size_t mpwbuf_pack_number (mpwbuf_t *b, lua_Number n);
size_t mpwbuf_pack_string (mpwbuf_t *b, const unsigned char *sval, size_t slen);
size_t mpwbuf_pack_anytype (mpwbuf_t *b, lua_State *L, int index);
size_t mpwbuf_pack_table (mpwbuf_t *b, lua_State *L, int index);
void mprbuf_unpack_anytype (mprbuf_t *b, lua_State *L);
void mprbuf_unpack_array (mprbuf_t *b, lua_State *L, int arylen);
void mprbuf_unpack_map (mprbuf_t *b, lua_State *L, int maplen);
void mprbuf_unpack_anytype (mprbuf_t *b, lua_State *L);

int MP_CONTAINER_TYPE_is_container (MP_CONTAINER_TYPE t);
char* MP_CONTAINER_TYPE_to_s (MP_CONTAINER_TYPE t);
int MP_CONTAINER_TYPE_sizesize (MP_CONTAINER_TYPE t);
int MP_CONTAINER_TYPE_is_map(MP_CONTAINER_TYPE t);
void unpacker_init (unpacker_t *u, size_t maxsize);
void mpstackent_init (mpstackent_t *e, MP_CONTAINER_TYPE t, size_t expect);
int unpacker_progress (unpacker_t *u, char ch);
mpstackent_t* unpacker_top (unpacker_t *u);
void unpacker_progress_datasize (unpacker_t *u, size_t progress);
int unpacker_push (unpacker_t *u, MP_CONTAINER_TYPE t, int expect);
int unpacker_container_needs_bytes (unpacker_t *u, size_t *dataneed);
void unpacker_dump (unpacker_t *u);
int unpacker_feed (unpacker_t *u, char *p, size_t len);
void unpacker_shift (unpacker_t *u, size_t l);

#endif /* MP_H */

