//This is a helper function
//Should do all the reading and passes that to main File

//All my includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//This is a helper function
//Should do all the reading and passes that to main File

//All my includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Declaring the functions.
//On first iteration, each function will return sample data. Testing my c syntax
//On second iteration, each function will return proper data.
char* read_ushort(char a){
  char *b = (char*)malloc(4);
  *b = 'j';
  return b;
}

//char* read_ushort(char* filename);
int read_int(FILE* ptr);
char* read_float(char* filename);
char read_byte(FILE* ptr);
char* read_string(FILE* ptr);
char read_char(char* handle);
int decode_code_point(char **s);
int __parse_file(char filename[40]);


//atrributes. The sizes of these still needs to be calculated
//Would rather store them all on the heap for some flexibility
int main(){
  printf("Starting...\n");
  int t =  __parse_file("H3Africa_2017_20021485_A3.bpm");
  //  char* j = read_ushort('o');
  //  free(j);
  char names[25][10];
  char snps [25][10];
  char chroms [25][10];
  char map_infos [25][10];
  char addresses [25][10];
  char normalization_ids [25][10];
  char assay_types [25][10];
  char normalization_lookups [25][10];
  char ref_strands [25][10];
  char source_strands [25][10];
  int num_loci = 0;
  char manifest_name[30];
  char control_config[25];

  //COMPLEMENT_MAP = {"A": "T", "T": "A", "C": "G", "G": "C", "D": "D", "I": "I"};
  //This should be a map.
  char mapKeys[7] = {'A', 'T', 'C', 'G', 'D', 'I'};
  char mapVals[7] = {'T','A', 'G', 'C', 'D','I'};

  return 0;
}


//Actually declaring the functions
//I think these should be put into anther file and imported. Just figuring out the syntax

int __parse_file(char filename[40]){
  char buffer[3];
  char* control_config;
  FILE *ptr;
  int version_flag = 4096;
  char ver;
  int versionInt;
  int num_loci; //Will need to make sure this datatype is beg enough

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
  ver = read_byte(ptr);

  printf("ver: %c\n\n", ver);
  if(ver != '1'){
    printf("Wrong BPM version\n"); // not a BPM 1 file
    exit(-12);
  }
	
  versionInt = read_int(ptr);

  if(ver & version_flag == version_flag){
    ver = ver ^ version_flag;
  }
  if(ver > 5 || ver < 3){
    printf("Unsupported BPM version");
    exit(-13);
  }

  if(ver > 1){
    control_config = read_string(ptr);
  }

  num_loci = read_int(ptr);

  free(control_config);
  return 1;
}

char read_byte(FILE* ptr){
  char byt[1];
  int success = fread(byt, sizeof(byt), 1, ptr);
  if(success != 1){
    printf("Byte was read unseccesfully.");
    exit(-13);
  }
  printf("value of Byte:%c\n\n", byt[0]);
  return '1';
  //  return byt[0];
  //   return struct.unpack("<B", handle.read(1))[0]
}

int read_int(FILE* ptr){
  return 1;//Sample Data

  // return struct.unpack("<i", handle.read(4))[0]
 }

char* read_string(FILE* ptr){//Getting sife of Data is NB to properly declare sife of storing varibles

  char* a = (char*)malloc(4);
  *a = 'B';
  int total_length = 0;
  int  partial_length = read_byte(ptr);
  int num_bytes = 0;
  while(partial_length & 0x80 > 0){
    total_length += (partial_length & 0x7F) << (7 * num_bytes);
    //  partial_length = ord(struct.unpack("c", handle.read(1))[0])
    num_bytes += 1;
  }

  total_length += partial_length << (7 * num_bytes);
  //   result = handle.read(total_length);
  //   reselt = result.decode(utf8)
//    if len(result) < total_length:
  //       raise Exception("Failed to read complete string")
           // else:
      //        return result

  return a;

}
