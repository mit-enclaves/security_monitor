#include <sm.h>

void platform_disable_speculation() {
    uint64_t mspec = read_csr(CSR_MSPEC);
    mspec |= MSPEC_NONE;
    write_csr(CSR_MSPEC, mspec);
}

void platform_enable_speculation() {
    uint64_t mspec = read_csr(CSR_MSPEC);
    mspec &= ~MSPEC_NONE;
    write_csr(CSR_MSPEC, mspec);
}

void platform_disable_predictors() {
    uint64_t mspec = read_csr(CSR_MSPEC);
    mspec |= MSPEC_NOTRAINPRED;
    mspec |= MSPEC_NOUSEPRED;
    write_csr(CSR_MSPEC, mspec);
}

void platform_enable_predictors() {
    uint64_t mspec = read_csr(CSR_MSPEC);
    mspec &= ~MSPEC_NOTRAINPRED;
    mspec &= ~MSPEC_NOUSEPRED;
    write_csr(CSR_MSPEC, mspec);
}

void platform_disable_L1() {
    uint64_t mspec = read_csr(CSR_MSPEC);
    mspec |= MSPEC_NOUSEL1;
    write_csr(CSR_MSPEC, mspec);
}

void platform_enable_L1() {
    uint64_t mspec = read_csr(CSR_MSPEC);
    mspec &= ~MSPEC_NOUSEL1;
    write_csr(CSR_MSPEC, mspec);
}