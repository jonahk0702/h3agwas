Fast and Efficient Processing of Illumina Genomic Data in C, and Other Adventures
By J. Kollenberg






Contents
Chapter 1 - The Goal
Chapter 2 - My Progress
Chapter 3 - Next steps
Chapter 4 – Tips
Chapter 5 – Code Walkthrough
Chapter 5 - Contact me

 
 
 
 




Chapter 1 - The Goal

Before you begin, I think it is important to understand the goal of this project. For some context, we are trying to sequence individuals' genes. As it stands, Wits uses the products of a company called Illumnia and the process is as follows.

Step 1
The actual test is done by dropping some blood onto a bunch of computer chips. From this step .idat files are created with all the information from the test stored in these large files.

Step 2
The .idat files require some processing and to be turned into .gtc files. This takes quite a bit of time and processing power.

Step 3
The .gtc files need to be turned into a Final Report. This is done by a very slow Python module written by Illumina and the Final Reports are very large files, containing information that is irrelevant to us. 

Step 4
A few columns of the Final Report are taken and transposed to create a .lgen file which can be imported into Plink, the final destination. 

The goal of this project is to write a single, super-efficient C script which takes in a .idat file and returns a Plink file, in one go. This script can also be custom designed to fit into Wits’ pipeline effortlessly as well as to only processing the parts we use. 

Although it may sound difficult, this task is not that complex. Mostly.

The first thing one must appreciate is that python is an incredibly slow language and C is incredibly quick. (To give you a sense of the scale, the official Python takes about 302 seconds to read the .gtc’s manifest file. My direct translation to C script takes 12). This means we can write a radically faster script by directly translating the current one, which is possible given that all the code in the original process is open source. But, in order to remove the redundancy, some degree of understanding is required in order to know which functions can be safely omitted when turning the general script into a specialised one.








Chapter 2 - My Progress 

We have decided to split up this project into multiple steps. The first big milestone would be completing a C script that take in a folder full of GTC files, and manifest file and creating a Plink file. 
All of my work so far has been contained in this step. As it stands, the script reads in all the information, does necessary changes/processors, locates important bits and discards the unimportant bits.

The current issues I am struggling with are:
1.	The original python script is very versatile, being able to handle as many GTCs as the user would like, with as many people in each GTC as desired. Mine is still trying to perfect single GTC with single person. In future it should be expanded to accommodate the variations. 
2.	GTCs come in different versions, with version 3,4 and 5 being acceptable. I have been testing my code exclusively with version 5 and support for 3 and 4 may be required.





 



Chapter 3 - Next steps

First, you would want to complete the first milestone and get the GTC to Plink up perfectly. I have the correct array, titled plus_array, but it needs to be encoded into Plinks encoding and put into a 2d array. After this is done, you should make sure that the process works for directories of more than one GTC. You can test whether you have done the process correctly but importing the output file into Plink.
Next, I would assume you would want to extend the code backwards meaning instead of taking GTCs as inputs, take .idats, do all processing that would have created GTCs and use those results for plink. This should not be a difficult task as python for processing Idats should be accessible. 

This will not be the end of the project. There has been talk of all the interesting files we can create if we have full control of the pipeline. From extending to VCFs to potentially creating our own file structures built to only support necessary information.









Chapter 4 - Tips

New projects often seem daunting at first. This is especially true if one is coming in without much C experience. However, I can confidently say that with a little perseverance C comes surprisingly easily, and it is such a useful tool to know. Below is a list of tips that drastically sped up the development process. 

Proper Testing
1.	Before you begin, I would suggest cloning the BeadPoolArray git into your local repository.
2.	Move each of the modules (files in the directory “Modules”) into the directory called “Examples”.
3.	In gtc_final_report.py (or any other example), change the imports to use to local files in the same directory 
	I.e. 	change	 	from .BeadArrayUtility import read_int, read_string, read_byte
to 		from BeadArrayUtility import read_int, read_string, read_byte
4.	This allows you to edit the modules and print out intermediate values as opposed to running the official module and only having access to final results.

Function Testing
1.	The main file has gotten quite long and can take a bit of time to run. And so, when adding a function that you are not fairly confident will perform as expected, you should test the function in its own script using sample values. 
2.	This can either be done by creating a separate C file in isolation of the main one. Alternatively, I often use an online C compiler which runs without any set up required. It is particularly good for testing memory management. The link can be found here.

In General 
1.	Watch your #includes. 
This may seem obvious but can save you a large amount of time.
 When replicating code in another environment make sure to use the proper #includes. And you compile your code, and you suddenly have a bunch of errors in code that used to work, make sure the #includes are spelt correctly and that you have not mistakenly added a letter.
 

Important emacs commands on Windows
There are so many possible commands, but the following are, in my opinion, the essentials. 
	Note, when I write “Ctrl s” I mean press (and release) control and then press s.
	When I type something in [ ], those are instructions to you. 
1.	Ctrl s - Save
2.	Ctrl x - exit
3.	Alt x [type words] goto-line [ click enter ] [ type a number ]- it scrolls down to line of the number you typed
4.	Alt s w [type a word] - searches for word you typed
5.	Control [space] [use arrow keys to select text] - Highlights and selects text
6.	Ctrl w - cuts selected text
7.	Ctrl y - pastes cut text










Chapter 5 – Code Walkthrough

__parse_file
-	This is the main file.
-	It begins by making sure the given files are of the correct version and readable.
-	Starts reading from .bpm file and assigning variables such as num_loci and the position of each locus and then information about each locus entry.
-	Up until printing out “Moving to GTC reader”, all the code comes from Illumina’s BeadPoolManifest.
-	All code after, comes from GenotypeCalls.
-	Genotype Calls takes in a folder of GTCs, and I attempt to imitate that.
-	Again, version checks are done.
-	The functions get_genotypes and get_base_calls_plus_strand actually extract the genomic data from the GTCs.
-	Then I free everything.

read_byte
-	Simply reads a byte from given file

read_int
-	Simply reads in a 4-byte int from given file.

read_string
-	A common issue is knowing how many bytes the string spans. Illumina’s solution is letting the first bit in each string byte denoting if the next byte also contains some string.
-	For example, if the bytes are as follows: 10101111 11011100 01111010, the first bit in the first byte is a 1 and says the second byte is in the string. The first bit of second byte says third byte is in string. The first bit of third byte is a 0, indicating that after reading the third byte, the string is complete.
-	Logically, this would mean there are only 7 useful bits in a byte, and this is why shift by 7.

read_test
-	This is the same as read_string but takes in a variable by reference and writes the string into that variable. 
-	The point is to avoid memory leaks.

create_locus 
-	Reads in portion of .bpm and creates a locus entry for each locus.

to_string
-	Takes an int and returns a char.
-	Comes from BeadPoolManifest

from_string 
-	Opposite of to_string 
-	Comes from BeadPoolManifest

ref_from_string
-	Comes from RefStrand object in BeadPoolManifest
-	Takes char, return an int.

hash, ht_pair, ht_create, ht_set, ht_get
-	Together these make up the hash map we use. 
-	It is super quick and basically cerates a dictionary between keys and values.
-	This implementation was inspired by https://youtu.be/wg8hZxMRwcw

EndsWithGtc
-	Checks if filename ends with .gtc

is_write_complete, get_logt_ratios, __get_generic_array_numpy
-	These functions taken from GenotypeCalls and necessary for reading the array in from the GTCs.
-	The only run with GTC version 3 or version 4, and so I have never run these functions and not quite sure they work. The GTCs I have seen have all been version 5.

get_slide_identifier, get_genotypes, __get_generic_array
-	Translation of those functions in GenotypeCalls, just help fetch correct data from GTCs.
 
get_base_calls_plus_strand
-	We are specifically looking for plus strand genomic data. So, this function helps us find it.

get_base_calls_generic
-	Fetches the data.
-	For each locus, we fetch the genotypes and nucleotides.
-	This function also decides which information belongs in the array and when to use the complement.

Complement
	Just fetches the complement.







Chapter 5 - Contact me

If you have any questions, please feel free to pop me an email at: jonahkollenberg1@gmail.com.

