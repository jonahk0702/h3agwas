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

//Declaring the functions.
//On first iteration, each function will return sample data. Testing my c syntax
//On second iteration, each function will return proper data.

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

//char* read_ushort(char* filename);
uint32_t read_int(FILE* ptr); // I know, read_int returns a long...
char* read_float(char* filename);
int read_byte(FILE* ptr);
char*  read_string(FILE* ptr);
char read_char(char* handle);
int decode_code_point(char **s);
int __parse_file(char* filename);

int id2hash(char* name);
int  hash_find(char ** bim_table, char * snp);
void hash_add(char * snp, int p);
void hash_init();
locusEntry* create_locus_entry(FILE* ptr);

//Now declaring functions from the SourceStrang object
char to_string(int source_strand);
int from_string(char source_strand);

int ref_from_string(char ref_strand);



//Some constants for the hash Table
#define hash_size (1<<30)

static int  table[hash_size];
const static int  mask = hash_size-1;
static char **id_table;


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

  hash_init();
  printf("hashtable initilised\n\n");
  fseek(ptr, 4 * num_loci, SEEK_CUR);


  for(int i = 0; i < num_loci; ++i){
    names[i] = read_string(ptr);
      //  (char*)malloc(strlen(tempStr) + 1);
      // strcpy(names[i], tempStr);
    //Here I want to make the Hash Table!
    // They key is tempStr. the bvalue is is i.
    hash_add(names[i], i);
    //Note that Id_table is the same as names. Can reuse
    //   id_table[i] = (char*)malloc(strlen(tempStr)+1);
    //strcpy(id_table[i], tempStr);
  }
  printf("Array of all names obtained\n\n");
  printf("Hash table created\n\n");

  int* normalization_id = (int*)calloc(num_loci, sizeof(uint8_t));

  //Not 10 here!! NUm loci!!
  for(int i = 0; i < num_loci; ++i){
    normalization_id[i] = read_byte(ptr);
  }
  printf("Normilizatio ID array created\n\n");

  int* assay_types = (int*)calloc(num_loci, sizeof(int));
  int* addresses = (int*)calloc(num_loci, sizeof(int));
  char** snps = (char**)calloc(num_loci, sizeof(char*));


  char** chroms = (char**)calloc(num_loci, sizeof(char*));

  int* map_infos = (int*)calloc(num_loci, sizeof(int));
  int* ref_strands = (int*)calloc(num_loci, sizeof(int));
  int* source_strands = (int*)calloc(num_loci, sizeof(int));


  //for(int i = 0; i < num_loci; ++i){
  for(int i = 0; i < 1; ++i){
    locusEntry *locus_entry = create_locus_entry(ptr);
    char* theName = locus_entry->name;
    printf("theName is :%s\n\n", theName);
    int* bb = hash_find(id_table, theName);
    printf("the BB is: %i\n\n", bb);
    int poss = table[bb];
    printf("Poss is :%i\n\n", poss);
    //      assay_types[name_lookup]
  }



  printf("I made it to the end.\n");
  printf("freeing memeory...\n");

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

//The hash Function
void hash_add(char * snp, int p) {
  int ind, i=1;
  ind = id2hash(snp);
  while(table[ind] != -1) {
    ind=(ind+i)&mask;
    i=i+2;
  }
  table[ind]=p;
}

int id2hash ( char * name) {
  int result=0;
  for(int i=0; name[i]!=0; i++) {
    result = (result<<1);
    result = (result+name[i]) ^ (name[i]<<8)+1027*(name[i]);
  }
  result=result&mask;
  return result;
}


int  hash_find(char ** id_table, char * snp) {
  int ind, orig,i=1;
  orig=ind = id2hash(snp);

  while((table[ind]!=-1) && (strcmp(id_table[table[ind]],snp) != 0)) {
    ind=(ind+i)&mask;
    i=i+2;
  }

  if (table[ind]==-1) {
    printf("No entry <%s> %d in hash table\n", snp, orig);
    exit(-17);
  }
  return table[ind];
}

void hash_init () {
  memset(table,-1,hash_size);
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
  printf("The given version is:%i\n\n", version);


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
    
    printf("Got to the end\n\n");
  }
  if(version == 7 || version == 8){
    int tempRead = 0;
    int readAttempt = fread(&tempRead, 4 * 4, 1, ptr);
 
  }

  if(version == 8){
    ref_strand = ref_from_string(read_string(ptr)[0]);
    printf("The ref strand is:%i\n\n", ref_strand);
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
  locus_entry->source_strand = 0;
  
  printf("line 437 reached\n\n");
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
