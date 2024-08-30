#include "../../../../utilities/liboqs_prng.h"
#include "../../../Call.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <oqs/oqs.h>

void Call(out_t *out, in_t *in, pp_t *pp, aux_t *aux, fmt_t *fmt)
{
    #ifdef OQS_VERSION_NUMBER
    #if OQS_VERSION_NUMBER == 0x0201811fL
    // NOT SUPPORTED, old liboqs uses sign_open rather than verify, and the internals change per scheme
    fprintf(stderr, "liboqs version not supported.\n");
    exit(1);
    #endif
    #endif

    char prg_seed[48];
    memset(prg_seed, 0, 48);
    sprintf(prg_seed, "call");

    OQS_randombytes_switch_algorithm(OQS_RAND_alg_nist_kat);
    OQS_randombytes_nist_kat_init_256bit((uint8_t *)prg_seed, NULL);

    OQS_SIG *sig = NULL;
    const char *sig_name = OQS_SIG_alg_identifier(pp->alg_id);
    sig = OQS_SIG_new(sig_name);
    if (sig == NULL) {
        fprintf(stderr, "%s was not enabled at compile-time.\n", sig_name);
        exit(1);
    }

    // NOTE: Passing in->buf is not enough! need to use fmt_t, and point to the initial byte of intended input
    //       For this reason, we instead define `offset` and then hash &(in->buf[offset]).
    //       Making this primitive-dependent eases the need for super complicated abstract handling of the format, etc

    // first retrieve the actual length of the signature (as opposed to the maximum possible length)
    size_t signature_len = 0;
    memcpy((u8 *)&signature_len, in->buf, sizeof(size_t));

    // enable this to cause a false bug for debugging
    // if (signature_len == 5) signature_len = sig->length_signature;

    // now we can grab the start of the signature
    u64 offset = bits_to_bytes(fmt->list[0].bitlen) + bits_to_bytes(fmt->list[1].bitlen);
    u8 *signature = &(in->buf[offset]);

    OQS_STATUS rc = OQS_SIG_verify(sig, aux->list[2].buf, aux->list[2].bytes, signature, signature_len, aux->list[0].buf);

    //OQS_STATUS rc = OQS_SIG_sign(sig, out->buf, &sign_len, &(in->buf[offset]), in->bytes, aux->list[1].buf);
    out->bytes = sizeof(OQS_STATUS);
    out->buf = (u8 *)malloc(out->bytes);
    memcpy((OQS_STATUS *)out->buf, &rc, out->bytes);
    out->retval = rc;
    // if (rc != OQS_SUCCESS) {
    //     fprintf(stderr, "ERROR: OQS_SIG_decaps failed!\n");
    //     exit(1);
    // }
    // NOTE: may want to check reval in Match instead
    OQS_SIG_free(sig);
}
