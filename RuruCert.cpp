#include "RuruCert.h"

RuruCert::RuruCert() 
    : key(EVP_PKEY_new()), x509(X509_new())
{
    BIGNUM *exponent = BN_new();
    RSA *rsa = RSA_new();
    BN_set_word(exponent, RSA_F4);

    RSA_generate_key_ex(rsa, 1024, exponent, NULL);
    EVP_PKEY_assign_RSA(key, rsa);

    X509_set_pubkey(x509, key);
    BIGNUM* serial_number = BN_new();
    BN_pseudo_rand(serial_number, 64, 0, 0);
    ASN1_INTEGER *asn1_serial_number = X509_get_serialNumber(x509);
    BN_to_ASN1_INTEGER(serial_number, asn1_serial_number);

    X509_set_version(x509, 0L);
    X509_NAME* name = X509_NAME_new();
    X509_NAME_add_entry_by_NID(name, NID_commonName, MBSTRING_UTF8, (unsigned char*)"rurusocket", -1, -1, 0);
    X509_set_subject_name(x509, name);
    X509_set_issuer_name(x509, name);
    X509_gmtime_adj(X509_get_notBefore(x509), 0);
    X509_gmtime_adj(X509_get_notAfter(x509), 365 * 24 * 3600);
    X509_sign(x509, key, EVP_sha1());

    uint32_t length = 32;
    uint8_t buffer[32] = {0};
    X509_digest(x509, EVP_sha256(), buffer, &length);

    for (uint32_t i = 0; i < length; ++i)
    {
        if (i < 31)
            snprintf(fingerPrint + i * 3, 4, "%02X:", buffer[i]);
        else
            snprintf(fingerPrint + i * 3, 3, "%02X", buffer[i]);
    }
    fingerPrint[95] = '\0';

    BN_free(exponent);
    BN_free(serial_number);
    X509_NAME_free(name);
}

RuruCert::~RuruCert()
{
    EVP_PKEY_free(key);
    X509_free(x509);
}