#ifndef CRYPTO_H
#define CRYPTO_H

/* Data structure for MD5 (Message-Digest) computation */
typedef struct {
  uint32_t      i[2];               /* number of _bits_ handled mod 2^64 */
  uint32_t      buf[4];             /* scratch buffer */
  unsigned char in[64];             /* input buffer */
  unsigned char digest[16];         /* actual digest after MD5Final call */
} MD5_CTX;

void MD5Init  ( MD5_CTX *mdContext);
void MD5Update( MD5_CTX *mdContext, unsigned char *inBuf, unsigned int inLen);
void MD5Final ( unsigned char hash[], MD5_CTX *mdContext);
void MD5Final_16(uint8_t buffer[], uint8_t string[]);

#endif
