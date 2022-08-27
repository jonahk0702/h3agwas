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
uint8_t read_byte(FILE* ptr);
char*  read_string(FILE* ptr);
char read_char(char* handle);
int decode_code_point(char **s);
uint8_t __parse_file(char* filename);


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



//Functions to read the GTC
int EndsWithGTC( char *string );
uint8_t is_write_complete(uint8_t version, char* fileName, uint32_t seekValue, uint32_t offset, uint32_t count);
int get_num_intensity_only(char* fileName, uint32_t seekValue);
uint8_t get_slide_identifier(char* fileName, uint32_t seekValue);

int get_logr_ratios(char* fileName, uint32_t seekValue, uint32_t offset, uint8_t numBytes, uint8_t version, uint32_t count);
uint32_t __get_generic_array_numpy(char* fileName, uint32_t seekValue, uint32_t offset, uint8_t numBytes, uint32_t count);
uint8_t __get_generic(char* fileName, uint32_t seekValue);  
char* read_string(FILE* ptr);
uint8_t* __get_generic_array(uint32_t seekValue, uint32_t offset, uint32_t count, char* fileName);
  
uint8_t* get_genotypes(char* fileName, uint32_t seekValue);

char * get_base_calls_generic(char** snps, int* ref_strands, uint8_t* genotypes, uint32_t seekValue, char* fileName, uint32_t num_loci);
char * get_base_calls_plus_strand(char** snps, int* ref_strands, uint8_t* genotypes, uint32_t seekValue, char* fileName, uint32_t num_loci);


char complement(char nucleotide);
