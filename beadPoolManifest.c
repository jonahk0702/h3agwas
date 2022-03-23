//This is a helper function
//Should do all the reading and passes that to main File

//All my includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Look into these
#include <sys/stat.h>
#include <unistd.h>//Unicodes??

//Declaring the functions.
//On first iteration, each function will return sample data. Testing my c syntax
//On second iteration, each function will return proper data. 
char* read_ushort(char* a){
	*a = 'b';
    return "cheese";
}

//char* read_ushort(char* filename);
char* read_int(char* filename);
char* read_float(char* filename);
char* read_byte(char* filename);
char* read_string(char* filename);
char read_char(char* handle);



int decode_code_point(char **s);
int __parse_file(char* filename);


//atrributes. The sizes of these still needs to be calculated
//Would rather store them all on the heap for some flexibility



int main(){
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

//	char* a = "I like cheese";
//	char * topping = read_ushort(&a);
	printf("Starting...");
//	printf("%s", *a);
	return 0;
}


//Actually declaring the functions
//I think these should be put into anther file and imported. Just figuring out the syntax


//In python this was just a function. Borrowed this from:
//https://gist.github.com/tylerneylon/9773800
// int decode_code_point(char **s) {
// 	int k = **s ? __builtin_clz(~(**s << 24)) : 0;  // Count # of leading 1 bits.
// 	int mask = (1 << (8 - k)) - 1;                  // All 1's with k leading 0's.
// 	int value = **s & mask;
// 	for (++(*s), --k; k > 0 && **s; --k, ++(*s)) {  // Note that k = #total bytes, or 0.
// 		value <<= 6;
// 	    value += (**s & 0x3F);
// 	}
// 	return value;
// }

//This file is bad...
// int __parse_file(char* filename){
// 	char buffer[3]; // I want three bytes, not bits.  I think this is correct, but not sure.

//  	FILE *ptr;

// 	ptr = fopen(filename,"rb");

// 	fread(buffer,sizeof(buffer),1,ptr);

// 	char header[3];
// 	header[1] = buffer[1];
// //	header = decode_code_point(buffer);Needs to be in, but error. Decode to utf-8

// 	if(strlen(header) != 3 || header != "BPM"){
// 		printf("Error here.\n");//Not a BPM file
// 	}else{
// 		char version = '1';//read_char(filename);
// 		if(version != '1'){
// 		printf("Error here. Wrong BPM version\n");//Not a BPM file

// 		}else{
// 			//Proceed.
// 		}
// 		//Here
			
// 	}
// 	return 1;
// }

//This method is bad.
 char read_char(char* filename){
 	// FILE* in_file = fopen("test.txt", "rb");
  //   if (!in_file) {
  //       perror("fopen");
  //       exit(EXIT_FAILURE);
  //   }

  //   struct stat sb;
  //   if (stat(filename, &sb) == -1) {
  //       perror("stat");
  //       exit(EXIT_FAILURE);
  //   }

  //   char* file_contents = malloc(sb.st_size);
  //   fread(file_contents, sb.st_size, 1, in_file);

  //   printf("read data: %s\n", file_contents);
 //   fclose(in_file);
//    char buffer[3]; // I want three bytes, not bits.  I think this is correct, but not sure.
//  	FILE *ptr;


  // 	char* file_contents = malloc(3);
  // 	read(fd, file_contents, sb.st_size);

 	// ptr = fopen(handle,"rb");
 	// fread(&buffer,sizeof(buffer),1,ptr);
  //   return *buffer;
}

//Python functions
/*
def read_ushort(handle):
    return frombuffer(handle.read(2), dtype=uint16)[0]

def read_int(handle):
    
    return struct.unpack("<i", handle.read(4))[0]

def read_float(handle):
    
    return frombuffer(handle.read(4), dtype=float32)[0]

def read_byte(handle):
    
    return struct.unpack("<B", handle.read(1))[0]

def read_string(handle):
    
    total_length = 0
    partial_length = read_byte(handle)
    num_bytes = 0
    while partial_length & 0x80 > 0:
        total_length += (partial_length & 0x7F) << (7 * num_bytes)
        partial_length = ord(struct.unpack("c", handle.read(1))[0])
        num_bytes += 1
    total_length += partial_length << (7 * num_bytes)
    result = handle.read(total_length)
    result = result.decode("utf-8")
    if len(result) < total_length:
        raise Exception("Failed to read complete string")
    else:
        return result
*/

