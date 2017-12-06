/**
 * \file base32.c
 * \version 1.0
 * \author Stefano
 * \date 10-11-2017
 * \brief Codifica base32
 */

#include <assert.h>  // assert()
#include <limits.h>  // CHAR_BIT

#include "base32.h"

#define min(x, y) ((x) < (y) ? (x) : (y))

static const unsigned char PADDING_CHAR = '='; /**<Il carattere di padding per le codifiche base32 e base64 è solitamente '=' */

/**
 * \brief Aggiunta dei caratteri di paddiing
 * @param buf[in, out] Buffer contenente i dati da aggiustare
 * @param len[in] Lunghezza del vettore.
 */
static void pad(unsigned char *buf, int len) {
    for (int i = 0; i < len; i++)
        buf[i] = PADDING_CHAR;
}

/**
 * \brief Ottiene il carattere relativo alla serie di bit
 * @param c[in] Carattere in input
 * @return Carattere codificato.
 */
static unsigned char encode_char(unsigned char c) {
    static unsigned char base32[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    return base32[c & 0x1F];  // 0001 1111
}

/**
 * \brief Ottiene la codifica originale dal carattere
 * @param c[in] Carattere codificato
 * @return Codifica originale
 */
static int decode_char(unsigned char c) {
    char retval = -1;

    if (c >= 'A' && c <= 'Z')
        retval = (char) (c - 'A');
    if (c >= '2' && c <= '7')
        retval = (char) (c - '2' + 26);

    assert(retval == -1 || ((retval & 0x1F) == retval));

    return retval;
}

static int get_octet(int block) {
    assert(block >= 0 && block < 8);
    return (block * 5) / 8;
}

static int get_offset(int block) {
    assert(block >= 0 && block < 8);
    return (8 - 5 - (5 * block) % 8);
}

static unsigned char shift_right(unsigned char byte, char offset) {
    if (offset > 0)
        return byte >> offset;
    else
        return byte << -offset;
}

static unsigned char shift_left(unsigned char byte, char offset) {
    return shift_right(byte, -offset);
}

/**
 * \brief Codifica di una sequenza
 * La funzione cicla una sequenza in input e la codifica in base32
 * @param plain[in] sequenza di input
 * @param len[in] lunghezza della sequenza
 * @param coded[out] sequenza codificata !Va preallocata
 */
static void encode_sequence(const unsigned char *plain, int len, unsigned char *coded) {
    static_assert(CHAR_BIT == 8, "");
    assert(len >= 0 && len <= 5);

    for (int block = 0; block < 8; block++) {
        int octet = get_octet(block);
        int junk = get_offset(block);

        if (octet >= len) {
            pad(&coded[block], 8 - block);
            return;
        }

        unsigned char c = shift_right(plain[octet], (char) junk);

        if (junk < 0
            && octet < len - 1) {
            c |= shift_right(plain[octet + 1], (char) (8 + junk));
        }
        coded[block] = encode_char(c);
    }
}

/**
 * \brief decodifica di una sequenza
 * La funzione cicla una sequenza in input e la decodifica
 * @param plain[in] sequenza di input
 * @param len[in] lunghezza della sequenza
 * @param coded[out] sequenza decodificata !Va preallocata
 */
static int decode_sequence(const unsigned char *coded, unsigned char *plain) {
    static_assert(CHAR_BIT == 8, "");
    assert(coded && plain);

    plain[0] = 0;
    for (int block = 0; block < 8; block++) {
        int offset = get_offset(block);
        int octet = get_octet(block);

        int c = decode_char(coded[block]);
        if (c < 0)
            return octet;

        plain[octet] |= shift_left((unsigned char) c, (char) offset);
        if (offset < 0) {
            assert(octet < 4);
            plain[octet + 1] = shift_left((unsigned char) c, (char) (8 + offset));
        }
    }
    return 5;
}

/**
 * \brief Decodifica in base32
 * La funzione decodifica la stringa in input in base32
 * @param coded[in] La sequenza codificata
 * @param plain[out] La sequaneza decodificata
 * @return Numero dei caratteri scritti
 */
size_t base32_decode(const unsigned char *coded, unsigned char *plain) {
    size_t written = 0;
    for (size_t i = 0, j = 0;; i += 8, j += 5) {
        int n = decode_sequence(&coded[i], &plain[j]);
        written += n;
        if (n < 5)
            return written;
    }
}

/**
 * \brief Codifica in base32
 * La funzione codifica la stringa in input in base32
 * @param coded[in] La sequenza da codificare
 * @param plain[out] La sequaneza codificata
 */
void base32_encode(const unsigned char *plain, size_t len, unsigned char *coded) {
    for (size_t i = 0, j = 0; i < len; i += 5, j += 8) {
        encode_sequence(&plain[i], (int) min(len - i, 5), &coded[j]);
    }
}
