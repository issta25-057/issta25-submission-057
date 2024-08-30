#include "../../../../utilities/liboqs_prng.h"
#include "../../../Call.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <oqs/oqs.h>

void Call(out_t *out, in_t *in, pp_t *pp, aux_t *aux, fmt_t *fmt)
{
    char prg_seed[48];
    memset(prg_seed, 0, 48);
    sprintf(prg_seed, "call");

    OQS_randombytes_switch_algorithm(OQS_RAND_alg_nist_kat);
    OQS_randombytes_nist_kat_init_256bit((uint8_t *)prg_seed, NULL);

    OQS_KEM *kem = NULL;
    const char *kem_name = OQS_KEM_alg_identifier(pp->alg_id);
    kem = OQS_KEM_new(kem_name);
    if (kem == NULL) {
        fprintf(stderr, "%s was not enabled at compile-time.\n", kem_name);
        exit(1);
    }

    out->bytes = kem->length_shared_secret;
    out->buf = (u8 *)calloc(out->bytes, 1);

    // NOTE: Passing in->buf is not enough! need to use fmt_t, and point to the initial byte of intended input
    //       For this reason, we instead define `offset` and then hash &(in->buf[offset]).
    //       Making this primitive-dependent eases the need for super complicated abstract handling of the format, etc
    u64 offset = bits_to_bytes(fmt->list[0].bitlen);
    OQS_STATUS rc = OQS_KEM_decaps(kem, out->buf, &(in->buf[offset]), aux->list[1].buf);
    out->retval = rc;
    // if (rc != OQS_SUCCESS) {
    //     fprintf(stderr, "ERROR: OQS_KEM_decaps failed!\n");
    //     exit(1);
    // }
    // NOTE: may want to check reval in Match instead
    OQS_KEM_free(kem);
}