#include <stdint.h>
#include <stdio.h>



typedef struct{
  char* ilmn_id;
  char* name;
  char* snp;
  char* chrom;
  int map_info;
  int assay_type;
  int address_a;
  int address_b;
  int ref_strand;
  int source_strand;
} locusEntry;



uint32_t read_int(FILE* ptr); // I know, read_int returns a long...
char* read_float(char* filename);
int read_byte(FILE* ptr);
char*  read_string(FILE* ptr);
char read_char(char* handle);
int decode_code_point(char **s);
int __parse_file(char* filename);


locusEntry* create_locus_entry(FILE* ptr);

//Now declaring functions from the SourceStrang object
char to_string(int source_strand);
int from_string(char source_strand);

int ref_from_string(char ref_strand);


#define TABLE_SIZE 10000000

typedef struct entry_t {
    char *key;
    int value;
    struct entry_t *next;
} entry_t;

typedef struct {
    entry_t **entries;
} ht_t;



unsigned int hash(const char *key);
entry_t *ht_pair(const char *key, const int value);
ht_t *ht_create(void);
void ht_set(ht_t *hashtable, const char *key, int value);
int ht_get(ht_t *hashtable, const char *key);
