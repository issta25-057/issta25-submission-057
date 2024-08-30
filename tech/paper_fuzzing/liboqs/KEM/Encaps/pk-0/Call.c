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

    u8 *shared_secret_e = calloc(kem->length_shared_secret, 1);
    if (shared_secret_e == NULL) {
        fprintf(stderr, "ERROR: malloc failed!\n");
        OQS_KEM_free(kem);
        exit(1);
    }

    u8 *ciphertext = calloc(kem->length_ciphertext, 1);
    if (ciphertext == NULL) {
        fprintf(stderr, "ERROR: malloc failed!\n");
        free(shared_secret_e);
        OQS_KEM_free(kem);
        exit(1);
    }

    u64 offset = bits_to_bytes(fmt->list[0].bitlen);
    OQS_STATUS rv_encaps = OQS_KEM_encaps(kem, ciphertext, shared_secret_e, &(in->buf[offset]));
    if (rv_encaps != OQS_SUCCESS) {
        fprintf(stderr, "ERROR: OQS_KEM_encaps failed!\n");
        buf_list_free(aux);
        OQS_KEM_free(kem);
        exit(1);
    }

    // in this version we save the ciphertext
    out->bytes = kem->length_ciphertext + kem->length_shared_secret;
    out->buf = (u8 *)calloc(out->bytes, 1);
    memcpy(out->buf, ciphertext, kem->length_ciphertext);
    memcpy(&(out->buf[kem->length_ciphertext]), shared_secret_e, kem->length_shared_secret);

    out->retval = rv_encaps;

    free(shared_secret_e);
    free(ciphertext);
    OQS_KEM_free(kem);
}