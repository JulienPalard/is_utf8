#include "is_utf8.h"

/*
  Check if the given unsigned char * is a valid utf-8 sequence.

  Return value :
  If the string is valid utf-8, 0 is returned.
  Else the position, starting from 1, is returned.

  Source:
   http://www.unicode.org/versions/Unicode7.0.0/UnicodeStandard-7.0.pdf
   page 124, 3.9 "Unicode Encoding Forms", "UTF-8"


  Table 3-7. Well-Formed UTF-8 Byte Sequences
  -----------------------------------------------------------------------------
  |  Code Points        | First Byte | Second Byte | Third Byte | Fourth Byte |
  |  U+0000..U+007F     |     00..7F |             |            |             |
  |  U+0080..U+07FF     |     C2..DF |      80..BF |            |             |
  |  U+0800..U+0FFF     |         E0 |      A0..BF |     80..BF |             |
  |  U+1000..U+CFFF     |     E1..EC |      80..BF |     80..BF |             |
  |  U+D000..U+D7FF     |         ED |      80..9F |     80..BF |             |
  |  U+E000..U+FFFF     |     EE..EF |      80..BF |     80..BF |             |
  |  U+10000..U+3FFFF   |         F0 |      90..BF |     80..BF |      80..BF |
  |  U+40000..U+FFFFF   |     F1..F3 |      80..BF |     80..BF |      80..BF |
  |  U+100000..U+10FFFF |         F4 |      80..8F |     80..BF |      80..BF |
  -----------------------------------------------------------------------------

*/
int is_utf8(unsigned char *str, size_t len, char **message)
{
    size_t i = 0;

    *message = NULL;
    while (i < len)
    {
        if (str[i] <= 0x7F) /* 00..7F */
        {
            i += 1;
        }
        else if (str[i] >= 0xC2 && str[i] <= 0xDF) /* C2..DF 80..BF */
        {
            if (i + 1 < len) /* Expect a 2nd byte */
            {
                if (str[i + 1] < 0x80 || str[i + 1] > 0xBF)
                {
                    *message = "After a first byte between C2 and DF, expecting a 2nd byte between 80 and BF";
                    return i + 1;
                }
            }
            else
            {
                *message = "After a first byte between C2 and DF, expecting a 2nd byte.";
                return i;
            }
            i += 2;
        }
        else if (str[i] == 0xE0) /* E0 A0..BF 80..BF */
        {
            if (i + 2 < len) /* Expect a 2nd and 3rd byte */
            {
                if (str[i + 1] < 0xA0 || str[i + 1] > 0xBF)
                {
                    *message = "After a first byte of E0, expecting a 2nd byte between A0 and BF.";
                    return i + 1;
                }
                if (str[i + 2] < 0x80 || str[i + 2] > 0xBF)
                {
                    *message = "After a first byte of E0, expecting a 3nd byte between 80 and BF.";
                    return i + 2;
                }
            }
            else
            {
                *message = "After a first byte of E0, expecting two following bytes.";
                return i;
            }
            i += 3;
        }
        else if (str[i] >= 0xE1 && str[i] <= 0xEC) /* E1..EC 80..BF 80..BF */
        {
            if (i + 2 < len) /* Expect a 2nd and 3rd byte */
            {
                if (str[i + 1] < 0x80 || str[i + 1] > 0xBF)
                {
                    *message = "After a first byte between E1 and EC, expecting the 2nd byte between 80 and BF.";
                    return i + 1;
                }
                if (str[i + 2] < 0x80 || str[i + 2] > 0xBF)
                {
                    *message = "After a first byte between E1 and EC, expecting the 3rd byte between 80 and BF.";
                    return i + 2;
                }
            }
            else
            {
                *message = "After a first byte between E1 and EC, expecting two following bytes.";
                return i;
            }
            i += 3;
        }
        else if (str[i] == 0xED) /* ED 80..9F 80..BF */
        {
            if (i + 2 < len) /* Expect a 2nd and 3rd byte */
            {
                if (str[i + 1] < 0x80 || str[i + 1] > 0x9F)
                {
                    *message = "After a first byte of ED, expecting 2nd byte between 80 and 9F.";
                    return i + 1;
                }
                if (str[i + 2] < 0x80 || str[i + 2] > 0xBF)
                {
                    *message = "After a first byte of ED, expecting 3rd byte between 80 and BF.";
                    return i + 2;
                }
            }
            else
            {
                *message = "After a first byte of ED, expecting two following bytes.";
                return i;
            }
            i += 3;
        }
        else if (str[i] >= 0xEE && str[i] <= 0xEF) /* EE..EF 80..BF 80..BF */
        {
            if (i + 2 < len) /* Expect a 2nd and 3rd byte */
            {
                if (str[i + 1] < 0x80 || str[i + 1] > 0xBF)
                {
                    *message = "After a first byte between EE and EF, expecting 2nd byte between 80 and BF.";
                    return i + 1;
                }
                if (str[i + 2] < 0x80 || str[i + 2] > 0xBF)
                {
                    *message = "After a first byte between EE and EF, expecting 3rd byte between 80 and BF.";
                    return i + 2;
                }
            }
            else
            {
                *message = "After a first byte between EE and EF, two following bytes.";
                return i;
            }
            i += 3;
        }
        else if (str[i] == 0xF0) /* F0 90..BF 80..BF 80..BF */
        {
            if (i + 3 < len) /* Expect a 2nd, 3rd 3th byte */
            {
                if (str[i + 1] < 0x90 || str[i + 1] > 0xBF)
                {
                    *message = "After a first byte of F0, expecting 2nd byte between 90 and BF.";
                    return i + 1;
                }
                if (str[i + 2] < 0x80 || str[i + 2] > 0xBF)
                {
                    *message = "After a first byte of F0, expecting 3rd byte between 80 and BF.";
                    return i + 2;
                }
                if (str[i + 3] < 0x80 || str[i + 3] > 0xBF)
                {
                    *message = "After a first byte of F0, expecting 4th byte between 80 and BF.";
                    return i + 3;
                }
            }
            else
            {
                *message = "After a first byte of F0, expecting three following bytes.";
                return i;
            }
            i += 4;
        }
        else if (str[i] >= 0xF1 && str[i] <= 0xF3) /* F1..F3 80..BF 80..BF 80..BF */
        {
            if (i + 3 < len) /* Expect a 2nd, 3rd 3th byte */
            {
                if (str[i + 1] < 0x80 || str[i + 1] > 0xBF)
                {
                    *message = "After a first byte of F1, expecting a 2nd byte between 80 and BF.";
                    return i + 1;
                }
                if (str[i + 2] < 0x80 || str[i + 2] > 0xBF)
                {
                    *message = "After a first byte of F1, expecting a 3rd byte between 80 and BF.";
                    return i + 2;
                }
                if (str[i + 3] < 0x80 || str[i + 3] > 0xBF)
                {
                    *message = "After a first byte of F1, expecting a 4th byte between 80 and BF.";
                    return i + 3;
                }
            }
            else
            {
                *message = "After a first byte of F1, expecting three following bytes.";
                return i;
            }
            i += 4;
        }
        else if (str[i] == 0xF4) /* F4 80..8F 80..BF 80..BF */
        {
            if (i + 3 < len) /* Expect a 2nd, 3rd 3th byte */
            {
                if (str[i + 1] < 0x80 || str[i + 1] > 0x8F)
                {
                    *message = "After a first byte of F4, expecting 2nd byte between 80 and 8F.";
                    return i + 1;
                }
                if (str[i + 2] < 0x80 || str[i + 2] > 0xBF)
                {
                    *message = "After a first byte of F4, expecting 3rd byte between 80 and BF.";
                    return i + 2;
                }
                if (str[i + 3] < 0x80 || str[i + 3] > 0xBF)
                {
                    *message = "After a first byte of F4, expecting 4th byte between 80 and BF.";
                    return i + 3;
                }
            }
            else
            {
                *message = "After a first byte of F4, expecting three following bytes.";
                return i;
            }
            i += 4;
        }
        else
        {
            *message = "Expecting bytes in the following ranges: 00..7F C2..F4.";
            return i;
        }
    }
    message = NULL;
    return 0;
}
