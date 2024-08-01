// EDCRE is built on top of `lec.cc` from cdrdao ( https://github.com/cdrdao/cdrdao/blob/master/trackdb/lec.cc ):

/*  cdrdao - write audio CD-Rs in disc-at-once mode
 *
 *  Copyright (C) 1998-2002 Andreas Mueller <andreas@daneb.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <assert.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

unsigned char existing_sector_header[4]; // store the existing header of the file

bool use_current_sector_header;


#define GF8_PRIM_POLY 0x11d // x^8 + x^4 + x^3 + x^2 + 1
#define EDC_POLY 0x8001801b // (x^16 + x^15 + x^2 + 1) (x^16 + x^2 + x + 1)
#define LEC_HEADER_OFFSET 12
#define LEC_DATA_OFFSET 16
#define LEC_MODE1_DATA_LEN 2048
#define LEC_MODE1_EDC_OFFSET 2064
#define LEC_MODE1_INTERMEDIATE_OFFSET 2068
#define LEC_MODE1_P_PARITY_OFFSET 2076
#define LEC_MODE1_Q_PARITY_OFFSET 2248
#define LEC_MODE2_FORM1_DATA_LEN (2048 + 8)
#define LEC_MODE2_FORM1_EDC_OFFSET 2072
#define LEC_MODE2_FORM2_DATA_LEN (2324 + 8)
#define LEC_MODE2_FORM2_EDC_OFFSET 2348

typedef uint8_t gf8_t;

static uint8_t GF8_LOG[256];
static gf8_t GF8_ILOG[256];

static const class Gf8_Q_Coeffs_Results_01
{
private:
  uint16_t table[43][256];

public:
  Gf8_Q_Coeffs_Results_01();
  ~Gf8_Q_Coeffs_Results_01() {}
  const uint16_t *operator[](int i) const { return &table[i][0]; }
  operator const uint16_t *() const { return &table[0][0]; }
} CF8_Q_COEFFS_RESULTS_01;

static const class CrcTable
{
private:
  uint32_t table[256];

public:
  CrcTable();
  ~CrcTable() {}
  uint32_t operator[](int i) const { return table[i]; }
  operator const uint32_t *() const { return table; }
} CRCTABLE;

// Creates the logarithm and inverse logarithm table that is required for performing multiplication in the GF(8) domain.
static void gf8_create_log_tables()
{
  uint8_t log;
  uint16_t b;

  for (b = 0; b <= 255; b++)
  {
    GF8_LOG[b] = 0;
    GF8_ILOG[b] = 0;
  }

  b = 1;

  for (log = 0; log < 255; log++)
  {
    GF8_LOG[(uint8_t)b] = log;
    GF8_ILOG[log] = (uint8_t)b;

    b <<= 1;

    if ((b & 0x100) != 0)
      b ^= GF8_PRIM_POLY;
  }
}

// Addition in the GF(8) domain: just the XOR of the values.
#define gf8_add(a, b) (a) ^ (b)

// Division in the GF(8) domain: Like multiplication but logarithms a subtracted.
static gf8_t gf8_div(gf8_t a, gf8_t b)
{
  int16_t sum;

  assert(b != 0);

  if (a == 0)
    return 0;

  sum = GF8_LOG[a] - GF8_LOG[b];

  if (sum < 0)
    sum += 255;

  return GF8_ILOG[sum];
}

Gf8_Q_Coeffs_Results_01::Gf8_Q_Coeffs_Results_01()
{
  int i, j;
  uint16_t c;
  gf8_t GF8_COEFFS_HELP[2][45];
  uint8_t GF8_Q_COEFFS[2][45];

  gf8_create_log_tables();

  /* build matrix H:
    1    1   ...  1   1
   a^44 a^43 ... a^1 a^0
  */

  for (j = 0; j < 45; j++)
  {
    GF8_COEFFS_HELP[0][j] = 1;                /* e0 */
    GF8_COEFFS_HELP[1][j] = GF8_ILOG[44 - j]; /* e1 */
  }

  // resolve equation system for parity byte 0 and 1

  // e1' = e1 + e0
  for (j = 0; j < 45; j++)
  {
    GF8_Q_COEFFS[1][j] = gf8_add(GF8_COEFFS_HELP[1][j],
                                 GF8_COEFFS_HELP[0][j]);
  }

  /* e1'' = e1' / (a^1 + 1) */
  for (j = 0; j < 45; j++)
  {
    GF8_Q_COEFFS[1][j] = gf8_div(GF8_Q_COEFFS[1][j], GF8_Q_COEFFS[1][43]);
  }

  // e0' = e0 + e1 / a^1
  for (j = 0; j < 45; j++)
  {
    GF8_Q_COEFFS[0][j] = gf8_add(GF8_COEFFS_HELP[0][j],
                                 gf8_div(GF8_COEFFS_HELP[1][j],
                                         GF8_ILOG[1]));
  }

  // e0'' = e0' / (1 + 1 / a^1)
  for (j = 0; j < 45; j++)
  {
    GF8_Q_COEFFS[0][j] = gf8_div(GF8_Q_COEFFS[0][j], GF8_Q_COEFFS[0][44]);
  }

  /*
   * Compute the products of 0..255 with all of the Q coefficients in
   * advance. When building the scalar product between the data vectors
   * and the P/Q vectors the individual products can be looked up in
   * this table
   *
   * The P parity coefficients are just a subset of the Q coefficients so
   * that we do not need to create a separate table for them.
   */

  for (j = 0; j < 43; j++)
  {

    table[j][0] = 0;

    for (i = 1; i < 256; i++)
    {
      c = GF8_LOG[i] + GF8_LOG[GF8_Q_COEFFS[0][j]];
      if (c >= 255)
        c -= 255;
      table[j][i] = GF8_ILOG[c];

      c = GF8_LOG[i] + GF8_LOG[GF8_Q_COEFFS[1][j]];
      if (c >= 255)
        c -= 255;
      table[j][i] |= GF8_ILOG[c] << 8;
    }
  }
}

// Reverses the bits in 'd'. 'bits' defines the bit width of 'd'.
static uint32_t mirror_bits(uint32_t d, int bits)
{
  int i;
  uint32_t r = 0;

  for (i = 0; i < bits; i++)
  {
    r <<= 1;

    if ((d & 0x1) != 0)
      r |= 0x1;

    d >>= 1;
  }

  return r;
}

// Build the CRC lookup table for EDC_POLY poly. The CRC is 32 bit wide and reversed (i.e. the bit stream is divided by the EDC_POLY with the LSB first order).
CrcTable::CrcTable()
{
  uint32_t i, j;
  uint32_t r;

  for (i = 0; i < 256; i++)
  {
    r = mirror_bits(i, 8);

    r <<= 24;

    for (j = 0; j < 8; j++)
    {
      if ((r & 0x80000000) != 0)
      {
        r <<= 1;
        r ^= EDC_POLY;
      }
      else
      {
        r <<= 1;
      }
    }

    r = mirror_bits(r, 32);

    table[i] = r;
  }
}

// Calculates the CRC of given data with given lengths based on the table lookup algorithm.
static uint32_t calc_edc(uint8_t *data, int len)
{
  uint32_t crc = 0;

  while (len--)
  {
    crc = CRCTABLE[(int)(crc ^ *data++) & 0xff] ^ (crc >> 8);
  }

  return crc;
}

// Calc EDC for a MODE 1 sector
static void calc_mode1_edc(uint8_t *sector)
{
  uint32_t crc = calc_edc(sector, LEC_MODE1_DATA_LEN + 16);

  sector[LEC_MODE1_EDC_OFFSET] = crc & 0xffL;
  sector[LEC_MODE1_EDC_OFFSET + 1] = (crc >> 8) & 0xffL;
  sector[LEC_MODE1_EDC_OFFSET + 2] = (crc >> 16) & 0xffL;
  sector[LEC_MODE1_EDC_OFFSET + 3] = (crc >> 24) & 0xffL;
}

// Calc EDC for a XA form 1 sector
static void calc_mode2_form1_edc(uint8_t *sector)
{
  uint32_t crc = calc_edc(sector + LEC_DATA_OFFSET,
                          LEC_MODE2_FORM1_DATA_LEN);

  sector[LEC_MODE2_FORM1_EDC_OFFSET] = crc & 0xffL;
  sector[LEC_MODE2_FORM1_EDC_OFFSET + 1] = (crc >> 8) & 0xffL;
  sector[LEC_MODE2_FORM1_EDC_OFFSET + 2] = (crc >> 16) & 0xffL;
  sector[LEC_MODE2_FORM1_EDC_OFFSET + 3] = (crc >> 24) & 0xffL;
}

// Calc EDC for a XA form 2 sector
static void calc_mode2_form2_edc(uint8_t *sector)
{
  uint32_t crc = calc_edc(sector + LEC_DATA_OFFSET,
                          LEC_MODE2_FORM2_DATA_LEN);

  sector[LEC_MODE2_FORM2_EDC_OFFSET] = crc & 0xffL;
  sector[LEC_MODE2_FORM2_EDC_OFFSET + 1] = (crc >> 8) & 0xffL;
  sector[LEC_MODE2_FORM2_EDC_OFFSET + 2] = (crc >> 16) & 0xffL;
  sector[LEC_MODE2_FORM2_EDC_OFFSET + 3] = (crc >> 24) & 0xffL;
}

// Writes the sync pattern to the given sector.
static void set_sync_pattern(uint8_t *sector)
{
  sector[0] = 0;
  sector[1] = sector[2] = sector[3] = sector[4] = sector[5] =
      sector[6] = sector[7] = sector[8] = sector[9] = sector[10] = 0xff;
  sector[11] = 0;
}

static uint8_t bin2bcd(uint8_t b)
{
  return (((b / 10) << 4) & 0xf0) | ((b % 10) & 0x0f);
}

// Builds the sector header.
static void set_sector_header(uint8_t mode, uint32_t adr, uint8_t *sector)
{
  if(use_current_sector_header)
  {
    sector[LEC_HEADER_OFFSET] = existing_sector_header[0];
    //printf("existing sector header byte: %02X\n", existing_sector_header[0]);
    sector[LEC_HEADER_OFFSET + 1] = existing_sector_header[1];
    //printf("existing sector header byte: %02X\n", existing_sector_header[1]);
    sector[LEC_HEADER_OFFSET + 2] = existing_sector_header[2];
    //printf("existing sector header byte: %02X\n", existing_sector_header[2]);
    sector[LEC_HEADER_OFFSET + 3] = existing_sector_header[3];
    //printf("existing sector header byte: %02X\n", existing_sector_header[3]);
  } else {
    sector[LEC_HEADER_OFFSET] = bin2bcd(adr / (60 * 75));
    sector[LEC_HEADER_OFFSET + 1] = bin2bcd((adr / 75) % 60);
    sector[LEC_HEADER_OFFSET + 2] = bin2bcd(adr % 75);
    sector[LEC_HEADER_OFFSET + 3] = mode;
  }
}

// Calculate the P parities for the sector. The 43 P vectors of length 24 are combined with the GF8_P_COEFFS.
static void calc_P_parity(uint8_t *sector)
{
  int i, j;
  uint16_t p01_msb, p01_lsb;
  uint8_t *p_lsb_start;
  uint8_t *p_lsb;
  uint8_t *p0, *p1;
  uint8_t d0, d1;

  p_lsb_start = sector + LEC_HEADER_OFFSET;

  p1 = sector + LEC_MODE1_P_PARITY_OFFSET;
  p0 = sector + LEC_MODE1_P_PARITY_OFFSET + 2 * 43;

  for (i = 0; i <= 42; i++)
  {
    p_lsb = p_lsb_start;

    p01_lsb = p01_msb = 0;

    for (j = 19; j <= 42; j++)
    {
      d0 = *p_lsb;
      d1 = *(p_lsb + 1);

      p01_lsb ^= CF8_Q_COEFFS_RESULTS_01[j][d0];
      p01_msb ^= CF8_Q_COEFFS_RESULTS_01[j][d1];

      p_lsb += 2 * 43;
    }

    *p0 = p01_lsb;
    *(p0 + 1) = p01_msb;

    *p1 = p01_lsb >> 8;
    *(p1 + 1) = p01_msb >> 8;

    p0 += 2;
    p1 += 2;

    p_lsb_start += 2;
  }
}

// Calculate the Q parities for the sector. The 26 Q vectors of length 43 are combined with the GF8_Q_COEFFS.
static void calc_Q_parity(uint8_t *sector)
{
  int i, j;
  uint16_t q01_lsb, q01_msb;
  uint8_t *q_lsb_start;
  uint8_t *q_lsb;
  uint8_t *q0, *q1, *q_start;
  uint8_t d0, d1;

  q_lsb_start = sector + LEC_HEADER_OFFSET;

  q_start = sector + LEC_MODE1_Q_PARITY_OFFSET;
  q1 = sector + LEC_MODE1_Q_PARITY_OFFSET;
  q0 = sector + LEC_MODE1_Q_PARITY_OFFSET + 2 * 26;

  for (i = 0; i <= 25; i++)
  {
    q_lsb = q_lsb_start;

    q01_lsb = q01_msb = 0;

    for (j = 0; j <= 42; j++)
    {
      d0 = *q_lsb;
      d1 = *(q_lsb + 1);

      q01_lsb ^= CF8_Q_COEFFS_RESULTS_01[j][d0];
      q01_msb ^= CF8_Q_COEFFS_RESULTS_01[j][d1];

      q_lsb += 2 * 44;

      if (q_lsb >= q_start)
        q_lsb -= 2 * 1118;
    }

    *q0 = q01_lsb;
    *(q0 + 1) = q01_msb;

    *q1 = q01_lsb >> 8;
    *(q1 + 1) = q01_msb >> 8;

    q0 += 2;
    q1 += 2;

    q_lsb_start += 2 * 43;
  }
}

// Encodes a MODE 0 sector. 'adr' is the current physical sector address. 'sector' must be 2352 byte wide
void lec_encode_mode0_sector(uint32_t adr, uint8_t *sector)
{
  uint16_t i;

  set_sync_pattern(sector);
  set_sector_header(0, adr, sector);

  sector += 16;

  for (i = 0; i < 2336; i++)
    *sector++ = 0;
}

// Encodes a MODE 1 sector. 'adr' is the current physical sector address. 'sector' must be 2352 byte wide containing 2048 bytes user data at offset 16
void lec_encode_mode1_sector(uint32_t adr, uint8_t *sector)
{
  set_sync_pattern(sector);
  set_sector_header(1, adr, sector);
  calc_mode1_edc(sector);

  // clear the intermediate field
  sector[LEC_MODE1_INTERMEDIATE_OFFSET] =
      sector[LEC_MODE1_INTERMEDIATE_OFFSET + 1] =
          sector[LEC_MODE1_INTERMEDIATE_OFFSET + 2] =
              sector[LEC_MODE1_INTERMEDIATE_OFFSET + 3] =
                  sector[LEC_MODE1_INTERMEDIATE_OFFSET + 4] =
                      sector[LEC_MODE1_INTERMEDIATE_OFFSET + 5] =
                          sector[LEC_MODE1_INTERMEDIATE_OFFSET + 6] =
                              sector[LEC_MODE1_INTERMEDIATE_OFFSET + 7] = 0;

  calc_P_parity(sector);
  calc_Q_parity(sector);
}

// Encodes a MODE 2 sector. 'adr' is the current physical sector address. 'sector' must be 2352 byte wide containing 2336 bytes user data at offset 16
void lec_encode_mode2_sector(uint32_t adr, uint8_t *sector)
{
  set_sync_pattern(sector);
  set_sector_header(2, adr, sector);
}

// Encodes a XA form 1 sector. 'adr' is the current physical sector address. 'sector' must be 2352 byte wide containing 2048+8 bytes user data at offset 16
void lec_encode_mode2_form1_sector(uint32_t adr, uint8_t *sector)
{
  set_sync_pattern(sector);
  calc_mode2_form1_edc(sector);

  // P/Q partiy must not contain the sector header so clear it
  sector[LEC_HEADER_OFFSET] =
      sector[LEC_HEADER_OFFSET + 1] =
          sector[LEC_HEADER_OFFSET + 2] =
              sector[LEC_HEADER_OFFSET + 3] = 0;

  calc_P_parity(sector);
  calc_Q_parity(sector);
  // Finally add the sector header
  set_sector_header(2, adr, sector);
}

// Encodes a XA form 2 sector. 'adr' is the current physical sector address. 'sector' must be 2352 byte wide containing 2324+8 bytes user data at offset 16
void lec_encode_mode2_form2_sector(uint32_t adr, uint8_t *sector)
{
  set_sync_pattern(sector);
  calc_mode2_form2_edc(sector);
  set_sector_header(2, adr, sector);
}

bool is_file(char *arg)
{
    FILE *file = fopen (arg, "rb");
    
    if(file == NULL)
    {
       return false;
    } else {
        fclose (file);
        return true;
    }
}

int main(int argc, char **argv)
{
    off_t file_size;
    const int pregap = 150;
    char *data_track_file = 0;
    
    int32_t data_track_fd;
    int32_t percentage;
    uint8_t buffer1[2352]; // original input sector buf
    uint8_t buffer2[2352]; // output sector buf with potentially fixed EDC/ECC data

    uint32_t lba = pregap; // Correct EDC/ECC throughout entire image starting at the first sector 0 (by default)
    uint32_t number_of_sectors_fixed = 0; // keep track of number of sectors with updated EDC/ECC data
    uint32_t custom_sector_offset = 0; // by default start at 0 (LBA 150)
    uint32_t number_of_mode_1_sectors = 0;
    uint32_t number_of_mode_2_form_1_sectors = 0;
    uint32_t number_of_mode_2_form_2_sectors = 0;

    bool verbose = false;
    bool test_validity_only = false;
    bool mode1 = false;
    bool mode2_form1 = false;
    bool mode2_form2 = false;

    printf("EDCRE %s - EDC/ECC Regenerator By Alex Free\nhttps://alex-free.github.io/edcre\nMade Possible By Modifying CDRDAO (GPLv2) Source Code:\nhttps://github.com/cdrdao/cdrdao\n\n", VERSION);

    if( (argc < 2) || (argc > 8) )
    {
        printf(
            "Error: Incorrect number of arguents.\n\n"

            "Usage: edcre <optional arguments> <input file>\n\n"
            
            "<input file>      The data track bin file. This is the first (i.e. track 01) or only bin file in a bin/cue cd image\n\n"

            "Optional Arguments:\n\n-v    Verbose, display each sector LBA number containing invalid EDC data, if any.\n\n"
            
            "-t   Test the disc image for sectors that contain invalid EDC/ECC. Does not modify the data track bin file in any way.\n\n"
            
            "-s    Start EDC/ECC regeneration at sector number following the -s argument instead of at sector 0. In example, -s 16 starts regeneration at sector 16 (LBA 166) which would be the system volume for a PSX disc image (and what is recommended most of the time). TOCPerfect Patcher users want -s 15 here however.\n\n"
            
            "-k   Keep existing sector header data from data file. This prevents EDCRE from regenerating the MM:SS:FF in the sector header. Useful for test_validity_onlying or regenerating EDC/ECC in a disc image file snippet (i.e. the last data track pregap of a Dreamcast GD-ROM image doesn't start at sector 0 and is a separate file).\n"
            );
        return 1;
    }

    for(int i = 1; i < argc; i++)
    {
        /* feature activation handling */
        if((strcmp(argv[i],"-v")==0) && (i < (argc - 1)))
        {
            verbose = true;
        }
        
        if((strcmp(argv[i],"-t")==0) && (i < (argc - 1)))
        {
            test_validity_only = true;
        }

        if((strcmp(argv[i],"-k")==0) && (i < (argc - 1)))
        {
            use_current_sector_header = true;
            printf("Using existing sector header from data file\n");
        }

        if((strcmp(argv[i],"-s")==0) && (i < (argc - 2)))
        {
            custom_sector_offset = strtoul(argv[i + 1], NULL, 0); // next argument
            lba = (lba + custom_sector_offset);
            custom_sector_offset = (custom_sector_offset * 0x930);
            printf("Starting EDC/EEC Regeneration at LBA %u (0x%08X)\n", lba, custom_sector_offset);
        }

        /* syntax error handling */
        if((strcmp(argv[i],"-v")==0) && (i == (argc - 1)) && (!is_file (argv[i]) ) )
        {
            fprintf(stderr, "Error: -v must be followed by a file\n");
            return 1;
        }

        if((strcmp(argv[i],"-t")==0) && (i == (argc - 1)) && (!is_file (argv[i]) ) )
        {
            fprintf(stderr, "Error: -t must be followed by a file\n");
            return 1;
        }

        if((strcmp(argv[i],"-k")==0) && (i == (argc - 1)) && (!is_file (argv[i]) ) )
        {
            fprintf(stderr, "Error: -k must be followed by a file\n");
            return 1;
        }
        
        if((strcmp(argv[i],"-s")==0) && (i >= (argc - 2)) && (!is_file (argv[i]) ) )
        {
            fprintf(stderr, "Error: -s must be followed by a number then a file\n");
            return 1;
        }
    }

    data_track_file = argv[(argc - 1)]; // last argument

    if((data_track_fd = open(data_track_file, O_RDWR)) < 0)
    {
        fprintf(stderr, "Cannot open data track bin file\n");
        return 1;
    }

    // Use lseek() to move the file pointer to the end of the file
    file_size = lseek(data_track_fd, 0, SEEK_END);
    
    if(file_size == (off_t) -1) 
    {
        perror("Error getting the file size");
        close(data_track_fd); // Close the file descriptor
        return 1;
    }

    unsigned int total_number_of_sectors = (file_size / 2352);

    lseek(data_track_fd, custom_sector_offset, SEEK_SET);

    while(1)
    {

        if(read(data_track_fd, buffer1, 2352) != 2352)
        {
            break; // EOF
        }

        percentage =( ( (lba - pregap + 1) * 100) / total_number_of_sectors); // + 1 since lba 150 = first sector (sector 0)
        printf("\rProgress: %d%%", percentage);
        fflush(stdout); // clear double buffered input so terminal cursor isn't going nuts
    
        if(!(
        (buffer1[0] == 0x00) &&
        (buffer1[1] == 0xFF) &&
        (buffer1[2] == 0xFF) &&
        (buffer1[3] == 0xFF) &&
        (buffer1[4] == 0xFF) &&
        (buffer1[5] == 0xFF) &&
        (buffer1[6] == 0xFF) &&
        (buffer1[7] == 0xFF) &&
        (buffer1[8] == 0xFF) &&
        (buffer1[9] == 0xFF) &&
        (buffer1[10] == 0xFF) &&
        (buffer1[11] == 0x00)
        ) ) 
        {
            printf("\rProgress: 100%%\nCDDA sectors detected, data sector read complete.");
            break; // verify this is a data sector by detecting sync pattern. If the sync pattern isn't the first 12 bytes of the current sector being checked then we have reached a CDDA sector (i.e. image has been binmerged with data track + audio tracks into one)
        }

        if(use_current_sector_header)
        {
            for(int i = 0; i < 4; i++)
            {
                existing_sector_header[i] = buffer1[i + 12]; // by default we regenerate sector header (MM:SS:FF position), but if your using an incomplete arbirtrary file snippet that doesn't start at sector 0 you don't want this.
            }
        }

        switch (*(buffer1 + 12 + 3))
        {
            case 1:
                memcpy(buffer2 + 16, buffer1 + 16, 2048);
                lec_encode_mode1_sector(lba, buffer2);
                mode1 = true;
                mode2_form1 = false;
                mode2_form2 = false;
                number_of_mode_1_sectors++;
                break;
            case 2:
                if ((*(buffer1 + 12 + 4 + 2) & 0x20) != 0)
                {
                    // Mode 2 form 2 sector
                    memcpy(buffer2 + 16, buffer1 + 16, 2324 + 8);
                    lec_encode_mode2_form2_sector(lba, buffer2);
                    mode1 = false;
                    mode2_form1 = false;
                    mode2_form2 = true;
                    number_of_mode_2_form_1_sectors++;
                } else {
                    // Mode 2 Form 1 sector
                    memcpy(buffer2 + 16, buffer1 + 16, 2048 + 8);
                    lec_encode_mode2_form1_sector(lba, buffer2);
                    mode1 = false;
                    mode2_form1 = true;
                    mode2_form2 = false;
                    number_of_mode_2_form_2_sectors++;
                }
                break;
        }

        if(memcmp(buffer1, buffer2, 2352) != 0) // original buffer is compared to a known correct version of the buffer
        {
            if((verbose) && (!test_validity_only))
            {
                if(mode1)
                {
                    if(use_current_sector_header)
                    {
                        printf("\nUpdated sector %u (MODE1)\n", (lba - pregap) );
                    } else {
                        printf("\nUpdated sector %u (LBA: %u) (MODE1)\n", (lba - pregap), lba);
                    }
                } else if(mode2_form1) {
                    if(use_current_sector_header)
                    {
                        printf("\nUpdated sector %u (MODE2_FORM1)\n", (lba - pregap) );
                    } else {
                        printf("\nUpdated sector %u (LBA: %u) (MODE2_FORM1)\n", (lba - pregap), lba);
                    }
                } else if(mode2_form2) {                
                    if(use_current_sector_header)
                    {
                        printf("\nUpdated sector %u (MODE2_FORM2)\n", (lba - pregap) );
                    } else {
                        printf("\nUpdated sector %u (LBA: %u) (MODE2_FORM2)\n", (lba - pregap), lba);
                    }                
                }
            } else if(test_validity_only) {
                if(mode1)
                {
                    if(use_current_sector_header)
                    {
                        printf("\nSector %u (MODE1) is not valid\n", (lba - pregap) );
                    } else {
                        printf("\nSector %u (LBA: %u) (MODE1) is not valid\n", (lba - pregap),lba);
                    }               
                } else if(mode2_form1) {
                    if(use_current_sector_header)
                    {
                        printf("\nSector %u (MODE2_FORM1) is not valid\n", (lba - pregap) );
                    } else {
                        printf("\nSector %u (LBA: %u) (MODE2_FORM1) is not valid\n", (lba - pregap),lba);
                    }  
                } else if(mode2_form2) {
                    if(use_current_sector_header)
                    {
                        printf("\nSector %u (MODE2_FORM1) is not valid\n", (lba - pregap) );
                    } else {
                        printf("\nSector %u (LBA: %u) (MODE2_FORM1) is not valid\n", (lba - pregap),lba);
                    }                  
                }
            }

            number_of_sectors_fixed++;
        }

        if(!test_validity_only)
        {
            lseek(data_track_fd, -2352, SEEK_CUR); // when we read() before, we advanced the fpos. We want to rewrite the previously read sector so go back a sector's worth and then write().
        
            if(write(data_track_fd, buffer2, 2352) != 2352)
            {
                printf("\nError writing sector at LBA: %u\n", lba);
                close(data_track_fd);
                return 1;
            } 
        }
        
        lba++;
    }

    close(data_track_fd);
    printf("\n\nScan report:\n************\n");
    printf("%d Mode 1 Sector(s)\n", number_of_mode_1_sectors);
    printf("%d Mode 2 Form 1 Sector(s)\n" , number_of_mode_2_form_1_sectors);
    printf("%d Mode 2 Form 2 Sector(s)\n" , number_of_mode_2_form_2_sectors);

    if(number_of_sectors_fixed > 0)
    {
        if(number_of_sectors_fixed == 1)
        {
            if(test_validity_only)
            {
                printf("\nFound invalid EDC/ECC data in 1 sector\n");
            } else {
                printf("\nUpdated EDC/ECC in 1 sector\n");
            }
        } else {
            if(test_validity_only)
            {
                printf("\nFound invalid EDC/ECC in %u sectors\n", number_of_sectors_fixed);
            } else {
                printf("\nUpdated EDC/ECC in %u sectors\n", number_of_sectors_fixed);
            }
        }
    } else {
        if(test_validity_only)
        {
            printf("\nAll scanned sectors already contain valid ECC/EDC data\n");
        } else {
            printf("\nNo sectors needed EDC/ECC regeneration, nothing done\n");
        }
    }
  
    return 0;
}
