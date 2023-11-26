#include <sm.h>

void platform_disable_speculation() {
    set_csr(CSR_MSPEC, MSPEC_NONE);
}

void platform_enable_speculation() {
    clear_csr(CSR_MSPEC, MSPEC_NONE);
}

void platform_disable_predictors() {
    set_csr(CSR_MSPEC, MSPEC_NOTRAINPRED);
    set_csr(CSR_MSPEC, MSPEC_NOUSEPRED);
}

void platform_enable_predictors() {
    clear_csr(CSR_MSPEC, MSPEC_NOTRAINPRED);
    clear_csr(CSR_MSPEC, MSPEC_NOUSEPRED);
}

void platform_disable_L1() {
    set_csr(CSR_MSPEC, MSPEC_NOUSEL1);
}

void platform_enable_L1() {
    clear_csr(CSR_MSPEC, MSPEC_NOUSEL1);
}