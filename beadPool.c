//This is a helper function
//Should do all the reading and passes that to main File
//All my includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
 //This is a helper function
//Should do all the reading and passes that to main File

//All my includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "beadPool.h"

//Just a test

#include <limits.h>


int cmpfunc (const void * a, const void * b);


//atrributes. The sizes of these still needs to be calculated
//Would rather store them all on the heap for some flexibility
int main(){
  printf("Starting...\n");
  
  int t =  __parse_file("/home/jonahk/H3Africa.bpm");

  
  //COMPLEMENT_MAP = {"A": "T", "T": "A", "C": "G", "G": "C", "D": "D", "I": "I"};
  //This should be a map.
  char mapKeys[7] = {'A', 'T', 'C', 'G', 'D', 'I'};
  char mapVals[7] = {'T','A', 'G', 'C', 'D','I'};

  return 0;
}





//Actually declaring the functions
//I think these should be put into anther file and imported. Just figuring out the syntax

int __parse_file(char* filename){

   ht_t *ht = ht_create();

   
  char buffer[3];

  char* unstrct_string;
  char* control_config;
  char* manifest_name;

  FILE *ptr;
  int version_flag = 4096;
  char versionChar;
  int versionInt;
  int readIntVer;
  uint32_t num_loci;


  ptr = fopen(filename,"rb");
  if (ptr == NULL) {
    printf("Cannot find filename.");
    exit(-9);
  }
  int err = fread(buffer,sizeof(buffer),1,ptr);
  if (err != 1) {
    printf("Unable to read Magic Number: <%d>\n",err);
    exit(-10);
  }

  if (strncmp(buffer,"BPM",3) != 0) {
    printf("Given file is not BPM <%d>\n",err);
    exit(-11);
  }

  //Possibly could have this in its own function
  int readAttempt = fread(&versionChar, sizeof(char), 1, ptr);
  if(readAttempt != 1){
    printf("Unable to read versionn: %d\n", readAttempt);
    exit(-14);
  }
  versionInt = (int)(versionChar);
  if(versionInt != 1){
    printf("Wrong BPM version\n"); // not a BPM 1 file
    exit(-12);


  }
  readAttempt = fread(&readIntVer, sizeof(readIntVer), 1, ptr);
  if(readAttempt != 1){
    printf("Error reading in int: %d\n", readAttempt);
    exit(-12);
  }
  printf("Version read:%i\n\n", readIntVer);

  if(readIntVer & version_flag == version_flag){
    readIntVer = readIntVer ^ version_flag;
  }

  if(readIntVer > 5 || readIntVer < 3){
    printf("Unsupported BPM version");
    exit(-13);
  }

  manifest_name = read_string(ptr);

  manifest_name[29] = '\0';
  printf("Manifest name read succesfully:%s\n\n", manifest_name);

  if(readIntVer > 1){
    control_config = read_string(ptr);
     printf("control_config read successfully\n\n");
  }

  num_loci = read_int(ptr);
  printf("Num loci:%i\n\n", num_loci);

  char** names = (char **)calloc(num_loci, sizeof(char*));

  fseek(ptr, 4 * num_loci, SEEK_CUR);

  printf("about to create hashtable\n\n");
  for(int i = 0; i < num_loci; ++i){
    names[i] = read_string(ptr);

    ht_set(ht, names[i], i);
   //       hash_add(names[i], i);
    }
  printf("Array of all names obtained\n\n");
  printf("Hash table created\n\n");

  int* normalization_ids = (int*)calloc(num_loci, sizeof(uint8_t));

  //Not 10 here!! NUm loci!!
  for(int i = 0; i < num_loci; ++i){
    normalization_ids[i] = read_byte(ptr);
  }

  int* assay_types = (int*)calloc(num_loci, sizeof(int));
  int* addresses = (int*)calloc(num_loci, sizeof(int));
  char** snps = (char**)calloc(num_loci, sizeof(char*));


  char** chroms = (char**)calloc(num_loci, sizeof(char*));

  int* map_infos = (int*)calloc(num_loci, sizeof(int));
  int* ref_strands = (int*)calloc(num_loci, sizeof(int));
  int* source_strands = (int*)calloc(num_loci, sizeof(int));


  //for(int i = 0; i < num_loci; ++i){
  for(int i = 0; i < num_loci; ++i){
    locusEntry *locus_entry = create_locus_entry(ptr);
    int arrayPos = ht_get(ht, locus_entry->name);

    assay_types[arrayPos] = locus_entry->assay_type;
    addresses[arrayPos] = locus_entry->address_a;
    snps[arrayPos] = locus_entry->snp;
    chroms[arrayPos] = locus_entry->chrom;
    
    map_infos[arrayPos] = locus_entry->map_info;
    ref_strands[arrayPos] = locus_entry->ref_strand;
    source_strands[arrayPos] = locus_entry->source_strand;
  }

  //I want to compare number of elements in each array
  if( sizeof(normalization_ids)/4 != sizeof(assay_types)/4){
    printf("Manifest format error: read invalid number of assay entries\n\n");
    exit(-59);
  }

  int* all_norms_ids = (int*)calloc(num_loci, sizeof(int));
  for(int i = 0; i < num_loci; ++i){
    normalization_ids[i] += 100 * assay_types[i];

    // # To mimic the byte-wrapping behavior from GenomeStudio, AutoCall, IAAP take the mod of 256
    normalization_ids[i] %= 256;
    all_norms_ids[i] = normalization_ids[i];
  }
  printf("Before sorting the list is: \n");

  qsort(normalization_ids, num_loci, sizeof(int), cmpfunc);





  printf("I made it to the end.\n");
  printf("freeing memeory...\n");

  free(all_norms_ids);
  //  free(names);
  // free(normalization_id);
  free(assay_types);
  // free(addresses);
  free(snps);
  free(chroms);
  free(map_infos);
  free(ref_strands);
  free(source_strands);

  return 1;
  
}

int read_byte(FILE* ptr){
  char readIn;
  int read;
  int readAttempt = fread(&readIn, sizeof(char), 1, ptr);
  if(readAttempt != 1){
    printf("Unable to read in byte: %d\n", readAttempt);
    exit(-14);
  }
  read = (int)(readIn);

  return read;
}

uint32_t read_int(FILE* ptr){
  uint32_t num;
  int success = fread(&num, sizeof(uint32_t), 1, ptr);
  if(success != 1){
    printf("int was read unsuccessfully:%i \n\n", success);
    exit(-14);
  }

   return num;
}

char* read_string(FILE* ptr){
  uint32_t total_length = 0;

  uint32_t num_bytes = 0;
  uint32_t read;
  char readIn;
  uint32_t readAttempt;

  int partial_length = read_byte(ptr);

  if(partial_length < 0){
    partial_length = 256 + partial_length;
  }


  int b = partial_length & 0x80;
  while(b > 0){
    total_length += (partial_length & 0x7F) << (7 * num_bytes);
    readAttempt = fread(&partial_length, sizeof(char), 1, ptr);
    if(readAttempt != 1){
      printf("Error with reading String");
      exit(-15);
    }
    b = partial_length & 0x80;
    num_bytes += 1;
  }


  total_length += partial_length << (7 * num_bytes);
  
  char* result;

  if(total_length == 0){
    result = "";

  }else{
    result=(char*)malloc(total_length+1);
    readAttempt = fread(result, total_length, 1, ptr);
    if(readAttempt != 1){
      printf("Error reading in string");
      exit(-12);
    }
    result[total_length] = '\0';

    if(strlen(result) < total_length){
      printf("Unable to read the entire string");
      exit(-12);
    }
  }

  return result;
}


//LocusEntry object functions

locusEntry* create_locus_entry(FILE* ptr){
  locusEntry* locus_entry = (locusEntry*)malloc(sizeof(locusEntry));
  

  //This is initiallisation Phase
  char* ilmn_id = "";
  char* name = "";
  char* snp = "";
  char* chrom = "";
  int map_info = -1;
  int assay_type = -1;
  int address_a = -1;
  int address_b = -1;
  int ref_strand = 0;
  int source_strand = 0;
  //Entering the pass_file phase
  int version = read_int(ptr);


 if(version == 6 || version == 7 || version == 8){
    ilmn_id = read_string(ptr);
    
    char* temp = (char*)malloc(sizeof(char)*45);
    
    int count = 0;
    int usedCounter = 0;
    
    int target = -1;
    for(int i = 0; ilmn_id[i]!=0; i++){
      if(ilmn_id[i] == '_'){
	target = i;
      }
    }
    
    char value = ilmn_id[target - 1];
    
    source_strand = from_string(value);

    name = read_string(ptr);

    char* a;
    for(int i = 0; i < 3; ++i){
      a = read_string(ptr);
    }
    
    int tempRead;
    int readAttempt = fread(&tempRead, 4, 1, ptr);
    
    
    for(int i = 0; i < 2; ++i){
      a = read_string(ptr);
    }

    snp = read_string(ptr);
    
    chrom = read_string(ptr);

    for(int i = 0; i < 2; ++ i){
      a = read_string(ptr);
    }

    map_info = atoi(read_string(ptr));

    for(int i = 0; i < 2; ++i){
      a = read_string(ptr);
    }

    address_a = read_int(ptr);
    address_b = read_int(ptr);

    for(int i = 0; i < 7; ++ i){
      a = read_string(ptr);
    }

    readAttempt = fread(&tempRead, 3, 1, ptr);
    assay_type = read_byte(ptr);
    if(assay_type != 0 && assay_type != 1 && assay_type != 2){
      printf("Format error in reading assay type from locus entry\n\n");
      exit(15);

    }

    if(address_b == 0){
      if(assay_type != 0){
	printf("Manifest format error: Assay type is inconsistent with address B");
	exit(15);
      }
    }else{
      if(assay_type == 0){
	printf("Manifest format error: Assay type is inconsistent with address B");
	exit(15);
      }
    }
    
  }
  if(version == 7 || version == 8){
    int tempRead = 0;
    int readAttempt = fread(&tempRead, 4 * 4, 1, ptr);
 
  }

  if(version == 8){
    ref_strand = ref_from_string(read_string(ptr)[0]);
  }
  
  locus_entry->ilmn_id = ilmn_id;
  locus_entry->name = name;
  locus_entry->snp = snp;
  
  locus_entry->chrom = chrom;
  locus_entry->map_info = map_info;
  locus_entry->assay_type = assay_type;
  locus_entry->address_a = address_a;
  locus_entry->address_b = address_b;
  locus_entry->ref_strand = ref_strand;
  locus_entry->source_strand = source_strand;
  
  return locus_entry;
}

//The functions from the SourceStrand object
char to_string(int source_strand){
  if(source_strand == 0){
    return 'U';
  }
  else if(source_strand == 1){
    return 'F';
  }
  else if(source_strand == 2){
    return 'R';

  }

  //Should not get here
  printf("Invalid source_stand given to the to_string function\n");
  exit(-14);
}

int from_string(char source_strand){
  if(source_strand == 'U' || source_strand == '\0'){
    return 0;
  }
  else if(source_strand == 'F'){
    return 1;
  }
  else if(source_strand == 'R'){
    return 2;
  }

  //This should not have run
  printf("invalid source_strand given in from_string function");
  exit(-13);
}

int ref_from_string(char ref_strand){
  if(ref_strand == 'U' || ref_strand == '\0'){
    return 0;
  }else if(ref_strand == '+'){
    return 1;
  }else if(ref_strand == '-'){
    return 2;
  }
}


unsigned int hash(const char *key) {
    unsigned long int value = 0;
    unsigned int i = 0;
    unsigned int key_len = strlen(key);

    // do several rounds of multiplication
    for (; i < key_len; ++i) {
        value = value * 37 + key[i];
    }

    // make sure value is 0 <= value < TABLE_SIZE
    value = value % TABLE_SIZE;

    return value;
}

entry_t *ht_pair(const char *key, const int value) {
    // allocate the entry
    entry_t *entry = malloc(sizeof(entry_t) * 1);
    entry->key = malloc(strlen(key) + 1);
    entry->value = 0;

      //(int* )malloc(sizeof(value) + 1);

    // copy the key and value in place
    strcpy(entry->key, key);
    entry->value = value;

    // next starts out null but may be set later on
    entry->next = NULL;

    return entry;
}

ht_t *ht_create(void) {
    // allocate table
    ht_t *hashtable = malloc(sizeof(ht_t) * 1);

    // allocate table entries
    hashtable->entries = malloc(sizeof(entry_t*) * TABLE_SIZE);

    // set each to null (needed for proper operation)
    int i = 0;
    for (; i < TABLE_SIZE; ++i) {
        hashtable->entries[i] = NULL;
    }

    return hashtable;
}

void ht_set(ht_t *hashtable, const char *key, int value) {
    unsigned int slot = hash(key);

    // try to look up an entry set
    entry_t *entry = hashtable->entries[slot];

    // no entry means slot empty, insert immediately
    if (entry == NULL) {
        hashtable->entries[slot] = ht_pair(key, value);
        return;
    }

    entry_t *prev;

    // walk through each entry until either the end is
    // reached or a matching key is found
    while (entry != NULL) {
        // check key
        if (strcmp(entry->key, key) == 0) {
            // match found, replace value
	  //            free(entry->value);
	  //            entry->value = malloc(strlen(value) + 1);
            entry->value = value;
            return;
        }

        // walk to next
        prev = entry;
        entry = prev->next;
    }

    // end of chain reached without a match, add new
    prev->next = ht_pair(key, value);
}

int ht_get(ht_t *hashtable, const char *key) {
    unsigned int slot = hash(key);

    // try to find a valid slot
    entry_t *entry = hashtable->entries[slot];

    // no slot means no entry
    if (entry == NULL) {
        return -1;
    }

    // walk through each entry in the slot, which could just be a single thing
    while (entry != NULL) {
        // return value if found
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }

        // proceed to next key if available
        entry = entry->next;
    }

    // reaching here means there were >= 1 entries but no key match
    return -1;
}
//Ending

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}
