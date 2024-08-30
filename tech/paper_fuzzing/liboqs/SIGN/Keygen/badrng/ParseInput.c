#include "../../../serialize.h"
#include "../../../API.h"
#include "../../../../utilities/bufutils.h"
#include "../../../../utilities/fmt_str_parser.h"
#include "../../../Call.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <oqs/oqs.h>


int main(int argc, char **argv)
{
    // make sure an output file is specified
    if (argc < 2)
    {
        printf("Usage: %s in_fn.bin\n", argv[0]);
        return 0;
    }

    u8 *buf;
    load(argv[1], &buf);
    pp_t pp;
    aux_t aux;
    fmt_t fmt;
    in_t x;
    in_t xp;
    out_t y;
    out_t yp;
    exp_res_t expres;

    unserialize(buf, &pp, &aux, &fmt, &x, &xp, &y, &expres);

    OQS_SIG *sig = NULL;
    const char *sig_name = OQS_SIG_alg_identifier(pp.alg_id);
    sig = OQS_SIG_new(sig_name);
    if (sig == NULL) {
        printf("%s was not enabled at compile-time.\n", sig_name);
        exit(1);
    }
    printf("SIG name: %s\n", sig_name);

    Call(&yp, &xp, &pp, &aux, &fmt);
    for (len_t i = 0; i < aux.list_len; i++) {
        printf("aux[%lu]:\t", i);
        print_buffer_hex(aux.list[i].buf, aux.list[i].bytes);
    }
    printf("x:\t");
    print_buffer_hex(x.buf, x.bytes);
    assert(bits_to_bytes(fmt.list[0].bitlen) == 1);
    assert(BufBytelen(&fmt)== x.bytes);
    printf("xp:\t");
    print_buffer_hex(xp.buf, xp.bytes);
    assert(bits_to_bytes(fmt.list[0].bitlen) == 1);
    assert(BufBytelen(&fmt)== xp.bytes);
    printf("y:\t");
    print_buffer_hex(y.buf, y.bytes);
    assert(y.bytes == sig->length_secret_key + sig->length_public_key);
    printf("yp:\t");
    assert(yp.bytes == sig->length_secret_key + sig->length_public_key);
    print_buffer_hex(yp.buf, yp.bytes);
    printf("expected: %s\n", expres ? "equal" : "unequal");
    bool equal_bytes = (bool)(y.bytes == yp.bytes);
    bool equal_bufs = false; 
    if (!equal_bytes) {
        printf("gotten: unequal bytelength, |y| = %lu, |yp| = %lu\n", y.bytes, yp.bytes);
    } else {
        equal_bufs = (bool)(memcmp(y.buf, yp.buf, y.bytes) == 0);
        printf("gotten: %s\n", equal_bufs ? "equal" : "unequal buffers");
        if (!equal_bufs) {
            printf("y xor yp: ");
            for (int i = 0; i < y.bytes; i++) {
                printf("%02x", y.buf[i] ^ yp.buf[i]);
            }
            printf("\n");
        }
    }
    if (expres != (equal_bytes && equal_bufs)) {
        printf("x xor xp: ");
        for (int i = 0; i < x.bytes; i++) {
            printf("%02x", x.buf[i] ^ xp.buf[i]);
        }
        printf("\n");
    }

    free(x.buf);
    free(xp.buf);
    free(y.buf);
    free(yp.buf);
    buf_list_free(&aux);
    free(fmt.list);
    free(buf);
    OQS_SIG_free(sig);
}